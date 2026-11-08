// gneural_motor.hpp - GNU GNeuralNet-backed online-learning motor policy for
// GTAngel v27. This is the adaptive NN actuator: a 20->16->2 tanh MLP trained
// online with gradient descent on accepted human/DTE demonstrations. Nothing
// here is locked, sealed or frozen - the network trains every session.
#ifndef GTANGEL_V27_GNEURAL_MOTOR_HPP
#define GTANGEL_V27_GNEURAL_MOTOR_HPP

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <string>
#include <vector>

extern "C" {
#include "network.h"
#include "error.h"
#include "gradient_descent.h"
#include "rnd.h"
#include "feedforward.h"
#include "save.h"
#include "load.h"
}

namespace v27gnn {

static constexpr int FeatureSize = 20;
static constexpr int HiddenSize = 16;
static constexpr int OutputSize = 2; // [0]=turn (right+), [1]=forward
static constexpr int MaxSamples = 256; // rolling demonstration buffer
static constexpr int TrainBatch = 16;  // most recent samples per pass
static constexpr int TrainEveryFrames = 20;

class GneuralMotorPolicy {
public:
    GneuralMotorPolicy() = default;
    ~GneuralMotorPolicy() { destroy(); }

    GneuralMotorPolicy(const GneuralMotorPolicy&) = delete;
    GneuralMotorPolicy& operator=(const GneuralMotorPolicy&) = delete;

    // Build the 20->16->2 tanh/linear network with small random weights.
    bool create() {
        if (net_) return true;
        net_ = network_alloc();
        if (!net_) return false;
        network_set_neuron_number(net_, FeatureSize + HiddenSize + OutputSize);
        network_set_layer_number(net_, 3);
        net_->layers[0].neurons = &net_->neurons[0];
        net_->layers[0].num_of_neurons = FeatureSize;
        net_->layers[1].neurons = &net_->neurons[FeatureSize];
        net_->layers[1].num_of_neurons = HiddenSize;
        net_->layers[2].neurons = &net_->neurons[FeatureSize + HiddenSize];
        net_->layers[2].num_of_neurons = OutputSize;
        for (int i = 0; i < FeatureSize; ++i) {
            net_->neurons[i].num_input = 0; // input neurons
        }
        for (int h = 0; h < HiddenSize; ++h) {
            neuron* ne = &net_->neurons[FeatureSize + h];
            network_neuron_set_connection_number(ne, FeatureSize);
            for (int c = 0; c < FeatureSize; ++c) {
                ne->connection[c] = &net_->neurons[c];
                ne->w[c] = (rnd() * 2.0 - 1.0) * 0.1;
            }
            ne->activation = TANH;
            ne->discriminant = LINEAR;
        }
        for (int o = 0; o < OutputSize; ++o) {
            neuron* ne = &net_->neurons[FeatureSize + HiddenSize + o];
            network_neuron_set_connection_number(ne, HiddenSize);
            for (int c = 0; c < HiddenSize; ++c) {
                ne->connection[c] = &net_->neurons[FeatureSize + c];
                ne->w[c] = (rnd() * 2.0 - 1.0) * 0.1;
            }
            ne->activation = TANH;
            ne->discriminant = LINEAR;
        }
        return true;
    }

    // Forward pass: features in, (turn, forward) out.
    bool infer(const float* features, float& turnOut, float& forwardOut) const {
        if (!net_) return false;
        for (int i = 0; i < FeatureSize; ++i)
            net_->neurons[i].output = features[i];
        feedforward(net_);
        turnOut = (float)net_->neurons[FeatureSize + HiddenSize].output;
        forwardOut = (float)net_->neurons[FeatureSize + HiddenSize + 1].output;
        return true;
    }

    // Store one demonstration pair in the rolling buffer.
    void remember(const float* features, float turn, float forward) {
        Sample s;
        for (int i = 0; i < FeatureSize; ++i) s.x[i] = features[i];
        s.y[0] = turn;
        s.y[1] = forward;
        if ((int)samples_.size() >= MaxSamples) samples_.erase(samples_.begin());
        samples_.push_back(s);
        ++newSamples_;
    }

    int pendingSamples() const { return newSamples_; }
    int totalSamples() const { return (int)samples_.size(); }
    uint64_t trainingUpdates() const { return trainingUpdates_; }
    uint64_t trainingSteps() const { return trainingSteps_; }
    float lastLoss() const { return lastLoss_; }

    // One online gradient-descent pass over the rolling buffer. Budgeted to
    // a few milliseconds: gamma=0.08, maxiter=8 (empirically stable).
    bool train() {
        if (!net_ || samples_.empty()) return false;
        const int n = (int)samples_.size();
        if (!cfg_) {
            cfg_ = network_config_alloc_default();
            if (!cfg_) return false;
            // alloc_default pre-ensures only 32 neurons x 16 fan-in; our net
            // is 38 neurons with fan-in 20 - grow the point buffers explicitly.
            network_config_ensure(cfg_, FeatureSize + HiddenSize + OutputSize, FeatureSize);
            cfg_->verbosity = OFF;
            cfg_->optimization_type = GRADIENT_DESCENT;
            cfg_->error_type = L2;
            // gamma set per-pass by the adaptive schedule below
            cfg_->maxiter = 1;
            cfg_->accuracy = 1e-4;
            cfg_->wmin = -1.0;
            cfg_->wmax = 1.0;
            cfg_->nxw = 1000;
        }
        // Train on the most recent TrainBatch samples: a full 256-point
        // pass costs ~600ms (central-difference gradient_descent), while a
        // 16-point single-iteration pass costs ~5ms - safe for the frame
        // loop. Older samples still shape the network across passes.
        const int batch = n < TrainBatch ? n : TrainBatch;
        for (int p = 0; p < batch; ++p) {
            const Sample& s = samples_[n - batch + p];
            for (int i = 0; i < FeatureSize; ++i)
                NET_PX(cfg_, p, i, 0) = s.x[i];
            for (int o = 0; o < OutputSize; ++o)
                NET_PY(cfg_, p, FeatureSize + HiddenSize + o) = s.y[o];
        }
        cfg_->num_points = batch; // training point count (error() reads this)
        // Adaptive learning rate: sqrt-shaped L2 error gives an almost
        // constant gradient magnitude near the optimum, so a fixed gamma
        // oscillates forever. Halve gamma when a pass fails to improve,
        // reset when it does - converges and stays responsive to new data.
        cfg_->gamma = gamma_;
        gradient_descent(net_, cfg_);
        const double after = error(net_, cfg_);
        if (after < lastError_ * 0.999 || newSamples_ > 0) gamma_ = 0.01; // fresh data re-explores
        else gamma_ = (gamma_ > 2e-4) ? gamma_ * 0.5 : 1e-4; // floor keeps learning alive
        lastError_ = after;
        lastLoss_ = (float)after;
        trainingSteps_ += (uint64_t)cfg_->maxiter;
        ++trainingUpdates_;
        newSamples_ = 0;
        return true;
    }

    bool save(const std::string& path) {
        if (!net_ || !cfg_) return false;
        if (cfg_->save_network_file_name) free(cfg_->save_network_file_name);
        cfg_->save_network_file_name = dup_(path.c_str());
        cfg_->save_neural_network = ON;
        network_save(net_, cfg_); // void; exits the process on I/O failure
        cfg_->save_neural_network = OFF;
        return true;
    }

    bool load(const std::string& path) {
        // network_load() exits the process on a missing file; guard first.
        if (!std::filesystem::exists(path)) return false;
        if (!net_ && !create()) return false;
        if (!cfg_ && !(cfg_ = network_config_alloc_default())) return false;
        network_config_ensure(cfg_, FeatureSize + HiddenSize + OutputSize, FeatureSize);
        cfg_->verbosity = OFF;
        if (cfg_->load_network_file_name) free(cfg_->load_network_file_name);
        cfg_->load_network_file_name = dup_(path.c_str());
        cfg_->load_neural_network = ON;
        network_load(net_, cfg_);
        cfg_->load_neural_network = OFF;
        return true;
    }

private:
    struct Sample {
        float x[FeatureSize];
        float y[OutputSize];
    };

    static char* dup_(const char* s) {
        char* d = (char*)malloc(strlen(s) + 1);
        if (d) strcpy(d, s);
        return d;
    }

    void destroy() {
        if (cfg_) { network_config_free(cfg_); cfg_ = nullptr; }
        if (net_) { network_free(net_); net_ = nullptr; }
    }

    network* net_{nullptr};
    network_config* cfg_{nullptr};
    std::vector<Sample> samples_;
    int newSamples_{0};
    uint64_t trainingUpdates_{0};
    uint64_t trainingSteps_{0};
    float lastLoss_{-1.0f};
    double gamma_{0.01};
    double lastError_{1e9};
};

} // namespace v27gnn

#endif // GTANGEL_V27_GNEURAL_MOTOR_HPP

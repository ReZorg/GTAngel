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
#include "learning_checkpoint.hpp"

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
static constexpr int MaxSamples = 256;
static constexpr int TrainBatch = 16;  // recent plus manoeuvre-balanced replay
static constexpr int TrainEveryFrames = 20;
static constexpr uint32_t InputContract = 2; // pre-action body state + intended goal

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
        if (!net_ || !features) return false;
        for (int i = 0; i < FeatureSize; ++i) {
            if(!std::isfinite(features[i]) || std::fabs(features[i])>100) return false;
            net_->neurons[i].output = features[i];
        }
        feedforward(net_);
        turnOut = (float)net_->neurons[FeatureSize + HiddenSize].output;
        forwardOut = (float)net_->neurons[FeatureSize + HiddenSize + 1].output;
        return std::isfinite(turnOut)&&std::isfinite(forwardOut);
    }

    // Store one demonstration pair in the rolling buffer.
    void remember(const float* features, float turn, float forward) {
        if(!features || !std::isfinite(turn)||!std::isfinite(forward)||
           std::fabs(turn)>1 || std::fabs(forward)>1) { ++invalidSamples_; return; }
        Sample s;
        for (int i = 0; i < FeatureSize; ++i) {
            if(!std::isfinite(features[i])||std::fabs(features[i])>100) {++invalidSamples_;return;}
            s.x[i] = features[i];
        }
        s.y[0] = turn;
        s.y[1] = forward;
        if ((int)samples_.size() >= MaxSamples) {
            // Retire the oldest sample from the most represented manoeuvre.
            // Straight cruising must not erase every turn/reverse/stop example.
            auto counts=replayCounts(); ++counts[category(s)];
            const int most=static_cast<int>(std::max_element(counts.begin(),counts.end())-counts.begin());
            const auto oldest=std::find_if(samples_.begin(),samples_.end(),
                [&](const Sample& sample){return category(sample)==most;});
            samples_.erase(oldest==samples_.end()?samples_.begin():oldest);
        }
        samples_.push_back(s);
        newSamples_=std::min(newSamples_+1,MaxSamples);
    }

    int pendingSamples() const { return newSamples_; }
    int totalSamples() const { return (int)samples_.size(); }
    uint64_t trainingUpdates() const { return trainingUpdates_; }
    uint64_t trainingSteps() const { return trainingSteps_; }
    float lastLoss() const { return lastLoss_; }
    uint64_t rejectedUpdates() const {return rejectedUpdates_;}
    uint64_t invalidSamples() const {return invalidSamples_;}
    double learningRate() const {return gamma_;}
    double lastBatchBefore() const {return lastBatchBefore_;}
    double lastBatchAfter() const {return lastBatchAfter_;}
    std::array<unsigned,5> replayCounts() const {
        std::array<unsigned,5> counts{};
        for(const auto& s:samples_) ++counts[category(s)];
        return counts;
    }
    std::array<float,FeatureSize> latestTrainingInput() const {
        std::array<float,FeatureSize> result{};
        if(!samples_.empty()) std::copy(std::begin(samples_.back().x),std::end(samples_.back().x),result.begin());
        return result;
    }
    bool adoptCurrentInputContract() {
        if(inputContract_==InputContract) return false;
        samples_.clear(); newSamples_=0; inputContract_=InputContract;
        lastLoss_=-1; lastError_=1e9;
        return true; // Weights, adaptive rate and lifetime steps are preserved.
    }
    uint64_t policyRevision() const {
        if(!net_) return 0;
        gtlearn::Writer weights;
        weights.put(inputContract_);
        for(unsigned i=0;i<net_->num_of_neurons;++i)
            for(unsigned j=0;j<net_->neurons[i].num_input;++j) weights.put(net_->neurons[i].w[j]);
        return gtlearn::checksum(weights.bytes);
    }
    void restoreLegacyCounters(uint64_t steps,uint64_t updates,float loss) {
        trainingSteps_=steps; trainingUpdates_=updates;
        if(std::isfinite(loss)) lastLoss_=loss;
    }
    void swap(GneuralMotorPolicy& other) noexcept {
        using std::swap;
        swap(net_,other.net_); swap(cfg_,other.cfg_); swap(samples_,other.samples_);
        swap(newSamples_,other.newSamples_); swap(trainingUpdates_,other.trainingUpdates_);
        swap(trainingSteps_,other.trainingSteps_); swap(lastLoss_,other.lastLoss_);
        swap(gamma_,other.gamma_); swap(lastError_,other.lastError_);
        swap(inputContract_,other.inputContract_); swap(rejectedUpdates_,other.rejectedUpdates_);
        swap(invalidSamples_,other.invalidSamples_);
        swap(lastBatchBefore_,other.lastBatchBefore_); swap(lastBatchAfter_,other.lastBatchAfter_);
    }
    std::string encode() const {
        if(!net_) throw std::runtime_error("motor network unavailable");
        gtlearn::Writer w;
        w.put<uint32_t>(2); w.put<uint32_t>(FeatureSize); w.put<uint32_t>(HiddenSize); w.put<uint32_t>(OutputSize);
        w.put(trainingSteps_); w.put(trainingUpdates_); w.put(lastLoss_);
        w.put(gamma_); w.put(lastError_); w.put<int32_t>(newSamples_);
        for(unsigned i=0;i<net_->num_of_neurons;++i)
            for(unsigned j=0;j<net_->neurons[i].num_input;++j) w.put(net_->neurons[i].w[j]);
        w.put<uint32_t>(static_cast<uint32_t>(samples_.size()));
        for(const auto& sample:samples_) {
            for(float x:sample.x) w.put(x);
            for(float y:sample.y) w.put(y);
        }
        w.put(inputContract_); w.put(rejectedUpdates_); w.put(invalidSamples_);
        return w.bytes;
    }
    bool decode(const std::string& bytes) {
        try {
            gtlearn::Reader r{bytes};
            const auto version=r.get<uint32_t>();
            if((version!=1&&version!=2)||r.get<uint32_t>()!=FeatureSize||
               r.get<uint32_t>()!=HiddenSize||r.get<uint32_t>()!=OutputSize) return false;
            GneuralMotorPolicy trial; if(!trial.create()) return false;
            trial.trainingSteps_=r.get<uint64_t>(); trial.trainingUpdates_=r.get<uint64_t>();
            trial.lastLoss_=r.get<float>(); trial.gamma_=r.get<double>(); trial.lastError_=r.get<double>();
            trial.newSamples_=r.get<int32_t>();
            if(trial.gamma_<=0||trial.gamma_>.1||trial.lastLoss_<-1||
               trial.lastError_<0||trial.newSamples_<0) return false;
            for(unsigned i=0;i<trial.net_->num_of_neurons;++i)
                for(unsigned j=0;j<trial.net_->neurons[i].num_input;++j) {
                    const double weight=r.get<double>();
                    if(std::fabs(weight)>1e6) return false;
                    trial.net_->neurons[i].w[j]=weight;
                }
            const uint32_t count=r.get<uint32_t>();
            if(count>MaxSamples || trial.newSamples_>MaxSamples) return false;
            trial.samples_.resize(count);
            for(auto& sample:trial.samples_) {
                for(float& x:sample.x) { x=r.get<float>(); if(std::fabs(x)>100) return false; }
                for(float& y:sample.y) { y=r.get<float>(); if(std::fabs(y)>1) return false; }
            }
            trial.inputContract_=version==1?1:r.get<uint32_t>();
            if(trial.inputContract_<1||trial.inputContract_>InputContract) return false;
            if(version==2) {
                trial.rejectedUpdates_=r.get<uint64_t>(); trial.invalidSamples_=r.get<uint64_t>();
            }
            r.end(); swap(trial); return true;
        } catch(const std::exception&) { return false; }
    }

    // One bounded online gradient-descent iteration over the recent samples.
    bool train() {
        if (!net_ || samples_.empty()) return false;
        const int n = (int)samples_.size();
        if (!cfg_) {
            cfg_ = network_config_alloc_default();
            if (!cfg_) return false;
            // alloc_default pre-ensures only 32 neurons x 16 fan-in; our net
            // is 38 neurons with fan-in 20 - grow the point buffers explicitly.
            network_config_ensure(cfg_, FeatureSize + HiddenSize + OutputSize, FeatureSize);
        }
        // load() also allocates cfg_, with maxiter=0 by default. Configure
        // training for both fresh and restored networks on every pass.
        cfg_->verbosity = OFF;
        cfg_->optimization_type = GRADIENT_DESCENT;
        cfg_->error_type = L2;
        cfg_->maxiter = 1;
        cfg_->accuracy = 1e-4;
        cfg_->wmin = -1.0;
        cfg_->wmax = 1.0;
        cfg_->nxw = 1000;
        // Keep the bounded 16-point budget, but train on both current experience
        // and a rotating selection of older manoeuvres. Each point appears once.
        const int batch = n < TrainBatch ? n : TrainBatch;
        std::vector<int> selected;
        for(int i=std::max(0,n-batch/2);i<n;++i) selected.push_back(i);
        for(int round=0;static_cast<int>(selected.size())<batch && round<n;++round)
            for(int kind=0;kind<5 && static_cast<int>(selected.size())<batch;++kind) {
                std::vector<int> group;
                for(int i=0;i<n;++i) if(category(samples_[i])==kind) group.push_back(i);
                if(group.empty()) continue;
                const int i=group[(trainingSteps_+round)%group.size()];
                if(std::find(selected.begin(),selected.end(),i)==selected.end()) selected.push_back(i);
            }
        for (int p = 0; p < batch; ++p) {
            const Sample& s = samples_[selected[p]];
            for (int i = 0; i < FeatureSize; ++i)
                NET_PX(cfg_, p, i, 0) = s.x[i];
            for (int o = 0; o < OutputSize; ++o)
                NET_PY(cfg_, p, FeatureSize + HiddenSize + o) = s.y[o];
        }
        cfg_->num_points = batch; // training point count (error() reads this)
        // Compare the same batch before and after the step. Comparing different
        // batches, or resetting gamma for every fresh sample, hid regressions.
        std::vector<double> beforeWeights;
        for(unsigned i=0;i<net_->num_of_neurons;++i)
            for(unsigned j=0;j<net_->neurons[i].num_input;++j) beforeWeights.push_back(net_->neurons[i].w[j]);
        const double before=error(net_,cfg_);
        if(!std::isfinite(before)) return false;
        // A genuine rise in error on new experience can reopen the rate after
        // convergence; acceptance still depends on this exact batch improving.
        if(newSamples_>0 && before>lastError_+.05) gamma_=std::max(gamma_,.001);
        cfg_->gamma = gamma_;
        gradient_descent(net_, cfg_);
        const double after = error(net_, cfg_);
        ++trainingSteps_;
        lastBatchBefore_=before; lastBatchAfter_=after;
        if(!std::isfinite(after) || after>before+1e-9) {
            size_t k=0;
            for(unsigned i=0;i<net_->num_of_neurons;++i)
                for(unsigned j=0;j<net_->neurons[i].num_input;++j) net_->neurons[i].w[j]=beforeWeights[k++];
            gamma_=std::max(1e-6,gamma_*.5);
            lastError_=before; lastLoss_=static_cast<float>(before);
            ++rejectedUpdates_;
            return false;
        }
        if(after<before-1e-10) gamma_=std::min(.01,gamma_*1.05);
        lastError_ = after;
        lastLoss_ = (float)after;
        ++trainingUpdates_;
        newSamples_ = 0;
        return true;
    }

    bool save(const std::string& path) {
        if(!net_) return false;
        try {
            std::ostringstream out; out<<std::setprecision(17)<<net_->num_of_neurons<<"\n";
            for(unsigned i=0;i<net_->num_of_neurons;++i) {
                const auto& n=net_->neurons[i];
                out<<i<<"\n"<<n.num_input<<"\n";
                for(unsigned j=0;j<n.num_input;++j) out<<n.w[j]<<"\n";
                for(unsigned j=0;j<n.num_input;++j) out<<n.connection[j]->global_id<<"\n";
                out<<n.activation<<"\n"<<n.discriminant<<"\n";
            }
            out<<net_->num_of_layers<<"\n";
            for(unsigned i=0;i<net_->num_of_layers;++i) {
                out<<net_->layers[i].num_of_neurons<<"\n";
                for(unsigned j=0;j<net_->layers[i].num_of_neurons;++j)
                    out<<net_->layers[i].neurons[j].global_id<<"\n";
            }
            return gtlearn::writeFile(path,out.str());
        } catch(const std::exception&) { return false; }
    }

    bool load(const std::string& path) {
        // Validate the complete fixed topology before replacing the live model.
        // The C loader calls exit() on failures and trusts unbounded dimensions.
        try {
            std::istringstream in(gtlearn::readFile(path));
            unsigned neurons=0;
            if(!(in>>neurons)||neurons!=FeatureSize+HiddenSize+OutputSize) return false;
            GneuralMotorPolicy trial; if(!trial.create()) return false;
            for(unsigned i=0;i<neurons;++i) {
                unsigned id=0,count=0;
                auto& n=trial.net_->neurons[i];
                if(!(in>>id>>count)||id!=i||count!=n.num_input) return false;
                for(unsigned j=0;j<count;++j)
                    if(!(in>>n.w[j])||!std::isfinite(n.w[j])||std::fabs(n.w[j])>1e6) return false;
                for(unsigned j=0;j<count;++j) {
                    unsigned connection=0;
                    if(!(in>>connection)||connection!=n.connection[j]->global_id) return false;
                }
                int activation=0,discriminant=0;
                if(!(in>>activation>>discriminant)||activation!=n.activation||discriminant!=n.discriminant)
                    return false;
            }
            unsigned layers=0; if(!(in>>layers)||layers!=3) return false;
            for(unsigned i=0;i<3;++i) {
                unsigned count=0; if(!(in>>count)||count!=trial.net_->layers[i].num_of_neurons) return false;
                for(unsigned j=0;j<count;++j) {
                    unsigned id=0;
                    if(!(in>>id)||id!=trial.net_->layers[i].neurons[j].global_id) return false;
                }
            }
            in>>std::ws; if(!in.eof()) return false;
            trial.inputContract_=1;
            swap(trial); return true;
        } catch(const std::exception&) { return false; }
    }

private:
    struct Sample {
        float x[FeatureSize];
        float y[OutputSize];
    };
    static int category(const Sample& s) {
        if(s.y[1]<-.05f) return 3; // reverse
        if(std::fabs(s.y[1])<.08f && std::fabs(s.y[0])<.15f) return 4; // stop
        if(s.y[0]<-.15f) return 1;
        if(s.y[0]>.15f) return 2;
        return 0; // straight
    }

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
    uint32_t inputContract_{InputContract};
    uint64_t rejectedUpdates_{0},invalidSamples_{0};
    double lastBatchBefore_{0},lastBatchAfter_{0};
};

} // namespace v27gnn

#endif // GTANGEL_V27_GNEURAL_MOTOR_HPP

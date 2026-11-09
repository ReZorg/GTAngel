#pragma once
// v27 observational reinforcement learning. No policy selection or effectors.
namespace v27learn {
struct OnlineNavigationObserver {
    uint64_t updates{0}, rejected{0}, collisions{0}, windows{0};
    uint32_t stateVisits[avatar3d::NavTrainer::BEARINGS][avatar3d::NavTrainer::DISTS][avatar3d::NavTrainer::OBST]{};
    uint32_t actionVisits[avatar3d::NavTrainer::BEARINGS][avatar3d::NavTrainer::DISTS][avatar3d::NavTrainer::OBST][avatar3d::NavTrainer::COUNT]{};
    uint32_t windowSteps{0};
    float windowReturn{0}, lastReward{0}, lastQDelta{0};
    static bool valid(const avatar3d::NavTrainer::State& s) {
        return s.bearing>=0 && s.bearing<avatar3d::NavTrainer::BEARINGS &&
               s.dist>=0 && s.dist<avatar3d::NavTrainer::DISTS &&
               s.obst>=0 && s.obst<avatar3d::NavTrainer::OBST;
    }
    static avatar3d::NavTrainer::Action action(float forward,float turn) {
        using N=avatar3d::NavTrainer;
        if(forward<-.01f) return N::BACK;
        if(forward>.01f) return turn<-.01f?N::FWD_L:turn>.01f?N::FWD_R:N::FWD;
        return turn<-.01f?N::TURN_L:turn>.01f?N::TURN_R:N::WAIT;
    }
    bool observe(avatar3d::NavTrainer& q,
                 const v15render::VisualNavEvidence& before,
                 const v15render::VisualNavEvidence& after,
                 float forward,float turn,float reward,bool humanAccepted,
                 bool contact,bool onFoot,bool enabled) {
        if(!enabled) return false;
        if(!humanAccepted || !onFoot || before.sourceSequence==0 ||
           after.sourceSequence<=before.sourceSequence || !valid(before.state) || !valid(after.state) ||
           !std::isfinite(forward) || !std::isfinite(turn) || !std::isfinite(reward) ||
           std::fabs(forward)>1 || std::fabs(turn)>1 || std::fabs(reward)>2 ||
           (std::fabs(forward)<.01f && std::fabs(turn)<.01f)) {
            ++rejected; return false;
        }
        const auto a=action(forward,turn);
        const float old=q.Q[before.state.bearing][before.state.dist][before.state.obst][a];
        q.update(before.state,a,reward,after.state);
        lastQDelta=q.Q[before.state.bearing][before.state.dist][before.state.obst][a]-old;
        ++updates; ++q.stepsTotal; ++windowSteps;
        ++stateVisits[before.state.bearing][before.state.dist][before.state.obst];
        ++actionVisits[before.state.bearing][before.state.dist][before.state.obst][a];
        if(contact) { ++collisions; ++q.collisions; }
        lastReward=reward; windowReturn+=reward;
        // These are fixed observational windows, NOT successful goal episodes.
        if(windowSteps==200) {
            ++windows; ++q.episodes;
            q.returnsPerEpisode.push_back(windowReturn);
            q.stepsPerEpisode.push_back(static_cast<int>(windowSteps));
            if(q.returnsPerEpisode.size()>1000) {
                q.returnsPerEpisode.erase(q.returnsPerEpisode.begin());
                q.stepsPerEpisode.erase(q.stepsPerEpisode.begin());
            }
            windowSteps=0; windowReturn=0;
        }
        return true;
    }
};
} // namespace v27learn

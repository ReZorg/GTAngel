#pragma once
namespace v27dte {
struct ControllerProposal { float forward{0},turn{0},uncertainty{.2f}; bool learned{false}; };
class LocalNavigation {
    std::mt19937 rng_{27001};
    uint32_t escapeTicks_{0};
public:
    uint64_t proposals{0},learnedProposals{0},accepted{0},contacts{0};
    void reset(){*this=LocalNavigation{};}
    ControllerProposal propose(const v15render::VisualNavEvidence& v,
            const avatar3d::NavTrainer& q,const v27learn::OnlineNavigationObserver& learning,
            bool contact) {
        ControllerProposal p;
        if(v.sourceSequence==0 || !v27learn::OnlineNavigationObserver::valid(v.state))return p;
        ++proposals;
        if(contact) { ++contacts;escapeTicks_=6; }
        // Brief unstick wiggle after real contact; keep it short so learning continues.
        if(escapeTicks_) { --escapeTicks_;p.forward=.10f;p.turn=.45f;return p; }
        // Screen-left is negative camera yaw; no goal coordinates are consulted.
        // Steer-and-creep: keep moving slowly while turning away instead of a
        // dead stop, so the avatar slides around pillars instead of sticking.
        if(v.obstacleVisible && v.nearestObstacleDepth<2.5f) {
            p.turn=v.state.obst<4?.45f:-.45f;
            p.forward=v.nearestObstacleDepth<1.2f?.08f:.15f;
            return p;
        }
        if(v.goalVisible) {
            const float bearing=(v.state.bearing-3.5f)/3.5f;
            p.turn=std::clamp(bearing*.5f,-.4f,.4f);
            p.forward=std::fabs(bearing)>.6f?.18f:.45f;
        } else {p.forward=.20f;p.turn=.28f;}
        // Only use Q entries actually visited in this local visual state.
        if(learning.stateVisits[v.state.bearing][v.state.dist][v.state.obst]>=8) {
            std::uniform_real_distribution<float> unit(0,1);
            if(unit(rng_)>q.epsilon) {
                float best=-1e30f;int chosen=-1;
                for(int a=0;a<avatar3d::NavTrainer::COUNT;++a)
                    if(learning.actionVisits[v.state.bearing][v.state.dist][v.state.obst][a]>0 &&
                       q.Q[v.state.bearing][v.state.dist][v.state.obst][a]>best) {
                        best=q.Q[v.state.bearing][v.state.dist][v.state.obst][a];chosen=a;
                    }
                if(chosen>=0) {
                    using N=avatar3d::NavTrainer;
                    const auto a=static_cast<N::Action>(chosen);
                    p.forward=a==N::BACK?-.25f:(a==N::FWD||a==N::FWD_L||a==N::FWD_R)?.45f:0;
                    p.turn=(a==N::TURN_L||a==N::FWD_L)?-.4f:(a==N::TURN_R||a==N::FWD_R)?.4f:0;
                    p.learned=true;++learnedProposals;
                }
            }
        }
        // A small bounded action variation supplies local exploration experience.
        else if(proposals%17==0) {p.turn=(proposals%34==0)?-.35f:.35f;}
        return p;
    }
};
} // namespace v27dte

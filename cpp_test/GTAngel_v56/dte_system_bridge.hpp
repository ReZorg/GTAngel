// GTAngel v26: local, advisory System 7 (8-node) -> System 8 (9-node) tree lens.
// The incidence arrays are generated from canonical trees, not inferred by an LLM.
// This module is deliberately outside SRD/MOTOR_COMMIT and WGR/NICHE_COMMIT.
#pragma once
#include <array>
#include <cstdint>
#include <cstdlib>
#include <algorithm>
#include "system7-system8-native.hpp"

namespace v26bridge {
struct LensState {
    bool valid{false};
    uint64_t visualSequence{0}, visualDigest{0}, bodySequence{0}, worldVersion{0};
    uint16_t root7{0}, root8{0}; // zero-based indexes; not a policy decision
    uint8_t free7{0}, free8{0};  // one-based family IDs
    uint16_t children{0};
    uint8_t childFamilies{0};
    std::array<uint16_t,286> childRootIndexes{};
};
class LensBridge final {
    LensState state_{};
public:
    const LensState& state() const {return state_;}
    void reset() {state_=LensState{};}
    // Called ONLY after GTAngelAGI, friend of LocalVisualIngress, validates a
    // current Sample against the renderer-owned 768x768 framebuffer and body.
    // Do not expose this function as a network or script ingress.
    void observe(const v25dte::Sample& sample,const v20cns::Body& body,
                 const v20cns::NicheHistory& niche) {
        reset();
        if (!sample.sequence() || sample.worldVersion()!=niche.worldVersion ||
            !v20cns::finitePose(body.position)) return;
        const int visibleGoal=sample.goalPixels()>20 ? 1:0;
        const int visibleNpc=sample.npcPixels()>10 ? 1:0;
        const int targetLeaves=1+visibleGoal+visibleNpc+(body.occupied?1:0);
        const int targetBranch=body.contact?2:(visibleGoal&&visibleNpc?1:0);
        const int targetHeight=body.occupied?3:visibleGoal?4:5;
        int bestScore=100000,chosen=-1;
        for(unsigned i=0;i<system78::Roots7.size();++i) {
            const auto& t=system78::Roots7[i];
            const int score=4*std::abs(int(t.leaves)-targetLeaves)+
                             3*std::abs(int(t.branch)-targetBranch)+
                             std::abs(int(t.height)-targetHeight);
            if(score<bestScore){bestScore=score;chosen=int(i);}
        }
        if(chosen<0) return;
        state_.valid=true;
        state_.visualSequence=sample.sequence();state_.visualDigest=sample.hash();
        state_.bodySequence=body.sequence;state_.worldVersion=niche.worldVersion;
        state_.root7=uint16_t(chosen);state_.free7=system78::Roots7[chosen].free;
        std::array<bool,48> seenFamily{};
        for(const auto& e:system78::Edges) if(e.parent==chosen) {
            if(e.child>=system78::Roots8.size() || state_.children>=state_.childRootIndexes.size()) {reset();return;}
            state_.childRootIndexes[state_.children++]=e.child;
            const uint8_t family=system78::Roots8[e.child].free;
            if(family==0 || family>47) {reset();return;}
            if(!seenFamily[family]) {seenFamily[family]=true;++state_.childFamilies;}
        }
        if(!state_.children || !state_.childFamilies){reset();return;}
        state_.root8=state_.childRootIndexes.front();
        state_.free8=system78::Roots8[state_.root8].free;
    }
};
static_assert(system78::Roots7.size()==115 && system78::Roots8.size()==286,
              "System 7/8 rooted tree counts must be exact");
} // namespace v26bridge

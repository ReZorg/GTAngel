// GTAngel v25 / DTE APL253: locally sealed, read-only cognition ingress.
// Included after v15render, v19game, and v20cns; no external producer API.
// In-process provenance is not cryptographic attestation against hostile injected code.
#include <cstring>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <optional>
#include <type_traits>
struct GTAngelAGI;
#include "dte_catalogue_253.hpp"
namespace v25dte {
using Frame = v15render::HighResFramebuffer;
static_assert(Frame::Width==768 && Frame::Height==768,"VRD sensor must remain 768x768");

// These extra predicates require a separately measured, typed record. The game
// does not currently measure all of them, so its production Sequence-27 view
// supplies no such record and those effects fail closed.
struct Sequence27Evidence {
    double functionImportance{-1}, perceptualLoadVariance{-1};
    double interactionAspectRatio{-1}; bool continuousQuest{false};
    int socialFacingWings{-1}, subscribedTopTenPercentWings{-1};
    uint64_t originBytes{0}, crossWriteBytes{0}; bool metadataOnly{false};
    double donorConfidence{-1}; uint64_t borrowedBytes{0}, donorWorkingSetBytes{0};
    uint64_t transitionCapacity{0}, peakMessageBytes{0}; int snapshotRetainedTicks{-1};
    double activeTaskIntrusion{-1}, routeAffordance{-1}, bestNeighbourAffordance{-1};
    bool stateEdgeLocus{false};
};
inline bool sequence27Guard(int id,const Sequence27Evidence& e) {
    switch(id) {
      case 190: return e.functionImportance>0.5 && e.functionImportance<=1.0 &&
                       e.perceptualLoadVariance>=0.2 && e.perceptualLoadVariance<=1.0;
      case 191: return e.continuousQuest && e.interactionAspectRatio>=1.0 &&
                       e.interactionAspectRatio<=2.0;
      case 192: return e.socialFacingWings>0 &&
                       e.subscribedTopTenPercentWings>=e.socialFacingWings;
      case 193: return e.metadataOnly && e.originBytes>0 && e.crossWriteBytes>0 &&
                       e.crossWriteBytes<=e.originBytes/5;
      case 194: return e.donorConfidence>0.7 && e.donorConfidence<=1.0 &&
                       e.donorWorkingSetBytes>0 && e.borrowedBytes>0 &&
                       e.borrowedBytes<e.donorWorkingSetBytes/20;
      case 195: return e.peakMessageBytes>0 && e.peakMessageBytes<=UINT64_MAX/3 &&
                       e.transitionCapacity>=3*e.peakMessageBytes &&
                       e.snapshotRetainedTicks>=30;
      case 196: return e.stateEdgeLocus && e.activeTaskIntrusion>=0.0 &&
                       e.activeTaskIntrusion<0.15 && e.routeAffordance>=0.0 &&
                       e.routeAffordance<=1.0 && e.bestNeighbourAffordance>=0.0 &&
                       e.bestNeighbourAffordance<=1.0 && e.routeAffordance>=e.bestNeighbourAffordance;
    }
    return false;
}

class LocalVisualIngress;
class Sample final {
    friend class LocalVisualIngress;
    uint64_t sequence_{0}, hash_{0}, capturedMs_{0}, worldVersion_{0}, bodySequence_{0};
    uint32_t goalPixels_{0}, goalDepthPixels_{0}, npcPixels_{0};
    Sample(uint64_t s,uint64_t h,uint64_t t,uint64_t v,uint64_t b,
           uint32_t g,uint32_t gd,uint32_t npc)
        :sequence_(s),hash_(h),capturedMs_(t),worldVersion_(v),bodySequence_(b),
         goalPixels_(g),goalDepthPixels_(gd),npcPixels_(npc) {}
public:
    Sample()=delete;
    uint64_t sequence() const {return sequence_;}
    uint64_t hash() const {return hash_;}
    uint64_t worldVersion() const {return worldVersion_;}
    uint32_t goalPixels() const {return goalPixels_;}
    uint32_t npcPixels() const {return npcPixels_;}
};
static_assert(!std::is_default_constructible<Sample>::value,"no forgeable default VRD sample");

class LocalVisualIngress final {
    friend struct ::GTAngelAGI; // only renderer-owning native game may mint samples
    friend class EchoCommons;
    const Frame* const boundFrame_;
    uint64_t lastIssued_{0};
    explicit LocalVisualIngress(const Frame& f):boundFrame_(&f) {}
    static uint64_t digest(const Frame& f,uint32_t& goal,uint32_t& goalDepth,uint32_t& npc) {
        uint64_t h=1469598103934665603ULL;
        goal=goalDepth=npc=0;
        for(size_t i=0;i<(size_t)Frame::PixelCount;++i) {
            const auto id=f.objectId[i];
            if(id==v15render::ObjectGoal) {++goal;if(std::isfinite(f.depth[i]) && f.depth[i]<1e20f) ++goalDepth;}
            if(id==v15render::ObjectNpc) ++npc;
            uint32_t depthBits=0; std::memcpy(&depthBits,&f.depth[i],sizeof(depthBits));
            h=(h^f.colour[i])*1099511628211ULL;
            h=(h^depthBits)*1099511628211ULL;
            h=(h^id)*1099511628211ULL;
        }
        return h;
    }
    std::optional<Sample> capture(uint64_t nowMs,const v20cns::Body& body,
                                  const v20cns::NicheHistory& niche) {
        const Frame& f=*boundFrame_;
        if(f.sequence==0 || f.sequence<=lastIssued_ ||
           f.colour.size()!=Frame::PixelCount || f.depth.size()!=Frame::PixelCount ||
           f.objectId.size()!=Frame::PixelCount || niche.worldVersion<1 || niche.worldVersion>2 ||
           !v20cns::finitePose(body.position) || !v20cns::finite(body.heading)) return std::nullopt;
        uint32_t g=0,d=0,n=0;
        const auto h=digest(f,g,d,n);
        if(g>0 && d!=g) return std::nullopt;
        lastIssued_=f.sequence;
        return Sample(f.sequence,h,nowMs,niche.worldVersion,body.sequence,g,d,n);
    }
    bool current(const Sample& s,uint64_t nowMs,const v20cns::Body& body,
                 const v20cns::NicheHistory& niche,uint32_t ttlMs=150) const {
        if(nowMs<s.capturedMs_ || nowMs-s.capturedMs_>ttlMs ||
           boundFrame_->sequence!=s.sequence_ || body.sequence!=s.bodySequence_ ||
           niche.worldVersion!=s.worldVersion_ ||
           boundFrame_->colour.size()!=Frame::PixelCount ||
           boundFrame_->depth.size()!=Frame::PixelCount ||
           boundFrame_->objectId.size()!=Frame::PixelCount) return false;
        uint32_t g=0,d=0,n=0;
        return digest(*boundFrame_,g,d,n)==s.hash_ && g==s.goalPixels_ &&
               d==s.goalDepthPixels_ && n==s.npcPixels_;
    }
    void reset() {lastIssued_=0;}
public:
    LocalVisualIngress()=delete;
    LocalVisualIngress(const LocalVisualIngress&)=delete;
    LocalVisualIngress& operator=(const LocalVisualIngress&)=delete;
};

struct EchoCandidate {uint16_t pattern{0};uint64_t frame{0},worldVersion{0},hash{0},expiryMs{0};};
class EchoCommons final {
    EchoCandidate candidate_{};
    uint64_t lastScanned_{0}; unsigned matched_{0},blockedSequence27_{0};
public:
    void reset() {candidate_={};lastScanned_=0;matched_=blockedSequence27_=0;}
    unsigned matched() const {return matched_;}
    unsigned blockedSequence27() const {return blockedSequence27_;}
    EchoCandidate candidate() const {return candidate_;} // metadata only, not a commit token
    bool scan(LocalVisualIngress& ingress,const Sample& s,uint64_t nowMs,
              const v20cns::Body& body,const v20cns::NicheHistory& niche,
              const v19game::GameplayLoop& game,const avatar3d::GameWorld& world) {
        candidate_={}; matched_=blockedSequence27_=0;
        if(s.sequence()<=lastScanned_ || !ingress.current(s,nowMs,body,niche)) return false;
        lastScanned_=s.sequence();
        // Only measured input channels are present; missing channels are UNKNOWN,
        // not zero. Unknown or unimplemented guards never grant motor/world power.
        auto measured=[&](const char* signal,double& value) {
            if(std::strcmp(signal,"visual_salience")==0) { value=std::min(1.0,(double)s.goalPixels()/100.0);return true; }
            if(std::strcmp(signal,"perceptual_confidence")==0) {value=s.goalPixels()>20?0.85:0.0;return true;}
            if(std::strcmp(signal,"collision_risk")==0) {value=body.contact?1.0:0.0;return true;}
            if(std::strcmp(signal,"world_budget")==0) {value=niche.spent==0?1.0:0.0;return true;}
            if(std::strcmp(signal,"niche_need")==0) {value=game.canRestore(body.position,world)?1.0:0.0;return true;}
            return false;
        };
        for(const auto& rule:v25rules::Catalogue) {
            double value=0;
            if(!measured(rule.signal,value)) continue;
            if(rule.lessEqual?value>rule.threshold:value<rule.threshold) continue;
            if(rule.id>=190 && rule.id<=196) {
                // No production social/write/route/session evidence supplied.
                ++blockedSequence27_;continue;
            }
            ++matched_; // counted as advisory match, NEVER an executed English guard
            if(rule.id==160 && niche.worldVersion==1 && niche.spent==0 &&
               !world.beaconRestored && std::fabs(body.speed)<=0.35f && !body.occupied &&
               s.goalPixels()>20 && game.canRestore(body.position,world)) {
                // APL160 edge-affordance specialization: a one-frame, 100ms
                // human-confirmable niche *proposal*, not a world mutation.
                candidate_={160,s.sequence(),niche.worldVersion,s.hash(),nowMs+100};
            }
        }
        return candidate_.pattern==160;
    }
    bool canConfirm(LocalVisualIngress& ingress,const Sample& s,uint64_t nowMs,
                    const v20cns::Body& body,const v20cns::NicheHistory& niche,
                    const v19game::GameplayLoop& game,const avatar3d::GameWorld& world) const {
        return candidate_.pattern==160 && candidate_.frame==s.sequence() &&
               candidate_.worldVersion==s.worldVersion() && candidate_.hash==s.hash() &&
               nowMs<=candidate_.expiryMs && !body.occupied && !world.beaconRestored &&
               game.canRestore(body.position,world) && ingress.current(s,nowMs,body,niche,100);
    }
    void clear() {candidate_={};}
};
} // namespace v25dte

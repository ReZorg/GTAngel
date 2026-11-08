struct GTAngelAGI;
namespace v20cns {
using avatar3d::Vec3;

static float distance2(Vec3 a, Vec3 b) {
    const float x = a.x-b.x, z = a.z-b.z;
    return x*x+z*z;
}
static bool finite(float x) { return std::isfinite(x); }
static bool finitePose(Vec3 p) { return finite(p.x)&&finite(p.y)&&finite(p.z); }

enum class Source : uint8_t { HumanLocal, Neural, Route, Script, Replay, AgentZero, Remote, DteLocal };
// Inserted within v20cns; bounded on-foot DTE authorization, not a world capability.
class DteLocalMotorSeal final {
    friend struct ::GTAngelAGI;
    friend class SpinalReflex;
    uint64_t frame_,digest_,issuedMs_,bodySequence_,worldVersion_;
    float forward_,turn_;
    DteLocalMotorSeal(uint64_t f,uint64_t d,uint64_t t,uint64_t b,uint64_t w,float forward,float turn)
        :frame_(f),digest_(d),issuedMs_(t),bodySequence_(b),worldVersion_(w),forward_(forward),turn_(turn){}
public:
    DteLocalMotorSeal()=delete;
};

struct Envelope { uint64_t sequence{0}, frame{0}, monotonicMs{0}; uint32_t ttlMs{100}; float confidence{1}; };
struct MotorProposal {
    Envelope evidence;
    Source source{Source::HumanLocal};
    float forward{0}, turn{0}, uncertainty{0}, collisionRisk{0};
    bool ethicsApproved{true};
    std::optional<DteLocalMotorSeal> localDteSeal;
};
struct Body {
    Vec3 position{0,0,0}; float heading{0}, speed{0}, steering{0}, wheelPhase{0};
    Vec3 cartPosition{1.5f,0,0};
    float cartHeading{0};
    bool occupied{false}, contact{false}; int gear{0}; uint64_t sequence{0};
};
struct Context {
    uint64_t frame{0}, nowMs{0}; float dt{0.05f};
    bool localFrameValid{true}; bool emergencyStop{false};
    uint64_t visualDigest{0}, worldVersion{0};
};
// Commit cannot be made by a proposal source. A token is a one-shot capability
// consumed by the effector, not a scalar stored in any public input packet.
class MotorCommit {
    friend class SpinalReflex;
    friend class BodyEffector;
    float forward_, turn_;
    uint64_t sequence_, nonce_;
    MotorCommit(float f, float t, uint64_t s, uint64_t n)
        : forward_(f), turn_(t), sequence_(s), nonce_(n) {}
public:
    MotorCommit() = delete;
};

class BodyEffector {
    friend class SpinalReflex;
    Body body_{};
    uint64_t nextNonce_{1};
    static bool clear(Vec3 p, float radius, const avatar3d::GameWorld& w) {
        if (!finitePose(p) || fabsf(p.x)>w.worldSize*0.5f-radius ||
            fabsf(p.z)>w.worldSize*0.5f-radius) return false;
        for (const auto& pillar : w.pillars)
            if (distance2(p,pillar.pos)<(radius+pillar.radius)*(radius+pillar.radius)) return false;
        for (const auto& b : w.buildings)
            if (std::fabs(p.x-b.pos.x)<radius+b.w*0.5f &&
                std::fabs(p.z-b.pos.z)<radius+b.d*0.5f) return false;
        if (distance2(p,w.npcPos)<(radius+0.55f)*(radius+0.55f)) return false;
        if (radius>0.5f &&
            (distance2(p,w.goal)<(radius+0.35f)*(radius+0.35f) ||
             (w.questShardVisible && distance2(p,w.questShard)<(radius+0.35f)*(radius+0.35f)))) return false;
        return true;
    }
    static bool swept(Vec3 a, Vec3 b, float radius, const avatar3d::GameWorld& w) {
        const float len = sqrtf(distance2(a,b));
        const int steps = std::max(1,(int)ceilf(len/0.10f));
        for (int i=1;i<=steps;++i) {
            float t=(float)i/(float)steps;
            if (!clear({a.x+(b.x-a.x)*t,0,a.z+(b.z-a.z)*t},radius,w)) return false;
        }
        return true;
    }
    bool consume(const MotorCommit& c, const Context& ctx, const avatar3d::GameWorld& world) {
        if (c.nonce_!=nextNonce_ || c.sequence_<=body_.sequence) return false;
        ++nextNonce_; // never replay a commit, even on collision
        Body next=body_;
        next.contact=false;
        if (next.occupied) {
            // Signed throttle/brake transition: reverse requires first braking to rest.
            const float demand=std::clamp(c.forward_,-1.0f,1.0f);
            float acceleration=0;
            if (fabsf(demand)<0.02f) acceleration=-next.speed*1.6f;
            else if (next.speed*demand < -0.02f) acceleration=-std::copysign(6.0f,next.speed);
            else acceleration=demand*3.2f-next.speed*0.32f;
            next.speed=std::clamp(next.speed+acceleration*ctx.dt,-3.0f,7.0f);
            if (fabsf(next.speed)<0.025f) next.speed=0;
            const float limit=0.55f/(1.0f+fabsf(next.speed)*0.12f);
            next.steering=std::clamp(c.turn_,-1.0f,1.0f)*limit;
            next.heading+=next.speed/1.65f*tanf(next.steering)*ctx.dt;
            next.gear=next.speed>0.025f ? 1 : next.speed< -0.025f ? -1 : 0;
            next.wheelPhase+=next.speed*ctx.dt/0.32f;
        } else {
            next.heading+=std::clamp(c.turn_,-1.0f,1.0f)*ctx.dt*2.0f;
            next.speed=std::clamp(c.forward_,-1.0f,1.0f)*2.0f;
            next.steering=0; next.gear=0;
        }
        Vec3 target=next.position+Vec3{sinf(next.heading)*next.speed*ctx.dt,0,
                                         cosf(next.heading)*next.speed*ctx.dt};
        if (!swept(next.position,target,next.occupied ? 0.76f : 0.25f,world)) {
            next.contact=true;
            if (!next.occupied) {
                // After rejecting the full swept motion, try independently
                // swept axes. This is *not* penetration recovery or a bypass;
                // every candidate is checked against the same local geometry.
                const Vec3 before=next.position;
                const Vec3 alongX{target.x,0,before.z};
                const Vec3 alongZ{before.x,0,target.z};
                const bool xClear=swept(before,alongX,0.25f,world);
                const bool zClear=swept(before,alongZ,0.25f,world);
                const float dx=target.x-before.x,dz=target.z-before.z;
                if (xClear && (!zClear || dx*dx>=dz*dz)) next.position=alongX;
                else if (zClear) next.position=alongZ;
                else if (xClear) next.position=alongX;
                // LW35: if axis-aligned slides both clipped a curved pillar,
                // test deflected tangential slide angles (all verified by swept()).
                if (distance2(before,next.position)<1e-10f) {
                    const float stepLen = std::sqrt(dx*dx + dz*dz) * 0.80f;
                    for (float ang : {0.52f, -0.52f, 0.95f, -0.95f, 1.35f, -1.35f}) {
                        const float hSlide = next.heading + ang;
                        const Vec3 cand{before.x + sinf(hSlide)*stepLen, 0.0f, before.z + cosf(hSlide)*stepLen};
                        if (swept(before, cand, 0.25f, world)) {
                            next.position = cand;
                            break;
                        }
                    }
                }
                if (distance2(before,next.position)<1e-10f) next.speed=0;
            } else { next.speed=0; next.gear=0; }
        } else next.position=target;
        if (next.occupied) { next.cartPosition=next.position; next.cartHeading=next.heading; }
        next.sequence=c.sequence_;
        body_=next;
        return true;
    }
public:
    const Body& state() const { return body_; }
    void reset() { body_=Body{}; nextNonce_=1; }
    // Only a bounded local enter/exit interaction can switch body mode.
    bool toggleCart(const avatar3d::GameWorld& world, Vec3 cartPosition) {
        if (!finitePose(cartPosition) || fabsf(body_.speed)>0.35f) return false;
        if (body_.occupied) {
            const Vec3 side{body_.position.x+cosf(body_.heading)*1.35f,0,
                            body_.position.z-sinf(body_.heading)*1.35f};
            if (!clear(side,0.25f,world) || distance2(side,world.goal)<0.5f) return false;
            body_.cartPosition=body_.position; body_.cartHeading=body_.heading;
            body_.position=side; body_.occupied=false;
        } else {
            if (distance2(cartPosition,body_.cartPosition)>0.01f ||
                distance2(body_.position,body_.cartPosition)>2.4f*2.4f ||
                !clear(body_.cartPosition,0.76f,world)) return false;
            body_.position=body_.cartPosition; body_.heading=body_.cartHeading;
            body_.occupied=true;
        }
        body_.speed=0; body_.gear=0; body_.contact=false;
        ++body_.sequence; ++nextNonce_;
        return true;
    }
    // Validated load only. Never exposed to network, policy, script, or proposal adapters.
    bool restoreLocal(const Body& b, const avatar3d::GameWorld& world) {
        if (!finitePose(b.position)||!finitePose(b.cartPosition)||!finite(b.heading)||
            !finite(b.cartHeading)||fabsf(b.cartHeading)>100000.0f||!finite(b.speed)||
            !finite(b.steering)||!finite(b.wheelPhase)||fabsf(b.speed)>7.01f||
            fabsf(b.heading)>100000.0f||fabsf(b.steering)>0.56f||
            !clear(b.position,b.occupied?0.76f:0.25f,world) ||
            !clear(b.cartPosition,0.76f,world) ||
            (b.occupied && distance2(b.position,b.cartPosition)>0.01f)) return false;
        body_=b; ++nextNonce_; return true;
    }
};

class SpinalReflex {
    uint64_t lastSequence_{0};
public:
    enum class Verdict { Accepted, Invalid, Stale, Unsafe, Untrusted, Collision, Replay };
    Verdict submit(const MotorProposal& p, const Context& ctx, BodyEffector& plant,
                   const avatar3d::GameWorld& world) {
        if (!finite(p.forward)||!finite(p.turn)||!finite(p.uncertainty)||
            !finite(p.collisionRisk)||!finite(p.evidence.confidence)||
            fabsf(p.forward)>1||fabsf(p.turn)>1||p.evidence.confidence<0||
            p.evidence.confidence>1||p.uncertainty<0||p.collisionRisk<0||
            p.evidence.ttlMs>250||ctx.dt<=0||ctx.dt>0.10f) return Verdict::Invalid;
        if (p.evidence.sequence<=lastSequence_) return Verdict::Replay;
        if (p.evidence.monotonicMs>ctx.nowMs || ctx.nowMs-p.evidence.monotonicMs>p.evidence.ttlMs ||
            p.evidence.frame!=ctx.frame || !ctx.localFrameValid) return Verdict::Stale;
        if (ctx.emergencyStop || !p.ethicsApproved || p.collisionRisk>0.80f ||
            p.uncertainty>0.60f || p.evidence.confidence<0.35f) return Verdict::Unsafe;
        // Only a CURRENT renderer-owned seal qualifies the bounded DTE operator.
        // Plain Neural/Route/Script/Replay/AgentZero/Remote sources remain rejected.
        if(p.source==Source::DteLocal) {
            if(!p.localDteSeal || plant.state().occupied) return Verdict::Untrusted;
            const auto& seal=*p.localDteSeal;
            if(seal.frame_!=ctx.frame || seal.digest_==0 || seal.digest_!=ctx.visualDigest ||
               seal.bodySequence_!=plant.state().sequence || seal.worldVersion_!=ctx.worldVersion ||
               seal.issuedMs_>ctx.nowMs || ctx.nowMs-seal.issuedMs_>100 ||
               seal.forward_!=p.forward || seal.turn_!=p.turn ||
               fabsf(p.forward)>.5f || fabsf(p.turn)>.6f) return Verdict::Stale;
        } else if(p.source!=Source::HumanLocal) return Verdict::Untrusted;
        lastSequence_=p.evidence.sequence;
        const MotorCommit commit(p.forward,p.turn,p.evidence.sequence,plant.nextNonce_);
        return plant.consume(commit,ctx,world) ? (plant.state().contact?Verdict::Collision:Verdict::Accepted)
                                                : Verdict::Invalid;
    }
    void reset() { lastSequence_=0; }
};

class MotorArbiter {
public:
    static Source choose(bool humanActive,bool routeActive,bool learnedAccepted) {
        if (humanActive) return Source::HumanLocal;
        if (routeActive) return Source::Route;
        if (learnedAccepted) return Source::Neural;
        return Source::HumanLocal; // bounded local neutral/coast braking
    }
};

struct NicheProposal {
    Envelope evidence;
    Source source{Source::HumanLocal};
    uint64_t expectedWorldVersion{0};
    Vec3 locus{}; float radius{0.45f}; uint32_t cost{1};
    bool inverseAvailable{true}, questGrounded{false};
};
class NicheCommit {
    friend class WorldGovernance;
    friend class WorldEffector;
    uint64_t version_, nonce_;
    NicheCommit(uint64_t v,uint64_t n):version_(v),nonce_(n){}
public:
    NicheCommit()=delete;
};
class WorldEffector {
    friend class WorldGovernance;
    uint64_t nextNonce_{1};
    bool apply(const NicheCommit& c, v19game::GameplayLoop& game, avatar3d::GameWorld& w) {
        if (c.nonce_!=nextNonce_) return false;
        ++nextNonce_;
        return game.governedRepairBeacon(w);
    }
public:
    void reset() { nextNonce_=1; }
};
struct NicheHistory {
    uint64_t worldVersion{1};
    uint32_t spent{0};
    bool reobserved{false}, rolledBack{false};
};
class WorldGovernance {
    NicheHistory history_{};
    uint64_t lastSequence_{0};
public:
    enum class Verdict { Accepted, Invalid, Stale, Unsafe, Untrusted, NoInverse,
                         Budget, Protected, Quest, ReobservationFailed };
    const NicheHistory& history() const { return history_; }
    void reset() { history_=NicheHistory{}; lastSequence_=0; }
    bool restoreLocal(const NicheHistory& h, const v19game::GameplayLoop& game) {
        if (h.worldVersion<1 || h.worldVersion>2 || h.spent>1 || h.rolledBack ||
            (game.quest.beaconRestored != (h.worldVersion==2)) ||
            h.spent!=(h.worldVersion==2?1u:0u) ||
            (h.worldVersion==2 && !h.reobserved)) return false;
        history_=h; lastSequence_=0; return true;
    }
    template<class Observe>
    Verdict submit(const NicheProposal& p, const Context& ctx, Vec3 player,
                   v19game::GameplayLoop& game, avatar3d::GameWorld& w,
                   WorldEffector& effector, Observe observe) {
        if (!finitePose(p.locus)||!finite(p.radius)||!finite(p.evidence.confidence)||
            p.radius<0.20f||p.radius>0.80f||p.evidence.confidence<0.35f||
            p.evidence.ttlMs>250||p.cost==0) return Verdict::Invalid;
        if (p.evidence.monotonicMs>ctx.nowMs||ctx.nowMs-p.evidence.monotonicMs>p.evidence.ttlMs||
            p.evidence.frame!=ctx.frame||!ctx.localFrameValid||
            p.expectedWorldVersion!=history_.worldVersion) return Verdict::Stale;
        if (p.evidence.sequence<=lastSequence_) return Verdict::Stale;
        if (ctx.emergencyStop) return Verdict::Unsafe;
        if (p.source!=Source::HumanLocal) return Verdict::Untrusted;
        if (!p.inverseAvailable) return Verdict::NoInverse;
        if (p.cost>1-history_.spent) return Verdict::Budget;
        if (distance2(p.locus,w.goal)>0.25f ||
            distance2(p.locus,w.npcPos)<(p.radius+1.0f)*(p.radius+1.0f) ||
            distance2(p.locus,w.questShard)<(p.radius+0.5f)*(p.radius+0.5f) ||
            distance2(p.locus,player)>3.0f*3.0f) return Verdict::Protected;
        if (!p.questGrounded||!game.canRestore(player,w)) return Verdict::Quest;
        lastSequence_=p.evidence.sequence;
        // The only world transform in v20 is the quest-authored beacon repair.
        // Snapshot includes the full quest and NPC memories, not only the visual bit.
        const auto priorQuest=game.quest;
        const auto priorNpc=game.ariadne;
        const auto priorWorld=w;
        const auto priorHistory=history_;
        const NicheCommit c(history_.worldVersion,effector.nextNonce_);
        if (!effector.apply(c,game,w)) return Verdict::Quest;
        ++history_.worldVersion; history_.spent+=p.cost;
        history_.reobserved=false;
        bool verified=false;
        try { verified=observe(priorWorld,w,history_.worldVersion); }
        catch (...) { verified=false; }
        if (!verified) {
            game.quest=priorQuest; game.ariadne=priorNpc; w=priorWorld;
            history_=priorHistory; history_.rolledBack=true;
            return Verdict::ReobservationFailed;
        }
        history_.reobserved=true; history_.rolledBack=false;
        return Verdict::Accepted;
    }
};

class Runtime {
    SpinalReflex spinal_;
    BodyEffector plant_;
    WorldGovernance governance_;
    WorldEffector worldEffector_;
    uint64_t sequence_{0};
public:
    const Body& body() const { return plant_.state(); }
    const NicheHistory& niche() const { return governance_.history(); }
    void reset() { spinal_.reset(); plant_.reset(); governance_.reset(); worldEffector_.reset(); sequence_=0; }
    SpinalReflex::Verdict motor(Source source,float forward,float turn,
                                float uncertainty,float risk,bool ethics,
                                const Context& ctx,const avatar3d::GameWorld& world) {
        MotorProposal p; p.evidence={++sequence_,ctx.frame,ctx.nowMs,100,1.0f};
        p.source=source; p.forward=forward; p.turn=turn;
        p.uncertainty=uncertainty; p.collisionRisk=risk; p.ethicsApproved=ethics;
        return spinal_.submit(p,ctx,plant_,world);
    }
    SpinalReflex::Verdict motorDte(const DteLocalMotorSeal& seal,float forward,float turn,
            float uncertainty,float risk,const Context& ctx,const avatar3d::GameWorld& world) {
        MotorProposal p; p.evidence={++sequence_,ctx.frame,ctx.nowMs,100,1.0f};
        p.source=Source::DteLocal;p.forward=forward;p.turn=turn;p.uncertainty=uncertainty;
        p.collisionRisk=risk;p.ethicsApproved=true;p.localDteSeal=seal;
        return spinal_.submit(p,ctx,plant_,world); // SRD alone mints MOTOR_COMMIT
    }
    bool toggleCart(const avatar3d::GameWorld& w, Vec3 parked) {
        if (!plant_.toggleCart(w,parked)) return false;
        sequence_=std::max(sequence_,plant_.state().sequence);
        return true;
    }
    template<class Observe>
    WorldGovernance::Verdict repair(Vec3 player, v19game::GameplayLoop& game,
                                    avatar3d::GameWorld& world, const Context& ctx,
                                    Observe observe) {
        if (plant_.state().occupied) return WorldGovernance::Verdict::Quest;
        NicheProposal p; p.evidence={++sequence_,ctx.frame,ctx.nowMs,100,1.0f};
        p.source=Source::HumanLocal; p.expectedWorldVersion=governance_.history().worldVersion;
        p.locus=world.goal; p.questGrounded=game.canRestore(player,world);
        return governance_.submit(p,ctx,player,game,world,worldEffector_,observe);
    }
    bool restoreLocal(const Body& b,const NicheHistory& h,
                      const v19game::GameplayLoop& game,const avatar3d::GameWorld& w) {
        BodyEffector trial;
        WorldGovernance govern;
        if (!trial.restoreLocal(b,w)||!govern.restoreLocal(h,game)) return false;
        plant_=trial; governance_=govern; spinal_.reset(); worldEffector_.reset(); sequence_=b.sequence;
        return true;
    }
};
} // namespace v20cns

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

// GTAngel v26: local, advisory System 7 (8-node) -> System 8 (9-node) tree lens.
// The incidence arrays are generated from canonical trees, not inferred by an LLM.
// This module is deliberately outside SRD/MOTOR_COMMIT and WGR/NICHE_COMMIT.
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




// Single-file v20 atomic JSON save. Strict bounded primitive parser; no external JSON library.

#include <optional>
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
        for (const auto& lamp : w.props) {
            const float dx=p.x-lamp.pos.x, dz=p.z-lamp.pos.z;
            if (dx*dx+dz*dz<(radius+0.20f)*(radius+0.20f)) return false;
        }
        return false;
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
        // LW55: approach-only mutual barriers for dynamic bodies — motion INTO a
        // vehicle/ped is vetoed, but any motion AWAY (escape when overlapping)
        // is always allowed, so the avatar can never be caged by a barrier.
        // Penetration-only (approach semantics): veto motion that CLOSES INTO a
        // body core — never a wide standoff — so the avatar can still reach the
        // coupe door, brush past traffic, and always escape an overlap.
        const float dB2 = distance2(b, w.vehiclePos), dA2 = distance2(a, w.vehiclePos);
        if (dB2 < (radius+0.95f)*(radius+0.95f) && dB2 < dA2) return false;
        for (const auto& v : w.traffic) {
            if (v.orbitR>0.0f) continue;
            const float dBt=distance2(b,v.pos), dAt=distance2(a,v.pos);
            if (dBt < (radius+1.45f)*(radius+1.45f) && dBt < dAt) return false;
        }
        const Vec3 ambP{w.ambX,0.0f,-1.4f}, busP{-1.4f,0.0f,w.busZ};
        if (distance2(b,ambP)<(radius+1.6f)*(radius+1.6f) && distance2(b,ambP)<distance2(a,ambP)) return false;
        if (distance2(b,busP)<(radius+1.8f)*(radius+1.8f) && distance2(b,busP)<distance2(a,busP)) return false;
        for (const auto& wk : w.walkers) {
            if (distance2(b,wk.pos)<(radius+0.32f)*(radius+0.32f) &&
                distance2(b,wk.pos)<distance2(a,wk.pos)) return false;
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
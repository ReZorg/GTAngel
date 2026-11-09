#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <type_traits>
using SHORT=short;
static constexpr int MOVEFILE_REPLACE_EXISTING=1, MOVEFILE_WRITE_THROUGH=8;
static SHORT GetAsyncKeyState(int) { return 0; }
static int DeleteFileA(const char* p) { return std::remove(p)==0; }
static int MoveFileExA(const char* from,const char* to,int) {
    std::remove(to); return std::rename(from,to)==0;
}
namespace avatar3d {
struct Vec3 { float x{0},y{0},z{0}; };
static Vec3 operator+(Vec3 a,Vec3 b) { return {a.x+b.x,a.y+b.y,a.z+b.z}; }
static Vec3 operator-(Vec3 a,Vec3 b) { return {a.x-b.x,a.y-b.y,a.z-b.z}; }
static Vec3 operator*(Vec3 a,float s) { return {a.x*s,a.y*s,a.z*s}; }
struct GameWorld {
    struct Pillar { Vec3 pos; float radius,height; };
    std::vector<Pillar> pillars;
    Vec3 goal{8,0,8},npcPos{-4,0,4},questShard{-6,0,-5};
    float npcHeading{2.35f},npcSpeed{0},worldSize{128};
    bool questShardVisible{true},beaconRestored{false},questComplete{false};
};
}
#include "gtangel_agi_v20_gameplay.hpp"
#include "gtangel_agi_v23_cns.hpp"
#include "gtangel_agi_v20_persistence.hpp"
static_assert(!std::is_default_constructible<v20cns::MotorCommit>::value,"SRD-only commit");
static_assert(!std::is_default_constructible<v20cns::NicheCommit>::value,"WGR-only commit");
static int count=0, failures=0;
static void check(bool ok,const char* label) {
    ++count;
    if (!ok) { ++failures; std::cerr << "FAIL " << count << ": " << label << '\n'; }
}
static v20cns::Context ctx(uint64_t frame=1,uint64_t now=1000) {
    return {frame,now,0.05f,true,false};
}
static v20cns::MotorProposal proposal(v20cns::Source source,uint64_t seq=1) {
    v20cns::MotorProposal p;
    p.source=source; p.evidence={seq,1,1000,100,1.0f}; p.forward=1;
    return p;
}
static void ready(v19game::GameplayLoop& g,avatar3d::GameWorld& w) {
    g.reset(w);
    const avatar3d::Vec3 near{-4,0,2.5f};
    g.interact(near,0,w); g.choose(1,w);
    g.interact(w.questShard,0,w);
}
int main() {
    using namespace v20cns;
    avatar3d::GameWorld world;
    BodyEffector plant;
    SpinalReflex spinal;
    check(MotorArbiter::choose(true,true,true)==Source::HumanLocal,
          "human local override precedes route and neural candidates");
    check(MotorArbiter::choose(false,true,true)==Source::Route,
          "route candidate is tagged, not silently promoted to human input");
    check(MotorArbiter::choose(false,false,true)==Source::Neural,
          "neural candidate remains tagged for spinal qualification");
    const Source all[]={Source::HumanLocal,Source::Neural,Source::Route,Source::Script,
                        Source::Replay,Source::AgentZero,Source::Remote};
    for (size_t i=0;i<7;++i) {
        auto p=proposal(all[i],i+1); p.collisionRisk=0.99f;
        auto before=plant.state();
        check(spinal.submit(p,ctx(),plant,world)==SpinalReflex::Verdict::Unsafe,
              "reflex pre-empts each motor source before body commit");
        check(plant.state().sequence==before.sequence && distance2(plant.state().position,before.position)==0,
              "rejected source cannot change body state");
    }
    uint64_t sequence=8;
    for (Source source : {Source::Neural,Source::Route,Source::Script,Source::Replay,
                          Source::AgentZero,Source::Remote}) {
        auto p=proposal(source,sequence++);
        check(spinal.submit(p,ctx(),plant,world)==SpinalReflex::Verdict::Untrusted,
              "automated source not qualified for v20 actuator authority");
        check(plant.state().sequence==0,"automated source has no effector write");
    }
    check(spinal.submit(proposal(Source::Remote,UINT64_MAX-1),ctx(),plant,world)==
          SpinalReflex::Verdict::Untrusted,"forged high motor sequence is rejected");
    auto invalid=proposal(Source::HumanLocal,sequence++); invalid.forward=INFINITY;
    check(spinal.submit(invalid,ctx(),plant,world)==SpinalReflex::Verdict::Invalid,
          "nonfinite control fails closed");
    auto stale=proposal(Source::HumanLocal,sequence++); stale.evidence.monotonicMs=800;
    check(spinal.submit(stale,ctx(),plant,world)==SpinalReflex::Verdict::Stale,
          "expired proposal fails closed");
    auto frame=proposal(Source::HumanLocal,sequence++); frame.evidence.frame=4;
    check(spinal.submit(frame,ctx(),plant,world)==SpinalReflex::Verdict::Stale,
          "stale visual frame fails closed");
    auto ethics=proposal(Source::HumanLocal,sequence++); ethics.ethicsApproved=false;
    check(spinal.submit(ethics,ctx(),plant,world)==SpinalReflex::Verdict::Unsafe,
          "ethics denial pre-empts human control");
    auto uncertain=proposal(Source::HumanLocal,sequence++); uncertain.uncertainty=0.8f;
    check(spinal.submit(uncertain,ctx(),plant,world)==SpinalReflex::Verdict::Unsafe,
          "uncertain motor command pre-empted");
    auto human=proposal(Source::HumanLocal,sequence++);
    check(spinal.submit(human,ctx(),plant,world)==SpinalReflex::Verdict::Accepted,
          "bounded local human proposal produces sole SRD commit");
    check(plant.state().sequence==human.evidence.sequence && plant.state().position.z>0,
          "human body motion is measured");
    check(spinal.submit(human,ctx(),plant,world)==SpinalReflex::Verdict::Replay,
          "committed proposal cannot be replayed");
    Context stop=ctx(); stop.emergencyStop=true;
    check(spinal.submit(proposal(Source::HumanLocal,sequence++),stop,plant,world)==
          SpinalReflex::Verdict::Unsafe,"emergency stop pre-empts human motor proposal");
    auto neutral=proposal(Source::HumanLocal,sequence++); neutral.forward=0;
    check(spinal.submit(neutral,ctx(),plant,world)==SpinalReflex::Verdict::Accepted,
          "local operator stops before boarding");
    check(!plant.toggleCart(world,{8,0,8}),"cart cannot be entered at distant beacon");
    check(plant.toggleCart(world,{1.5f,0,0}),"local on-foot entry into parked cart");
    check(plant.state().occupied,"body schema records cart occupancy");
    sequence=std::max(sequence,plant.state().sequence+1);
    float start=plant.state().position.z;
    for (int i=0;i<25;++i) {
        auto p=proposal(Source::HumanLocal,sequence++);
        p.evidence.frame=2+i; p.evidence.monotonicMs=1100+i*50;
        check(spinal.submit(p,ctx(2+i,1100+i*50),plant,world)==SpinalReflex::Verdict::Accepted,
              "deterministic vehicle tick accepted");
    }
    check(plant.state().position.z>start && plant.state().speed>0 && plant.state().gear==1,
          "vehicle accelerates with signed gear and measured displacement");
    auto brake=proposal(Source::HumanLocal,sequence++); brake.forward=-1;
    brake.evidence.frame=27; brake.evidence.monotonicMs=2400;
    float speed=plant.state().speed;
    spinal.submit(brake,ctx(27,2400),plant,world);
    check(plant.state().speed<speed && plant.state().speed>=0,"reverse intent first brakes forward gear");
    check(!plant.toggleCart(world,{1.5f,0,0}),"moving cart cannot be exited");
    // Standalone collision proof: sweep against a pillar while entering its footprint.
    BodyEffector collider; SpinalReflex reflex;
    check(collider.toggleCart(world,{1.5f,0,0}),"collision fixture begins in cart");
    world.pillars.push_back({{1.5f,0,1.8f},0.4f,2});
    bool hit=false;
    for (int i=1;i<50;++i) {
        auto p=proposal(Source::HumanLocal,i); p.evidence.frame=i;
        p.evidence.monotonicMs=1000+50*i;
        if (reflex.submit(p,ctx(i,1000+50*i),collider,world)==SpinalReflex::Verdict::Collision) {
            hit=true; break;
        }
    }
    check(hit && collider.state().contact,"swept local collision stops cart");
    check(distance2(collider.state().position,world.pillars.back().pos)>1.16f*1.16f,
          "cart footprint does not tunnel into pillar");
    world.pillars.clear();
    avatar3d::GameWorld parkWorld;
    v19game::GameplayLoop parkGame; parkGame.reset(parkWorld);
    Runtime park;
    check(park.toggleCart(parkWorld,{1.5f,0,0}),"runtime operator boards local cart");
    for (int i=1;i<=85;++i)
        park.motor(Source::HumanLocal,i<=10?1.0f:0.0f,0,0,0,true,
                   ctx(i,(uint64_t)(1000+i*50)),parkWorld);
    const auto parkedAt=park.body().cartPosition;
    check(park.toggleCart(parkWorld,parkedAt),"stationary cart exit accepted");
    check(distance2(park.body().cartPosition,parkedAt)==0 &&
          distance2(park.body().position,parkedAt)>1.0f,
          "exiting keeps cart parked while foot body moves to its side");
    check(v20save::write("v20_cart_save.json",parkGame,park),
          "atomic v2 save includes parked cart state");
    Runtime parkedLoad; v19game::GameplayLoop parkReload; avatar3d::GameWorld parkWorldReload;
    parkReload.reset(parkWorldReload);
    check(v20save::load("v20_cart_save.json","missing_v1.json",parkReload,
                        parkWorldReload,parkedLoad,[](const auto&){return false;}),
          "unrestored v2 cart load needs no beacon observation");
    check(distance2(parkedLoad.body().cartPosition,parkedAt)<0.001f &&
          !parkedLoad.body().occupied,"parked cart persists without teleporting to spawn");
    DeleteFileA("v20_cart_save.json");
    v19game::GameplayLoop game; ready(game,world);
    check(game.canRestore(world.goal,world),"quest grounds repair after shard recovery");
    check(game.interact(world.goal,0,world).find("requires local World Governance")!=std::string::npos &&
          !world.beaconRestored,"direct v19 interaction cannot bypass WGR");
    WorldGovernance governor; WorldEffector effector;
    auto niche=[&](uint64_t seq=1) {
        NicheProposal p; p.evidence={seq,1,1000,100,1.0f};
        p.expectedWorldVersion=governor.history().worldVersion;
        p.locus=world.goal; p.questGrounded=true; return p;
    };
    for (Source s:all) {
        auto p=niche((uint64_t)s+1); p.source=s; p.expectedWorldVersion=0;
        check(governor.submit(p,ctx(),world.goal,game,world,effector,
              [](auto&,auto&,auto){return true;})==WorldGovernance::Verdict::Stale,
              "stale version pre-empts every niche source");
        check(!world.beaconRestored,"stale niche proposal cannot mutate world");
    }
    uint64_t nseq=20;
    for (Source s:{Source::Neural,Source::Route,Source::Script,Source::Replay,
                   Source::AgentZero,Source::Remote}) {
        auto p=niche(nseq++); p.source=s;
        check(governor.submit(p,ctx(),world.goal,game,world,effector,
              [](auto&,auto&,auto){return true;})==WorldGovernance::Verdict::Untrusted,
              "non-human proposal cannot mint niche commit in v20");
    }
    auto poison=niche(UINT64_MAX-1); poison.source=Source::Remote;
    check(governor.submit(poison,ctx(),world.goal,game,world,effector,
          [](auto&,auto&,auto){return true;})==WorldGovernance::Verdict::Untrusted,
          "forged high niche sequence is rejected without poisoning the cursor");
    auto p=niche(nseq++); p.inverseAvailable=false;
    check(governor.submit(p,ctx(),world.goal,game,world,effector,
          [](auto&,auto&,auto){return true;})==WorldGovernance::Verdict::NoInverse,
          "missing snapshot/inverse is rejected");
    p=niche(nseq++); p.cost=2;
    check(governor.submit(p,ctx(),world.goal,game,world,effector,
          [](auto&,auto&,auto){return true;})==WorldGovernance::Verdict::Budget,
          "overbudget niche mutation rejected");
    p=niche(nseq++); p.locus=world.npcPos;
    check(governor.submit(p,ctx(),world.goal,game,world,effector,
          [](auto&,auto&,auto){return true;})==WorldGovernance::Verdict::Protected,
          "Ariadne protected-centre overlap rejected");
    p=niche(nseq++); p.questGrounded=false;
    check(governor.submit(p,ctx(),world.goal,game,world,effector,
          [](auto&,auto&,auto){return true;})==WorldGovernance::Verdict::Quest,
          "unearned repair is rejected");
    const auto beforeQuest=game.quest;
    const auto beforeNpc=game.ariadne;
    p=niche(nseq++);
    check(governor.submit(p,ctx(),world.goal,game,world,effector,
          [](auto&,auto&,auto){return false;})==WorldGovernance::Verdict::ReobservationFailed,
          "world commit without rendered evidence rolls back");
    check(!game.quest.beaconRestored && !world.beaconRestored &&
          game.quest.groundedEvents==beforeQuest.groundedEvents &&
          game.ariadne.disposition==beforeNpc.disposition &&
          governor.history().worldVersion==1 && governor.history().rolledBack,
          "rollback restores quest, NPC, world and version");
    p=niche(nseq++);
    check(governor.submit(p,ctx(),world.goal,game,world,effector,
          [](auto&,auto&,auto)->bool { throw 1; })==WorldGovernance::Verdict::ReobservationFailed &&
          !world.beaconRestored && governor.history().worldVersion==1,
          "throwing observer also rolls back without escaping authority path");
    p=niche(nseq++);
    check(governor.submit(p,ctx(),world.goal,game,world,effector,
          [](auto& old,auto& next,uint64_t v) {
            return !old.beaconRestored && next.beaconRestored && v==2;
          })==WorldGovernance::Verdict::Accepted,
          "WGR-only niche commit follows confirmed consequence");
    check(world.beaconRestored && game.quest.stage==v19game::QuestStage::ReturnToAriadne &&
          governor.history().worldVersion==2 && governor.history().reobserved,
          "accepted change increments version and advances original quest");
    p=niche(nseq++);
    check(governor.submit(p,ctx(),world.goal,game,world,effector,
          [](auto&,auto&,auto){return true;})==WorldGovernance::Verdict::Budget,
          "committed mutation exhausts v20 niche budget");
    game.interact({-4,0,2.5f},0,world); game.choose(1,world);
    check(game.quest.stage==v19game::QuestStage::Complete &&
          game.quest.hasResonanceSigil && world.questComplete,
          "full Ariadne reward remains attainable after WGR repair");
    auto state=game.quest;
    check(v20save::write("v20_test_save.json",game,Runtime{})==false,
          "save rejects unobserved CNS/quest mismatch");
    // Roundtrip a coherent complete v20 fixture via the same public runtime route.
    avatar3d::GameWorld world2; v19game::GameplayLoop game2; ready(game2,world2);
    Runtime cns;
    const auto accepted=cns.repair(world2.goal,game2,world2,ctx(),
         [](auto& a,auto& b,uint64_t v){return !a.beaconRestored && b.beaconRestored && v==2;});
    check(accepted==WorldGovernance::Verdict::Accepted,"runtime repair completes WGR transaction");
    check(v20save::write("v20_test_save.json",game2,cns),"v20 atomic save succeeds");
    v19game::GameplayLoop loaded; avatar3d::GameWorld loadedWorld;
    loaded.reset(loadedWorld); Runtime loadedCns;
    check(!v20save::load("v20_test_save.json","missing_v1.json",loaded,loadedWorld,loadedCns,
                         [](const auto&){return false;}) &&
          loaded.quest.stage==v19game::QuestStage::Dormant,
          "restored save cannot load when local visual re-observation fails");
    check(v20save::load("v20_test_save.json","missing_v1.json",loaded,loadedWorld,loadedCns,
                        [](const auto& w){return w.beaconRestored;}),
          "v20 state loads and validates atomically");
    check(loadedWorld.beaconRestored && loadedCns.niche().worldVersion==2 &&
          loaded.quest.stage==v19game::QuestStage::ReturnToAriadne,
          "v20 save retains quest and niche version");
    { std::ofstream out("v20_test_save.json",std::ios::trunc);
      out << "{\"version\":2,\"questStage\":4,\"beaconRestored\":1,\"bodyX\":NaN}"; }
    const auto snapshot=loaded.quest.stage;
    check(!v20save::load("v20_test_save.json","missing_v1.json",loaded,loadedWorld,loadedCns,
                         [](const auto&){return true;}) &&
          loaded.quest.stage==snapshot,"corrupt v2 load leaves live state unchanged");
    DeleteFileA("v20_test_save.json");
    // Real v19 schema and migration path: prepare quest, write old save, then load
    // through v20 without invoking the v19 lax parser.
    game2.quest.stage=v19game::QuestStage::SeekShard; game2.quest.hasSignalShard=false;
    game2.quest.shardVisible=true; game2.quest.beaconRestored=false;
    check(game2.save(),"original v1 JSON save prepared for migration");
    v19game::GameplayLoop migrated; avatar3d::GameWorld migratedWorld;
    migrated.reset(migratedWorld); Runtime migratedCns;
    check(v20save::load("v20_test_save.json",v19game::GameplayLoop::SavePath,
                       migrated,migratedWorld,migratedCns,[](const auto&){return true;}),
          "v19 save migrates to v20 defaults");
    check(migrated.quest.stage==v19game::QuestStage::SeekShard &&
          migratedCns.niche().worldVersion==1 && !migratedCns.body().occupied,
          "v1 migration preserves quest with non-occupied cart and version one");
    DeleteFileA(v19game::GameplayLoop::SavePath);
    std::cout << "{\"ok\":" << (failures?"false":"true")
              << ",\"checks\":"<<count<<",\"failures\":"<<failures<<"}\n";
    return failures?1:0;
}

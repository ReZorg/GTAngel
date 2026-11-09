#include "verify_v25_fixture.hpp"
#include <type_traits>
#include <optional>
#include <sstream>
#include <iomanip>
#include <limits>
static_assert(!std::is_default_constructible<v25dte::Sample>::value,"certificate cannot be default-minted");
static_assert(!std::is_constructible<v25dte::LocalVisualIngress,
                                   v15render::HighResFramebuffer&>::value,
              "unregistered producer cannot bind itself to VRD");
static_assert(sizeof(v25rules::Catalogue)/sizeof(v25rules::Rule)==253,
              "all 253 APL rules must be present");

// Test authority has the *same* renderer-owner type as the production app;
// foreign non-friend types cannot call capture/reset or fabricate a Sample.
struct GTAngelAGI {
    v15render::HighResRenderer renderer;
    v20cns::Runtime cns;
    v25dte::LocalVisualIngress ingress{renderer.framebuffer};
    std::optional<v25dte::Sample> capture(uint64_t t) {
        return ingress.capture(t,cns.body(),cns.niche());
    }
    bool current(const v25dte::Sample& s,uint64_t t) {
        return ingress.current(s,t,cns.body(),cns.niche());
    }
};
static int checks=0,failures=0;
static void ck(bool p,const char* message) {
    ++checks;
    if(!p) {++failures;std::cerr<<"FAIL "<<checks<<' '<<message<<'\n';}
}
static void testSequence27(){
    using v25dte::Sequence27Evidence;
    using v25dte::sequence27Guard;
    for(int i=190;i<=196;++i) ck(!sequence27Guard(i,{}),"missing Seq27 evidence fails closed");
    Sequence27Evidence e;
    e.functionImportance=.8; e.perceptualLoadVariance=.25;
    e.interactionAspectRatio=1.5;e.continuousQuest=true;
    e.socialFacingWings=2;e.subscribedTopTenPercentWings=2;
    e.originBytes=1000;e.crossWriteBytes=199;e.metadataOnly=true;
    e.donorConfidence=.8;e.borrowedBytes=40;e.donorWorkingSetBytes=1000;
    e.transitionCapacity=301;e.peakMessageBytes=100;e.snapshotRetainedTicks=30;
    e.stateEdgeLocus=true;e.activeTaskIntrusion=.1;
    e.routeAffordance=.9;e.bestNeighbourAffordance=.7;
    for(int i=190;i<=196;++i) ck(sequence27Guard(i,e),"typed Seq27 positive predicate");
    e.perceptualLoadVariance=.19;ck(!sequence27Guard(190,e),"APL190 importance and variance required");e.perceptualLoadVariance=.25;
    e.interactionAspectRatio=2.01;ck(!sequence27Guard(191,e),"APL191 session aspect bounded");e.interactionAspectRatio=1.5;
    e.subscribedTopTenPercentWings=1;ck(!sequence27Guard(192,e),"APL192 every social wing subscribed");e.subscribedTopTenPercentWings=2;
    e.metadataOnly=false;ck(!sequence27Guard(193,e),"APL193 excludes raw frame payload");e.metadataOnly=true;
    e.borrowedBytes=50;ck(!sequence27Guard(194,e),"APL194 strict five percent donor limit");e.borrowedBytes=40;
    e.snapshotRetainedTicks=29;ck(!sequence27Guard(195,e),"APL195 snapshots retained thirty ticks");e.snapshotRetainedTicks=30;
    e.stateEdgeLocus=false;ck(!sequence27Guard(196,e),"APL196 corner/edge location mandatory");e.stateEdgeLocus=true;
    e.bestNeighbourAffordance=-1;ck(!sequence27Guard(196,e),"APL196 missing neighbour fails closed");
    e.bestNeighbourAffordance=.7;e.peakMessageBytes=UINT64_MAX;
    ck(!sequence27Guard(195,e),"APL195 multiplication overflow fails closed");
    e.peakMessageBytes=100;e.borrowedBytes=UINT64_MAX;
    ck(!sequence27Guard(194,e),"APL194 multiplication overflow fails closed");
}

int main(){
    testSequence27();
    using namespace v25dte;
    avatar3d::GameWorld world;world.generate(1337,8);
    v19game::GameplayLoop game;game.reset(world);
    const auto nearAriadne=avatar3d::Vec3{-4,0,2.5f};
    game.interact(nearAriadne,0,world);game.choose(1,world);
    game.interact(world.questShard,0,world);
    ck(game.canRestore(world.goal,world),"fixture has a grounded shard/repair quest stage");
    GTAngelAGI host;
    v20cns::Body positioned;
    positioned.position=world.goal; positioned.sequence=1;
    ck(host.cns.restoreLocal(positioned,v20cns::NicheHistory{},game,world),
       "test-only validated player pose reaches the quest beacon");
    avatar3d::AvatarSkeleton avatar;avatar.pose(world.goal,0,0,0);
    auto draw=[&](const avatar3d::GameWorld& w) {
        host.renderer.render(w,avatar,{w.goal.x,2.2f,w.goal.z-4.5f},0,-.22f,false);
    };
    draw(world);
    auto before=host.capture(1000);
    ck(before.has_value(),"renderer owner mints local 768 VRD certificate");
    if(!before) return 1;
    ck(before->goalPixels()>20,"beacon present in real colour/depth/object-id scene");
    ck(host.current(*before,1000),"intact local frame/body/world accepted");
    ck(!host.capture(1001),"one certificate only per rendered frame sequence");
    ck(!host.current(*before,1151),"stale VRD frame expires");
    const uint32_t originalColour=host.renderer.framebuffer.colour[0];
    host.renderer.framebuffer.colour[0]^=1;
    ck(!host.current(*before,1002),"tampered local framebuffer hash rejected");
    host.renderer.framebuffer.colour[0]=originalColour;
    v25dte::EchoCommons echo;
    ck(echo.scan(host.ingress,*before,1002,host.cns.body(),host.cns.niche(),game,world),
       "grounded APL160 niche advisory from local pixel evidence and quest");
    ck(echo.candidate().pattern==160 && echo.candidate().worldVersion==1,
       "niche candidate carries rule ID and current version, not a commit");
    ck(echo.blockedSequence27()>0,"Seq27 scalar matches remain blocked without English guard measurements");
    ck(echo.canConfirm(host.ingress,*before,1002,host.cns.body(),host.cns.niche(),game,world),
       "E may locally confirm while proposal frame and world version still match");
    ck(!echo.canConfirm(host.ingress,*before,1103,host.cns.body(),host.cns.niche(),game,world),
       "E cannot confirm expired DTE proposal");
    ck(!echo.scan(host.ingress,*before,1003,host.cns.body(),host.cns.niche(),game,world),
       "replayed frame cannot issue a second candidate");
    draw(world);auto second=host.capture(1010);
    ck(second.has_value(),"new actual rendered frame can be sampled");
    ck(!host.current(*before,1010),"older frame invalidated after redraw");
    if(!second) return 1;
    ck(echo.scan(host.ingress,*second,1011,host.cns.body(),host.cns.niche(),game,world),
       "new local frame makes renewed advisory only");
    v20cns::WorldGovernance governance;
    v20cns::WorldEffector worldEffector;
    v20cns::NicheProposal forged;
    forged.source=v20cns::Source::AgentZero;
    forged.evidence={1,second->sequence(),1011,100,1.0f};
    forged.expectedWorldVersion=1;forged.locus=world.goal;
    forged.questGrounded=true;
    const auto context=v20cns::Context{second->sequence(),1011,.05f,true,false};
    const auto reject=governance.submit(forged,context,world.goal,game,world,worldEffector,
                                         [](auto&,auto&,auto){return true;});
    ck(reject==v20cns::WorldGovernance::Verdict::Untrusted,
       "DTE/Agent Zero can propose but never mint NICHE_COMMIT");
    ck(!world.beaconRestored && governance.history().worldVersion==1,
       "rejected non-human candidate leaves world unchanged");
    auto observe=[&](const avatar3d::GameWorld& pre,const avatar3d::GameWorld& post,uint64_t version) {
        auto measure=[&](const avatar3d::GameWorld& w){
            v15render::HighResRenderer camera;
            camera.render(w,avatar,{w.goal.x,2.2f,w.goal.z-4.5f},0,-.22f,false);
            int pixels=0,validDepth=0;uint64_t colour=0;
            for(size_t i=0;i<camera.framebuffer.objectId.size();++i)
                if(camera.framebuffer.objectId[i]==v15render::ObjectGoal) {
                    ++pixels;colour+=camera.framebuffer.colour[i];
                    if(std::isfinite(camera.framebuffer.depth[i])) ++validDepth;
                }
            return std::array<uint64_t,3>{(uint64_t)pixels,(uint64_t)validDepth,colour};
        };
        const auto a=measure(pre),b=measure(post);
        return version==2 && !pre.beaconRestored && post.beaconRestored &&
               a[0]>20 && b[0]>a[0] && b[0]==b[1] && a[2]!=b[2];
    };
    const bool locallyApproved=echo.canConfirm(host.ingress,*second,1012,
                               host.cns.body(),host.cns.niche(),game,world);
    ck(locallyApproved,"human E action finds fresh DTE recommendation");
    const auto accepted=host.cns.repair(world.goal,game,world,context,observe);
    ck(accepted==v20cns::WorldGovernance::Verdict::Accepted,
       "human E/WGR commits specialized repair with actual framebuffer consequence");
    ck(world.beaconRestored && host.cns.niche().worldVersion==2 && host.cns.niche().reobserved,
       "Civic beacon accepted only after authoritative re-observation");
    ck(!echo.canConfirm(host.ingress,*second,1013,host.cns.body(),host.cns.niche(),game,world),
       "old DTE proposal cannot survive successful world-version change");
    draw(world);ck(!host.current(*second,1014),"old VRD certificate invalidated by restored scene");
    // Test independently that a failed visual receipt rolls back the full world.
    avatar3d::GameWorld rollbackWorld;rollbackWorld.generate(1337,8);
    v19game::GameplayLoop rollbackGame;rollbackGame.reset(rollbackWorld);
    rollbackGame.interact(nearAriadne,0,rollbackWorld);rollbackGame.choose(1,rollbackWorld);
    rollbackGame.interact(rollbackWorld.questShard,0,rollbackWorld);
    v20cns::Runtime rollbackCns;
    const auto rolled=rollbackCns.repair(rollbackWorld.goal,rollbackGame,rollbackWorld,
                          {second->sequence(),1011,.05f,true,false},
                          [](auto&,auto&,uint64_t){return false;});
    ck(rolled==v20cns::WorldGovernance::Verdict::ReobservationFailed &&
       !rollbackWorld.beaconRestored && rollbackCns.niche().worldVersion==1 &&
       rollbackCns.niche().rolledBack,"failed consequence restores complete pre-commit world snapshot");
    std::ofstream report("echo_commons_simulation.json");
    report<<"{\"checks\":"<<checks<<",\"failures\":"<<failures
          <<",\"sourceRules\":253,\"sequence27AliasGuards\":7,\"rule\":160,"
          <<"\"localFrame\":"<<second->sequence()<<",\"goalPixelsBefore\":"<<second->goalPixels()
          <<",\"proposalSource\":\"DTE advisory\",\"agentZeroWorldVerdict\":\"Untrusted\","
          <<"\"operatorConfirmed\":"<<(locallyApproved?"true":"false")
          <<",\"wgrAccepted\":"<<(accepted==v20cns::WorldGovernance::Verdict::Accepted?"true":"false")
          <<",\"worldVersionAfter\":"<<host.cns.niche().worldVersion
          <<",\"frameConsequenceVerified\":"<<(host.cns.niche().reobserved?"true":"false")
          <<",\"rollbackProved\":"<<(rolled==v20cns::WorldGovernance::Verdict::ReobservationFailed?"true":"false")
          <<"}\n";
    std::cout<<"Echo Commons: "<<checks<<" checks, "<<failures<<" failures; rule APL160 / 768 VRD / WGR re-observation\n";
    return failures?1:0;
}

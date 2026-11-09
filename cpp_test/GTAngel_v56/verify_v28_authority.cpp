#include "verify_v27_components.hpp"
using SHORT=short;
static SHORT GetAsyncKeyState(int){return 0;}
static int DeleteFileA(const char* p){return std::remove(p)==0;}
static constexpr int MOVEFILE_REPLACE_EXISTING=1,MOVEFILE_WRITE_THROUGH=8;
static int MoveFileExA(const char* a,const char* b,int){std::remove(b);return std::rename(a,b)==0;}
#include "gtangel_agi_v20_gameplay.hpp"
#include "gtangel_agi_v27_cns.hpp"
struct GTAngelAGI { static v20cns::DteLocalMotorSeal seal(uint64_t frame,uint64_t digest,uint64_t ms,uint64_t body,uint64_t world,float f,float t){return {frame,digest,ms,body,world,f,t};} };
static int n=0,fail=0;
static void ck(bool ok,const char* name){++n;if(!ok){++fail;std::cerr<<"FAIL "<<name<<'\n';}}
int main(){
    using namespace v20cns;using V=SpinalReflex::Verdict;
    static_assert(!std::is_default_constructible<DteLocalMotorSeal>::value,"unforgeable default");
    avatar3d::GameWorld world;world.pillars.clear();world.goal={40,0,40};world.questShardVisible=false;
    BodyEffector plant;SpinalReflex s;
    Context ctx{7,1000,.05f,true,false};ctx.visualDigest=12345;ctx.worldVersion=1;
    MotorProposal p;p.source=Source::DteLocal;p.forward=.4f;p.turn=.2f;p.evidence={1,7,1000,100,1};
    ck(s.submit(p,ctx,plant,world)==V::Untrusted,"DteLocal tag alone cannot actuate");
    p.localDteSeal=GTAngelAGI::seal(7,12345,1000,0,1,.4f,.2f);
    for(auto source:{Source::Neural,Source::Route,Source::Script,Source::Replay,Source::AgentZero,Source::Remote}) {
        p.source=source;ck(s.submit(p,ctx,plant,world)==V::Untrusted,"remote/raw sources remain untrusted even with seal");
    }
    p.source=Source::DteLocal;
    auto bad=ctx;bad.visualDigest=54321;ck(s.submit(p,bad,plant,world)==V::Stale,"changed visual digest");
    bad=ctx;bad.worldVersion=2;ck(s.submit(p,bad,plant,world)==V::Stale,"changed world");
    bad=ctx;bad.nowMs=1101;ck(s.submit(p,bad,plant,world)==V::Stale,"expired seal");
    bad=ctx;bad.localFrameValid=false;ck(s.submit(p,bad,plant,world)==V::Stale,"no current local frame");
    p.turn=.3f;ck(s.submit(p,ctx,plant,world)==V::Stale,"altered controller request");p.turn=.2f;
    p.forward=.8f;p.localDteSeal=GTAngelAGI::seal(7,12345,1000,0,1,.8f,.2f);ck(s.submit(p,ctx,plant,world)==V::Stale,"speed bound");
    p.forward=.4f;p.turn=.7f;p.localDteSeal=GTAngelAGI::seal(7,12345,1000,0,1,.4f,.7f);ck(s.submit(p,ctx,plant,world)==V::Stale,"turn bound");
    p.turn=.2f;p.localDteSeal=GTAngelAGI::seal(7,12345,1000,0,1,.4f,.2f);
    ck(s.submit(p,ctx,plant,world)==V::Accepted&&plant.state().sequence==1,"sealed DTE commits only through SRD");
    ck(s.submit(p,ctx,plant,world)==V::Replay,"replayed evidence");
    p.evidence.sequence=2;ck(s.submit(p,ctx,plant,world)==V::Stale,"body changed after original seal");
    p.localDteSeal=GTAngelAGI::seal(7,12345,1000,1,1,.4f,.2f);
    ctx.emergencyStop=true;ck(s.submit(p,ctx,plant,world)==V::Unsafe,"emergency stop preempts DTE");ctx.emergencyStop=false;
    p.ethicsApproved=false;ck(s.submit(p,ctx,plant,world)==V::Unsafe,"local policy gate");p.ethicsApproved=true;
    p.forward=std::numeric_limits<float>::quiet_NaN();ck(s.submit(p,ctx,plant,world)==V::Invalid,"nonfinite controller");p.forward=.4f;
    p.turn=0;p.localDteSeal=GTAngelAGI::seal(7,12345,1000,1,1,.4f,0);
    world.pillars.push_back({{plant.state().position.x,0,plant.state().position.z+.32f},.15f,2});
    const auto before=plant.state().position;
    ck(s.submit(p,ctx,plant,world)==V::Collision&&plant.state().contact,"real geometry collision blocks DTE");
    ck(avatar3d::len(plant.state().position-before)<.04f,"bounded collision displacement");
    Runtime rt;ck(rt.motor(Source::DteLocal,.4f,0,0,0,true,ctx,world)==V::Untrusted,"public motor API cannot mint seal");
    std::cout<<"v27 authority "<<(n-fail)<<"/"<<n<<" checks\n";return fail?1:0;
}

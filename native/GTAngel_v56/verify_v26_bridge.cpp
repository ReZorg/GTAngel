#include "verify_v25_fixture.hpp"
#include "dte_system_bridge.hpp"
#include <bitset>
#include <iostream>
#include <optional>
#include <type_traits>

static_assert(system78::Roots7.size()==115 && system78::Roots8.size()==286,"exact tree census");
static_assert(!std::is_default_constructible<v25dte::Sample>::value,"only renderer owner mints frame samples");
struct GTAngelAGI {
    v15render::HighResRenderer renderer;
    v20cns::Runtime cns;
    v25dte::LocalVisualIngress ingress{renderer.framebuffer};
    v26bridge::LensBridge bridge;
    std::optional<v25dte::Sample> capture(uint64_t t){return ingress.capture(t,cns.body(),cns.niche());}
    bool accept(const v25dte::Sample& s,uint64_t t){
        if(!ingress.current(s,t,cns.body(),cns.niche())){bridge.reset();return false;}
        bridge.observe(s,cns.body(),cns.niche());return bridge.state().valid;
    }
};
int checks=0, failures=0;
void ck(bool ok,const char* label){++checks;if(!ok){++failures;std::cerr<<"FAIL "<<checks<<" "<<label<<"\n";}}
int main(){
    std::array<unsigned,115> outdegree{};
    std::bitset<286> allChildren;
    std::bitset<48> allFree8;
    std::bitset<24> allFree7;
    for(const auto& p:system78::Roots7){ck(p.free>=1&&p.free<=23,"S7 family valid");allFree7.set(p.free);}
    for(const auto& p:system78::Roots8)ck(p.free>=1&&p.free<=47,"S8 family valid");
    for(const auto& e:system78::Edges){
        ck(e.parent<115&&e.child<286&&e.witnesses>=1&&e.witnesses<=8,"typed leaf-addition edge");
        if(e.parent<115&&e.child<286){outdegree[e.parent]++;allChildren.set(e.child);allFree8.set(system78::Roots8[e.child].free);}
    }
    ck(allFree7.count()==23,"all S7 free families represented");
    ck(allChildren.count()==286,"every S8 rooted child reached");
    ck(allFree8.count()==47,"every S8 free family reached");
    for(const auto count:outdegree)ck(count>0,"each S7 rooted term has descendants");
    GTAngelAGI host;
    avatar3d::GameWorld world; world.generate(1337,8);
    avatar3d::AvatarSkeleton avatar;avatar.pose(world.goal,0,0,0);
    host.renderer.render(world,avatar,{world.goal.x,2.2f,world.goal.z-4.5f},0,-.22f,false);
    const auto first=host.capture(1000);
    ck(first.has_value(),"real local high-res frame minted by renderer owner");
    if(!first)return 1;
    ck(first->goalPixels()>20,"actual rendered beacon semantic pixels present");
    ck(host.accept(*first,1000),"valid local observation reaches read-only tree bridge");
    const auto lens=host.bridge.state();
    ck(lens.visualSequence==host.renderer.framebuffer.sequence && lens.visualDigest==first->hash(),
       "bridge stores actual visual sequence and digest");
    ck(lens.worldVersion==host.cns.niche().worldVersion && lens.bodySequence==host.cns.body().sequence,
       "bridge binds local world and body");
    ck(lens.children>0 && lens.childFamilies>0 && lens.childFamilies<=lens.children,
       "selection resolves actual rooted and quotient children");
    ck(lens.root7<115 && lens.root8<286 && lens.free7<=23 && lens.free8<=47,
       "selected rooted/free identities are bounded");
    auto colour=host.renderer.framebuffer.colour[0];host.renderer.framebuffer.colour[0]^=1;
    ck(!host.accept(*first,1001) && !host.bridge.state().valid,"tampered colour breaks ingress and clears lens");
    host.renderer.framebuffer.colour[0]=colour;
    ck(!host.accept(*first,1151),"stale local frame cannot yield a lens");
    host.renderer.render(world,avatar,{world.goal.x,2.2f,world.goal.z-4.5f},0,-.22f,false);
    const auto second=host.capture(1160);
    ck(second.has_value() && (!second || host.accept(*second,1160)),"fresh frame renews lens");
    ck(!host.accept(*first,1160),"earlier frame fails after re-render");
    ck(!host.bridge.state().valid,"failed validation clears stale perspective");
    std::cout<<"v26 bridge "<<checks-failures<<"/"<<checks<<" checks; edges="
             <<system78::Edges.size()<<" all rooted S8="<<allChildren.count()
             <<" free S8="<<allFree8.count()<<"\n";
    return failures?1:0;
}

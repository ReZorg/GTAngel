#define main v24_sensor_benchmark_main
#include "verify_v24_renderer.cpp"
#undef main
#include <type_traits>
static_assert(v15render::HighResFramebuffer::Width==768 && v15render::HighResFramebuffer::Height==768,"AI sensor contract");
static_assert(v15render::DisplayFramebuffer::Width==1536 && v15render::DisplayFramebuffer::Height==864,"presentation contract");
static_assert(!std::is_same<v15render::HighResFramebuffer,v15render::DisplayFramebuffer>::value,"display surface must be distinct");
int main(){
    avatar3d::GameWorld world;world.generate(1337,8);
    avatar3d::AvatarSkeleton player;player.pose({0,0,0},0,0,0.05f);
    v15render::HighResRenderer sensor;
    v15render::DisplayRenderer display;
    if(!sensor.loadDteAsset("assets/dte_v22_lod.bin")||!display.loadDteAsset("assets/dte_v22_lod.bin"))return 2;
    const auto eye=avatar3d::Vec3{0,2.25f,-4.2f};
    sensor.render(world,player,eye,0.0f,-0.22f,true);
    v15render::PerceptionPyramid pyramid;pyramid.derive(sensor.framebuffer);
    const auto sensorColour=sensor.framebuffer.colour;
    const auto sensorDepth=sensor.framebuffer.depth;
    const auto sensorId=sensor.framebuffer.objectId;
    const auto retina=pyramid.retina.luminance;
    const auto sequence=sensor.framebuffer.sequence;
    display.render(world,player,eye,0.0f,-0.22f,true);
    const bool untouched=sensor.framebuffer.colour==sensorColour &&
        sensor.framebuffer.depth==sensorDepth && sensor.framebuffer.objectId==sensorId &&
        sensor.framebuffer.sequence==sequence && retina==pyramid.retina.luminance;
    const int displayAvatar=(int)std::count(display.framebuffer.objectId.begin(),display.framebuffer.objectId.end(),v15render::ObjectAvatar);
    const int displayGround=(int)std::count(display.framebuffer.objectId.begin(),display.framebuffer.objectId.end(),v15render::ObjectGround);
    const int sensorAvatar=countObject(sensor.framebuffer,v15render::ObjectAvatar);
    const bool distinct=displayAvatar>sensorAvatar && displayGround>0 &&
        display.framebuffer.colour.size()==1536u*864u && display.framebuffer.depth.size()==1536u*864u &&
        display.framebuffer.objectId.size()==1536u*864u &&
        display.framebuffer.colour.data()!=sensor.framebuffer.colour.data();
    const bool good=untouched&&distinct;
    std::ofstream out("v24_display_benchmark.json");
    out<<"{\"valid\":"<<(good?"true":"false")
       <<",\"ai_pixels\":"<<sensorColour.size()<<",\"display_pixels\":"<<display.framebuffer.colour.size()
       <<",\"ai_avatar_pixels\":"<<sensorAvatar<<",\"display_avatar_pixels\":"<<displayAvatar
       <<",\"sensor_unchanged\":"<<(untouched?"true":"false")
       <<",\"display_ms\":"<<display.metrics.renderMs<<"}\n";
    std::ofstream image("assets/v24_true_display.ppm",std::ios::binary);
    image<<"P6\n1536 864\n255\n";
    for(uint32_t px:display.framebuffer.colour){auto c=v15render::DisplayFramebuffer::unpack(px);
        image.put((char)c.r);image.put((char)c.g);image.put((char)c.b);}
    std::cout<<"v24_dual_raster="<<(good?"PASS":"FAIL")<<" ai="<<sensorColour.size()
             <<" display="<<display.framebuffer.colour.size()<<" display_avatar="<<displayAvatar
             <<" sensor_unchanged="<<untouched<<" display_ms="<<display.metrics.renderMs<<'\n';
    return good?0:1;
}

class DtePoseLibrary {
public:
    static const DtePoseLibrary& instance() {
        static DtePoseLibrary lib;
        return lib;
    }
    bool valid() const { return valid_; }
    float duration(int clip) const {
        return (clip>=0 && clip<kClips && fps_[clip]>0.0f) ? (float)frameCount_[clip]/fps_[clip] : 0.0f;
    }
    // phase in [0,1); out receives 16 joint displacements (metres).
    void sample(int clip, float phase, Vec3 out[16]) const {
        for (int i=0;i<16;++i) out[i]={0.0f,0.0f,0.0f};
        if (!valid_ || clip<0 || clip>=kClips || frameCount_[clip]<1) return;
        const float p = phase - std::floor(phase);
        const float f = p * (float)frameCount_[clip];
        const int i0 = (int)f, i1 = (i0+1) % (int)frameCount_[clip];
        const float a = f - (float)i0;
        const Vec3* d0 = &data_[offset_[clip] + (size_t)i0*16];
        const Vec3* d1 = &data_[offset_[clip] + (size_t)i1*16];
        for (int i=0;i<16;++i)
            out[i] = {d0[i].x+(d1[i].x-d0[i].x)*a,
                      d0[i].y+(d1[i].y-d0[i].y)*a,
                      d0[i].z+(d1[i].z-d0[i].z)*a};
    }
private:
    static constexpr int kClips = 8;
    static constexpr uint32_t kMaxFrames = 256;
    bool valid_{false};
    uint32_t frameCount_[kClips]{};
    float fps_[kClips]{};
    size_t offset_[kClips]{};
    std::vector<Vec3> data_;
    static std::string posePath() {
#ifdef _WIN32
        char exe[4096]{};
        const unsigned long n = GetModuleFileNameA(nullptr, exe, sizeof(exe));
        if (n>0 && n<sizeof(exe)-1) {
            std::string v(exe, exe+n);
            const size_t slash = v.find_last_of("\\/");
            if (slash!=std::string::npos)
                return v.substr(0,slash+1)+"assets\\dte_v25_poses.bin";
        }
#endif
        return "assets/dte_v25_poses.bin";
    }
    explicit DtePoseLibrary() {
        std::ifstream s(posePath(), std::ios::binary);
        if (!s) return; // optional cosmetic asset
        s.seekg(0, std::ios::end);
        const std::streamoff n = s.tellg();
        s.seekg(0, std::ios::beg);
        if (n < 16 || n > 1024*1024) return;
        std::vector<uint8_t> b((size_t)n);
        if (!s.read(reinterpret_cast<char*>(b.data()), n)) return;
        static const uint8_t magic[8] = {'D','T','E','P','2','5','L',0};
        if (std::memcmp(b.data(), magic, 8) != 0) return;
        const uint32_t version = b[8]|(b[9]<<8)|(b[10]<<16)|((uint32_t)b[11]<<24);
        const uint32_t clips = b[12]|(b[13]<<8)|(b[14]<<16)|((uint32_t)b[15]<<24);
        if (version != 1 || clips != (uint32_t)kClips) return;
        size_t at = 16;
        for (int c=0;c<kClips;++c) {
            if (at+8 > b.size()) return;
            frameCount_[c] = b[at]|(b[at+1]<<8)|(b[at+2]<<16)|((uint32_t)b[at+3]<<24);
            std::memcpy(&fps_[c], &b[at+4], 4);
            at += 8;
            if (frameCount_[c]==0 || frameCount_[c]>kMaxFrames ||
                !std::isfinite(fps_[c]) || fps_[c]<=0.0f) return;
            const size_t count = (size_t)frameCount_[c]*16;
            if (at + count*12 > b.size()) return;
            offset_[c] = data_.size();
            const size_t base = data_.size();
            data_.resize(base+count);
            std::memcpy(&data_[base], &b[at], count*12);
            at += count*12;
        }
        if (at != b.size()) return;
        for (const auto& v : data_)
            if (!std::isfinite(v.x) || !std::isfinite(v.y) || !std::isfinite(v.z)) {
                data_.clear(); return;
            }
        valid_ = true;
    }
};

struct AvatarSkeleton {
    enum JointId { Root=0, Spine, Neck, Head,
                   ShoulderL, ElbowL, HandL, ShoulderR, ElbowR, HandR,
                   HipL, KneeL, FootL, HipR, KneeR, FootR, COUNT };
    enum class Motion { Explore, CombatEntry, Guard, Recovery };
    Vec3 joints[COUNT]{};
    Vec3 wingRoot[2]{}, wingBoom[2]{}, wingTip[2]{};
    Vec3 cheek[3]{}, headset[2]{};
    float walkPhase{0.0f}, walkBlend{0.0f}, motionTime{0.0f}, wingDeploy{0.0f}, orbIntensity{0.0f};
    Motion motion{Motion::Explore};
    bool inspect{false};
    float idleTime{0.0f}; // LW21 idle clip phase
    Vec3 lastPosition{};

    static float smooth(float a,float b,float t) {
        const float u=std::clamp((t-a)/(b-a),0.0f,1.0f);
        return u*u*(3.0f-2.0f*u);
    }
    bool triggerCombat() {
        if (motion!=Motion::Explore) return false;
        motion=Motion::CombatEntry; motionTime=0; return true;
    }
    bool releaseCombat() {
        if (motion!=Motion::Guard) return false;
        motion=Motion::Recovery; motionTime=0; return true;
    }
    void pose(Vec3 position,float heading,float speed,float dt) {
        // 1.3 m per two-step cycle at 1.3 m/s; the authoritative BodyEffector
        // supplies world displacement. No pose operation changes position.
        const float delta=std::clamp(dt,0.0f,0.1f);
        const float travel=std::isfinite(speed)?std::clamp(std::fabs(speed),0.0f,2.0f):0.0f;
        const float target=(motion==Motion::Explore)?std::clamp(travel/1.3f,0.0f,1.0f):0.0f;
        const float blendStep=delta/0.20f;
        walkBlend+=std::clamp(target-walkBlend,-blendStep,blendStep);
        if (delta>0 && travel>0.025f && motion==Motion::Explore)
            walkPhase+=6.2831853f*travel*delta/1.30f;
        if (delta>0) {
            motionTime+=delta;
            if (motion==Motion::CombatEntry && motionTime+0.0001f>=1.50f) {
                motion=Motion::Guard; motionTime=0;
            } else if (motion==Motion::Recovery && motionTime>=0.30f) {
                motion=Motion::Explore; motionTime=0;
            }
        }
        const float frames=motion==Motion::CombatEntry?motionTime*24.0f:
                           motion==Motion::Guard?36.0f:0.0f;
        // Unlock -> boom rotate -> fan. No spontaneous wing motion in Explore.
        wingDeploy=motion==Motion::Explore?0.0f:
            motion==Motion::Recovery?1.0f-smooth(0.0f,0.30f,motionTime):
            motion==Motion::Guard?1.0f:smooth(16.0f,27.0f,frames);
        orbIntensity=motion==Motion::Guard?0.34f:
            motion==Motion::CombatEntry?smooth(18.0f,26.0f,frames):
            motion==Motion::Recovery?0.34f*(1.0f-smooth(0,0.3f,motionTime)):0.0f;
        const bool fighting=motion==Motion::CombatEntry||motion==Motion::Guard||motion==Motion::Recovery;
        const float load=fighting?(motion==Motion::CombatEntry?smooth(4,14,frames):1.0f):0.0f;
        const float locomotion=walkBlend;
        const float runBlend=std::clamp((travel-0.55f)/0.35f,0.0f,1.0f)*locomotion;
        const float strideScale=1.0f+0.36f*runBlend;
        const float step=std::sin(walkPhase)*locomotion*strideScale;
        const float bob=(0.032f+0.022f*runBlend)*(1.0f-std::cos(2.0f*walkPhase))*locomotion;
        const float ch=std::cos(heading),sh=std::sin(heading);
        auto R=[&](Vec3 v){return Vec3{v.x*ch+v.z*sh,v.y,-v.x*sh+v.z*ch}+position;};
        auto Rvec=[&](Vec3 v){return Vec3{v.x*ch+v.z*sh,v.y,-v.x*sh+v.z*ch};};
        const float stance=0.12f*load, crouch=0.12f*load;
        const float runLean=0.042f*runBlend;
        joints[Root]=R({0,0.95f+bob-crouch,0});
        joints[Spine]=R({0,1.25f+bob-crouch*0.5f,0.025f*load+runLean*0.6f});
        joints[Neck]=R({0,1.50f+bob-crouch*0.3f,runLean});
        joints[Head]=R({(inspect && !fighting)?0.022f:0.0f,1.70f+bob-crouch*0.2f,runLean*1.15f});
        for (int side=0;side<2;++side) {
            const float s=side==0?-1.0f:1.0f;
            const int shoulder=side==0?ShoulderL:ShoulderR;
            const int elbow=side==0?ElbowL:ElbowR;
            const int hand=side==0?HandL:HandR;
            const int hip=side==0?HipL:HipR;
            const int knee=side==0?KneeL:KneeR;
            const int foot=side==0?FootL:FootR;
            const float footSwing=s*step;
            const float kneeLift=std::max(0.0f,footSwing)*(0.06f+0.08f*runBlend);
            const float lift=std::max(0.0f,footSwing)*(0.12f+0.08f*runBlend);
            joints[shoulder]=R({s*0.22f,1.45f+bob-crouch*0.3f,runLean*0.8f});
            joints[elbow]=R({s*(0.28f+0.11f*load),1.15f+bob+0.04f*runBlend, -footSwing*0.24f+0.18f*load});
            joints[hand]=R({s*(0.30f+0.16f*load),0.85f+bob+0.35f*load+0.10f*runBlend,
                            -footSwing*0.44f+0.32f*load+0.06f*runBlend});
            joints[hip]=R({s*(0.12f+stance),0.95f+bob-crouch,0});
            joints[knee]=R({s*(0.14f+stance),0.50f+kneeLift-crouch*0.7f,
                            footSwing*0.28f+0.09f*load});
            joints[foot]=R({s*(0.15f+stance),0.05f+lift,
                            footSwing*0.46f+0.08f*load});
            wingRoot[side]=R({s*0.19f,1.43f+bob-crouch*0.3f,-0.16f+runLean*0.5f});
            wingBoom[side]=R({s*(0.26f+0.47f*wingDeploy),1.78f+0.26f*wingDeploy+bob,-0.24f});
            wingTip[side]=R({s*(0.30f+1.13f*wingDeploy),1.96f+0.62f*wingDeploy+bob,-0.31f});
            headset[side]=R({s*0.133f,1.72f+bob,runLean*1.15f});
        }
        // LW40: rotate baked v25/v28 GLB clip displacements by (ch, sh) into
        // the avatar's facing heading and transition between Walk24 (clip 0)
        // and Run16 (clip 7) at travel > 0.65 m/s.
        {
            const DtePoseLibrary& pl = DtePoseLibrary::instance();
            if (pl.valid()) {
                Vec3 disp[16];
                if (walkBlend > 0.03f) {
                    const int locoClip = (travel > 0.65f) ? 7 : 0;
                    pl.sample(locoClip, walkPhase*0.15915494f, disp);
                    const float w = 0.85f*walkBlend;
                    for (int i=0;i<COUNT;++i) {
                        const Vec3 rd = Rvec(disp[i]);
                        joints[i] = {joints[i].x+rd.x*w, joints[i].y+rd.y*w, joints[i].z+rd.z*w};
                    }
                } else if (fighting) {
                    int clip=-1; float phase=0.0f;
                    if (motion==Motion::CombatEntry) { clip=4; phase=std::clamp(motionTime/1.5f,0.0f,1.0f); }
                    else if (motion==Motion::Guard) { clip=6; phase=std::clamp(motionTime/1.2f,0.0f,1.0f); }
                    else if (motion==Motion::Recovery) { clip=5; phase=std::clamp(motionTime/0.30f,0.0f,1.0f); }
                    if (clip>=0) {
                        pl.sample(clip, phase, disp);
                        for (int i=0;i<COUNT;++i) {
                            const Vec3 rd = Rvec(disp[i]);
                            joints[i] = {joints[i].x+rd.x*0.9f, joints[i].y+rd.y*0.9f, joints[i].z+rd.z*0.9f};
                        }
                    }
                } else {
                    idleTime += delta;
                    const float idleDur = pl.duration(1);
                    if (inspect)
                        pl.sample(2, idleDur>0.01f ? idleTime/idleDur : 0.0f, disp);
                    else
                        pl.sample(1, idleDur>0.01f ? idleTime/idleDur : 0.0f, disp);
                    for (int i=0;i<COUNT;++i) {
                        const Vec3 rd = Rvec(disp[i]);
                        joints[i] = {joints[i].x+rd.x*0.9f, joints[i].y+rd.y*0.9f, joints[i].z+rd.z*0.9f};
                    }
                }
            }
        }
        for (int i=0;i<3;++i) cheek[i]=R({0.09f+0.026f*i,1.65f-0.05f*i+bob,0.094f});
        lastPosition=position;
    }
};

// ── Gameworld ──// ── Gameworld ──
// Ground plane + obstacle pillars + goal beacon. The avatar must navigate
// from spawn to the beacon while avoiding obstacles.

#include "verify_v56_components.hpp"
#include "skill_curriculum.hpp"
static int checks=0,failures=0;
static void check(bool ok,const char* label){++checks;if(!ok){++failures;std::cerr<<"FAIL "<<label<<'\n';}}
static int checkpointChild(const std::string& directory) {
    gtlearn::Store store(directory);
    for(const auto& path:store.candidates()) {
        gtlearn::Snapshot snapshot; v27gnn::GneuralMotorPolicy motor;
        if(!store.read(path,snapshot)||!motor.decode(snapshot.payload[1])) continue;
        if(motor.encode()!=snapshot.payload[1]) return 2;
        float features[v27gnn::FeatureSize]{};
        features[0]=1; features[6]=.5f; features[v27gnn::FeatureSize-1]=1;
        float turn=0,forward=0;
        gtlearn::Reader expected{snapshot.payload[0]};
        const float expectedTurn=expected.get<float>(),expectedForward=expected.get<float>();
        if(!motor.infer(features,turn,forward)||turn!=expectedTurn||forward!=expectedForward) return 3;
        const auto steps=motor.trainingSteps();
        motor.remember(features,-.4f,-.6f);
        if(!motor.train()||motor.trainingSteps()<=steps) return 4;
        return 0;
    }
    return 1;
}
int main(int argc,char** argv){
    if(argc==3 && std::string(argv[1])=="--checkpoint-child") return checkpointChild(argv[2]);
    char executable[MAX_PATH]{};
    GetModuleFileNameA(nullptr,executable,MAX_PATH);
    const auto testRoot=(std::filesystem::path(executable).parent_path()/"../../build/learning-tests"/
                         ("verify-"+std::to_string(GetCurrentProcessId()))).lexically_normal();
    std::filesystem::create_directories(testRoot);
    {
        gtskill::Curriculum skills;
        for(int i=0;i<1000;++i) skills.observe(gtskill::Skill::Walk,.1f,false);
        check(!skills.ready(gtskill::Skill::Walk),"training alone never grants mastery");
        for(unsigned seed=0;seed<8;++seed) {
            check(skills.assess(gtskill::Skill::Walk,123,1,{seed,true,0,0,8,10}),"valid walking assessment");
            skills.assess(gtskill::Skill::Run,123,1,{seed,true,0,0,6,10});
        }
        check(skills.ready(gtskill::Skill::Walk)&&skills.ready(gtskill::Skill::Run),"distinct successful assessments validate skills");
        skills.assess(gtskill::Skill::Run,123,1,{7,true,0,0,6,10});
        check(skills.record(gtskill::Skill::Run).evaluations.size()==8,"duplicate seed does not inflate mastery");
        check(skills.select(false,false,10,10,{}).skill==gtskill::Skill::Walk,"saved scores cannot grant absent motor capability");
        check(skills.select(false,false,10,10,{true,false}).skill==gtskill::Skill::Run,"validated running selected in clear space");
        check(skills.select(false,true,10,10,{true,false}).skill==gtskill::Skill::Walk,"contact forces walking fallback");
        const auto snapshot=skills.encode();
        gtskill::Curriculum restored;
        check(restored.decode(snapshot)&&restored.encode()==snapshot,"skill evidence and lifetime observations persist");
        check(!restored.decode(snapshot.substr(0,snapshot.size()-1))&&restored.encode()==snapshot,
              "truncated skill state leaves incumbent intact");
        restored.assess(gtskill::Skill::Run,123,1,{0,true,.5f,0,6,10});
        check(!restored.ready(gtskill::Skill::Run),"safety regression demotes skill");
        restored.assess(gtskill::Skill::Walk,124,1,{0,true,0,0,8,10});
        check(!restored.ready(gtskill::Skill::Walk),"new model requires fresh evaluation");
        gtlearn::Autosave schedule; schedule.attempted(100,true);
        check(!schedule.due(60099,true)&&schedule.due(60100,true),"autosave is elapsed-time based");
        schedule.attempted(60100,false);
        check(!schedule.due(60101,true)&&schedule.due(70100,true),"failed autosave retries without a tight loop");
    }
    {
        bool continuous=true, bounded=true;
        for(bool avatarBehind:{false,true})
            for(float r2:{0.0f,.5f,1.0f,2.0f}) {
                float previous=v15render::occluderOpacity(0,r2,avatarBehind);
                for(int i=1;i<=500;++i) {
                    const float opacity=v15render::occluderOpacity(i*.01f,r2,avatarBehind);
                    bounded=bounded && opacity>=.12f && opacity<=1.0f;
                    continuous=continuous && std::fabs(opacity-previous)<.025f;
                    previous=opacity;
                }
            }
        check(bounded,"camera fade stays within opacity bounds");
        check(continuous,"camera fade is continuous across depth thresholds");
        check(v15render::occluderOpacity(4.25f,0,true)==1.0f,
              "distant occluders return to full opacity");
    }
    using N=avatar3d::NavTrainer;
    v27learn::OnlineNavigationObserver o; N q;
    v15render::VisualNavEvidence a,b;
    a.sourceSequence=1;b.sourceSequence=2;a.state={4,2,4};b.state={3,1,4};
    check(o.observe(q,a,b,1,0,.2f,true,false,true,true),"accepted local observed action");
    check(o.updates==1&&q.stepsTotal==1&&std::fabs(q.Q[4][2][4][N::FWD]-.03f)<1e-6f,"actual Bellman weight update");
    check(o.action(1,-.68f)==N::FWD_L&&o.action(1,.68f)==N::FWD_R,"forward turn mapping");
    check(o.action(0,-.68f)==N::TURN_L&&o.action(0,.68f)==N::TURN_R&&o.action(-.55f,0)==N::BACK,"turn/back mapping");
    check(!o.observe(q,a,b,0,0,.1f,true,false,true,true),"idle ignored");
    check(!o.observe(q,a,b,1,0,.1f,false,false,true,true),"unaccepted ignored");
    check(!o.observe(q,a,b,1,0,.1f,true,false,false,true),"cart ignored");
    check(!o.observe(q,a,b,1,0,.1f,true,false,true,false),"disabled ignored");
    auto old=b;b.sourceSequence=1;check(!o.observe(q,a,b,1,0,.1f,true,false,true,true),"stale observed frame");b=old;
    a.sourceSequence=0;check(!o.observe(q,a,b,1,0,.1f,true,false,true,true),"no local source");a.sourceSequence=1;
    b.state.dist=-1;check(!o.observe(q,a,b,1,0,.1f,true,false,true,true),"bad state");b=old;
    check(!o.observe(q,a,b,std::numeric_limits<float>::quiet_NaN(),0,.1f,true,false,true,true),"nonfinite controller");
    check(!o.observe(q,a,b,1,0,std::numeric_limits<float>::quiet_NaN(),true,false,true,true),"nonfinite reward");
    check(o.observe(q,a,b,1,0,-1,true,true,true,true),"contact is actual negative learning signal");
    check(o.collisions==1&&q.collisions==1,"contact counters");
    for(int i=0;i<198;++i)check(o.observe(q,a,b,1,0,-.01f,true,false,true,true),"200 observation window");
    check(o.updates==200&&o.windows==1&&q.episodes==1&&q.successes==0,"windows do not fake successes");
    // Real 768 render -> 96 tensor; frozen native CNN/GRU inference, not fixture logits.
    avatar3d::GameWorld world;world.generate(1337,48);avatar3d::AvatarSkeleton avatar;
    avatar.pose({0,0,0},0,0,0);
    v15render::HighResRenderer render;
    render.loadDteAsset("assets/dte_v26_lod.bin");
    render.render(world,avatar,{0,2.5f,-4},0,-.4f,true);
    v16motor::VisualTensor96 visual;visual.derive(render.framebuffer);
    v17policy::NativeVisuomotorPolicy policy;
    check(policy.load("models/gtangel_visuomotor.gvmw"),"verified real NN weight load");
    const auto proprio=v16motor::makeProprioception(0,0,0,false,true,{},0,0,0,0);
    v17policy::Prediction pred;
    for(int i=0;i<12;++i){pred=policy.infer(visual,proprio);check(pred.valid&&std::isfinite(pred.latencyMs),"real finite CNN GRU inference");}
    check(pred.latencyMs>0&&pred.collisionProbability>=0&&pred.collisionProbability<=1,"measured latency/head");
    // Echo persona (superhot-gamer-girl echo.json): emotion resolution contract.
    {
        v27dte::EchoPersona echo;
        const v27dte::EchoPersonaState& s=echo.state();
        check(std::string(s.emotion)=="BORED_STOMPING"&&std::string(s.expression)=="PLAYFUL_BORED","echo persona default state");
        auto r=echo.resolve(0.5,0.3,0);
        check(std::string(r.emotion)=="VICTORY_ROYALE"&&std::string(r.expression)=="TRIUMPHANT_GRIN","high valence high arousal is victory");
        r=echo.resolve(0.5,-0.3,2);
        check(std::string(r.emotion)=="COZY_RECALL","rest mode maps to cozy recall");
        r=echo.resolve(-0.4,0.4,3);
        check(std::string(r.emotion)=="GETTING_TILTED"&&std::string(r.facs)=="AU4 AU7 AU23","alarm negative valence is tilted");
        r=echo.resolve(0.2,0.0,1);
        check(std::string(r.emotion)=="FLOW_STATE","focused positive valence is flow");
        r=echo.resolve(0.0,0.0,0);
        check(std::string(r.emotion)=="BORED_STOMPING","neutral is playful bored");
        r=echo.resolve(-0.1,0.0,0);
        check(std::string(r.emotion)=="BRANCH_DIVERGE"&&r.spice>0.45f,"negative neutral is mischievous branch");
        r=echo.resolve(0.5,0.3,0);
        check(r.confidence>0.0f&&r.confidence<=1.0f&&echo.state().updates>0,"confidence bounded and updates counted");
        bool allSpiceBounded=true;
        for (double v=-1.0; v<=1.0; v+=0.05) for (double a=-1.0; a<=1.0; a+=0.05)
            for (int m=0; m<4; ++m) { auto x=echo.resolve(v,a,m); if (x.spice<0.0f||x.spice>1.0f) allSpiceBounded=false; }
        check(allSpiceBounded,"spice always bounded 0..1 across full valence/arousal grid");
    }
    v17policy::EmbodiedSafetyShield shield;
    // v27: NN actuation is UNLOCKED for adaptive learning. The shield remains as
    // safety (bounds/uncertainty), never as a lock: assist+ethics approved means
    // the decision is evaluated on its merits, not rejected as "shadow mode".
    const auto decision=shield.decide(pred,{},0,true,true);
    check(decision.reason!="shadow mode"&&decision.reason!="ethics gate","NN motor unlocked for learning");
    {
        // Synthetic agreement: a prediction matching the expert must be accepted.
        v17policy::Prediction agree{};
        agree.valid=true;agree.uncertainty=0.05f;agree.collisionProbability=0.05f;
        for(int i=0;i<6;++i)agree.controller.values[i]=0.0f;
        agree.controller.values[1]=0.3f;agree.controller.values[5]=1.0f; // forward 0.3, commit
        v16motor::ControllerTarget expert{};expert.values[1]=0.3f;expert.values[5]=1.0f; // forward, commit
        const auto d2=shield.decide(agree,expert,0.02f,true,true);
        check(d2.accepted,"unlocked shield accepts agreeing NN action");
    }
    {
        // GNN motor convergence: the GNU GNeuralNet policy must actually learn.
        v27gnn::GneuralMotorPolicy gnn;
        check(gnn.create(),"GNN motor network created");
        float feat[v27gnn::FeatureSize];
        for(int i=0;i<v27gnn::FeatureSize;++i)feat[i]=0.0f;
        feat[0]=1.0f;feat[6]=0.5f;feat[v27gnn::FeatureSize-1]=1.0f; // goal visible, bearing right, bias
        float t0=0,f0=0;
        check(gnn.infer(feat,t0,f0),"GNN inference before training");
        float firstLoss=-1,lastLoss=-1;
        for(int it=0;it<400;++it){
            gnn.remember(feat,0.4f,0.6f); // teach: turn 0.4, forward 0.6
            if(gnn.train()){ const float L=gnn.lastLoss(); if(firstLoss<0)firstLoss=L; lastLoss=L; }
        }
        check(firstLoss>=0&&lastLoss>=0&&lastLoss<=firstLoss,"GNN training loss non-increasing");
        float t1=0,f1=0;
        check(gnn.infer(feat,t1,f1),"GNN inference after training");
        check(std::fabs(t1-0.4f)<0.25f&&std::fabs(f1-0.6f)<0.25f,"GNN converged toward taught action");
        const std::string tmpNet=(testRoot/"motor.net").string();
        check(gnn.save(tmpNet),"GNN save");
        v27gnn::GneuralMotorPolicy gnn2;
        check(gnn2.create()&&gnn2.load(tmpNet),"GNN reload");
        float t2=0,f2=0;
        check(gnn2.infer(feat,t2,f2)&&std::fabs(t2-t1)<1e-4f&&std::fabs(f2-f1)<1e-4f,"GNN round-trip exact");
        // A restored network must take real gradient steps, not just report
        // training calls. Teach the opposite action and measure its output.
        const float beforeError=(t2+0.4f)*(t2+0.4f)+(f2+0.6f)*(f2+0.6f);
        for(int it=0;it<80;++it) {
            gnn2.remember(feat,-0.4f,-0.6f);
            gnn2.train();
        }
        check(gnn2.trainingSteps()>0,"restored GNN performs gradient steps");
        float t3=0,f3=0;
        check(gnn2.infer(feat,t3,f3) &&
              (t3+0.4f)*(t3+0.4f)+(f3+0.6f)*(f3+0.6f)<beforeError*0.5f,
              "restored GNN learns a changed target");
        const auto state=gnn2.encode();
        v27gnn::GneuralMotorPolicy resumed;
        check(resumed.decode(state)&&resumed.encode()==state,"complete GNN training state round-trips exactly");
        check(resumed.totalSamples()==gnn2.totalSamples()&&resumed.trainingSteps()==gnn2.trainingSteps(),
              "replay samples and gradient counts survive reload");
        check(!resumed.decode(state.substr(0,state.size()/2))&&resumed.encode()==state,
              "invalid GNN checkpoint cannot partially replace learner");
        const auto beforeBadLoad=resumed.encode();
        gtlearn::writeFile(testRoot/"bad.net","999999999\n");
        check(!resumed.load((testRoot/"bad.net").string())&&resumed.encode()==beforeBadLoad,
              "invalid legacy topology fails without exiting or mutating network");
        gtlearn::Store store(testRoot/"checkpoints");
        gtlearn::Writer expected; expected.put(t3); expected.put(f3);
        gtskill::Curriculum skills;
        std::array<std::string,3> payload{expected.bytes,state,skills.encode()};
        std::string first,second;
        check(store.save(payload,first)&&store.save(payload,second),"complete checkpoint generations publish");
        check(store.pinBest(first),"best checkpoint can be retained independently");
        gtlearn::writeFile(testRoot/"checkpoints"/second/"motor.bin","damaged");
        gtlearn::Snapshot restoredSnapshot;
        check(!store.read(testRoot/"checkpoints"/second,restoredSnapshot),"checksum rejects corrupted generation");
        check(store.read(testRoot/"checkpoints"/first,restoredSnapshot),"previous generation survives failed latest");
        std::filesystem::create_directory(testRoot/"checkpoints"/"cp-999999.pending");
        check(store.candidates().size()==2,"incomplete generations are not recovery candidates");
        std::string command="\""+std::string(executable)+"\" --checkpoint-child \""+(testRoot/"checkpoints").string()+"\"";
        STARTUPINFOA si{}; si.cb=sizeof(si); PROCESS_INFORMATION pi{};
        bool childOk=CreateProcessA(executable,command.data(),nullptr,nullptr,FALSE,0,nullptr,nullptr,&si,&pi)!=0;
        if(childOk) {
            const auto wait=WaitForSingleObject(pi.hProcess,60000);
            DWORD status=1;
            if(wait==WAIT_OBJECT_0) GetExitCodeProcess(pi.hProcess,&status);
            else TerminateProcess(pi.hProcess,1);
            childOk=status==0; CloseHandle(pi.hThread); CloseHandle(pi.hProcess);
        }
        check(childOk,"fresh process recovers predictions and continues training");
        for(int i=0;i<7;++i) store.save(payload,second);
        check(std::filesystem::exists(testRoot/"checkpoints"/first)&&store.candidates().size()<=7,
              "retention preserves best checkpoint and bounds complete history");
        gtlearn::writeFile(testRoot/"blocked","not a directory");
        gtlearn::Store blocked(testRoot/"blocked");
        check(!blocked.save(payload,second),"checkpoint I/O failures return status without exiting");
        std::remove(tmpNet.c_str());
    }
    policy.reset();check(policy.loaded(),"recurrent reset keeps weights");
    check(policy.infer(visual,proprio).uncertainty==1.0f,"first sample uncertainty reset");
    check(!policy.load("models/missing.gvmw")&&!policy.loaded(),"missing weights fail closed");
    std::cout<<"v56 learning "<<(checks-failures)<<"/"<<checks<<" checks; real Q updates="<<o.updates<<"; actual NN latency="<<pred.latencyMs<<" ms\n";
    return failures?1:0;
}

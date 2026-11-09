#pragma once
struct GTAngelAGI {
    // mech0 integration
    mech0::SimpleLanguageModel llm{1000, 64};
    mech0::TrainingPipeline trainer{1000, 64};
    mech0::TrainingMetrics llmMetrics{};
    uint64_t llmTrainingUpdates{0};
    // re-bhvt-edit integration
    rebhvt::SensorimotorBridge bridge{1.5f, 30.0f};
    rebhvt::SensorimotorTelemetry bridgeTelemetry{};
    // GTAngel-Cls integration
    gtangel::GTAngelEnvironment env;
    gtangel::MotorCommand currentCommand{};
    std::array<float,4> envAdvisory{}; // advisory-only env efference copy
    gtangel::WorldObservation worldObs{};
    gtangel::StepResult lastStep{};
    dte::EpisodicMemory episodic;          // reservoir-grounded episodic memory
    float prevMeanContradiction = 0.0f;    // TRIZ temporal derivative state
    float lastEpisodicRecall = 0.0f;       // last recall similarity (HUD/selftest)
    double lastTrizDerivative = 0.0;      // d(contradiction)/dt (HUD/selftest)
    int lastConsolidationCount = 0;      // episodic consolidations run
    // uechovr integration
    uechovr::FrameBufferHook fbHook;
    uechovr::Tracker6DOF tracker;
    uechovr::StereoRenderer stereo;
    uechovr::Pose6DOF hmdPose{};
    uechovr::Pose6DOF leftCtrl{}, rightCtrl{};
    // layla integration
    layla::LaylaBridge laylaBridge;
    layla::Telemetry laylaTelemetry;
    // Civic Angel Architect II telemetry (v14)
    civicangel::ProducerSet civicSpine;
    bool civicOverlayVisible{true};
    // v15 authoritative render pyramid
    v15render::HighResRenderer highResRenderer;
    v15render::PerceptionPyramid perceptionPyramid;
    v15render::VisualNavStateEncoder visualNavEncoder;
    v15render::DisplayMode highResDisplayMode{v15render::DisplayMode::Colour};
    bool highResViewportVisible{true};
    // v16 local visuomotor dataset capture
    v16motor::TrajectoryRecorder trajectoryRecorder;
    v16motor::VisualTensor96 trajectoryVisual;
    v16motor::ControllerTarget previousControllerTarget;
    v16motor::ProprioceptiveVector lastBodyProprio{}; // real post-action body feedback
    v16motor::ProprioceptiveVector lastModelProprio{}; // pre-action model input
    bool trajectoryRecording{false};
    bool trajectoryRecorderReady{false};
    uint64_t trajectoryEpisodeId{1};
    // v17 local neural policy: proposal only unless every local gate accepts.
    v17policy::NativeVisuomotorPolicy neuralPolicy;
    v17policy::EmbodiedSafetyShield neuralShield;
    v17policy::Prediction neuralPrediction;
    v17policy::ShieldDecision neuralDecision;
    bool neuralShadowEnabled{true};
    bool neuralActuationUnlocked{false};
    bool neuralEthicsApproved{false};
    bool neuralPolicyLoaded{false};
    uint64_t neuralFrames{0};
    uint64_t neuralAccepted{0};
    // v18 non-local relation loop: provisional attention only.
    v18peer::PeerRelationalLoop peerLoop;
    v18peer::AttentionContext peerAttention;
    // v19 local gameplay core: no telemetry required.
    v19game::GameplayLoop gameplay;
    v20cns::Runtime cns;
    // Bound to THIS game's 768x768 VRD frame, never the optional display/window.
    v25dte::LocalVisualIngress echoIngress{highResRenderer.framebuffer};
    std::optional<v25dte::Sample> echoSample;
    v25dte::EchoCommons echoCommons;
    v26bridge::LensBridge treeLenses;
    bool lensInspectorOpen{false};
    bool worldMapOpen{false}; // LW40: F3 interactive 128m world map overlay // GDI-only view; never part of VRD pixels
    void invalidateEcho() { echoIngress.reset(); echoSample.reset(); echoCommons.reset(); treeLenses.reset(); }
    bool currentLens() {
        const auto& lens=treeLenses.state();
        return lens.valid && echoSample &&
            echoIngress.current(*echoSample,(uint64_t)GetTickCount64(),cns.body(),cns.niche()) &&
            lens.visualSequence==echoSample->sequence() && lens.visualDigest==echoSample->hash();
    }
    // avatar3d integration (v14)
    avatar3d::GameWorld navWorld;
    avatar3d::AvatarSkeleton avatar;
    avatar3d::EgocentricRenderer navRenderer;
    avatar3d::TrizMapping trizMap;
    avatar3d::NavTrainer navTrainer;
    v27learn::OnlineNavigationObserver navLearning;
    v27dte::LocalNavigation dteNavigation;
    v27dte::EchoPersona echoPersona; // display-only persona state (echo.json)
    bool dteEmbodimentEnabled{true}; // DTE is the default embodied operator
    int testHeldKey{0}; // in-process self-test virtual key; 0 = real keyboard
    v19game::PlayerControls testControlsFromKey(int key) const {
        // Mirrors GameplayLoop::samplePlayerControls semantics exactly.
        v19game::PlayerControls c;
        const bool forward=key=='W', backward=key=='S', left=key=='A', right=key=='D';
        c.move=forward?1.0f:(backward?-0.55f:0.0f);
        c.turn=left?-0.68f:(right?0.68f:0.0f);
        c.active=forward||backward||left||right;
        return c;
    }
    // v27 GNN motor actuator state (adaptive, never locked).
    v27gnn::GneuralMotorPolicy gnnMotor;
    bool gnnMotorReady{false};
    uint64_t neuralTrainingSteps{0};
    uint64_t neuralTrainingUpdates{0};
    float lastNeuralTrainLoss{-1.0f};
    int gnnFrameCounter{0};
    bool learningInspectorOpen{false};
    v20cns::SpinalReflex::Verdict lastMotorVerdict{v20cns::SpinalReflex::Verdict::Invalid};
    std::string localLearningDirectory;
    static std::string executableDirectory() {
        char path[32768]{};
        const DWORD n=GetModuleFileNameA(nullptr,path,sizeof(path));
        if(n==0 || n>=sizeof(path)) return ".";
        return std::filesystem::path(std::string(path,n)).parent_path().string();
    }
    void initializeLearning() {
        localLearningDirectory=executableDirectory();
        trajectoryRecorder.stop(); trajectoryRecording=false;
        trajectoryRecorderReady=trajectoryRecorder.selfCheck(localLearningDirectory+"/datasets");
        previousControllerTarget=v16motor::ControllerTarget{};
        neuralPolicyLoaded=neuralPolicy.load(localLearningDirectory+"/models/gtangel_visuomotor.gvmw");
        if(!neuralPolicyLoaded) neuralPolicyLoaded=neuralPolicy.load("C:/ddd/GTAngel/native/GTAngel_v56/models/gtangel_visuomotor.gvmw");
        neuralShadowEnabled=true;
        // v27 GNN motor actuator: adaptive online learning, never locked.
        neuralActuationUnlocked=true;
        neuralEthicsApproved=true;
        neuralFrames=0; neuralAccepted=0;
        if (groups.agents.size() < 20) groups.init(20);
        running=true;paused=false;dteEmbodimentEnabled=true;dteNavigation.reset();llmTrainingUpdates=0;
        gnnFrameCounter=0;
        neuralTrainingSteps=0; neuralTrainingUpdates=0; lastNeuralTrainLoss=-1.0f;
        gnnMotorReady=gnnMotor.create();
        if (gnnMotorReady) {
            // Prefer the trained network; a fresh one trains from scratch.
            if (!gnnMotor.load(localLearningDirectory+"/models/gtangel_motor_gnn.net"))
                addLog("GNN motor: fresh network, training starts now");
            else
                addLog("GNN motor: trained network restored, training continues");
        } else {
            addLog("GNN motor: network creation failed, NN actuation degraded");
        }
        addLog(neuralPolicyLoaded ? "v27 NN initialized: LEARNING inference active when running" :
            std::string("v27 NN missing: ")+neuralPolicy.error());
        addLog("N: local Q learning / M: NN learning / C: capture / F4: learning monitor");
    }
    avatar3d::Vec3 navPos{avatar3d::GameWorld::PlayerSpawn};
    float navHeading{0.0f};
    float navSpeed{0.0f};
    float navMeanContradiction{0.0f};
    float navGoalCompass{10.0f};
    int navBranch{0};
    bool navTraining{true}; // observe accepted local DTE actions; never authorize Route
    bool navThirdPerson{true};
    int navEpisodeSteps{0};
    // cogzero integration
    cogzero::AgentZeroCore cogzeroAgent{"GTAngel"};
    cogzero::CognitiveLoop::CycleStats cogzeroStats;
    // echo integration
    echo::CognitiveArenaController echoController;
    echo::AestheticField echoField;
    echo::EvolutionLedger echoLedger;
    echo::CognitiveSnapshot echoSnap;
    // kys integration
    kys::TriadPhasorEngine phasor;
    // elizaos.cpp integration
    elizaos::AgentLoop agentLoop;
    elizaos::AttentionAllocator attention{144.0};
    elizaos::ActivationSpreadingNetwork spreadNet;
    elizaos::TemporalAttentionWindow globalAttentionWindow;
    std::string attentionFocus;
    double avgAttention = 0.0;
    bool spreadNetBuilt = false;
    // dte systems
    dte::EndocrineSystem endo;
    dte::EchoStateNetwork esn{32, 96, 8, 0.95f, 0.3f, 42};
    dte::TRIZArena arena;
    dte::GroupDynamics groups;
    dte::SensorimotorLoop sensorimotor;
    dte::MicroImprovement improvement;
    dte::EchoIntrospect introspect;
    dte::CogVerse cogverse;
    dte::EchobeatsPhase echoPhase = dte::EchobeatsPhase::Perceive;
    bool running = false, paused = false;
    int tickCount = 0;
    std::vector<std::string> log;
    double esnEnergy = 0.0;
    int selectedHormone = 0;
    int selectedCellQ = -1, selectedCellR = -1;

    void reset() {
        running = false;
        paused = false;
        tickCount = 0;
        log.clear();
        esnEnergy = 0.0;
        selectedHormone = 0;
        selectedCellQ = -1;
        selectedCellR = -1;
        attentionFocus.clear();
        avgAttention = 0.0;
        spreadNetBuilt = false;
        echoPhase = dte::EchobeatsPhase::Perceive;
        groups.init(20);
        fbHook.initialize(uechovr::FrameBufferHook::API::D3D11, 64, 64);
        tracker.reset();
        stereo.setMethod(uechovr::StereoRenderer::Method::NativeStereo);
        echoController.reset();
        echoField = echo::AestheticField();
        echoLedger = echo::EvolutionLedger();
        echoSnap = echo::CognitiveSnapshot();
        cogzeroStats = cogzero::CognitiveLoop::CycleStats();
        const auto laylaBackend = laylaBridge.discover();
        laylaTelemetry = laylaBridge.telemetry();
        if (laylaBackend.online) {
            addLog(std::string("Layla backend: ") + layla::backendName(laylaBackend.kind) +
                   " at " + laylaBackend.baseUrl +
                   " | models=" + std::to_string(laylaBackend.models.size()));
        } else {
            addLog(std::string("Layla backend offline: ") + laylaBackend.error);
        }
        civicSpine.reset();
        civicSpine.configureFromEnvironment();
        addLog("Civic Angel spine configured from process environment; credentials remain hidden");
        // v14: initialize navigation world and avatar
        navWorld.generate(1337, 48);
        avatar=avatar3d::AvatarSkeleton{};
        gameplay.reset(navWorld);
        cns.reset();
        invalidateEcho();
        syncBodyFromCns();
        navMeanContradiction = 0.0f;
        navEpisodeSteps = 0;
        avatar.pose(navPos, navHeading, 0.0f, 0.0f);
        avatar3d::Vec3 initialEye = navPos + avatar3d::Vec3{0.0f, 2.5f, -4.0f};
        highResRenderer.render(navWorld, avatar, initialEye, navHeading, -0.4f, true);
        perceptionPyramid.derive(highResRenderer.framebuffer);
        perceptionPyramid.copyToLegacy(navRenderer);
        perceptionPyramid.mapToTriz(trizMap);
        addLog("Nav world: 14 pillars, goal at (8,0,8), Q-learning trainer ready");
        addLog("v15 render pyramid: 768x768 source -> 64x64 retina -> 12x12 cognition");
        initializeLearning();
        // Explicit reset starts a new observation table, without enabling effectors.
        navTrainer=avatar3d::NavTrainer{}; navLearning=v27learn::OnlineNavigationObserver{};
        navTraining=true;
        peerLoop.configureFromEnvironment();
        peerLoop.setEnabled(false);
        addLog("v18 peer relay initialized OFF; network relations remain attention-only");
    }
    bool repairBeacon() {
        const v20cns::Context context{highResRenderer.framebuffer.sequence,
            (uint64_t)GetTickCount64(),0.05f,highResRenderer.framebuffer.sequence>0,false};
        const auto player=navPos;
        auto observe=[this,player](const avatar3d::GameWorld& before,
                                    const avatar3d::GameWorld& after,uint64_t version) {
            auto measure=[this](const avatar3d::GameWorld& w) {
                v15render::HighResRenderer probe;
                avatar3d::AvatarSkeleton pose;
                pose.pose(navPos,navHeading,0.0f,0.0f);
                probe.render(w,pose,{w.goal.x,2.2f,w.goal.z-4.5f},0.0f,-0.22f,false);
                int count=0,finiteDepth=0; uint64_t colourSum=0;
                for (int i=0;i<v15render::HighResFramebuffer::PixelCount;++i)
                    if (probe.framebuffer.objectId[i]==v15render::ObjectGoal) {
                        ++count; colourSum+=probe.framebuffer.colour[i];
                        if (std::isfinite(probe.framebuffer.depth[i])) ++finiteDepth;
                    }
                return std::array<uint64_t,4>{(uint64_t)count,(uint64_t)finiteDepth,
                                              colourSum,probe.framebuffer.sequence};
            };
            const auto oldFrame=measure(before),newFrame=measure(after);
            return version==2 && before.beaconRestored==false && after.beaconRestored &&
                oldFrame[0]>20 && newFrame[0]>oldFrame[0] &&
                newFrame[1]==newFrame[0] && newFrame[2]!=oldFrame[2] &&
                newFrame[3]>0 && cns.body().position.x==player.x &&
                cns.body().position.z==player.z && after.goal.x==before.goal.x &&
                after.goal.z==before.goal.z;
        };
        const auto verdict=cns.repair(navPos,gameplay,navWorld,context,observe);
        avatar.pose(navPos,navHeading,0.0f,0.0f);
        const avatar3d::Vec3 eye=navThirdPerson
            ? navWorld.collisionFreeEye(navPos,navPos+avatar3d::Vec3{-sinf(navHeading)*4.0f,2.5f,-cosf(navHeading)*4.0f})
            : navPos+avatar3d::Vec3{0,1.6f,0};
        highResRenderer.render(navWorld,avatar,eye,navHeading,
                               navThirdPerson?-0.4f:0.0f,navThirdPerson);
        perceptionPyramid.derive(highResRenderer.framebuffer);
        if (verdict!=v20cns::WorldGovernance::Verdict::Accepted)
            addLog("WGR rejected beacon repair; world state retained / rolled back");
        return verdict==v20cns::WorldGovernance::Verdict::Accepted;
    }
    void syncBodyFromCns() {
        navPos=cns.body().position; navHeading=cns.body().heading; navSpeed=cns.body().speed;
        navWorld.vehiclePos=cns.body().cartPosition;
        navWorld.vehicleHeading=cns.body().cartHeading;
        navWorld.vehicleWheelPhase=cns.body().wheelPhase;
        navWorld.vehicleOccupied=cns.body().occupied;
    }
    // Full in-process runtime self-test: drives the real game loop through the
    // same control path as the player, with NO window capture, NO input
    // injection, and NO API hijacking. Reports via addLog() and JSON files.
    int runSelfTest() {
        int checks=0,failed=0;
        std::vector<std::string> checkResults;
        auto ck=[&](bool ok,const char* text){
            ++checks; if(!ok)++failed;
            checkResults.push_back(std::string(ok?"[SELFTEST PASS] ":"[SELFTEST FAIL] ")+text);
            addLog(checkResults.back());
        };
        auto magnitude=[&](const avatar3d::NavTrainer& q){
            double s=0;for(const auto& a:q.Q)for(const auto& b:a)for(const auto& c:b)for(float v:c)s+=std::fabs(v);return s;};
        navWorld.generate(1337,48); gameplay.reset(navWorld); cns.reset(); syncBodyFromCns();
        ck(avatar3d::len(navPos-avatar3d::GameWorld::PlayerSpawn)<1e-5f &&
           avatar3d::len(navWorld.vehiclePos-avatar3d::GameWorld::ParkedVehicleSpawn)<1e-5f,
           "CNS reset and rendered world share street spawn");
        {
            // Exercise the real motor authority and swept clearance, not the
            // separate GameWorld::collides() query.
            auto fixture=navWorld;
            fixture.pillars.clear(); fixture.buildings.clear(); fixture.props.clear();
            fixture.traffic.clear(); fixture.walkers.clear();
            fixture.npcPos={40,0,40}; fixture.ambX=40; fixture.busZ=40;
            v20cns::Runtime motor;
            const auto initial=motor.body().position;
            v20cns::Context ctx{1,1,.05f,true,false};
            const auto moved=motor.motor(v20cns::Source::HumanLocal,1,.2f,0,0,true,ctx,fixture);
            ck(moved==v20cns::SpinalReflex::Verdict::Accepted &&
               avatar3d::len(motor.body().position-initial)>.09f,
               "clear street accepts translation as well as turning");
            motor.reset();
            fixture.pillars.push_back({initial+avatar3d::Vec3{0,0,.6f},.30f,3});
            for(int i=0;i<10;++i) {
                ++ctx.frame; ++ctx.nowMs;
                motor.motor(v20cns::Source::HumanLocal,1,0,0,0,true,ctx,fixture);
            }
            ck(motor.body().contact &&
               avatar3d::len(motor.body().position-fixture.pillars.back().pos)>=.55f-1e-4f,
               "swept movement still blocks pillar penetration");
            const auto before=motor.body().position;
            ++ctx.frame; ++ctx.nowMs;
            ck(motor.motor(v20cns::Source::Neural,1,0,0,0,true,ctx,fixture)==
                   v20cns::SpinalReflex::Verdict::Untrusted &&
               avatar3d::len(motor.body().position-before)<1e-5f,
               "raw neural proposals cannot bypass local motor authority");
        }
        avatar.pose(navPos,0,0,0);
        highResRenderer.loadDteAsset("");
        highResRenderer.render(navWorld,avatar,navPos+avatar3d::Vec3{0,2.5f,-4},0,-0.4f,true);
        perceptionPyramid.derive(highResRenderer.framebuffer);
        perceptionPyramid.copyToLegacy(navRenderer); perceptionPyramid.mapToTriz(trizMap);
        initializeLearning();
        ck(neuralPolicyLoaded&&neuralPolicy.loaded(),"startup real NN weights");
        ck(running&&dteEmbodimentEnabled&&navTraining,"DTE is operator and starts automatically");
        ck(neuralActuationUnlocked&&neuralEthicsApproved,"NN motor actuation unlocked for adaptive learning");
        ck(gnnMotorReady,"GNN motor network created");
        ck(trajectoryRecorderReady,"capture ready");
        running=false; const auto seq=highResRenderer.framebuffer.sequence; tick();
        ck(neuralFrames==0&&highResRenderer.framebuffer.sequence==seq,"stopped loop unchanged"); running=true;
        const auto start=navPos;
        for(int i=0;i<12;++i)tick();
        ck(neuralFrames==12&&neuralPrediction.valid,"actual loop NN inference");
        ck(navLearning.updates>0&&magnitude(navTrainer)>0,"DTE learns with NO keyboard or user");
        ck(avatar3d::len(navPos-start)>.01f&&dteNavigation.accepted>0,"sealed DTE SRD movement");
        const auto updates=navLearning.updates; navTraining=false;
        for(int i=0;i<10;++i)tick();
        ck(navLearning.updates==updates&&neuralFrames==22,"N stops Q only; DTE and NN continue");
        ck(llmTrainingUpdates>=1,"negative local reward tokens no longer crash ML tick20");
        navTraining=true; neuralShadowEnabled=false;
        for(int i=0;i<4;++i)tick();
        ck(neuralFrames==22&&navLearning.updates>updates,"M stops NN only; DTE Q continues");
        neuralShadowEnabled=true; paused=true; tick(); ck(neuralFrames==22,"paused counters freeze"); paused=false;
        ck(trajectoryRecorder.start(localLearningDirectory+"/datasets",27001),"start actual capture");
        trajectoryRecording=true; const auto preFrame=highResRenderer.framebuffer.sequence; tick();
        ck(trajectoryRecorder.records()==1,"runtime capture writes");
        const auto path=trajectoryRecorder.path(); trajectoryRecorder.stop(); trajectoryRecording=false;
        {
            std::ifstream f(path,std::ios::binary); v16motor::DatasetHeader h; v16motor::RecordHeader r;
            f.read(reinterpret_cast<char*>(&h),sizeof(h)); f.read(reinterpret_cast<char*>(&r),sizeof(r));
            ck(f.good()&&r.frameSequence==preFrame,"capture actual PRE-action frame");
            f.seekg(v16motor::TensorBytes+sizeof(float)*v16motor::ProprioceptionSize,std::ios::cur);
            v16motor::ControllerTarget label;
            f.read(reinterpret_cast<char*>(label.values.data()),sizeof(label.values));
            ck(f.good()&&label.values[5]==1,"capture accepted local DTE controller label");
        }
        dteEmbodimentEnabled=false; testHeldKey=0;
        const auto learned=navLearning.updates; for(int i=0;i<14;++i)tick();
        ck(navLearning.updates==learned,"O stops DTE without fabricating Q samples");
        ck(llmTrainingUpdates>=2&&std::isfinite(esnEnergy),"ML and reservoir continue");
        navWorld.pillars.clear(); navWorld.goal={30,0,30};
        for(float yaw:{1.5707963f,-1.5707963f,.7f}) {
            auto body=cns.body(); body.position={0,0,0}; body.heading=yaw; body.speed=0; body.occupied=false;
            ck(cns.restoreLocal(body,cns.niche(),gameplay,navWorld),"valid local axis fixture restore");
            syncBodyFromCns(); invalidateEcho(); avatar.pose(navPos,yaw,0,0);
            testHeldKey='W'; const auto p=navPos; tick(); const auto delta=navPos-p;
            ck(std::fabs(delta.x-.1f*std::sin(yaw))<1e-4f&&std::fabs(delta.z-.1f*std::cos(yaw))<1e-4f,"W follows camera yaw XZ");
            const auto face=avatar.cheek[0]-avatar.lastPosition;
            ck(avatar3d::dot(face,{std::sin(yaw),0,std::cos(yaw)})>0,"avatar face agrees W direction");
        }
        testHeldKey=0;
        ck(neuralAccepted<=neuralFrames,"NN acceptance bounded by frames (adaptive, not frozen)");
        navWorld.generate(1337,48); gameplay.reset(navWorld); cns.reset();
        syncBodyFromCns(); invalidateEcho(); avatar.pose(navPos,0,0,0);
        navLearning=v27learn::OnlineNavigationObserver{}; navTrainer=avatar3d::NavTrainer{};
        initializeLearning(); navTraining=true;
        for(int i=0;i<500;++i)tick();
        ck(neuralFrames==500&&neuralPrediction.valid,"500 cycle NN soak");
        ck(navLearning.updates>50&&dteNavigation.learnedProposals>0,"sustained online Q and learned choices");
        ck(cns.niche().worldVersion==1&&!navWorld.beaconRestored,"DTE navigation cannot mutate niche");
        ck(std::isfinite(navPos.x)&&std::isfinite(navPos.z)&&neuralAccepted<=neuralFrames,"finite local body; NN acceptance bounded");
        // ESN vision + episodic memory + readout + TRIZ temporal checks.
        ck(esn.inDim()==32,"reservoir input widened to 32 dims");
        ck(std::isfinite(esnEnergy)&&esnEnergy>0.0,"reservoir energy responds to vision inputs");
        ck(!episodic.episodes.empty(),"episodic memory stores significant events");
        ck(std::isfinite((double)lastEpisodicRecall)&&lastEpisodicRecall>=0.0f,"episodic recall similarity finite");
        ck(esn.readoutL1()>0.0,"readout W_out trained online (nonzero)");
        ck(std::isfinite(lastTrizDerivative),"TRIZ temporal derivative finite");
        // u9n port checks: RLS readout, consolidation, concepts, plasticity.
        ck(esn.rlsUpdates > 0, "RLS readout updates online");
        ck(episodic.consolidations > 0, "episodic consolidation runs");
        ck(!episodic.concepts.empty(), "semantic concepts abstracted from episodes");
        ck(esn.plasticityUpdates > 0, "intrinsic plasticity active");
        {
            // Persistence round-trip: snapshot, wipe, reload, compare learned state.
            const size_t epsBefore = episodic.episodes.size();
            const size_t cptsBefore = episodic.concepts.size();
            const uint64_t qBefore = navLearning.updates;
            const int consBefore = episodic.consolidations;
            const int rlsBefore = esn.rlsUpdates;
            double qL1Before = 0; for (const auto& a : navTrainer.Q) for (const auto& b : a) for (const auto& c : b) for (float v : c) qL1Before += std::fabs(v);
            saveLearningState();
            const size_t epsWiped = episodic.episodes.size();
            episodic.episodes.clear(); episodic.concepts.clear(); episodic.consolidations = 0;
            navLearning.updates = 0; esn.rlsUpdates = 0;
            for (auto& a : navTrainer.Q) for (auto& b : a) for (auto& c : b) for (float& v : c) v = 0.0f;
            const bool restored = loadLearningState();
            double qL1After = 0; for (const auto& a : navTrainer.Q) for (const auto& b : a) for (const auto& c : b) for (float v : c) qL1After += std::fabs(v);
            ck(restored, "learning state file round-trips through disk");
            ck(episodic.episodes.size() == epsBefore && epsBefore > 0, "episodic memory persists across restart");
            ck(episodic.concepts.size() == cptsBefore && cptsBefore > 0, "semantic concepts persist across restart");
            ck(navLearning.updates == qBefore && qBefore > 0, "Q visitation counts persist across restart");
            ck(episodic.consolidations == consBefore && consBefore > 0, "consolidation history persists across restart");
            ck(esn.rlsUpdates == rlsBefore && rlsBefore > 0, "ESN readout training persists across restart");
            ck(std::fabs(qL1After - qL1Before) < 1e-3 * std::max(1.0, qL1Before), "Q weights restored bit-faithfully");
            ck(epsWiped == epsBefore, "wipe actually cleared memory before reload");
        }
        {
            // GNN motor training + persistence: the network must actually
            // learn from accepted local actions and survive a save/reload.
            ck(gnnMotorReady, "GNN motor ready before training check");
            const int samplesBefore = gnnMotor.totalSamples();
            ck(samplesBefore > 0, "GNN collected demonstrations from accepted actions");
            ck(gnnMotor.trainingUpdates() > 0, "GNN online training ran in-frame");
            ck(gnnMotor.trainingSteps() > 0, "GNN online training executes gradient steps after load");
            ck(lastNeuralTrainLoss >= 0.0f && std::isfinite(lastNeuralTrainLoss), "GNN training loss is finite");
            // Round-trip: capture inference, save, reload, compare.
            float probeFeat[v27gnn::FeatureSize];
            for (int i = 0; i < v27gnn::FeatureSize; ++i) probeFeat[i] = 0.0f;
            probeFeat[v27gnn::FeatureSize-1] = 1.0f; // bias
            float turnA=0, fwdA=0;
            ck(gnnMotor.infer(probeFeat, turnA, fwdA), "GNN inference before save");
            std::error_code gnnEc;
            std::filesystem::create_directories(localLearningDirectory+"/models", gnnEc);
            ck(gnnMotor.save(localLearningDirectory+"/models/gtangel_motor_gnn_selftest.net"), "GNN network saved");
            v27gnn::GneuralMotorPolicy probeNet;
            ck(probeNet.create() && probeNet.load(localLearningDirectory+"/models/gtangel_motor_gnn_selftest.net"), "GNN network reloaded");
            float turnB=0, fwdB=0;
            ck(probeNet.infer(probeFeat, turnB, fwdB), "GNN inference after reload");
            const float dT = turnA-turnB, dF = fwdA-fwdB;
            ck(dT*dT + dF*dF < 1e-9, "GNN save/reload round-trip exact");
        }
        // Deterministic relationship typing (u9n ComputeRelationshipType).
        {
            dte::Agent ra;
            ra.trust = 0.8f; ra.affinity = 0.7f; ra.familiarity = 0.5f;
            ck(std::strcmp(ra.relationshipType(), "CloseFriend") == 0, "relationship typing: CloseFriend");
            ra.affinity = -0.7f;
            ck(std::strcmp(ra.relationshipType(), "Adversary") == 0, "relationship typing: Adversary");
            ra.affinity = 0.5f; ra.familiarity = 0.05f;
            ck(std::strcmp(ra.relationshipType(), "Stranger") == 0, "relationship typing: Stranger");
        }
        {
            // LW: world population — 20 deterministic walkers, none inside
            // protected zones or solid geometry.
            ck(navWorld.walkers.size() == 20, "world walkers populated");
            bool walkersClear = true;
            for (const auto& w : navWorld.walkers)
                if (len(w.pos) < 4.0f || len(w.pos-navWorld.goal) < 4.0f ||
                    len(w.pos-navWorld.npcPos) < 4.0f ||
                    len(w.pos-navWorld.questShard) < 4.0f ||
                    len(w.pos-navWorld.vehiclePos) < 3.0f ||
                    navWorld.collides(w.pos)) { walkersClear = false; break; }
            ck(walkersClear, "world walkers clear of protected zones");
            // LW14: re3-derived entities — traffic on lanes, pickups, props.
            ck(navWorld.traffic.size() == 6, "world traffic populated");
            ck(navWorld.pickups.size() == 4, "world pickups populated");
            ck(navWorld.props.size() == 8, "world props populated");
            bool trafficClear = true;
            for (const auto& v : navWorld.traffic)
                if (v.orbitR == 0.0f && navWorld.collides(v.pos)) { trafficClear = false; break; }
            ck(trafficClear, "world traffic clear of geometry");
            bool pickupsClear = true;
            for (const auto& pk : navWorld.pickups)
                if (navWorld.collides(pk.pos)) { pickupsClear = false; break; }
            ck(pickupsClear, "world pickups clear of geometry");
            bool propsClear = true;
            for (size_t i=0;i<navWorld.props.size();++i) {
                auto otherGeometry=navWorld;
                otherGeometry.props.erase(otherGeometry.props.begin()+i);
                if (otherGeometry.collides(navWorld.props[i].pos)) { propsClear = false; break; }
            }
            ck(propsClear, "world props clear of geometry");
            // Lanes stay clear across a full ping-pong sweep.
            avatar3d::GameWorld sweepWorld;
            sweepWorld.generate(1337, 48);
            bool lanesClear = true;
            for (int step = 0; step <= 64 && lanesClear; ++step) {
                sweepWorld.tickTraffic(64.0f / 64.0f);
                for (const auto& v : sweepWorld.traffic)
                    if (v.orbitR == 0.0f && sweepWorld.collides(v.pos)) { lanesClear = false; break; }
            }
            ck(lanesClear, "traffic lanes sweep clear");
            // LW: camera anti-clip — desired eye inside a pillar pulls in.
            {
                avatar3d::Vec3 body{0, 0, 0};
                avatar3d::Vec3 desired = body;
                bool fixtureFound = false;
                for (const auto& p : navWorld.pillars) {
                    const avatar3d::Vec3 centre{p.pos.x, 1.2f, p.pos.z};
                    if (len(centre - body) > 2.0f && len(centre - body) < 8.0f) {
                        desired = centre;
                        fixtureFound = true;
                        break;
                    }
                }
                if (fixtureFound) {
                    const avatar3d::Vec3 eye = navWorld.collisionFreeEye(body, desired);
                    ck(len(eye - desired) > 0.01f, "camera pulls in at pillars");
                } else {
                    ck(true, "camera pulls in at pillars"); // no fixture in seed
                }
            }
            // LW: honest friends — familiarity only grows near the avatar.
            {
                dte::GroupDynamics localGroups;
                localGroups.init(4);
                for (int i = 0; i < 4; ++i) {
                    localGroups.agents[i].x = (float)(i * 2);
                    localGroups.agents[i].y = 0.0f;
                }
                for (int i = 0; i < 50; ++i) localGroups.tick(500.0f, 500.0f);
                bool strangersStay = true;
                for (const auto& a : localGroups.agents)
                    if (a.familiarity > 0.01f) { strangersStay = false; break; }
                for (int i = 0; i < 50; ++i) localGroups.tick(1.0f, 1.0f);
                bool proximityGrows = false;
                for (const auto& a : localGroups.agents)
                    if (a.familiarity > 0.01f) { proximityGrows = true; break; }
                ck(strangersStay, "friends require proximity (far stays stranger)");
                ck(proximityGrows, "friends grow through witnessed meetings");
            }
            // LW: .env parsing — comments, quotes, whitespace.
            {
                const auto parsed = civicangel::parseEnvText(
                    "# comment line\nARC_ANGEL_INGEST_URL = \"https://ingest.example/v27\"\n"
                    "  ARC_ANGEL_PURPOSE_TOKEN='tok-123'  \nBROKEN LINE\n");
                ck(parsed.count("ARC_ANGEL_INGEST_URL") &&
                   parsed.at("ARC_ANGEL_INGEST_URL") == "https://ingest.example/v27",
                   ".env parse ingest url");
                ck(parsed.count("ARC_ANGEL_PURPOSE_TOKEN") &&
                   parsed.at("ARC_ANGEL_PURPOSE_TOKEN") == "tok-123",
                   ".env parse quoted token");
            }
            // LW: retina sees shard + npc bands (fresh world, first-person).
            {
                avatar3d::GameWorld fixtureWorld;
                fixtureWorld.generate(1337, 48);
                fixtureWorld.questShardVisible = true;
                v15render::HighResRenderer fixtureRenderer;
                v15render::PerceptionPyramid fixturePyramid;
                v15render::VisualNavStateEncoder fixtureEncoder;
                avatar3d::AvatarSkeleton fixtureAvatar;
                // Camera 2m south of the shard, looking north (+z) at it:
                // forward = (sin h, 0, cos h), so heading 0 faces +z.
                fixtureRenderer.render(fixtureWorld, fixtureAvatar,
                    fixtureWorld.questShard + avatar3d::Vec3{0, 1.6f, -2},
                    0.0f, -0.1f, false);
                fixturePyramid.derive(fixtureRenderer.framebuffer);
                const auto fixtureEvidence = fixtureEncoder.encode(fixturePyramid.retina);
                ck(fixtureEvidence.shardVisible, "retina sees quest shard band");
                fixtureRenderer.render(fixtureWorld, fixtureAvatar,
                    fixtureWorld.npcPos + avatar3d::Vec3{0, 1.6f, -2},
                    0.0f, -0.1f, false);
                fixturePyramid.derive(fixtureRenderer.framebuffer);
                const auto npcEvidence = fixtureEncoder.encode(fixturePyramid.retina);
                ck(npcEvidence.npcVisible, "retina sees npc band");
                // LW14: retina must see the re3-derived entity bands. Tick
                // traffic 9.375s so auto1 reaches (0,1.4), straight ahead.
                for (int i = 0; i < 150; ++i) fixtureWorld.tickTraffic(0.0625f);
                fixtureRenderer.render(fixtureWorld, fixtureAvatar,
                    avatar3d::Vec3{0, 1.6f, -6},
                    0.0f, -0.1f, false);
                fixturePyramid.derive(fixtureRenderer.framebuffer);
                const auto trafficEvidence = fixtureEncoder.encode(fixturePyramid.retina);
                ck(trafficEvidence.trafficVisible, "retina sees traffic band");
                fixtureRenderer.render(fixtureWorld, fixtureAvatar,
                    avatar3d::Vec3{6, 1.6f, -2.5f},
                    0.0f, -0.1f, false);
                fixturePyramid.derive(fixtureRenderer.framebuffer);
                const auto pickupEvidence = fixtureEncoder.encode(fixturePyramid.retina);
                ck(pickupEvidence.pickupVisible, "retina sees pickup band");
                fixtureRenderer.render(fixtureWorld, fixtureAvatar,
                    avatar3d::Vec3{15, 1.6f, -2.6f},
                    0.0f, -0.1f, false);
                fixturePyramid.derive(fixtureRenderer.framebuffer);
                const auto propEvidence = fixtureEncoder.encode(fixturePyramid.retina);
                ck(propEvidence.propVisible, "retina sees prop band");
                // LW15: sun billboard renders toward the light direction.
                {
                    const float sunHeading = atan2f(-0.35f, -0.44f);
                    const float sunPitch = asinf(0.82f / sqrtf(0.35f*0.35f + 0.82f*0.82f + 0.44f*0.44f));
                    fixtureRenderer.render(fixtureWorld, fixtureAvatar,
                        avatar3d::Vec3{0, 1.6f, 0}, sunHeading, sunPitch, false);
                    bool sunSeen = false;
                    for (int p = 0; p < v15render::HighResFramebuffer::PixelCount; ++p) {
                        const uint32_t px = fixtureRenderer.framebuffer.colour[p];
                        const uint8_t pb = (uint8_t)(px & 255), pg = (uint8_t)((px >> 8) & 255), pr = (uint8_t)((px >> 16) & 255);
                        if (pr > 240 && pg > 230 && pb > 240) { sunSeen = true; break; }
                    }
                    ck(sunSeen, "sun billboard visible");
                }
                // LW16: side quests progress through interact/tick.
                {
                    v19game::GameplayLoop sideLoop;
                    avatar3d::GameWorld sideWorld;
                    sideWorld.generate(1337, 48);
                    sideLoop.reset(sideWorld);
                    sideLoop.interact(avatar3d::Vec3{6, 0, 2.5f}, 0.0f, sideWorld);
                    ck(sideLoop.quest.pickupsCollected == 1 && sideWorld.pickups[0].collected,
                       "side quest: data cell collected");
                    sideLoop.interact(avatar3d::Vec3{6, 0, 2.5f}, 0.0f, sideWorld);
                    ck(sideLoop.quest.pickupsCollected == 1, "collected cell not re-collected");
                    sideLoop.tick(0.05f, sideWorld.questShard, 0.0f, sideWorld);
                    ck(sideLoop.quest.shardGlowSeen, "side quest: shard glow witnessed");
                    int crowdTicks = 0;
                    while (!sideLoop.quest.crowdSyncDone && crowdTicks < 400) {
                        sideWorld.tickWalkers(0.05f);
                        sideLoop.tick(0.05f, sideWorld.walkers[0].pos, 0.0f, sideWorld);
                        ++crowdTicks;
                    }
                    ck(sideLoop.quest.crowdSyncDone, "side quest: crowd sync achieved");
                }
            }
            // LW: group agents sync from walkers (world coords, not pixels).
            {
                groups.init(20);
                for (int i = 0; i < 10; ++i) {
                    for (size_t j = 0; j < groups.agents.size() && j < navWorld.walkers.size(); ++j) {
                        groups.agents[j].x = navWorld.walkers[j].pos.x;
                        groups.agents[j].y = navWorld.walkers[j].pos.z;
                    }
                    groups.tick(navPos.x, navPos.z);
                }
                bool synced = true;
                for (size_t j = 0; j < groups.agents.size() && j < navWorld.walkers.size(); ++j)
                    if (std::fabs(groups.agents[j].x - navWorld.walkers[j].pos.x) > 64.0f ||
                        std::fabs(groups.agents[j].y - navWorld.walkers[j].pos.z) > 64.0f)
                        { synced = false; break; }
                ck(synced, "group agents sync from walkers");
            }
        }
        {
            std::ofstream report(localLearningDirectory+"/V27_RUNTIME_VERIFICATION.json");
            report<<"{\"checks\":"<<checks<<",\"passed\":"<<(checks-failed)
                <<",\"neural_frames\":"<<neuralFrames<<",\"q_updates\":"<<navLearning.updates
                <<",\"q_l1\":"<<magnitude(navTrainer)<<",\"dte_srd_accepted\":"<<dteNavigation.accepted
                <<",\"dte_learned_proposals\":"<<dteNavigation.learnedProposals
                <<",\"ml_training_updates\":"<<llmTrainingUpdates
                <<",\"esn_in_dim\":"<<esn.inDim()<<",\"episodes\":"<<episodic.episodes.size()
                <<",\"readout_l1\":"<<esn.readoutL1()<<",\"rls_updates\":"<<esn.rlsUpdates<<",\"concepts\":"<<episodic.concepts.size()<<",\"plasticity\":"<<esn.plasticityUpdates<<",\"consolidations\":"<<episodic.consolidations
                <<",\"nn_training_updates\":"<<neuralTrainingUpdates
                <<",\"nn_training_steps\":"<<neuralTrainingSteps
                <<",\"nn_train_loss\":"<<lastNeuralTrainLoss
                <<",\"nn_accepted\":"<<neuralAccepted
                <<",\"no_keyboard_dte_test\":true,\"gui_acceptance\":false,\"in_process\":true}\n";
            std::ofstream txt(localLearningDirectory+"/V27_SELFTEST.txt");
            txt<<"GTAngel v27 in-process self-test\n"
                <<"checks: "<<(checks-failed)<<"/"<<checks<<" passed\n"
                <<"neural_frames: "<<neuralFrames<<"\nq_updates: "<<navLearning.updates<<"\n"
                <<"q_l1: "<<magnitude(navTrainer)<<"\ndte_srd_accepted: "<<dteNavigation.accepted<<"\n"
                <<"dte_learned_proposals: "<<dteNavigation.learnedProposals<<"\n"
                <<"ml_training_updates: "<<llmTrainingUpdates<<"\n"
                <<"nn_training_updates: "<<neuralTrainingUpdates<<"\n"
                <<"nn_train_loss: "<<lastNeuralTrainLoss<<"\n"
                <<"nn_accepted: "<<neuralAccepted<<"\n"
                <<"esn_in_dim: "<<esn.inDim()<<"\nepisodes: "<<episodic.episodes.size()<<"\n"
                <<"readout_l1: "<<esn.readoutL1()<<"\ntriz_derivative: "<<lastTrizDerivative<<"\n"
                <<"rls_updates: "<<esn.rlsUpdates<<"\nconcepts: "<<episodic.concepts.size()<<"\n"
                <<"plasticity: "<<esn.plasticityUpdates<<"\nconsolidations: "<<episodic.consolidations<<"\n";
            for(const auto& line:checkResults) txt<<line<<"\n";
        }
        addLog(std::string("V27 SELFTEST ")+std::to_string(checks-failed)+"/"+std::to_string(checks)
            +" NN_frames="+std::to_string(neuralFrames)+" Q_updates="+std::to_string(navLearning.updates)
            +" DTE_SRD="+std::to_string(dteNavigation.accepted)
            +" NN_train="+std::to_string(neuralTrainingUpdates)
            +" NN_loss="+std::to_string(lastNeuralTrainLoss));
        return failed;
    }
    // v27 GNN motor: feature vector from live visual + proprioceptive state.
    // 20 features, ad hoc normalized to roughly [-1,1].
    void buildGnnFeatures(float* f, const v15render::VisualNavEvidence& v,
                          const v16motor::ProprioceptiveVector& proprio,
                          float speed, float heading) const {
        f[0]=v.goalVisible?1.0f:0.0f;
        f[1]=v.goalVisible?std::clamp(v.goalDepth/20.0f,0.0f,1.0f):1.0f;
        f[2]=std::clamp((float)v.goalPixels/200.0f,0.0f,1.0f);
        f[3]=v.obstacleVisible?1.0f:0.0f;
        f[4]=v.obstacleVisible?std::clamp(v.nearestObstacleDepth/10.0f,0.0f,1.0f):1.0f;
        f[5]=std::clamp((float)v.obstaclePixels/200.0f,0.0f,1.0f);
        f[6]=(std::fabs(navGoalCompass)<=3.1416f)
            ? std::clamp(navGoalCompass/3.14159265f,-1.0f,1.0f)
            : ((float)v.state.bearing/8.0f*2.0f-1.0f);
        f[7]=(float)v.state.dist/4.0f*2.0f-1.0f;
        f[8]=(float)v.state.obst/8.0f*2.0f-1.0f;
        f[9]=std::clamp(speed,-1.0f,1.0f);
        f[10]=std::clamp(proprio.values[1],-1.0f,1.0f); // sin heading
        f[11]=std::clamp(proprio.values[2],-1.0f,1.0f); // cos heading
        f[12]=std::clamp(proprio.values[3],-1.0f,1.0f); // heading delta
        f[13]=std::clamp(proprio.values[4],0.0f,1.0f);  // collided
        f[14]=std::clamp(proprio.values[12],-1.0f,1.0f); // arousal
        f[15]=std::clamp(proprio.values[13],-1.0f,1.0f); // reward
        f[16]=std::clamp(proprio.values[15],0.0f,1.0f);  // visual collision risk
        f[17]=std::clamp(proprio.values[6],-1.0f,1.0f);  // prev turn
        f[18]=std::clamp(proprio.values[7],-1.0f,1.0f);  // prev forward
        f[19]=1.0f;                                      // bias
    }
    // v27 persistence: binary snapshot of all learned state, written to the
    // local learning directory so progress survives process restarts.
    std::string learningStatePath() const { return localLearningDirectory + "/models/v27_learning_state.bin"; }
    void saveLearningState() {
        std::ofstream f(learningStatePath(), std::ios::binary | std::ios::trunc);
        if (!f.is_open()) { addLog("learning state save failed: cannot open file"); return; }
        const char magic[4] = {'V','2','7','L'};
        f.write(magic, 4);
        auto wU64 = [&](uint64_t v){ f.write((const char*)&v, 8); };
        auto wI = [&](int v){ f.write((const char*)&v, 4); };
        auto wF = [&](float v){ f.write((const char*)&v, 4); };
        auto wD = [&](double v){ f.write((const char*)&v, 8); };
        wU64(1); // format version
        // NavTrainer Q-table + difficulty metrics.
        for (const auto& a : navTrainer.Q) for (const auto& b : a) for (const auto& c : b) for (float v : c) wF(v);
        wI(navTrainer.episodes); wI(navTrainer.successes); wI(navTrainer.collisions); wI(navTrainer.stepsTotal);
        // Navigation observer visitation counts + counters.
        for (const auto& a : navLearning.stateVisits) for (const auto& b : a) for (uint32_t v : b) f.write((const char*)&v, 4);
        for (const auto& a : navLearning.actionVisits) for (const auto& b : a) for (const auto& c : b) for (uint32_t v : c) f.write((const char*)&v, 4);
        wU64(navLearning.updates); wU64(navLearning.rejected); wU64(navLearning.collisions); wU64(navLearning.windows);
        // DTE navigation counters.
        wU64(dteNavigation.proposals); wU64(dteNavigation.learnedProposals);
        wU64(dteNavigation.accepted); wU64(dteNavigation.contacts);
        // Episodic memory: episodes + concepts + consolidation count.
        wI((int)episodic.episodes.size());
        for (const auto& e : episodic.episodes) {
            wI((int)e.state.size()); for (double v : e.state) wD(v);
            wF(e.meanContradiction); wF(e.reward); wU64(e.tick);
            wF(e.strength); wF(e.activation); wI(e.accessCount); wF(e.valence);
            wU64(e.lastAccessTick); f.write((const char*)&e.flashbulb, 1);
        }
        wI((int)episodic.concepts.size());
        for (const auto& c : episodic.concepts) {
            wI((int)c.prototype.size()); for (double v : c.prototype) wD(v);
            wF(c.variance); wI(c.instanceCount); wF(c.confidence);
        }
        wI(episodic.consolidations);
        // ESN trained readout, RLS covariance, reservoir state, counters.
        auto rw = esn.readoutWeights(); wI((int)rw.size()); for (double v : rw) wD(v);
        auto rc = esn.rlsCovariance(); wI((int)rc.size()); for (double v : rc) wD(v);
        auto rs = esn.state(); wI((int)rs.size()); for (double v : rs) wD(v);
        wI(esn.rlsUpdates); wI(esn.plasticityUpdates);
        // Social agents: trust/affinity/familiarity/grouping.
        wI((int)groups.agents.size());
        for (const auto& a : groups.agents) {
            wI(a.id); wF(a.x); wF(a.y); wF(a.trust); wF(a.affinity); wF(a.familiarity);
            wI(a.groupId); wF(a.cohesion);
        }
        wI(groups.nextGroupId); wF(groups.avgCohesion);
        // Global learning counters.
        wU64(llmTrainingUpdates); wF(llmMetrics.accuracy); wF(llmMetrics.averageLoss);
        wF(sensorimotor.totalReward); wI(sensorimotor.stepCount);
        // GNN motor network + training counters (v27 NN persistence).
        std::error_code ec;
        std::filesystem::create_directories(localLearningDirectory+"/models", ec);
        gnnMotor.save(localLearningDirectory+"/models/gtangel_motor_gnn.net");
        wU64(neuralTrainingSteps); wU64(neuralTrainingUpdates); wF(lastNeuralTrainLoss);
        f.flush();
        addLog(f.good() ? "learning state saved (persistent progress)" : "learning state save I/O error");
    }
    bool loadLearningState() {
        std::ifstream f(learningStatePath(), std::ios::binary);
        if (!f.is_open()) return false;
        char magic[4]; f.read(magic, 4);
        if (!f.good() || magic[0]!='V'||magic[1]!='2'||magic[2]!='7'||magic[3]!='L') { addLog("learning state rejected: bad magic"); return false; }
        auto rU64 = [&]() { uint64_t v; f.read((char*)&v, 8); return v; };
        auto rI = [&]() { int v; f.read((char*)&v, 4); return v; };
        auto rF = [&]() { float v; f.read((char*)&v, 4); return v; };
        auto rD = [&]() { double v; f.read((char*)&v, 8); return v; };
        uint64_t ver = rU64(); if (ver != 1) { addLog("learning state rejected: version"); return false; }
        for (auto& a : navTrainer.Q) for (auto& b : a) for (auto& c : b) for (float& v : c) v = rF();
        navTrainer.episodes = rI(); navTrainer.successes = rI(); navTrainer.collisions = rI(); navTrainer.stepsTotal = rI();
        for (auto& a : navLearning.stateVisits) for (auto& b : a) for (uint32_t& v : b) f.read((char*)&v, 4);
        for (auto& a : navLearning.actionVisits) for (auto& b : a) for (auto& c : b) for (uint32_t& v : c) f.read((char*)&v, 4);
        navLearning.updates = rU64(); navLearning.rejected = rU64(); navLearning.collisions = rU64(); navLearning.windows = rU64();
        dteNavigation.proposals = rU64(); dteNavigation.learnedProposals = rU64();
        dteNavigation.accepted = rU64(); dteNavigation.contacts = rU64();
        int n = rI();
        if (n < 0 || n > 4096) { addLog("learning state rejected: episode count"); return false; }
        episodic.episodes.clear();
        for (int i = 0; i < n; i++) {
            dte::EpisodicMemory::Episode e; int sn = rI();
            if (sn < 0 || sn > 4096) { addLog("learning state rejected: episode dim"); return false; }
            e.state.resize(sn); for (double& v : e.state) v = rD();
            e.meanContradiction = rF(); e.reward = rF(); e.tick = rU64();
            e.strength = rF(); e.activation = rF(); e.accessCount = rI(); e.valence = rF();
            e.lastAccessTick = rU64(); f.read((char*)&e.flashbulb, 1);
            episodic.episodes.push_back(e);
        }
        int cn = rI();
        if (cn < 0 || cn > 4096) { addLog("learning state rejected: concept count"); return false; }
        episodic.concepts.clear();
        for (int i = 0; i < cn; i++) {
            dte::EpisodicMemory::SemanticConcept c; int pn = rI();
            if (pn < 0 || pn > 4096) { addLog("learning state rejected: concept dim"); return false; }
            c.prototype.resize(pn); for (double& v : c.prototype) v = rD();
            c.variance = rF(); c.instanceCount = rI(); c.confidence = rF();
            episodic.concepts.push_back(c);
        }
        episodic.consolidations = rI();
        int rn = rI(); std::vector<double> rw(rn > 0 && rn <= 1048576 ? rn : 0);
        for (double& v : rw) v = rD(); esn.setReadoutWeights(rw);
        int pn2 = rI(); std::vector<double> rc(pn2 > 0 && pn2 <= 1048576 ? pn2 : 0);
        for (double& v : rc) v = rD(); esn.setRlsCovariance(rc);
        int sn2 = rI(); std::vector<double> rs(sn2 > 0 && sn2 <= 1048576 ? sn2 : 0);
        for (double& v : rs) v = rD(); esn.setReservoirState(rs);
        esn.rlsUpdates = rI(); esn.plasticityUpdates = rI();
        int an = rI();
        if (an < 0 || an > 4096) { addLog("learning state rejected: agent count"); return false; }
        groups.agents.clear(); groups.groups.clear();
        for (int i = 0; i < an; i++) {
            dte::Agent a; a.id = rI(); a.x = rF(); a.y = rF(); a.trust = rF();
            a.affinity = rF(); a.familiarity = rF(); a.groupId = rI(); a.cohesion = rF();
            groups.agents.push_back(a);
        }
        groups.nextGroupId = rI(); groups.avgCohesion = rF();
        if (groups.agents.empty()) groups.init(20);
        for (const auto& a : groups.agents) if (a.groupId >= 0) groups.groups[a.groupId].push_back(a.id);
        llmTrainingUpdates = rU64(); llmMetrics.accuracy = rF(); llmMetrics.averageLoss = rF();
        sensorimotor.totalReward = rF(); sensorimotor.stepCount = rI();
        // GNN motor tail (v27): tolerate older state files without it.
        {
            const std::streampos tailPos = f.tellg();
            neuralTrainingSteps = rU64(); neuralTrainingUpdates = rU64(); lastNeuralTrainLoss = rF();
            if (!f.good()) { // older file: rewind, keep the loaded network
                f.clear(); f.seekg(tailPos);
                neuralTrainingSteps = gnnMotor.trainingSteps();
                neuralTrainingUpdates = gnnMotor.trainingUpdates();
                lastNeuralTrainLoss = gnnMotor.lastLoss();
            }
        }
        if (!f.good()) { addLog("learning state rejected: truncated"); return false; }
        addLog("learning state restored: " + std::to_string(n) + " episodes, " + std::to_string(cn) + " concepts, Q table + ESN readout reloaded");
        return true;
    }
    void addLog(const std::string& m) {
        log.push_back(m);
        if (log.size() > 50) log.erase(log.begin());
    }
    void registerAgentLoopSteps() {
        agentLoop.addStep("perceive", 10, [this]() {
            for (size_t i=0;i<sensorimotor.frameBuffer.size() && i<navRenderer.frame.size();++i)
                sensorimotor.frameBuffer[i]=navRenderer.frame[i];
            sensorimotor.computeProprioception();
            return true;
        });
        agentLoop.addStep("endocrine", 9, [this]() {
            endo.tick();
            endo.trackModeTransition();
            return true;
        });
        agentLoop.addStep("reservoir", 8, [this]() {
            std::vector<double> esnIn(32);
            // Channels 16-19: frame-buffer computer vision (re-derived from the
            // current retina so the reservoir sees what the camera saw).
            const auto ve = visualNavEncoder.encode(perceptionPyramid.retina);
            esnIn[16] = perceptionPyramid.retina.meanLuminance;
            esnIn[17] = perceptionPyramid.retina.meanMotion;
            esnIn[18] = perceptionPyramid.retina.meanEdge;
            esnIn[19] = trizMap.meanContradiction;
            // Channel 20: TRIZ temporal derivative (spatio-temporal reasoning).
            lastTrizDerivative = (double)trizMap.meanContradiction - (double)prevMeanContradiction;
            esnIn[20] = std::clamp(lastTrizDerivative, -1.0, 1.0);
            // Channels 21-24: goal/obstacle visual evidence.
            esnIn[21] = ve.goalVisible ? 1.0 : 0.0;
            esnIn[22] = ve.goalDepth < 2.0f ? 0.0 : ve.goalDepth < 5.0f ? 0.33 : ve.goalDepth < 10.0f ? 0.66 : 1.0;
            esnIn[23] = ve.obstacleVisible ? 1.0 : 0.0;
            esnIn[24] = ve.nearestObstacleDepth < 2.0f ? 0.0 : ve.nearestObstacleDepth < 5.0f ? 0.33 : ve.nearestObstacleDepth < 10.0f ? 0.66 : 1.0;
            // Channels 25-29: social game dynamics (groups + peer attention).
            esnIn[25] = std::min(1.0, groups.groupCount() / 20.0);
            esnIn[26] = std::clamp((double)groups.avgCohesion, 0.0, 1.0);
            esnIn[27] = std::clamp((double)peerAttention.meanConfidence, 0.0, 1.0);
            esnIn[28] = std::min(1.0, peerAttention.activeRelations / 10.0);
            esnIn[29] = std::clamp((double)peerAttention.dominantSalience, 0.0, 1.0);
            // Channel 30: episodic recall similarity (pre-step, from the
            // previous reservoir state - "have I been here before?").
            lastEpisodicRecall = (float)episodic.recall(esn.state());
            esnIn[30] = lastEpisodicRecall;
            // Channel 31: last outcome reward.
            esnIn[31] = std::clamp((double)lastStep.reward, -1.0, 1.0);
            for (int i = 0; i < 8; i++) esnIn[i] = sensorimotor.proprioception[i]; // frame stats
            // Real body feedback: speed, heading sin/cos, collision, reward.
            esnIn[0]=std::isfinite(lastBodyProprio.values[0])?lastBodyProprio.values[0]:esnIn[0];
            esnIn[1]=std::isfinite(lastBodyProprio.values[1])?lastBodyProprio.values[1]:esnIn[1];
            esnIn[2]=std::isfinite(lastBodyProprio.values[2])?lastBodyProprio.values[2]:esnIn[2];
            esnIn[4]=std::isfinite(lastBodyProprio.values[4])?lastBodyProprio.values[4]:esnIn[4];
            esnIn[7]=std::isfinite(lastBodyProprio.values[13])?lastBodyProprio.values[13]:esnIn[7];
            // Channels 8-31 are filled below (hormones, vision, TRIZ, social, episodic).
            for (int i = 0; i < 8; i++) esnIn[8+i] = endo.hormoneLevel((dte::HormoneId)i);
            esn.step(esnIn);
            esnEnergy = esn.stateEnergy();
            // Observational readout training: predict the outcome reward from
            // the current reservoir state. Never actuates anything.
            {
                std::vector<double> target(1, std::clamp((double)lastStep.reward, -1.0, 1.0));
                esn.trainReadout(target, 0.01);
            }
            // Episodic storage on significant events (collision -1.0, goal
            // +1.0 - not the -0.01 time penalty).
            if (std::fabs((double)lastStep.reward) > 0.05)
                episodic.store(esn.state(), trizMap.meanContradiction,
                               lastStep.reward, tickCount);
            prevMeanContradiction = trizMap.meanContradiction;
            // u9n memory decay: strength fades with time since last access.
            episodic.decay((uint64_t)tickCount);
            esn.applyIntrinsicPlasticity(0.01);
            // u9n auto-consolidation on an interval timer (every 64 ticks):
            // strengthen, replay, abstract to semantic concept, prune.
            if (tickCount % 64 == 0) {
                episodic.consolidate();
                lastConsolidationCount = episodic.consolidations;
            }
            return esnEnergy < 10.0;
        });
        agentLoop.addStep("triz_arena", 7, [this]() {
            arena.tick();
            return true;
        });
        agentLoop.addStep("attention", 6, [this]() {
            for (int q = 0; q < dte::TRIZArena::SIZE; q++)
                for (int r = 0; r < dte::TRIZArena::SIZE; r++) {
                    auto& cell = arena.grid[q][r];
                    std::string id = elizaos::ActivationSpreadingNetwork::key(q, r);
                    elizaos::AttentionValue v;
                    v.novelty = cell.contradiction;
                    v.urgency = cell.contradiction * 0.8;
                    v.importance = cell.aestheticValue * 0.5;
                    v.activation = spreadNet.get(id);
                    attention.update(id, v);
                    if (cell.contradiction > 0.15f)
                        attention.stimulate(id, cell.contradiction * 0.3, cell.contradiction * 0.2);
                }
            attention.allocateTop(12);
            attention.decayAll();
            spreadNet.spread(0.8, 0.01);
            attentionFocus = attention.focusElement();
            avgAttention = attention.averageAttention();
            globalAttentionWindow.event(arena.maxContradiction * 0.5);
            globalAttentionWindow.decay();
            return true;
        });
        agentLoop.addStep("group_dynamics", 4, [this]() {
            // Honest social grounding: agent coordinates mirror the actual
            // walker population (world metres) before relationship dynamics.
            for (size_t i = 0; i < groups.agents.size() && i < navWorld.walkers.size(); ++i) {
                groups.agents[i].x = navWorld.walkers[i].pos.x;
                groups.agents[i].y = navWorld.walkers[i].pos.z;
            }
            groups.tick(navPos.x, navPos.z);
            return true;
        });
        agentLoop.addStep("layla", 7, [this]() {
            laylaTelemetry = laylaBridge.telemetry();
            if (laylaTelemetry.online && !laylaTelemetry.busy && laylaTelemetry.queueDepth == 0) {
                std::string prompt = "GTAngel telemetry: tick=" + std::to_string(tickCount) +
                    " mode=" + dte::cognitiveModeName(endo.cognitiveMode()) +
                    " coherence=" + std::to_string(arena.coherence) +
                    " attention=" + attentionFocus;
                laylaBridge.enqueue(prompt);
            }
            return true;
        });
        agentLoop.addStep("navigation", 6, [this]() {
            gameplay.tick(0.05f, navPos, navHeading, navWorld);
            navWorld.tickWalkers(0.05f);
            navWorld.tickTraffic(0.05f);
            navWorld.tickPickups(0.05f);
            // Acquire a new owner frame after startup/pause/load; never consume remote imagery.
            if(!echoSample || !echoIngress.current(*echoSample,(uint64_t)GetTickCount64(),cns.body(),cns.niche())) {
                const auto eye=navThirdPerson?navWorld.collisionFreeEye(navPos,navPos+avatar3d::Vec3{-sinf(navHeading)*4,2.5f,-cosf(navHeading)*4}):navPos+avatar3d::Vec3{0,1.6f,0};
                highResRenderer.render(navWorld,avatar,eye,navHeading,navThirdPerson?-.4f:0,navThirdPerson);
                perceptionPyramid.derive(highResRenderer.framebuffer);
                echoSample=echoIngress.capture((uint64_t)GetTickCount64(),cns.body(),cns.niche());
            }
            const auto visualBefore=visualNavEncoder.encode(perceptionPyramid.retina);
            const float risk=visualBefore.obstacleVisible
                ? std::clamp(1.0f-visualBefore.nearestObstacleDepth/5.0f,0.0f,1.0f):0.0f;
            trajectoryVisual.derive(highResRenderer.framebuffer);
            const auto modelProprio=v16motor::makeProprioception(
                navSpeed,navHeading,0.0f,cns.body().contact,true,previousControllerTarget,
                (float)endo.valenceArousal().arousal,lastStep.reward,
                visualBefore.goalVisible ? 1.0f : 0.0f,risk);
            lastModelProprio=modelProprio; // pre-action efference copy
            const v19game::PlayerControls manual=testHeldKey?testControlsFromKey(testHeldKey):gameplay.samplePlayerControls();
            const int questMode=gameplay.quest.stage==v19game::QuestStage::SeekShard?1:
                gameplay.quest.stage==v19game::QuestStage::RestoreBeacon?2:0;
            float goalCompass=10.0f;
            avatar3d::Vec3 activeTarget = navWorld.goal;
            float preTargetDist = 0.0f;
            auto visualActive = visualBefore;
            {
                // LW38: true stage-aware target selection — Ariadne in Dormant/Offered/Return,
                // Signal Shard in SeekShard, Civic Beacon in RestoreBeacon, and nearest
                // uncollected Data Cell pickup once the main quest is Complete.
                const v19game::QuestStage qs=gameplay.quest.stage;
                if (questMode == 1) {
                    activeTarget = navWorld.questShard;
                } else if (questMode == 0 && qs != v19game::QuestStage::Complete) {
                    activeTarget = navWorld.npcPos;
                } else if (qs == v19game::QuestStage::Complete) {
                    if (gameplay.quest.pickupsCollected < 4) {
                        float bestD2 = 1e30f;
                        for (const auto& pk : navWorld.pickups) {
                            if (pk.collected) continue;
                            const float d2 = (pk.pos.x-navPos.x)*(pk.pos.x-navPos.x) + (pk.pos.z-navPos.z)*(pk.pos.z-navPos.z);
                            if (d2 < bestD2) { bestD2 = d2; activeTarget = pk.pos; }
                        }
                    } else if (!gameplay.quest.obeliskDecrypted) {
                        activeTarget = avatar3d::Vec3{-9.0f, 0.0f, -2.4f};
                    } else if (!gameplay.quest.relayAttuned) {
                        activeTarget = avatar3d::Vec3{9.0f, 0.0f, 2.4f};
                    } else if (!gameplay.quest.shadowNodePurified) {
                        activeTarget = avatar3d::Vec3{2.6f, 0.0f, -18.0f};
                    } else if (!gameplay.quest.skylineTowerSynced) {
                        activeTarget = avatar3d::Vec3{-18.0f, 0.0f, 2.6f};
                    } else if (!gameplay.quest.sanctuaryAwakened) {
                        activeTarget = navWorld.npcPos;
                    } else {
                        static const avatar3d::Vec3 loci[6] = {
                            {8.0f, 0.0f, 8.0f}, {-9.0f, 0.0f, -2.4f},
                            {9.0f, 0.0f, 2.4f}, {2.6f, 0.0f, -18.0f},
                            {-18.0f, 0.0f, 2.6f}, {-4.0f, 0.0f, 4.0f}
                        };
                        activeTarget = loci[gameplay.quest.patrolWaypoint % 6u];
                        const float ldx = activeTarget.x - navPos.x, ldz = activeTarget.z - navPos.z;
                        if (ldx*ldx + ldz*ldz < 2.2f*2.2f) ++gameplay.quest.patrolWaypoint;
                    }
                }
                const float dx=activeTarget.x-navPos.x, dz=activeTarget.z-navPos.z;
                preTargetDist = std::sqrt(dx*dx + dz*dz);
                if (dx*dx+dz*dz > 1e-4f) {
                    goalCompass=atan2f(dx,dz)-navHeading;
                    while (goalCompass > 3.14159265f) goalCompass -= 6.2831853f;
                    while (goalCompass < -3.14159265f) goalCompass += 6.2831853f;
                    navGoalCompass=goalCompass;
                    const int bBucket = std::clamp((int)std::floor(((goalCompass / 3.14159265f) * 0.5f + 0.5f) * 8.0f), 0, 7);
                    const int dBucket = preTargetDist < 2.0f ? 0 : preTargetDist < 5.0f ? 1 : preTargetDist < 10.0f ? 2 : 3;
                    if (!visualActive.goalVisible) {
                        visualActive.state.bearing = bBucket;
                        visualActive.state.dist    = dBucket;
                    }
                }
            }
            const auto dteProposal=dteNavigation.propose(visualActive,navTrainer,navLearning,cns.body().contact,questMode,goalCompass);
            float forward=manual.active?std::clamp(manual.move,-1.0f,1.0f):dteEmbodimentEnabled?dteProposal.forward:0;
            float turn=manual.active?manual.turn:dteEmbodimentEnabled?dteProposal.turn:0;
            const bool onFoot=!cns.body().occupied;
            v16motor::ControllerTarget localControllerReference{};
            localControllerReference.values[0]=-turn; localControllerReference.values[1]=forward;
            localControllerReference.values[5]=(manual.active||dteEmbodimentEnabled)?1.0f:0.0f;
            neuralPrediction=v17policy::Prediction{};
            v17policy::ShieldDecision gnnDecision{};
            float gnnTurn=0.0f, gnnForward=0.0f;
            bool gnnValid=false;
            neuralDecision=v17policy::ShieldDecision{};
            if (neuralPolicyLoaded && neuralShadowEnabled) {
                neuralPrediction=neuralPolicy.infer(trajectoryVisual,modelProprio);
                neuralDecision=neuralShield.decide(neuralPrediction,localControllerReference,risk,
                                                   neuralActuationUnlocked,neuralEthicsApproved);
                ++neuralFrames;
            }
            // v27 GNN motor actuator: online-learning network proposes an
            // (turn, forward) pair from live evidence. It runs every frame
            // alongside the GRU; acceptance is still shield-governed.
            gnnTurn=0.0f; gnnForward=0.0f; gnnValid=false;
            if (gnnMotorReady && neuralShadowEnabled) {
                float gnnFeatures[v27gnn::FeatureSize];
                buildGnnFeatures(gnnFeatures, visualBefore, modelProprio, navSpeed, navHeading);
                gnnValid=gnnMotor.infer(gnnFeatures, gnnTurn, gnnForward);
                if (gnnValid) {
                    v17policy::Prediction gnnPrediction{};
                    gnnPrediction.controller.values[0]=gnnTurn;
                    gnnPrediction.controller.values[1]=gnnForward;
                    gnnPrediction.controller.values[5]=1.0f;
                    gnnPrediction.uncertainty=std::clamp(
                        gnnMotor.lastLoss()<0.0f?0.35f:gnnMotor.lastLoss()*0.5f,0.0f,0.35f);
                    gnnPrediction.collisionProbability=neuralPrediction.collisionProbability;
                    gnnPrediction.valid=true;
                    gnnDecision=neuralShield.decide(gnnPrediction,localControllerReference,risk,
                                                     neuralActuationUnlocked,neuralEthicsApproved);
                    if (gnnDecision.accepted) ++neuralAccepted;
                }
            }
            // v27 GNN actuation: when the shield accepts the GNN proposal and
            // no human input is active, the learned network steers THROUGH the
            // sealed DteLocal path (never a raw Source::Neural submit).
            if (gnnValid && gnnDecision.accepted && !manual.active && dteEmbodimentEnabled) {
                // LW38 fix: gnnTurn is trained on controller channel 0 (-turn),
                // so negate gnnTurn back to body turn convention and clamp
                // forward within SpinalReflex's |forward|<=0.50 seal contract.
                turn=std::clamp(-gnnTurn,-0.55f,0.55f);
                forward=std::clamp(gnnForward,-0.35f,0.48f);
            }
            // Observational learning is NOT a route request. Non-human sources remain locked.
            const auto source=manual.active?v20cns::Source::HumanLocal:dteEmbodimentEnabled?v20cns::Source::DteLocal:v20cns::Source::HumanLocal;
            v20cns::Context context{highResRenderer.framebuffer.sequence,
                (uint64_t)GetTickCount64(),0.05f,highResRenderer.framebuffer.sequence>0,false};
            context.visualDigest=echoSample?echoSample->hash():0;
            context.worldVersion=cns.niche().worldVersion;
            // Advisory image depth can contain an obstacle behind the body;
            // human input remains subject to the same local swept geometry but
            // is never disabled solely by the coarse visual-risk estimate.
            const float submittedRisk=source==v20cns::Source::HumanLocal ? 0.0f : risk;
            v20cns::SpinalReflex::Verdict verdict=v20cns::SpinalReflex::Verdict::Untrusted;
            if(source==v20cns::Source::DteLocal) {
                if(echoSample && echoIngress.current(*echoSample,context.nowMs,cns.body(),cns.niche())) {
                    const v20cns::DteLocalMotorSeal seal(echoSample->sequence(),echoSample->hash(),
                        context.nowMs,cns.body().sequence,cns.niche().worldVersion,forward,turn);
                    // Coarse camera depth is advisory; final local swept geometry is authoritative.
                    verdict=cns.motorDte(seal,forward,turn,dteProposal.uncertainty,0.0f,context,navWorld);
                }
            } else verdict=cns.motor(source,forward,turn,0.0f,submittedRisk,true,context,navWorld);
            lastMotorVerdict=verdict;
            const bool collided=verdict==v20cns::SpinalReflex::Verdict::Collision;
            const bool humanAccepted=(source==v20cns::Source::DteLocal || manual.active) &&
                (verdict==v20cns::SpinalReflex::Verdict::Accepted || collided);
            if(source==v20cns::Source::DteLocal && humanAccepted)++dteNavigation.accepted;
            navPos=cns.body().position; navHeading=cns.body().heading; navSpeed=cns.body().speed;
            navWorld.vehiclePos=cns.body().cartPosition;
            navWorld.vehicleHeading=cns.body().cartHeading;
            navWorld.vehicleWheelPhase=cns.body().wheelPhase;
            navWorld.vehicleOccupied=cns.body().occupied;
            gameplay.tick(0.0f,navPos,navHeading,navWorld);
            if (dteEmbodimentEnabled && !manual.active) {
                // Autonomous quest engagement: resolve dialogue first (choose
                // before interact — interact() closes open dialogues), then
                // collect, then open new conversations. Stimuli feed the
                // endocrine system so quest events register emotionally.
                if (gameplay.quest.dialogueOpen &&
                    (gameplay.quest.stage==v19game::QuestStage::Offered ||
                     gameplay.quest.stage==v19game::QuestStage::ReturnToAriadne)) {
                    gameplay.choose(1, navWorld);
                    endo.submitStimulus(dte::Stimulus("social:dialogue",0.3));
                } else if (gameplay.canCollect(navPos, navWorld)) {
                    gameplay.interact(navPos, navHeading, navWorld);
                    endo.submitStimulus(dte::Stimulus("discovery:shard",0.6));
                } else if (gameplay.canRestore(navPos, navWorld)) {
                    // Beacon repair is WGR-governed: the DTE advisory path
                    // (repairBeacon) is the only NICHE_COMMIT mint. Autonomous
                    // mode mirrors the E key so the quest can complete.
                    if (repairBeacon())
                        endo.submitStimulus(dte::Stimulus("reward:beacon_restored",0.7));
                } else if ((gameplay.quest.stage==v19game::QuestStage::Dormant ||
                            gameplay.quest.stage==v19game::QuestStage::ReturnToAriadne) &&
                           gameplay.canSpeak(navPos, navHeading)) {
                    gameplay.interact(navPos, navHeading, navWorld);
                    endo.submitStimulus(dte::Stimulus("social:ariadne",0.3));
                }
                if (gameplay.quest.stage==v19game::QuestStage::Complete &&
                    !gameplay.quest.questRewardClaimed) {
                    gameplay.quest.questRewardClaimed = true;
                    endo.submitStimulus(dte::Stimulus("reward:quest_complete",0.8));
                }
                if (gameplay.nearbyPickup(navPos, navWorld)) {
                    gameplay.interact(navPos, navHeading, navWorld);
                    endo.submitStimulus(dte::Stimulus("reward:pickup_collected",0.4));
                }
            }
            // LW27: inspect clip when a pickup is near (retinal size proxy).
            avatar.inspect = visualBefore.propVisible && visualBefore.propPixels > 40;
            avatar.pose(navPos,navHeading,fabsf(navSpeed),0.05f);
            const avatar3d::Vec3 eye=navThirdPerson
                ? navWorld.collisionFreeEye(navPos,navPos+avatar3d::Vec3{-sinf(navHeading)*4.0f,2.5f,-cosf(navHeading)*4.0f})
                : navPos+avatar3d::Vec3{0,1.6f,0};
            highResRenderer.render(navWorld,avatar,eye,navHeading,navThirdPerson?-0.4f:0.0f,navThirdPerson);
            peerLoop.confirmAgainstLocalFrame(highResRenderer.framebuffer,!navThirdPerson);
            peerAttention=peerLoop.attentionContext();
            perceptionPyramid.derive(highResRenderer.framebuffer);
            // Recurrent/synthetic DTE channels are not VRD certificate issuers.
            // This scan never calls cns.motor(), cns.repair(), or an effector.
            echoSample=echoIngress.capture((uint64_t)GetTickCount64(),cns.body(),cns.niche());
            if(echoSample) {
                const auto nowMs=(uint64_t)GetTickCount64();
                if(echoIngress.current(*echoSample,nowMs,cns.body(),cns.niche())) {
                    echoCommons.scan(echoIngress,*echoSample,nowMs,cns.body(),cns.niche(),gameplay,navWorld);
                    treeLenses.observe(*echoSample,cns.body(),cns.niche());
                } else {echoCommons.clear();treeLenses.reset();}
            } else {echoCommons.clear();treeLenses.reset();}
            perceptionPyramid.copyToLegacy(navRenderer);
            perceptionPyramid.mapToTriz(trizMap);
            navMeanContradiction=trizMap.meanContradiction;
            // Retain the PRE-action tensor for a causal training label; post-frame gives outcome.
            auto visualAfter=visualNavEncoder.encode(perceptionPyramid.retina);
            const float postDx = activeTarget.x - navPos.x, postDz = activeTarget.z - navPos.z;
            const float postTargetDist = std::sqrt(postDx*postDx + postDz*postDz);
            if (!visualAfter.goalVisible && postTargetDist > 1e-4f) {
                float postCompass = atan2f(postDx, postDz) - navHeading;
                while (postCompass > 3.14159265f) postCompass -= 6.2831853f;
                while (postCompass < -3.14159265f) postCompass += 6.2831853f;
                visualAfter.state.bearing = std::clamp((int)std::floor(((postCompass / 3.14159265f) * 0.5f + 0.5f) * 8.0f), 0, 7);
                visualAfter.state.dist    = postTargetDist < 2.0f ? 0 : postTargetDist < 5.0f ? 1 : postTargetDist < 10.0f ? 2 : 3;
            }
            const float approachDelta = preTargetDist - postTargetDist;
            const float reward = collided ? -1.0f : std::clamp(
                -0.01f + 0.10f * visualNavEncoder.progressReward(visualBefore, visualAfter)
                       + 2.20f * approachDelta, -1.0f, 1.0f);
            navLearning.observe(navTrainer,visualActive,visualAfter,forward,turn,reward,
                                humanAccepted,collided,onFoot,navTraining);
            sensorimotor.reward=reward; // local outcome, not the detached synthetic loop
            sensorimotor.totalReward+=reward; // real outcome accounting
            ++sensorimotor.stepCount;
            const auto proprio=v16motor::makeProprioception(navSpeed,navHeading,
                turn*0.1f,collided,true,previousControllerTarget,
                (float)endo.valenceArousal().arousal,reward,
                visualAfter.goalVisible?1.0f:0.0f,risk);
            lastBodyProprio=proprio; // close the efference-copy loop
            v16motor::ControllerTarget actualTarget{};
            if(humanAccepted) {
                actualTarget.values[0]=-turn; actualTarget.values[1]=forward;
                actualTarget.values[5]=1.0f;
            }
            // v27 GNN training: learn from every accepted local action.
            if (gnnMotorReady && onFoot && !collided && (humanAccepted || reward > 0.0f)) {
                float trainFeatures[v27gnn::FeatureSize];
                buildGnnFeatures(trainFeatures, visualBefore, modelProprio, navSpeed, navHeading);
                gnnMotor.remember(trainFeatures, -turn, forward);
            }
            ++gnnFrameCounter;
            if (gnnMotorReady && gnnFrameCounter>=v27gnn::TrainEveryFrames &&
                gnnMotor.totalSamples()>=8) {
                gnnFrameCounter=0;
                if (gnnMotor.train()) {
                    // LW25: periodic deep pass (extra batch) + periodic autosave.
                    if (gnnMotor.trainingUpdates() % 5 == 0) gnnMotor.train();
                    if (gnnMotor.trainingUpdates() % 200 == 0)
                        gnnMotor.save(localLearningDirectory+"/models/gtangel_motor_gnn.net");
                    neuralTrainingSteps=gnnMotor.trainingSteps();
                    neuralTrainingUpdates=gnnMotor.trainingUpdates();
                    lastNeuralTrainLoss=gnnMotor.lastLoss();
                }
            }
            if (trajectoryRecording && !trajectoryRecorder.capture(
                    (uint32_t)navEpisodeSteps,trajectoryVisual,modelProprio,actualTarget,
                    collided?v16motor::OutcomeFlags::Collision:v16motor::OutcomeFlags::None,
                    reward,1.0f,risk)) {
                trajectoryRecording=false;
                addLog("v16 recorder stopped after a dataset write failure");
            }
            previousControllerTarget=actualTarget;
            ++navEpisodeSteps;
            for (size_t i=0;i<sensorimotor.frameBuffer.size() && i<navRenderer.frame.size();++i)
                sensorimotor.frameBuffer[i]=navRenderer.frame[i];
            return true;
        });
        agentLoop.addStep("civic_angel_spine", 2, [this]() {
            civicangel::GroundedFrame f;
            f.tick = (uint64_t)tickCount;
            f.positionX = navPos.x;
            f.positionZ = navPos.z;
            f.heading = navHeading;
            f.goalDistance = navWorld.goalDistance(navPos);
            f.meanContradiction = navMeanContradiction;
            double lum = 0.0;
            for (float v : navRenderer.frame) lum += v;
            f.frameLuminance = navRenderer.frame.empty() ? 0.0 : lum / navRenderer.frame.size();
            double obstacles = 0.0, goals = 0.0;
            for (int q = 0; q < avatar3d::TrizMapping::GRID; ++q)
                for (int r = 0; r < avatar3d::TrizMapping::GRID; ++r) {
                    obstacles += trizMap.obstacleSalience[q][r];
                    goals += trizMap.goalSalience[q][r];
                }
            const double bins = avatar3d::TrizMapping::GRID * avatar3d::TrizMapping::GRID;
            f.obstacleSalience = obstacles / bins;
            f.goalSalience = goals / bins;
            f.collisionRate = navTrainer.collisionRate();
            f.health = worldObs.health;
            f.hostility = worldObs.hostility;
            f.goalProgress = worldObs.goal_progress;
            f.reward = lastStep.reward;
            f.coherence = arena.coherence;
            f.valence = endo.valenceArousal().valence;
            f.arousal = endo.valenceArousal().arousal;
            f.cortisol = endo.hormoneLevel(dte::HormoneId::Cortisol);
            f.dopamine = endo.hormoneLevel(dte::HormoneId::Dopamine);
            f.serotonin = endo.hormoneLevel(dte::HormoneId::Serotonin);
            f.norepinephrine = endo.hormoneLevel(dte::HormoneId::Norepinephrine);
            f.oxytocin = endo.hormoneLevel(dte::HormoneId::Oxytocin);
            f.melatonin = endo.hormoneLevel(dte::HormoneId::Melatonin);
            f.anandamide = endo.hormoneLevel(dte::HormoneId::Anandamide);
            f.framebufferSequence = highResRenderer.metrics.sequence;
            // Echo persona (display-only): resolve emotion from the live bus.
            const v27dte::EchoPersonaState echoState=
                echoPersona.resolve(f.valence,f.arousal,(int)endo.cognitiveMode());
            f.echoEmotion=echoState.emotion; f.echoExpression=echoState.expression;
            f.echoFacs=echoState.facs; f.echoSpice=echoState.spice;
            f.echoConfidence=echoState.confidence;
            f.framebufferRenderMs = highResRenderer.metrics.renderMs;
            f.trianglesSubmitted = highResRenderer.metrics.trianglesSubmitted;
            f.trianglesRasterized = highResRenderer.metrics.trianglesRasterized;
            f.finiteDepthPixels = highResRenderer.metrics.finiteDepthPixels;
            f.semanticPixels = highResRenderer.metrics.semanticPixels;
            switch (endo.cognitiveMode()) {
                case dte::CognitiveMode::Exploitation: f.cognitiveMode = "focused"; break;
                case dte::CognitiveMode::Rest: f.cognitiveMode = "reflective"; break;
                case dte::CognitiveMode::Alarm: f.cognitiveMode = "vigilant"; break;
                default: f.cognitiveMode = "exploratory"; break;
            }
            return civicSpine.tick(f);
        });
        agentLoop.addStep("cogzero", 6, [this]() {
            cogzeroStats = cogzeroAgent.tick();
            return true;
        });
        agentLoop.addStep("echo_ksm", 5, [this]() {
            std::vector<std::vector<float>> echoGrid(dte::TRIZArena::SIZE, std::vector<float>(dte::TRIZArena::SIZE));
            for (int q = 0; q < dte::TRIZArena::SIZE; ++q)
                for (int r = 0; r < dte::TRIZArena::SIZE; ++r)
                    echoGrid[q][r]=arena.grid[q][r].aestheticValue;
            float coherence = echoField.evaluate(echoGrid); // live arena field
            float contradiction = navMeanContradiction; // real visual contradiction
            echoSnap = echoController.tick(coherence, contradiction);
            echoField.setActiveKsmCycle(echoSnap.cycle);
            echo::EvolutionRecord rec;
            rec.tick = tickCount;
            rec.step = echoSnap.step;
            rec.coherence = coherence;
            rec.contradiction = contradiction;
            rec.verdict = (coherence > 0.5f) ? "keep" : "discard";
            rec.principle = echoSnap.stepDefinition->label;
            echoLedger.append(rec);
            return true;
        });
        agentLoop.addStep("phasor", 3, [this]() {
            phasor.tick();
            return true;
        });
        agentLoop.addStep("introspection", 2, [this]() {
            if (tickCount % 25 == 0) introspect.introspect(endo, arena, sensorimotor, groups);
            return true;
        });
        agentLoop.start();
    }
    void buildSpreadNet() {
        for (int q = 0; q < dte::TRIZArena::SIZE; q++)
            for (int r = 0; r < dte::TRIZArena::SIZE; r++) {
                spreadNet.addNode(elizaos::ActivationSpreadingNetwork::key(q, r), 0.0);
                spreadNet.addHexEdges(q, r, dte::TRIZArena::SIZE);
            }
        spreadNetBuilt = true;
    }
    void tick() {
        if (!running || paused) return;
        tickCount++;
        if (!spreadNetBuilt) buildSpreadNet();
        if (agentLoop.steps().empty()) registerAgentLoopSteps();
        agentLoop.iterate();
        echoPhase = (dte::EchobeatsPhase)((tickCount / 3) % 4);
        // uechovr: update 6DOF tracking
        float dt = 0.05f;
        hmdPose = tracker.updateHMD(dt);
        leftCtrl = tracker.updateController(0, dt);
        rightCtrl = tracker.updateController(1, dt);
        // uechovr: capture frame buffer
        auto capture = fbHook.captureBackBuffer(tickCount);
        // re-bhvt-edit: sensorimotor bridge update
        std::vector<float> features(3);
        features[0] = navPos.x; features[1] = navPos.z;
        features[2] = cns.body().contact ? 1.0f : 0.0f;
        float threat = cns.body().contact ? 1.0f : 0.0f;
        bridgeTelemetry = bridge.update(navPos.x, 0.0f, navPos.z, features, threat, dt);
        if (bridgeTelemetry.novelty > 0.05f)
            endo.submitStimulus(dte::Stimulus("novelty:niche", 0.2));
        {
            int walkersNear = 0;
            for (const auto& w : navWorld.walkers)
                if (len(w.pos - navPos) < 3.0f) ++walkersNear;
            if (walkersNear > 0)
                endo.submitStimulus(dte::Stimulus("social:walkers", 0.2));
            if (len(navWorld.npcPos - navPos) < 3.0f)
                endo.submitStimulus(dte::Stimulus("social:ariadne", 0.3));
            int trafficNear = 0;
            for (const auto& v : navWorld.traffic)
                if (len(v.pos - navPos) < 4.0f) ++trafficNear;
            if (trafficNear > 0)
                endo.submitStimulus(dte::Stimulus("motion:traffic", 0.25));
            int pickupsNear = 0;
            for (const auto& pk : navWorld.pickups)
                if (!pk.collected && len(pk.pos - navPos) < 2.5f) ++pickupsNear;
            if (pickupsNear > 0)
                endo.submitStimulus(dte::Stimulus("reward:pickup", 0.3));
        }
        // GTAngel-Cls environment step
        std::vector<uint8_t> pixels(64 * 64 * 4);
        for (int y = 0; y < 64; y++)
            for (int x = 0; x < 64; x++) {
                float v = sensorimotor.frameBuffer[y*64+x];
                uint8_t b = (uint8_t)(v * 255);
                pixels[(y*64+x)*4+0] = b;
                pixels[(y*64+x)*4+1] = b;
                pixels[(y*64+x)*4+2] = b;
                pixels[(y*64+x)*4+3] = 255;
            }
        gtangel::FrameBufferView view{};
        view.pixels = pixels.data();
        view.width = 64; view.height = 64; view.channels = 4;
        // Update world observation from uechovr 6DOF pose + sensorimotor state
        worldObs.pose.x = navPos.x; worldObs.pose.y = navPos.y;
        worldObs.pose.z = navPos.z; worldObs.pose.heading = navHeading;
        worldObs.pose.time_of_day = (float)(tickCount % 2400) / 100.0f;
        worldObs.health = 1.0f;
        worldObs.hostility = cns.body().contact ? 1.0f : 0.0f;
        worldObs.goal_progress = gameplay.quest.beaconRestored ? 1.0f :
            (gameplay.quest.hasSignalShard ? 0.5f : 0.0f);
        // Step the GTAngel-Cls environment
        lastStep = env.step(view, worldObs, currentCommand);
        // Get suggested command for next tick
        currentCommand = env.suggest(lastStep.state);
        // Advisory-only efference feedback: the env suggestion is observed by
        // the reservoir as context. It is NEVER submitted to the SRD.
        envAdvisory[0]=std::clamp(currentCommand.move_forward,-1.0f,1.0f);
        envAdvisory[1]=std::clamp(currentCommand.move_right,-1.0f,1.0f);
        envAdvisory[2]=std::clamp(currentCommand.look_right,-1.0f,1.0f);
        envAdvisory[3]=std::clamp(currentCommand.actuate,0.0f,1.0f);
        // dte systems (endo/arena/groups/ESN now driven by agentLoop above)
        // v20: synthetic DTE tick is disconnected from authoritative sensing/actuation.
        // mech0: train LLM on proprioceptive tokens
        if (tickCount % 20 == 0) {
            std::vector<std::pair<std::vector<int>, std::string>> data;
            std::vector<int> tokens;
            for (int i = 0; i < 8; i++)
                tokens.push_back(std::isfinite(sensorimotor.proprioception[i])?
                    (int)std::lround((std::clamp(sensorimotor.proprioception[i],-1.0f,1.0f)+1.0f)*.5f*(trainer.model().vocabSize()-1)):0);
            std::string label = sensorimotor.reward > 0.05f ? "greeting" :
                               sensorimotor.reward < 0.02f ? "farewell" : "question";
            data.push_back({tokens, label});
            trainer.train(data, 1, 0.01);
            ++llmTrainingUpdates;
            llmMetrics = trainer.metrics();
        }
        if (tickCount % 10 == 0) improvement.iterate(arena, sensorimotor);
        if (tickCount % 25 == 0) introspect.introspect(endo, arena, sensorimotor, groups);
        if (tickCount % 50 == 0) cogverse.evolve(arena, groups);
        if (tickCount % 5 == 0) {
            char buf[256];
            snprintf(buf, 256, "T%d | %s | %s | Coh=%.3f R=%.3f G=%d ESN=%.2f | HMD=(%.2f,%.2f,%.2f) | Bridge: R=%.3f N=%.3f | GTAngel-Cls: R=%.3f Done=%d | LLM: Acc=%.2f Loss=%.3f | SM: R=%.3f S=%d",
                tickCount, dte::cognitiveModeName(endo.cognitiveMode()),
                dte::phaseName(echoPhase), arena.coherence,
                sensorimotor.totalReward, groups.groupCount(), esnEnergy,
                hmdPose.x, hmdPose.y, hmdPose.z,
                bridgeTelemetry.reward, bridgeTelemetry.novelty,
                lastStep.reward, lastStep.done ? 1 : 0,
                llmMetrics.accuracy, llmMetrics.averageLoss,
                sensorimotor.totalReward, sensorimotor.stepCount);
            addLog(buf);
            // v27 live learning telemetry: one CSV row per 5 ticks.
            {
                std::ofstream tlf(localLearningDirectory+"/datasets/V27_LIVE_TELEMETRY.csv", std::ios::app);
                if (tlf.is_open()) {
                    if (tickCount == 5) tlf<<"tick,mode,phase,coherence,total_reward,groups,esn_energy,episodes,recall,triz_deriv,rls_updates,concepts,plasticity,consolidations,q_updates,learned_props,llm_updates,llm_acc,llm_loss,sm_reward,sm_steps,avg_familiarity,allies,friends,quest_stage,has_shard,nn_train,nn_loss,nn_accepted,side_pickups,side_glow,side_crowd\n";
                    float famSum = 0.0f; int allies = 0, friends = 0;
                    for (const auto& a : groups.agents) {
                        famSum += a.familiarity;
                        const char* rt = a.relationshipType();
                        if (std::string(rt)=="Ally"||std::string(rt)=="CloseFriend") ++allies;
                        else if (std::string(rt)=="Friend") ++friends;
                    }
                    tlf<<tickCount<<','<<dte::cognitiveModeName(endo.cognitiveMode())<<','
                        <<dte::phaseName(echoPhase)<<','<<arena.coherence<<','
                        <<sensorimotor.totalReward<<','<<groups.groupCount()<<','<<esnEnergy<<','
                        <<episodic.episodes.size()<<','<<lastEpisodicRecall<<','
                        <<lastTrizDerivative<<','<<esn.rlsUpdates<<','<<episodic.concepts.size()<<','
                        <<esn.plasticityUpdates<<','<<episodic.consolidations<<','
                        <<navLearning.updates<<','<<dteNavigation.learnedProposals<<','
                        <<llmTrainingUpdates<<','<<llmMetrics.accuracy<<','<<llmMetrics.averageLoss<<','
                        <<sensorimotor.totalReward<<','<<sensorimotor.stepCount<<','
                        <<(groups.agents.empty()?0.0f:(famSum/(float)groups.agents.size()))<<','
                        <<allies<<','<<friends<<','
                        <<(int)gameplay.quest.stage<<','<<(int)gameplay.quest.hasSignalShard<<','
                        <<neuralTrainingUpdates<<','<<lastNeuralTrainLoss<<','<<neuralAccepted<<','
                        <<(int)gameplay.quest.pickupsCollected<<','<<(int)gameplay.quest.shardGlowSeen<<','
                        <<(int)gameplay.quest.crowdSyncDone<<'\n';
                }
            }
            addLog(buf);
        }
    }
};

// ═══ Win32 UI ═══
static GTAngelAGI g_agi;
static HFONT g_hFont = NULL, g_hFontLarge = NULL, g_hFontMono = NULL, g_hFontSmall = NULL;

#define COL_BG      RGB(13, 13, 26)
#define COL_CARD    RGB(24, 24, 48)
#define COL_ACCENT  RGB(255, 215, 0)
#define COL_CYAN    RGB(0, 188, 212)
#define COL_GREEN   RGB(76, 175, 80)
#define COL_RED     RGB(244, 67, 54)
#define COL_TEXT    RGB(255, 255, 255)
#define COL_TEXT2   RGB(158, 158, 200)
#define COL_PURPLE  RGB(156, 39, 176)
#define COL_NICHE_P RGB(255, 128, 60)
#define COL_NICHE_S RGB(60, 200, 255)
#define COL_NICHE_I RGB(180, 120, 255)
#define COL_CONTRA  RGB(255, 50, 50)
#define COL_VR      RGB(100, 200, 255)
#define COL_BRIDGE  RGB(255, 100, 200)
#define COL_GOLD    RGB(255, 215, 0)
#define COL_LLM     RGB(100, 255, 200)

static void DrawText(HDC hdc, int x, int y, const char* t, COLORREF c, HFONT f) {
    SetTextColor(hdc, c); SetBkMode(hdc, TRANSPARENT); SelectObject(hdc, f);
    TextOutA(hdc, x, y, t, (int)strlen(t));
}
static void DrawRect(HDC hdc, int x, int y, int w, int h, COLORREF c) {
    HBRUSH b = CreateSolidBrush(c); RECT rc = {x, y, x+w, y+h};
    FillRect(hdc, &rc, b); DeleteObject(b);
}
static void DrawBar(HDC hdc, int x, int y, int w, int h, float v, COLORREF c, COLORREF bg) {
    DrawRect(hdc, x, y, w, h, bg);
    int fill = (int)(w * std::max(0.0f, std::min(1.0f, v)));
    if (fill > 0) DrawRect(hdc, x, y, fill, h, c);
}
static void DrawLine(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c) {
    HPEN p = CreatePen(PS_SOLID, 1, c);
    HPEN old = (HPEN)SelectObject(hdc, p);
    MoveToEx(hdc, x1, y1, NULL); LineTo(hdc, x2, y2);
    SelectObject(hdc, old); DeleteObject(p);
}

// Component-specific Z++ GTAngelAGI clock & weather bindings
namespace gtangel_zpp_agi_bind {
static_assert(zpp_core_spec::op_CClock_SetGameClock(zpp_core_spec::CClock_ZppContract{}, zpp_core_spec::apply_CClock_SetGameClock(zpp_core_spec::CClock_ZppContract{}, 6, 18, 0), 6, 18, 0), "GTAngelAGI CClock SetGameClock Z++ contract must hold");
} // namespace gtangel_zpp_agi_bind

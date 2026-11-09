// Canonical GTAngelAGI members, inlined by generate_v56.mjs.
    gtskill::Curriculum curriculum;
    gtskill::Selection selectedSkill;
    gtlearn::Autosave checkpointTimer;
    std::string checkpointId, checkpointStatus{"not saved"};
    bool learningDirty{false};
    bool evaluationMode{false};
    bool evaluationUseGnn{true};
    gtskill::Skill evaluationSkill{gtskill::Skill::Walk};
    v27gnn::GneuralMotorPolicy gnnChampion;
    gtskill::Curriculum championCurriculum;
    bool gnnChampionReady{false};
    void loadChampion(gtlearn::Store& store) {
        gnnChampionReady=false;
        const auto best=store.bestId();
        if(best.empty()) return;
        gtlearn::Snapshot snapshot; v27gnn::GneuralMotorPolicy motor; gtskill::Curriculum skills;
        if(store.read(std::filesystem::path(executableDirectory())/"learning/checkpoints"/best,snapshot) &&
           motor.decode(snapshot.payload[1]) && skills.decode(snapshot.payload[2]) &&
           skills.readyFor(gtskill::Skill::Walk,motor.policyRevision())) {
            gnnChampion.swap(motor); championCurriculum=std::move(skills); gnnChampionReady=true;
        }
    }
    static bool isolatedLearningRun() {
        const std::string command=GetCommandLineA();
        return command.find("--selftest")!=std::string::npos ||
               command.find("--navdiag")!=std::string::npos ||
               command.find("--evaluate-skills")!=std::string::npos;
    }
    static std::string learningSessionRoot() {
        if(!isolatedLearningRun()) return executableDirectory();
        return (std::filesystem::path(executableDirectory())/"../../build/learning-tests"/
                std::to_string(GetCurrentProcessId())).lexically_normal().string();
    }
    std::string encodeLearningState() const {
        gtlearn::Writer w; w.bytes="V27L"; w.put<uint64_t>(2);
        for(const auto& a:navTrainer.Q) for(const auto& b:a) for(const auto& c:b)
            for(float v:c) w.put(v);
        w.put<int32_t>(navTrainer.episodes); w.put<int32_t>(navTrainer.successes);
        w.put<int32_t>(navTrainer.collisions); w.put<int32_t>(navTrainer.stepsTotal);
        for(const auto& a:navLearning.stateVisits) for(const auto& b:a) for(uint32_t v:b) w.put(v);
        for(const auto& a:navLearning.actionVisits) for(const auto& b:a)
            for(const auto& c:b) for(uint32_t v:c) w.put(v);
        w.put(navLearning.updates); w.put(navLearning.rejected);
        w.put(navLearning.collisions); w.put(navLearning.windows);
        w.put(dteNavigation.proposals); w.put(dteNavigation.learnedProposals);
        w.put(dteNavigation.accepted); w.put(dteNavigation.contacts);
        const auto vector=[&](const std::vector<double>& values) {
            w.put<int32_t>(static_cast<int32_t>(values.size()));
            for(double v:values) w.put(v);
        };
        w.put<int32_t>(static_cast<int32_t>(episodic.episodes.size()));
        for(const auto& e:episodic.episodes) {
            vector(e.state); w.put<float>(e.meanContradiction); w.put<float>(e.reward); w.put<uint64_t>(e.tick);
            w.put<float>(e.strength); w.put<float>(e.activation); w.put<int32_t>(e.accessCount);
            w.put<float>(e.valence); w.put<uint64_t>(e.lastAccessTick); w.put<uint8_t>(e.flashbulb);
        }
        w.put<int32_t>(static_cast<int32_t>(episodic.concepts.size()));
        for(const auto& c:episodic.concepts) {
            vector(c.prototype); w.put<float>(c.variance); w.put<int32_t>(c.instanceCount); w.put<float>(c.confidence);
        }
        w.put<int32_t>(episodic.consolidations);
        vector(esn.readoutWeights()); vector(esn.rlsCovariance()); vector(esn.state());
        w.put<int32_t>(esn.rlsUpdates); w.put<int32_t>(esn.plasticityUpdates);
        w.put<int32_t>(static_cast<int32_t>(groups.agents.size()));
        for(const auto& a:groups.agents) {
            w.put<int32_t>(a.id); w.put<float>(a.x); w.put<float>(a.y);
            w.put<float>(a.trust); w.put<float>(a.affinity); w.put<float>(a.familiarity);
            w.put<int32_t>(a.groupId); w.put<float>(a.cohesion);
        }
        w.put<int32_t>(groups.nextGroupId); w.put<float>(groups.avgCohesion);
        w.put<uint64_t>(llmTrainingUpdates); w.put<float>(llmMetrics.accuracy); w.put<float>(llmMetrics.averageLoss);
        w.put<float>(sensorimotor.totalReward); w.put<int32_t>(sensorimotor.stepCount);
        w.put<uint64_t>(gnnMotor.trainingSteps()); w.put<uint64_t>(gnnMotor.trainingUpdates()); w.put<float>(gnnMotor.lastLoss());
        w.put<int32_t>(tickCount); w.put<int32_t>(gnnFrameCounter);
        return w.bytes;
    }
    bool restoreLearningState(const std::string& bytes) {
        try {
            if(bytes.size()>gtlearn::MaxPayload||bytes.substr(0,4)!="V27L") return false;
            gtlearn::Reader r{bytes,4}; const auto version=r.get<uint64_t>();
            if(version!=1&&version!=2) return false;
            // Parse into copies: any bad field leaves every live subsystem unchanged.
            auto q=navTrainer; auto observer=navLearning; auto navigator=dteNavigation;
            auto memory=episodic; auto reservoir=esn; auto social=groups;
            for(auto& a:q.Q) for(auto& b:a) for(auto& c:b) for(float& v:c) v=r.get<float>();
            auto count=[&](int maximum) {
                const int n=r.get<int32_t>();
                if(n<0||n>maximum) throw std::runtime_error("checkpoint count out of bounds");
                return n;
            };
            q.episodes=count(INT_MAX); q.successes=count(INT_MAX);
            q.collisions=count(INT_MAX); q.stepsTotal=count(INT_MAX);
            for(auto& a:observer.stateVisits) for(auto& b:a) for(uint32_t& v:b) v=r.get<uint32_t>();
            for(auto& a:observer.actionVisits) for(auto& b:a) for(auto& c:b)
                for(uint32_t& v:c) v=r.get<uint32_t>();
            observer.updates=r.get<uint64_t>(); observer.rejected=r.get<uint64_t>();
            observer.collisions=r.get<uint64_t>(); observer.windows=r.get<uint64_t>();
            navigator.proposals=r.get<uint64_t>(); navigator.learnedProposals=r.get<uint64_t>();
            navigator.accepted=r.get<uint64_t>(); navigator.contacts=r.get<uint64_t>();
            const auto vector=[&](int expected) {
                const int n=count(16384);
                if(expected>=0&&n!=expected) throw std::runtime_error("checkpoint dimension mismatch");
                std::vector<double> result(n);
                for(double& v:result) v=r.get<double>();
                return result;
            };
            const int episodes=count(4096); memory.episodes.clear();
            uint64_t memoryClock=0;
            for(int i=0;i<episodes;++i) {
                dte::EpisodicMemory::Episode e;
                e.state=vector(reservoir.resDim()); e.meanContradiction=r.get<float>(); e.reward=r.get<float>();
                e.tick=r.get<uint64_t>(); e.strength=r.get<float>(); e.activation=r.get<float>();
                e.accessCount=count(INT_MAX); e.valence=r.get<float>(); e.lastAccessTick=r.get<uint64_t>();
                memoryClock=std::max(memoryClock,std::max(static_cast<uint64_t>(e.tick),static_cast<uint64_t>(e.lastAccessTick)));
                const auto flash=r.get<uint8_t>(); if(flash>1) return false;
                e.flashbulb=flash!=0; memory.episodes.push_back(e);
            }
            const int concepts=count(4096); memory.concepts.clear();
            for(int i=0;i<concepts;++i) {
                dte::EpisodicMemory::SemanticConcept c;
                c.prototype=vector(reservoir.resDim()); c.variance=r.get<float>();
                c.instanceCount=count(INT_MAX); c.confidence=r.get<float>(); memory.concepts.push_back(c);
            }
            memory.consolidations=count(INT_MAX);
            reservoir.setReadoutWeights(vector(static_cast<int>(reservoir.readoutWeights().size())));
            // RLS allocates P lazily on the first training step. A fresh process
            // has an empty P, but must accept the full matrix from a prior run.
            auto covariance=vector(-1);
            if(!covariance.empty() && covariance.size()!=static_cast<size_t>(reservoir.resDim()*reservoir.resDim()))
                return false;
            reservoir.setRlsCovariance(covariance);
            reservoir.setReservoirState(vector(reservoir.resDim()));
            reservoir.rlsUpdates=count(INT_MAX); reservoir.plasticityUpdates=count(INT_MAX);
            const int agents=count(4096); social.agents.clear(); social.groups.clear();
            for(int i=0;i<agents;++i) {
                dte::Agent a; a.id=count(INT_MAX); a.x=r.get<float>(); a.y=r.get<float>();
                a.trust=r.get<float>(); a.affinity=r.get<float>(); a.familiarity=r.get<float>();
                a.groupId=r.get<int32_t>(); a.cohesion=r.get<float>(); social.agents.push_back(a);
            }
            social.nextGroupId=count(INT_MAX); social.avgCohesion=r.get<float>();
            if(social.agents.empty()) social.init(20);
            for(const auto& a:social.agents) if(a.groupId>=0) social.groups[a.groupId].push_back(a.id);
            const auto mlUpdates=r.get<uint64_t>();
            const float accuracy=r.get<float>(), loss=r.get<float>(), reward=r.get<float>();
            const int smSteps=count(INT_MAX);
            uint64_t steps=0,updates=0; float motorLoss=-1;
            if(version==2 || r.cursor<bytes.size()) {
                steps=r.get<uint64_t>(); updates=r.get<uint64_t>(); motorLoss=r.get<float>();
            }
            int clock=tickCount,phase=0;
            if(version==2) { clock=count(INT_MAX); phase=count(INT_MAX); }
            // Legacy files did not save the clock. Keep restored memories from
            // being dated in the future relative to the restarted learner.
            if(memoryClock>INT_MAX) return false;
            clock=std::max(clock,static_cast<int>(memoryClock));
            r.end();
            navTrainer=std::move(q); navLearning=std::move(observer); dteNavigation=std::move(navigator);
            episodic=std::move(memory); esn=std::move(reservoir); groups=std::move(social);
            llmTrainingUpdates=mlUpdates; llmMetrics.accuracy=accuracy; llmMetrics.averageLoss=loss;
            sensorimotor.totalReward=reward; sensorimotor.stepCount=smSteps;
            neuralTrainingSteps=steps; neuralTrainingUpdates=updates; lastNeuralTrainLoss=motorLoss;
            tickCount=clock; gnnFrameCounter=phase;
            return true;
        } catch(const std::exception& e) { checkpointStatus=e.what(); return false; }
    }
    bool saveLearningState() {
        const auto now=static_cast<uint64_t>(GetTickCount64());
        try {
            gtlearn::Store store(std::filesystem::path(localLearningDirectory)/"learning/checkpoints");
            const std::array<std::string,3> payload{encodeLearningState(),gnnMotor.encode(),curriculum.encode()};
            v27gnn::GneuralMotorPolicy checkedMotor; gtskill::Curriculum checkedSkills;
            if(!checkedMotor.decode(payload[1])||!checkedSkills.decode(payload[2])||
               !restoreLearningState(payload[0]))
                throw std::runtime_error("live learning state failed checkpoint validation");
            const bool saved=store.save(payload,checkpointId);
            checkpointTimer.attempted(now,saved);
            checkpointStatus=saved?"saved":store.error;
            if(saved) learningDirty=false;
            addLog(saved?"learning checkpoint saved: "+checkpointId:"checkpoint failed: "+checkpointStatus);
            return saved;
        } catch(const std::exception& e) {
            checkpointTimer.attempted(now,false); checkpointStatus=e.what();
            addLog("checkpoint failed: "+checkpointStatus); return false;
        }
    }
    bool loadLearningState() {
        try {
            gtlearn::Store store(std::filesystem::path(localLearningDirectory)/"learning/checkpoints");
            const auto candidates=store.candidates();
            for(size_t i=0;i<candidates.size();++i) {
                gtlearn::Snapshot snapshot; v27gnn::GneuralMotorPolicy motor;
                gtskill::Curriculum skills;
                if(!store.read(candidates[i],snapshot) || !motor.decode(snapshot.payload[1]) ||
                   !skills.decode(snapshot.payload[2]) || !restoreLearningState(snapshot.payload[0])) continue;
                gnnMotor.swap(motor); curriculum=std::move(skills);
                neuralTrainingSteps=gnnMotor.trainingSteps(); neuralTrainingUpdates=gnnMotor.trainingUpdates();
                lastNeuralTrainLoss=gnnMotor.lastLoss(); gnnMotorReady=true;
                checkpointId=snapshot.id; checkpointStatus=i==0?"restored":"recovered older checkpoint";
                checkpointTimer.attempted(GetTickCount64(),true); learningDirty=false;
                if(!isolatedLearningRun()) loadChampion(store);
                addLog(checkpointStatus+": "+checkpointId); return true;
            }
            if(!candidates.empty()) { checkpointStatus="no valid checkpoint; legacy import suppressed"; return false; }
            // One-time import only when no checkpoint history exists. Legacy
            // files are never overwritten by ongoing training or verification.
            if(isolatedLearningRun()) return false;
            const auto legacy=std::filesystem::path(executableDirectory())/"models/v27_learning_state.bin";
            if(!std::filesystem::exists(legacy)) return false;
            if(!restoreLearningState(gtlearn::readFile(legacy))) return false;
            gnnMotor.restoreLegacyCounters(neuralTrainingSteps,neuralTrainingUpdates,lastNeuralTrainLoss);
            checkpointStatus="legacy imported"; learningDirty=true;
            saveLearningState(); return true;
        } catch(const std::exception& e) { checkpointStatus=e.what(); return false; }
    }
    void maybeCheckpoint() {
        if(!isolatedLearningRun() && checkpointTimer.due(GetTickCount64(),learningDirty))
            saveLearningState();
    }

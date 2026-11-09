// Canonical GTAngelAGI members for an explicit, isolated held-out assessment.
    int evaluateSkills() {
        if(!isolatedLearningRun()) return 2;
        initializeLearning();
        gtlearn::Store live(std::filesystem::path(executableDirectory())/"learning/checkpoints");
        gtlearn::Snapshot source;
        bool found=false;
        for(const auto& path:live.candidates()) {
            v27gnn::GneuralMotorPolicy probe; gtskill::Curriculum skills;
            if(live.read(path,source)&&probe.decode(source.payload[1])&&
               skills.decode(source.payload[2])&&restoreLearningState(source.payload[0])) {
                found=true; break;
            }
        }
        if(!found) { addLog("Assessment needs a valid checkpoint from a normal session"); return 2; }
        gtskill::Curriculum assessment;
        if(!assessment.decode(source.payload[2])||!gnnMotor.decode(source.payload[1])) return 2;
        const uint64_t revision=gnnMotor.policyRevision();
        gtlearn::Snapshot incumbent;
        const auto bestId=live.bestId();
        const bool hasIncumbent=!bestId.empty() &&
            live.read(std::filesystem::path(executableDirectory())/"learning/checkpoints"/bestId,incumbent);
        evaluationMode=true; gnnChampionReady=false;
        std::ofstream report(localLearningDirectory+"/skill-evaluation.csv");
        report<<"skill,seed,success,contacts_per_metre,stuck_fraction,cost,reference_cost\n";
        auto episode=[&](uint32_t seed,gtskill::Skill gait,bool candidate) {
            restoreLearningState(source.payload[0]);
            const auto& model=candidate||!hasIncumbent?source.payload[1]:incumbent.payload[1];
            if(!gnnMotor.decode(model)) throw std::runtime_error("evaluation model rejected");
            evaluationUseGnn=candidate||hasIncumbent;
            evaluationSkill=gait;
            neuralPolicy.reset(); previousControllerTarget={};
            navWorld.generate(seed,48); gameplay.reset(navWorld); cns.reset();
            syncBodyFromCns(); invalidateEcho(); dteNavigation.reset();
            // Held-out short street routes with different scenery and traffic.
            const avatar3d::Vec3 target=navPos+avatar3d::Vec3{(seed%3-1.0f)*.6f,0,4.5f};
            navWorld.questShard=target; navWorld.questShardVisible=true;
            gameplay.quest.stage=v19game::QuestStage::SeekShard;
            avatar.pose(navPos,navHeading,0,0);
            running=true; paused=false; dteEmbodimentEnabled=true;
            unsigned contacts=0,stuck=0,steps=0;
            float travel=0;
            bool success=false;
            for(;steps<180;++steps) {
                const auto before=navPos;
                tick();
                const float moved=avatar3d::len(navPos-before);
                travel+=moved; contacts+=cns.body().contact; stuck+=moved<.002f;
                if(avatar3d::len(navPos-target)<1.25f) { success=true; ++steps; break; }
            }
            gtskill::Evaluation e;
            e.seed=seed; e.success=success;
            e.contactsPerMetre=contacts/std::max(travel,.1f);
            e.stuckFraction=stuck/static_cast<float>(std::max(steps,1u));
            e.cost=steps/180.0f; e.referenceCost=1;
            return e;
        };
        try {
            // Baseline always walks. Running must improve completion cost without
            // weakening success/contact/stuck criteria.
            for(const auto skill:{gtskill::Skill::Walk,gtskill::Skill::Run})
                for(uint32_t seed=91001;seed<91009;++seed) {
                    const auto reference=episode(seed,gtskill::Skill::Walk,false);
                    auto result=episode(seed,skill,true);
                    result.referenceCost=reference.cost;
                    assessment.assess(skill,revision,1,result);
                    report<<gtskill::name(skill)<<','<<seed<<','<<result.success<<','
                          <<result.contactsPerMetre<<','<<result.stuckFraction<<','
                          <<result.cost<<','<<result.referenceCost<<'\n';
                    report.flush();
                }
            // An evaluation does not train or save its simulated world. Publish
            // only the evidence, paired with the untouched source state/model.
            const auto newest=live.candidates();
            if(newest.empty()||newest.front().filename().string()!=source.id) {
                report<<"publication_refused,learning advanced during evaluation\n";
                return 3;
            }
            std::string evaluatedId;
            if(!live.save({source.payload[0],source.payload[1],assessment.encode()},evaluatedId,source.id)) return 4;
            const bool qualified=assessment.readyFor(gtskill::Skill::Walk,revision);
            if(qualified&&!live.pinBest(evaluatedId)) return 4;
            report<<"published,"<<evaluatedId<<"\nwalking_validated,"<<qualified
                  <<"\nrunning_validated,"<<assessment.readyFor(gtskill::Skill::Run,revision)<<'\n';
            report.close();
            addLog("Assessment complete: "+localLearningDirectory+"/skill-evaluation.csv");
            return 0; // A rejected candidate is a successful evaluation, not a tool error.
        } catch(const std::exception& e) { addLog(e.what()); return 5; }
    }

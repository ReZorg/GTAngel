// Canonical GTAngelAGI members for isolated practice and held-out assessment.
    static void resetSkillWorld(avatar3d::GameWorld& world,uint32_t seed) {
        // generate() repopulates geometry but does not reset moving traffic,
        // protected quest positions or all animation clocks.
        world=avatar3d::GameWorld{};
        world.generate(seed,48);
    }
    int evaluateSkills(bool practice=false) {
        if(!isolatedLearningRun()) return 2;
        int scenarioIndex=-1;
        const std::string command=GetCommandLineA();
        const auto selector=command.find("--scenario=");
        if(selector!=std::string::npos) {
            const auto digit=selector+11;
            if(practice||digit>=command.size()||command[digit]<'0'||command[digit]>'7'||
               (digit+1<command.size()&&command[digit+1]!=' ')) return 2;
            scenarioIndex=command[digit]-'0';
        }
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
        if(!found) { addLog("Practice/assessment needs a valid session checkpoint"); return 2; }
        gtskill::Curriculum assessment;
        if(!assessment.decode(source.payload[2])||!gnnMotor.decode(source.payload[1])) return 2;
        curriculum=assessment;
        gnnMotor.adoptCurrentInputContract();
        source.payload[1]=gnnMotor.encode(); // Explicit contract migration, preserving weights.
        const uint64_t revision=gnnMotor.policyRevision(), stepsBefore=gnnMotor.trainingSteps();
        gtlearn::Snapshot incumbent;
        const auto bestId=live.bestId();
        v27gnn::GneuralMotorPolicy incumbentMotor;
        gtskill::Curriculum incumbentSkills;
        const bool hasIncumbent=!bestId.empty() &&
            live.read(std::filesystem::path(executableDirectory())/"learning/checkpoints"/bestId,incumbent) &&
            incumbentMotor.decode(incumbent.payload[1]) && incumbentSkills.decode(incumbent.payload[2]) &&
            incumbentSkills.readyFor(gtskill::Skill::Walk,incumbentMotor.policyRevision());
        evaluationMode=!practice; practiceMode=practice; gnnChampionReady=false;
        std::ofstream report(localLearningDirectory+(practice?"/walking-practice.csv":"/skill-evaluation.csv"));
        if(!report) return 4;
        report<<"role,skill,scenario,seed,success,contacts_per_metre,stuck_fraction,cost,reference_cost,gradient_steps\n";
        std::ofstream trace;
        if(scenarioIndex>=0) {
            trace.open(localLearningDirectory+"/scenario-trace.csv");
            trace<<"candidate,skill,seed,tick,x,z,heading,remaining,guard,forward,turn,contact\n";
        }
        // Reset the signals used by motor proprioception for each paired trial.
        const auto initialEndo=endo;
        const auto initialSensorimotor=sensorimotor;
        const auto initialArena=arena;
        const auto initialStep=lastStep;
        const auto initialTriz=trizMap;
        auto episode=[&](const gtskill::Scenario& scene,gtskill::Skill gait,bool candidate) {
            if(!restoreLearningState(source.payload[0])) throw std::runtime_error("evaluation state rejected");
            endo=initialEndo; sensorimotor=initialSensorimotor; arena=initialArena;
            lastStep=initialStep; trizMap=initialTriz;
            if(!practice) {
                const auto& model=candidate||!hasIncumbent?source.payload[1]:incumbent.payload[1];
                if(!gnnMotor.decode(model)) throw std::runtime_error("evaluation model rejected");
            }
            evaluationUseGnn=candidate||hasIncumbent;
            evaluationSkill=gait;
            neuralPolicy.reset(); previousControllerTarget={}; lastBodyProprio={}; lastModelProprio={};
            avatar=avatar3d::AvatarSkeleton{}; walkingGuard.reset(); navEpisodeSteps=0; testHeldKey=0;
            std::srand(scene.seed);
            resetSkillWorld(navWorld,scene.seed); gameplay.reset(navWorld); cns.reset();
            auto body=cns.body(); body.heading=scene.heading; body.speed=0;
            if(!cns.restoreLocal(body,cns.niche(),gameplay,navWorld))
                throw std::runtime_error("scenario spawn is not clear");
            syncBodyFromCns(); invalidateEcho(); dteNavigation.reset();
            const avatar3d::Vec3 target=navPos+avatar3d::Vec3{scene.lateral,0,4.5f};
            if(scene.obstacle) navWorld.pillars.push_back({navPos+avatar3d::Vec3{0,0,1.8f},.35f,2.5f});
            navWorld.questShard=target; navWorld.questShardVisible=true;
            gameplay.quest.stage=v19game::QuestStage::SeekShard;
            gameplay.quest.shardVisible=true;
            avatar.pose(navPos,navHeading,0,0);
            const auto eye=navThirdPerson
                ? navWorld.collisionFreeEye(navPos,navPos+avatar3d::Vec3{-sinf(navHeading)*4,2.5f,-cosf(navHeading)*4})
                : navPos+avatar3d::Vec3{0,1.6f,0};
            highResRenderer.render(navWorld,avatar,eye,navHeading,navThirdPerson?-.4f:0,navThirdPerson);
            perceptionPyramid.derive(highResRenderer.framebuffer);
            perceptionPyramid.copyToLegacy(navRenderer);
            running=true; paused=false; dteEmbodimentEnabled=true;
            unsigned contacts=0,stuck=0,steps=0;
            float travel=0;
            bool success=false;
            for(;steps<240;++steps) {
                const auto before=navPos; const float headingBefore=navHeading;
                tick();
                const float moved=avatar3d::len(navPos-before);
                travel+=moved; contacts+=cns.body().contact;
                if(scenarioIndex>=0 && steps%10==0)
                    trace<<candidate<<','<<gtskill::name(gait)<<','<<scene.seed<<','<<steps<<','
                         <<navPos.x<<','<<navPos.z<<','<<navHeading<<','<<avatar3d::len(navPos-target)<<','
                         <<(walkingGuardActive?walkingGuard.reason():"baseline")<<','<<previousControllerTarget.values[1]<<','
                         <<-previousControllerTarget.values[0]<<','<<cns.body().contact<<'\n';
                const float rotation=std::fabs(std::remainder(navHeading-headingBefore,6.2831853f));
                // Deliberate rotation is progress, not a stuck frame.
                stuck+=moved<.002f && rotation<.002f && std::fabs(previousControllerTarget.values[1])>.05f;
                if(avatar3d::len(navPos-target)<1.25f) { success=true; ++steps; break; }
            }
            gtskill::Evaluation e;
            e.seed=scene.seed; e.success=success;
            e.contactsPerMetre=contacts/std::max(travel,.1f);
            e.stuckFraction=stuck/static_cast<float>(std::max(steps,1u));
            e.cost=steps/240.0f; e.referenceCost=1;
            return e;
        };
        auto row=[&](const char* role,gtskill::Skill skill,const gtskill::Scenario& scene,const gtskill::Evaluation& result) {
            report<<role<<','<<gtskill::name(skill)<<','<<scene.name<<','<<scene.seed<<','<<result.success<<','
                  <<result.contactsPerMetre<<','<<result.stuckFraction<<','
                  <<result.cost<<','<<result.referenceCost<<','<<gnnMotor.trainingSteps()<<'\n';
            report.flush();
            if(!report) throw std::runtime_error("practice/assessment report write failed");
        };
        try {
            if(practice) {
                // Two bounded passes use training-only seeds. Motor learning
                // continues across episodes; practice-world memories/Q state
                // never replace the live agent's state.
                for(unsigned pass=0;pass<2;++pass)
                    for(unsigned i=0;i<gtskill::ScenarioCount;++i) {
                        const auto scene=gtskill::scenario(i,true);
                        row("practice",gtskill::Skill::Walk,scene,episode(scene,gtskill::Skill::Walk,false));
                    }
            } else {
                for(unsigned i=0;i<gtskill::ScenarioCount;++i) {
                    if(scenarioIndex>=0 && static_cast<int>(i)!=scenarioIndex) continue;
                    const auto scene=gtskill::scenario(i);
                    const auto reference=episode(scene,gtskill::Skill::Walk,false);
                    row("reference",gtskill::Skill::Walk,scene,reference);
                    for(const auto skill:{gtskill::Skill::Walk,gtskill::Skill::Run}) {
                        auto result=episode(scene,skill,true);
                        result.referenceCost=reference.cost;
                        assessment.assess(skill,revision,gtskill::EvaluationSuite,result);
                        row("candidate",skill,scene,result);
                    }
                }
            }
            if(scenarioIndex>=0) {
                report<<"diagnostic_only,no checkpoint or promotion published\n";
                return 0;
            }
            // Compare-and-publish under the writer lock: an isolated process
            // cannot overwrite learning that advanced during its run.
            std::string publishedId;
            const auto motorState=practice?gnnMotor.encode():source.payload[1];
            auto persistedState=source.payload[0];
            if(practice) {
                assessment=curriculum;
                if(!restoreLearningState(source.payload[0])) throw std::runtime_error("source state no longer valid");
                // Preserve the live Q/ESN/memory state while synchronizing the
                // redundant neural counters with the practiced motor learner.
                persistedState=encodeLearningState();
            }
            if(!live.save({persistedState,motorState,assessment.encode()},publishedId,source.id)) {
                report<<"publication_refused,"<<live.error<<'\n'; return 4;
            }
            const bool qualified=!practice&&assessment.readyFor(gtskill::Skill::Walk,revision);
            if(qualified&&!live.pinBest(publishedId)) return 4;
            report<<"published,"<<publishedId<<"\ngradient_steps_before,"<<stepsBefore
                  <<"\ngradient_steps_after,"<<gnnMotor.trainingSteps()<<'\n';
            if(!practice)
                report<<"walking_validated,"<<qualified<<"\nwalking_reason,"
                      <<assessment.record(gtskill::Skill::Walk).status()
                      <<"\nrunning_validated,"<<assessment.readyFor(gtskill::Skill::Run,revision)
                      <<"\nrunning_reason,"<<assessment.record(gtskill::Skill::Run).status()<<'\n';
            report.close();
            return report.fail()?4:0; // Rejected assessments are valid results.
        } catch(const std::exception& e) {
            report<<"error,"<<e.what()<<'\n'; addLog(e.what()); return 5;
        }
    }

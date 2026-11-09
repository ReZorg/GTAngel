
// ---- Patch generate_v27.mjs with Echo persona splices (idempotent) ----
const fs = require('fs');
const path = require('path');
const here = __dirname;
const gen = fs.readFileSync(path.join(here, 'generate_v27.mjs'), 'utf8');
const patch = `
// --- Echo persona binding (superhot-gamer-girl personas/echo.json) ---
// Emotion resolution from the live endocrine bus; classification only.
once('} // namespace v27dte', \`// Echo persona (superhot-gamer-girl personas/echo.json): resolves the live
// 10-channel endocrine bus into the persona emotion vocabulary used by
// echo.character.json endocrine.event_map / echo.live2d.json expressions.
// Pure classification: it never proposes, vetoes, or actuates anything.
struct EchoPersonaState {
    const char* emotion{"BORED_STOMPING"};   // echo.live2d.json key
    const char* expression{"PLAYFUL_BORED"};// Cubism expression name
    const char* facs{"AU43 AU15"};          // FACS AU set (display contract)
    float spice{0.0f};                      // resolved adaptive spice 0..1
    float confidence{0.0f};                // 1 - margin to nearest boundary
    uint64_t updates{0};
};
class EchoPersona {
public:
    EchoPersonaState resolve(double valence,double arousal,int cognitiveMode) {
        // Thresholds mirror dte::EndocrineSystem::computeCognitiveMode boundaries.
        const bool alarmed = cognitiveMode==3; // Alarm
        const bool resting  = cognitiveMode==2; // Rest
        const bool focused = cognitiveMode==1;  // Exploitation
        const char* emotion; const char* expression; const char* facs; float spice;
        if (alarmed && valence<-0.1) { emotion="GETTING_TILTED"; expression="FRUSTRATED_FOCUS"; facs="AU4 AU7 AU23"; spice=0.40f; }
        else if (valence>0.35 && arousal>0.15) { emotion="VICTORY_ROYALE"; expression="TRIUMPHANT_GRIN"; facs="AU6 AU12 AU25"; spice=0.40f; }
        else if (valence>0.35 && arousal<=0.15) { emotion="COZY_RECALL"; expression="WARM_REMEMBERING"; facs="AU6 AU12 AU43"; spice=0.30f; }
        else if (valence>0.15 && arousal>0.25) { emotion="CLUTCH_MOMENT"; expression="FOCUSED_INTENSE"; facs="AU4 AU5 AU7"; spice=0.40f; }
        else if (valence>0.15 && focused) { emotion="FLOW_STATE"; expression="SERENE_FOCUS"; facs="AU43 AU6"; spice=0.30f; }
        else if (valence>0.15) { emotion="RESONANT_SYNC"; expression="ATTUNED_SMILE"; facs="AU6 AU12 AU45"; spice=0.30f; }
        else if (valence<-0.15 && arousal>0.3) { emotion="SURPRISE_ATTACK"; expression="STARTLED_ALERT"; facs="AU1 AU2 AU5 AU26"; spice=0.40f; }
        else if (valence<-0.05 && !resting) { emotion="BRANCH_DIVERGE"; expression="MISCHIEVOUS_BRANCH"; facs="AU12R AU2 AU5"; spice=0.50f; }
        else if (resting) { emotion="COZY_RECALL"; expression="WARM_REMEMBERING"; facs="AU6 AU12 AU43"; spice=0.30f; }
        else { emotion="BORED_STOMPING"; expression="PLAYFUL_BORED"; facs="AU43 AU15"; spice=0.30f; }
        // Confidence = distance to the nearest decision boundary (saturated).
        const double m1=std::fabs(valence-0.35),m2=std::fabs(valence-0.15),m3=std::fabs(valence+0.05),
                     m4=std::fabs(arousal-0.15),m5=std::fabs(arousal-0.25);
        const double margin=std::min({m1,m2,m3,m4,m5});
        state_.emotion=emotion; state_.expression=expression; state_.facs=facs; state_.spice=spice;
        state_.confidence=(float)std::min(1.0,margin/0.20);
        ++state_.updates;
        return state_;
    }
    const EchoPersonaState& state() const { return state_; }
private:
    EchoPersonaState state_{};
};

} // namespace v27dte\`, 'echo persona type');
once('    v27dte::LocalNavigation dteNavigation;', \`    v27dte::LocalNavigation dteNavigation;
    v27dte::EchoPersona echoPersona; // display-only persona state (echo.json)\`, 'echo persona member');
once('            f.framebufferSequence = highResRenderer.metrics.sequence;', \`            f.framebufferSequence = highResRenderer.metrics.sequence;
            // Echo persona (display-only): resolve emotion from the live bus.
            const v27dte::EchoPersonaState echoState=
                echoPersona.resolve(f.valence,f.arousal,(int)endo.cognitiveMode());
            f.echoEmotion=echoState.emotion; f.echoExpression=echoState.expression;
            f.echoFacs=echoState.facs; f.echoSpice=echoState.spice;
            f.echoConfidence=echoState.confidence;\`, 'echo persona spine wiring');
once('    uint64_t framebufferSequence{0};', \`    // Echo persona display fields (superhot-gamer-girl echo.json contract).
    const char* echoEmotion{"BORED_STOMPING"};
    const char* echoExpression{"PLAYFUL_BORED"};
    const char* echoFacs{"AU43 AU15"};
    double echoSpice{0.0};
    double echoConfidence{0.0};
    uint64_t framebufferSequence{0};\`, 'echo persona frame fields');
once('DrawText(hdc,56,298,"Motor: DTE-LOCAL/SRD only | NN and route actuation remain LOCKED",COL_CYAN,g_hFontSmall);', \`DrawText(hdc,56,298,"Motor: DTE-LOCAL/SRD only | NN and route actuation remain LOCKED",COL_CYAN,g_hFontSmall);
        snprintf(info,sizeof(info),"Echo: %s / %s / spice %.2f / conf %.2f",
            echoPersona.state().emotion,echoPersona.state().facs,
            echoPersona.state().spice,echoPersona.state().confidence);
        DrawText(hdc,56,348,info,RGB(238,130,238),g_hFontSmall); // persona display only\`, 'echo persona monitor');
`;
if (gen.includes('echo persona type')) { console.log('splices already present'); process.exit(0); }
const marker = "fs.writeFileSync('gtangel_agi_v27_full.cpp',out);";
if (!gen.includes(marker)) throw new Error('write marker missing');
fs.writeFileSync(path.join(here, 'generate_v27.mjs'), gen.replace(marker, patch + '\n' + marker));
console.log('generate_v27.mjs patched with Echo persona splices');

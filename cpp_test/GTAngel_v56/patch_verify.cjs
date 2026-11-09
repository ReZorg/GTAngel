// ---- Add Echo persona checks to verify_v27_learning.cpp (idempotent) ----
const fs = require('fs');
const path = require('path');
const f = path.join(__dirname, 'verify_v27_learning.cpp');
let src = fs.readFileSync(f, 'utf8');
if (src.includes('echo persona')) { console.log('already patched'); process.exit(0); }
const anchor = '    v17policy::EmbodiedSafetyShield shield;';
const checks = `    // Echo persona (superhot-gamer-girl echo.json): emotion resolution contract.
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
`;
if (!src.includes(anchor)) throw new Error('anchor missing');
src = src.replace(anchor, checks + anchor);
fs.writeFileSync(f, src);
console.log('verify_v27_learning.cpp patched with echo persona checks');

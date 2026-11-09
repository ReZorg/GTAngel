#pragma once
#include <algorithm>
#include <cmath>

namespace gtskill {
// Local steering only. The callback is a read-only query of the body's actual
// swept collision geometry; returned demands still require a motor commit.
class WalkingGuard {
    bool hasGoal_{false}, detouring_{false};
    unsigned blockedTicks_{0};
    float goalX_{0},goalZ_{0},waypointX_{0},waypointZ_{0};
    const char* reason_{"direct"};
    static float angle(float v) {return std::remainder(v,6.2831853f);}
public:
    struct Demand {float forward,turn;};
    void reset() {*this=WalkingGuard{};}
    const char* reason() const {return reason_;}
    template<class ClearPath>
    Demand apply(float x,float z,float heading,float bearing,float distance,
                 float forward,float turn,const ClearPath& clear) {
        if(!std::isfinite(x)||!std::isfinite(z)||!std::isfinite(heading)||
           !std::isfinite(bearing)||!std::isfinite(distance)||distance<0||
           !std::isfinite(forward)||!std::isfinite(turn)) {
            reset(); reason_="invalid demand"; return {0,0};
        }
        if(distance<1.0f) {detouring_=false;reason_="at goal";return {0,0};}
        const float gx=x+std::sin(heading+bearing)*distance;
        const float gz=z+std::cos(heading+bearing)*distance;
        if(!hasGoal_||std::hypot(gx-goalX_,gz-goalZ_)>1) detouring_=false;
        hasGoal_=true;goalX_=gx;goalZ_=gz;
        if(detouring_) {
            const float remaining=std::hypot(waypointX_-x,waypointZ_-z);
            if(remaining<.2f) detouring_=false;
            else {
                const float desired=std::atan2(waypointX_-x,waypointZ_-z);
                const float error=angle(desired-heading);
                reason_="local detour";
                if(std::fabs(error)>.7f) return {0,std::clamp(error*.9f,-.55f,.55f)};
                const float length=std::min(.7f,remaining);
                const float steering=std::clamp(error*.9f,-.55f,.55f);
                const float next=heading+steering*.1f;
                if(clear(x+std::sin(next)*.045f,z+std::cos(next)*.045f) &&
                   clear(x+std::sin(next)*length,z+std::cos(next)*length)) {
                    blockedTicks_=0;
                    return {.45f,steering};
                }
                // The current heading may still point into the obstacle even
                // though the chosen path is clear. Finish turning, rather than
                // cancelling the waypoint and repeatedly switching sides.
                if(clear(waypointX_,waypointZ_) || ++blockedTicks_<20)
                    return {0,steering};
                detouring_=false; blockedTicks_=0;
            }
        }
        if(std::fabs(bearing)>.9f) {
            reason_="aligning before moving";
            return {0,std::clamp(bearing*.9f,-.55f,.55f)};
        }
        const float nextHeading=heading+turn*.1f;
        const float look=std::max(.45f,std::fabs(forward)*1.5f);
        const float sign=forward<0?-1.0f:1.0f;
        if(std::fabs(forward)<.01f ||
           (clear(x+std::sin(nextHeading)*forward*.1f,z+std::cos(nextHeading)*forward*.1f) &&
            clear(x+std::sin(nextHeading)*look*sign,z+std::cos(nextHeading)*look*sign))) {
            reason_="direct"; return {forward,turn};
        }
        if(forward>0) {
            // Whole short paths, not just endpoint circles, must be clear.
            for(float offset:{1.05f,-1.05f,1.57f,-1.57f,2.1f,-2.1f}) {
                const float h=heading+bearing+offset;
                const float wx=x+std::sin(h)*1.35f,wz=z+std::cos(h)*1.35f;
                if(clear(wx,wz)) {
                    waypointX_=wx;waypointZ_=wz;detouring_=true;blockedTicks_=0;reason_="local detour";
                    return {0,std::clamp(angle(h-heading)*.9f,-.55f,.55f)};
                }
            }
        }
        reason_="waiting for clearance";
        return {0,0};
    }
};
} // namespace gtskill

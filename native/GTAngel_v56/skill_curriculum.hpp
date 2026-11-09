#pragma once
#include "learning_checkpoint.hpp"
#include "walking_guard.hpp"

namespace gtskill {
constexpr uint32_t EvaluationSuite = 3; // fresh worlds and the actual incumbent controller
constexpr unsigned ScenarioCount = 8;
enum class Skill:uint32_t { Walk, Run, Drive, Quest, Strategy, Count };
struct Scenario {
    uint32_t seed;
    float heading, lateral;
    bool obstacle;
    const char* name;
};
inline Scenario scenario(unsigned index,bool practice=false) {
    if(index>=ScenarioCount) throw std::out_of_range("skill scenario");
    const unsigned kind=index/2;
    const float angle=index%2?2.4f:1.0f;
    return {(practice?71001u:91001u)+index,kind==1?angle:kind==2?-angle:0,
            index%2?.6f:-.6f,kind==3,
            kind==0?"straight":kind==1?"left turn":kind==2?"right turn":"detour"};
}
inline const char* name(Skill s) {
    constexpr const char* names[]={"walking","running","driving","quest execution","strategy"};
    return static_cast<size_t>(s)<5?names[static_cast<size_t>(s)]:"unknown";
}
struct Evaluation {
    uint32_t seed{0};
    bool success{false};
    float contactsPerMetre{0}, stuckFraction{0}, cost{0}, referenceCost{0};
};
struct Record {
    uint64_t observations{0}, contacts{0};
    double metres{0};
    uint64_t modelRevision{0};
    uint32_t suite{0};
    std::vector<Evaluation> evaluations;
    const char* status() const {
        if(suite!=EvaluationSuite) return "needs current assessment";
        if(evaluations.size()<ScenarioCount) return "more scenarios needed";
        unsigned successes=0;
        double cost=0, baseline=0;
        for(const auto& e:evaluations) {
            successes+=e.success;
            if(e.contactsPerMetre>.02f) return "contacts above limit";
            if(e.stuckFraction>.05f) return "too much blocked movement";
            cost+=e.cost; baseline+=e.referenceCost;
        }
        if(successes*10<evaluations.size()*9) return "completion rate too low";
        if(cost>baseline) return "slower than reference";
        return "validated";
    }
    bool validated() const {return std::string(status())=="validated";}
    unsigned successes() const {
        unsigned n=0; for(const auto& e:evaluations) n+=e.success; return n;
    }
};
struct Selection { Skill skill{Skill::Walk}; const char* reason{"walking baseline"}; };
struct Capabilities {
    bool running{false}, driving{false}; // Code-owned capabilities, never loaded from a file.
};
class Curriculum {
    std::array<Record,5> records_{};
public:
    const Record& record(Skill s) const { return records_.at(static_cast<size_t>(s)); }
    void observe(Skill s,float metres,bool contact) {
        if(!std::isfinite(metres)||metres<0||metres>2) return;
        auto& r=records_.at(static_cast<size_t>(s));
        ++r.observations; r.contacts+=contact; r.metres+=metres;
    }
    bool assess(Skill skill,uint64_t modelRevision,uint32_t suite,const Evaluation& e) {
        if(!modelRevision||!suite||!std::isfinite(e.contactsPerMetre)||
           !std::isfinite(e.stuckFraction)||!std::isfinite(e.cost)||!std::isfinite(e.referenceCost)||
           e.contactsPerMetre<0||e.stuckFraction<0||e.stuckFraction>1||
           e.cost<0||e.referenceCost<=0) return false;
        auto& r=records_.at(static_cast<size_t>(skill));
        if(r.modelRevision!=modelRevision||r.suite!=suite) {
            r.evaluations.clear(); r.modelRevision=modelRevision; r.suite=suite;
        }
        auto found=std::find_if(r.evaluations.begin(),r.evaluations.end(),
                               [&](const Evaluation& old){return old.seed==e.seed;});
        if(found!=r.evaluations.end()) *found=e;
        else {
            if(r.evaluations.size()==16) r.evaluations.erase(r.evaluations.begin());
            r.evaluations.push_back(e);
        }
        return true;
    }
    bool ready(Skill s) const {
        if(!record(s).validated()) return false;
        if(s==Skill::Walk) return true;
        if(s==Skill::Strategy) return ready(Skill::Quest);
        return ready(Skill::Walk);
    }
    bool readyFor(Skill s,uint64_t revision) const {
        if(!revision || !record(s).validated() || record(s).modelRevision!=revision) return false;
        if(s==Skill::Walk) return true;
        return readyFor(s==Skill::Strategy?Skill::Quest:Skill::Walk,revision);
    }
    Selection select(bool occupied,bool contact,float obstacleDepth,float targetDistance,
                     const Capabilities& capability,uint64_t revision=0) const {
        const auto qualified=[&](Skill s){return revision?readyFor(s,revision):ready(s);};
        if(occupied) return {Skill::Drive,capability.driving&&qualified(Skill::Drive)?
            "validated driving":"manual driving; autonomous authority unavailable"};
        if(contact||!std::isfinite(obstacleDepth)||!std::isfinite(targetDistance)||
           obstacleDepth<5||targetDistance<4)
            return {Skill::Walk,"near obstacle or interaction"};
        if(capability.running&&qualified(Skill::Run)) return {Skill::Run,"validated running"};
        return {Skill::Walk,!qualified(Skill::Run)?"running awaits evaluation":"running controller unavailable"};
    }
    std::string encode() const {
        gtlearn::Writer w; w.put<uint32_t>(1);
        for(const auto& r:records_) {
            w.put(r.observations); w.put(r.contacts); w.put(r.metres);
            w.put(r.modelRevision); w.put(r.suite);
            w.put<uint32_t>(static_cast<uint32_t>(r.evaluations.size()));
            for(const auto& e:r.evaluations) {
                w.put(e.seed); w.put<uint8_t>(e.success); w.put(e.contactsPerMetre);
                w.put(e.stuckFraction); w.put(e.cost); w.put(e.referenceCost);
            }
        }
        return w.bytes;
    }
    bool decode(const std::string& bytes) {
        try {
            gtlearn::Reader r{bytes}; Curriculum trial;
            if(r.get<uint32_t>()!=1) return false;
            for(size_t i=0;i<5;++i) {
                auto& record=trial.records_[i];
                record.observations=r.get<uint64_t>(); record.contacts=r.get<uint64_t>();
                record.metres=r.get<double>();
                const auto revision=r.get<uint64_t>(); const auto suite=r.get<uint32_t>();
                const auto count=r.get<uint32_t>();
                if(record.contacts>record.observations||record.metres<0||count>16) return false;
                for(uint32_t j=0;j<count;++j) {
                    Evaluation e; e.seed=r.get<uint32_t>();
                    const auto success=r.get<uint8_t>(); if(success>1) return false;
                    e.success=success!=0; e.contactsPerMetre=r.get<float>();
                    e.stuckFraction=r.get<float>(); e.cost=r.get<float>(); e.referenceCost=r.get<float>();
                    if(!trial.assess(static_cast<Skill>(i),revision,suite,e)) return false;
                }
                if(record.evaluations.size()!=count) return false; // No duplicated scenario evidence.
            }
            r.end(); *this=std::move(trial); return true;
        } catch(const std::exception&) { return false; }
    }
};
} // namespace gtskill

#pragma once
#ifndef GTANGEL_ZPP_SAVE_SPEC_GUARD
#define GTANGEL_ZPP_SAVE_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/save
// Derived from C:/ddd/GTAngel/cpp_test/zpp/save/*.zpp
// Modules: 5 | Formal Schemas: 143 | Free Types: 10
// ============================================================================
namespace zpp_save_spec {
static constexpr int kZppModuleCount = 5;
static constexpr int kZppSchemaCount = 143;
static constexpr int kZppEnumCount   = 10;
struct ZppVec3 {
    float x{0.0f}, y{0.0f}, z{0.0f};
    constexpr bool operator==(const ZppVec3& o) const { return x == o.x && y == o.y && z == o.z; }
    constexpr bool operator!=(const ZppVec3& o) const { return !(*this == o); }
};
// Free Type from zpp/save/GenericGameStorage.zpp
enum class GenericGameStorage_Enum_1 : int32_t { SAVE_TYPE_NONE_v = 0, SAVE_TYPE_32_BIT_v = 1, SAVE_TYPE_64_BIT_v = 2, SAVE_TYPE_MSVC_v = 4, SAVE_TYPE_GCC_v = 8, SAVE_TYPE_STEAM_v = 16 };
// Free Type from zpp/save/GenericGameStorage.zpp
enum class GenericGameStorage_Enum_2 : int32_t { SAVE_TYPE_NONE_v = 0, SAVE_TYPE_32_BIT_v = 1, SAVE_TYPE_64_BIT_v = 2, SAVE_TYPE_MSVC_v = 4, SAVE_TYPE_GCC_v = 8 };
// Free Type from zpp/save/MemoryCard.zpp
enum class MemoryCard_Enum_1 : int32_t { CARD_ONE_v = 0, CARD_TWO_v = 1, MAX_CARDS_v = 2 };
// Free Type from zpp/save/MemoryCard.zpp
enum class MCSTATE : int32_t { MCSTATE_OK_v = 0, MCSTATE_NEED_500KB_v = 1, MCSTATE_NEED_200KB_v = 2, MCSTATE_NOCARD_v = 3 };
// Free Type from zpp/save/MemoryCard.zpp
enum class SLOTINFO : int32_t { SLOT_PRESENT_v = 0, SLOT_NOTPRESENT_v = 1, SLOT_CORRUPTED_v = 2 };
// Free Type from zpp/save/MemoryCard.zpp
enum class MemoryCard_Enum_5 : int32_t { ERR_NONE_v = 0, ERR_NOFORMAT_v = 1, ERR_DIRNOENTRY_v = 2, ERR_OPENNOENTRY_v = 3, ERR_DELETENOENTRY_v = 4, ERR_DELETEDENIED_v = 5, ERR_DELETEFAILED_v = 6, ERR_WRITEFULLDEVICE_v = 7, ERR_WRITENOENTRY_v = 8, ERR_WRITEDENIED_v = 9, ERR_FLUSHNOENTRY_v = 10, ERR_WRITEFAILED_v = 11, ERR_FORMATFAILED_v = 12, ERR_FILETABLENOENTRY_v = 13, ERR_DIRFULLDEVICE_v = 14, ERR_DIRBADENTRY_v = 15, ERR_FILEFULLDEVICE_v = 16, ERR_FILENOPATHENTRY_v = 17, ERR_FILEDENIED_v = 18, ERR_FILEUPLIMIT_v = 19, ERR_READNOENTRY_v = 20, ERR_READDENIED_v = 21, ERR_LOADFAILED_v = 22, ERR_SAVEFAILED_v = 23, ERR_DATACORRUPTED_v = 24, ERR_NOROOTDIR_v = 25, NO_ERR_SUCCESS_v = 26 };
// State Schema Contract from zpp/save/Date.zpp
struct CDate_ZppContract {
    int32_t m_nSecond{0}; // Z++: ℤ
    int32_t m_nMinute{0}; // Z++: ℤ
    int32_t m_nHour{0}; // Z++: ℤ
    int32_t m_nDay{0}; // Z++: ℤ
    int32_t m_nMonth{0}; // Z++: ℤ
    int32_t m_nYear{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/save/GenericGameStorage.zpp
struct GenericGameStorageModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{1606}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 1606);
    }
};
// State Schema Contract from zpp/save/SaveBuf.zpp
struct SaveBufModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{164}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 164);
    }
};
// Delta Operation Schema Contract (ΔCDate) from zpp/save/Date.zpp
constexpr bool op_CDate_CDate(const CDate_ZppContract& pre, const CDate_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_nYear == 0) && (post.m_nSecond == 0) && (post.m_nMinute == 0) && (post.m_nHour == 0) && (post.m_nDay == 0) && (post.m_nMonth == 0);
}
constexpr CDate_ZppContract apply_CDate_CDate(const CDate_ZppContract& pre) {
    CDate_ZppContract post = pre;
    post.m_nYear = 0;
    post.m_nSecond = 0;
    post.m_nMinute = 0;
    post.m_nHour = 0;
    post.m_nDay = 0;
    post.m_nMonth = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCDate) from zpp/save/Date.zpp
constexpr bool op_CDate_PopulateDateFields(const CDate_ZppContract& pre, const CDate_ZppContract& post, int32_t m_nSecond_in = 25, int32_t m_nMinute_in = 25, int32_t m_nHour_in = 25, int32_t m_nDay_in = 25, int32_t m_nMonth_in = 25, int32_t m_nYear_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_nSecond == m_nSecond_in) && (post.m_nMinute == m_nMinute_in) && (post.m_nHour == m_nHour_in) && (post.m_nDay == m_nDay_in) && (post.m_nMonth == m_nMonth_in) && (post.m_nYear == m_nYear_in);
}
constexpr CDate_ZppContract apply_CDate_PopulateDateFields(const CDate_ZppContract& pre, int32_t m_nSecond_in = 25, int32_t m_nMinute_in = 25, int32_t m_nHour_in = 25, int32_t m_nDay_in = 25, int32_t m_nMonth_in = 25, int32_t m_nYear_in = 25) {
    CDate_ZppContract post = pre;
    post.m_nSecond = m_nSecond_in;
    post.m_nMinute = m_nMinute_in;
    post.m_nHour = m_nHour_in;
    post.m_nDay = m_nDay_in;
    post.m_nMonth = m_nMonth_in;
    post.m_nYear = m_nYear_in;
    return post;
}
constexpr bool verify_save_zpp_invariants() {
    {
        CDate_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CDate_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        GenericGameStorageModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        GenericGameStorageModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        SaveBufModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        SaveBufModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        CDate_ZppContract pre{};
        pre.m_nYear = 1;
        pre.m_nSecond = 1;
        pre.m_nMinute = 1;
        pre.m_nHour = 1;
        pre.m_nDay = 1;
        pre.m_nMonth = 1;
        const auto post = apply_CDate_CDate(pre);
        if (!op_CDate_CDate(pre, post)) return false;
        {
            CDate_ZppContract badPost = post;
            badPost.m_nYear = 1;
            if (op_CDate_CDate(pre, badPost)) return false;
        }
        {
            CDate_ZppContract badPost = post;
            badPost.m_nSecond = 1;
            if (op_CDate_CDate(pre, badPost)) return false;
        }
        {
            CDate_ZppContract badPost = post;
            badPost.m_nMinute = 1;
            if (op_CDate_CDate(pre, badPost)) return false;
        }
        {
            CDate_ZppContract badPost = post;
            badPost.m_nHour = 1;
            if (op_CDate_CDate(pre, badPost)) return false;
        }
        {
            CDate_ZppContract badPost = post;
            badPost.m_nDay = 1;
            if (op_CDate_CDate(pre, badPost)) return false;
        }
        {
            CDate_ZppContract badPost = post;
            badPost.m_nMonth = 1;
            if (op_CDate_CDate(pre, badPost)) return false;
        }
    }
    {
        CDate_ZppContract pre{};
        const int32_t m_nSecond_in = 25;
        const int32_t m_nMinute_in = 25;
        const int32_t m_nHour_in = 25;
        const int32_t m_nDay_in = 25;
        const int32_t m_nMonth_in = 25;
        const int32_t m_nYear_in = 25;
        pre.m_nSecond = 0;
        pre.m_nMinute = 0;
        pre.m_nHour = 0;
        pre.m_nDay = 0;
        pre.m_nMonth = 0;
        pre.m_nYear = 0;
        const auto post = apply_CDate_PopulateDateFields(pre, m_nSecond_in, m_nMinute_in, m_nHour_in, m_nDay_in, m_nMonth_in, m_nYear_in);
        if (!op_CDate_PopulateDateFields(pre, post, m_nSecond_in, m_nMinute_in, m_nHour_in, m_nDay_in, m_nMonth_in, m_nYear_in)) return false;
        {
            CDate_ZppContract badPost = post;
            badPost.m_nSecond = m_nSecond_in + 1;
            if (op_CDate_PopulateDateFields(pre, badPost, m_nSecond_in, m_nMinute_in, m_nHour_in, m_nDay_in, m_nMonth_in, m_nYear_in)) return false;
        }
        {
            CDate_ZppContract badPost = post;
            badPost.m_nMinute = m_nMinute_in + 1;
            if (op_CDate_PopulateDateFields(pre, badPost, m_nSecond_in, m_nMinute_in, m_nHour_in, m_nDay_in, m_nMonth_in, m_nYear_in)) return false;
        }
        {
            CDate_ZppContract badPost = post;
            badPost.m_nHour = m_nHour_in + 1;
            if (op_CDate_PopulateDateFields(pre, badPost, m_nSecond_in, m_nMinute_in, m_nHour_in, m_nDay_in, m_nMonth_in, m_nYear_in)) return false;
        }
        {
            CDate_ZppContract badPost = post;
            badPost.m_nDay = m_nDay_in + 1;
            if (op_CDate_PopulateDateFields(pre, badPost, m_nSecond_in, m_nMinute_in, m_nHour_in, m_nDay_in, m_nMonth_in, m_nYear_in)) return false;
        }
        {
            CDate_ZppContract badPost = post;
            badPost.m_nMonth = m_nMonth_in + 1;
            if (op_CDate_PopulateDateFields(pre, badPost, m_nSecond_in, m_nMinute_in, m_nHour_in, m_nDay_in, m_nMonth_in, m_nYear_in)) return false;
        }
        {
            CDate_ZppContract badPost = post;
            badPost.m_nYear = m_nYear_in + 1;
            if (op_CDate_PopulateDateFields(pre, badPost, m_nSecond_in, m_nMinute_in, m_nHour_in, m_nDay_in, m_nMonth_in, m_nYear_in)) return false;
        }
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_save_zpp_invariants(), "Z++ formal contract invariant failed for save");
} // namespace zpp_save_spec
#endif // GTANGEL_ZPP_SAVE_SPEC_GUARD
namespace v20save {
static constexpr const char* SavePath = R"(C:\ddd\GTAngel\cpp_test\gtangel_v20_save.json)";
static bool number(const std::string& text,const char* key,double& result) {
    const std::string needle=std::string("\"")+key+"\"";
    size_t pos=text.find(needle);
    if (pos==std::string::npos||text.find(needle,pos+needle.size())!=std::string::npos) return false;
    pos=text.find(':',pos+needle.size());
    if (pos==std::string::npos) return false;
    const char* begin=text.c_str()+pos+1;
    while (*begin==' '||*begin=='\n'||*begin=='\t') ++begin;
    if (!((*begin>='0'&&*begin<='9')||*begin=='-'||*begin=='+')) return false;
    char* end=nullptr;
    result=strtod(begin,&end);
    if (end==begin||!std::isfinite(result)||(*end!=','&&*end!='}'&&*end!=' '&&*end!='\n')) return false;
    return true;
}
static bool integral(const std::string& t,const char* k,int lo,int hi,int& out) {
    double value=0;
    if (!number(t,k,value)||value<lo||value>hi||floor(value)!=value) return false;
    out=(int)value; return true;
}
static bool scalar(const std::string& t,const char* k,float lo,float hi,float& out) {
    double value=0;
    if (!number(t,k,value)||value<lo||value>hi) return false;
    out=(float)value; return true;
}
static bool decode(const std::string& text,int version, v19game::GameplayLoop& game,
                   avatar3d::GameWorld& world, v20cns::Body& body,
                   v20cns::NicheHistory& niche) {
    if (text.size()>8192||text.empty()||text.front()!='{'||text.find_last_of('}')==std::string::npos) return false;
    int v=0,stage=0,shard=0,sigil=0,visible=0,beacon=0,events=0,declined=0,accepted=0,restored=0;
    float disposition=0;
    if (!integral(text,"version",version,version,v) ||
        !integral(text,"questStage",0,5,stage) ||
        !integral(text,"hasSignalShard",0,1,shard) ||
        !integral(text,"hasResonanceSigil",0,1,sigil) ||
        !integral(text,"shardVisible",0,1,visible) ||
        !integral(text,"beaconRestored",0,1,beacon) ||
        !integral(text,"groundedEvents",0,10000000,events) ||
        !scalar(text,"disposition",0,1,disposition) ||
        !integral(text,"declined",0,1,declined) ||
        !integral(text,"accepted",0,1,accepted) ||
        !integral(text,"restored",0,1,restored)) return false;
    // Quest invariants are stronger than simple type checks; never load a completed
    // purpose without its earned Sigil or an unspent shard into a restored beacon.
    if ((beacon && (stage<4||shard||visible||!restored)) ||
        (!beacon && (stage>=4||sigil)) || (sigil!=(stage==5)) ||
        (stage==3 && (!shard||visible))) return false;
    game.quest.stage=(v19game::QuestStage)stage;
    game.quest.hasSignalShard=shard; game.quest.hasResonanceSigil=sigil;
    game.quest.shardVisible=visible; game.quest.beaconRestored=beacon;
    game.quest.groundedEvents=(uint32_t)events;
    game.quest.dialogueOpen=false; game.quest.journalOpen=true;
    game.quest.persistenceStatus="LOADED"; game.quest.lastEvent="Gameplay state loaded";
    game.ariadne.disposition=disposition;
    game.ariadne.remembersDecline=declined; game.ariadne.remembersAcceptance=accepted;
    game.ariadne.remembersRestoration=restored;
    game.publishWorld(world);
    if (version==1) {
        body=v20cns::Body{};
        niche={beacon?2u:1u,beacon?1u:0u,beacon!=0,false};
        return true;
    }
    int occupied=0,gear=0,contact=0,wv=0,spent=0,reobserved=0,rollback=0;
    if (!scalar(text,"bodyX",-9.75f,9.75f,body.position.x)||
        !scalar(text,"bodyZ",-9.75f,9.75f,body.position.z)||
        !scalar(text,"cartX",-9.24f,9.24f,body.cartPosition.x)||
        !scalar(text,"cartZ",-9.24f,9.24f,body.cartPosition.z)||
        !scalar(text,"cartHeading",-10000,10000,body.cartHeading)||
        !scalar(text,"bodyHeading",-10000,10000,body.heading)||
        !scalar(text,"bodySpeed",-3,7,body.speed)||
        !scalar(text,"bodySteering",-.56f,.56f,body.steering)||
        !scalar(text,"wheelPhase",-100000,100000,body.wheelPhase)||
        !integral(text,"occupied",0,1,occupied)||
        !integral(text,"gear",-1,1,gear)||
        !integral(text,"contact",0,1,contact)||
        !integral(text,"worldVersion",1,2,wv)||
        !integral(text,"nicheSpent",0,1,spent)||
        !integral(text,"reobserved",0,1,reobserved)||
        !integral(text,"rolledBack",0,1,rollback)) return false;
    if ((occupied && ((body.speed>0.025f && gear!=1) ||
                      (body.speed< -0.025f && gear!=-1) ||
                      (fabsf(body.speed)<=0.025f && gear!=0))) ||
        (!occupied && gear!=0)) return false;
    body.position.y=0; body.cartPosition.y=0;
    body.occupied=occupied; body.gear=gear; body.contact=contact;
    niche={(uint64_t)wv,(uint32_t)spent,reobserved!=0,rollback!=0};
    return true;
}
static bool write(const char* path,const v19game::GameplayLoop& g,const v20cns::Runtime& c) {
    const auto& h=c.niche();
    if (h.rolledBack || h.worldVersion<1 || h.worldVersion>2 ||
        h.spent!=(h.worldVersion==2?1u:0u) ||
        g.quest.beaconRestored!=(h.worldVersion==2) ||
        (h.worldVersion==2 && !h.reobserved) ||
        (g.quest.beaconRestored && ((int)g.quest.stage<4 ||
            g.quest.hasSignalShard || g.quest.shardVisible || !g.ariadne.remembersRestoration)) ||
        (g.quest.hasResonanceSigil!=(g.quest.stage==v19game::QuestStage::Complete)) ||
        !v20cns::finitePose(c.body().position) ||
        !v20cns::finitePose(c.body().cartPosition) ||
        !v20cns::finite(c.body().cartHeading) ||
        !v20cns::finite(c.body().heading) ||
        !v20cns::finite(c.body().speed)) return false;
    const std::string temp=std::string(path)+".tmp";
    std::ofstream out(temp,std::ios::trunc|std::ios::binary);
    if (!out) return false;
    const auto& q=g.quest; const auto& b=c.body(); const auto& n=c.niche();
    out << "{\n  \"version\": 2,\n"
        << "  \"questStage\": " << (int)q.stage << ",\n"
        << "  \"hasSignalShard\": " << (int)q.hasSignalShard << ",\n"
        << "  \"hasResonanceSigil\": " << (int)q.hasResonanceSigil << ",\n"
        << "  \"shardVisible\": " << (int)q.shardVisible << ",\n"
        << "  \"beaconRestored\": " << (int)q.beaconRestored << ",\n"
        << "  \"groundedEvents\": " << q.groundedEvents << ",\n"
        << "  \"disposition\": " << g.ariadne.disposition << ",\n"
        << "  \"declined\": " << (int)g.ariadne.remembersDecline << ",\n"
        << "  \"accepted\": " << (int)g.ariadne.remembersAcceptance << ",\n"
        << "  \"restored\": " << (int)g.ariadne.remembersRestoration << ",\n"
        << "  \"bodyX\": " << b.position.x << ",\n"
        << "  \"bodyZ\": " << b.position.z << ",\n"
        << "  \"cartX\": " << b.cartPosition.x << ",\n"
        << "  \"cartZ\": " << b.cartPosition.z << ",\n"
        << "  \"cartHeading\": " << b.cartHeading << ",\n"
        << "  \"bodyHeading\": " << b.heading << ",\n"
        << "  \"bodySpeed\": " << b.speed << ",\n"
        << "  \"bodySteering\": " << b.steering << ",\n"
        << "  \"wheelPhase\": " << b.wheelPhase << ",\n"
        << "  \"occupied\": " << (int)b.occupied << ",\n"
        << "  \"gear\": " << b.gear << ",\n"
        << "  \"contact\": " << (int)b.contact << ",\n"
        << "  \"worldVersion\": " << n.worldVersion << ",\n"
        << "  \"nicheSpent\": " << n.spent << ",\n"
        << "  \"reobserved\": " << (int)n.reobserved << ",\n"
        << "  \"rolledBack\": " << (int)n.rolledBack << "\n}\n";
    out.flush(); bool okay=out.good(); out.close(); okay=okay&&out.good();
    if (!okay || !MoveFileExA(temp.c_str(),path,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)) {
        DeleteFileA(temp.c_str()); return false;
    }
    return true;
}
template<class Observe>
static bool load(const char* path,const char* legacyPath,v19game::GameplayLoop& game,
                 avatar3d::GameWorld& world,v20cns::Runtime& c,Observe observe) {
    std::ifstream in(path,std::ios::binary);
    bool legacy=!in.good();
    if (legacy) { in.clear(); in.open(legacyPath,std::ios::binary); }
    if (!in) return false;
    std::string text((std::istreambuf_iterator<char>(in)),std::istreambuf_iterator<char>());
    auto nextGame=game; auto nextWorld=world;
    v20cns::Body nextBody{}; v20cns::NicheHistory nextNiche{};
    if (!decode(text,legacy?1:2,nextGame,nextWorld,nextBody,nextNiche)) return false;
    if (nextWorld.beaconRestored) {
        bool observed=false;
        try { observed=observe(nextWorld); } catch (...) { observed=false; }
        if (!observed) return false;
        nextNiche.reobserved=true;
    }
    v20cns::Runtime nextCns;
    if (!nextCns.restoreLocal(nextBody,nextNiche,nextGame,nextWorld)) return false;
    game=nextGame; world=nextWorld; c=nextCns;
    return true;
}
} // namespace v20save


// ═══ Application State ═══

#pragma once
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#endif

namespace gtlearn {
constexpr size_t MaxPayload = 4 * 1024 * 1024;
struct Writer {
    std::string bytes;
    template<class T> void put(T value) {
        static_assert(std::is_arithmetic<T>::value, "scalar checkpoint field");
        bytes.append(reinterpret_cast<const char*>(&value), sizeof(value));
    }
};
struct Reader {
    const std::string& bytes;
    size_t cursor{0};
    template<class T> T get() {
        static_assert(std::is_arithmetic<T>::value, "scalar checkpoint field");
        if (cursor > bytes.size() || sizeof(T) > bytes.size()-cursor)
            throw std::runtime_error("truncated checkpoint");
        T value{};
        std::memcpy(&value, bytes.data()+cursor, sizeof(value));
        cursor+=sizeof(value);
        if constexpr(std::is_floating_point<T>::value)
            if (!std::isfinite(value)) throw std::runtime_error("non-finite checkpoint field");
        return value;
    }
    void end() const {
        if (cursor!=bytes.size()) throw std::runtime_error("unexpected checkpoint tail");
    }
};
inline uint64_t checksum(const std::string& bytes) {
    uint64_t hash=14695981039346656037ull;
    for(unsigned char c:bytes) { hash^=c; hash*=1099511628211ull; }
    return hash; // Integrity against accidental damage, not an authentication signature.
}
inline bool flushFile(const std::filesystem::path& path) {
#ifdef _WIN32
    HANDLE file=CreateFileW(path.c_str(),GENERIC_WRITE,FILE_SHARE_READ,nullptr,
                            OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
    if(file==INVALID_HANDLE_VALUE) return false;
    const bool ok=FlushFileBuffers(file)!=0;
    CloseHandle(file);
    return ok;
#else
    const int file=open(path.c_str(),O_WRONLY);
    if(file<0) return false;
    const bool ok=fsync(file)==0;
    close(file);
    return ok;
#endif
}
inline bool writeFile(const std::filesystem::path& path,const std::string& bytes) {
    std::ofstream out(path,std::ios::binary|std::ios::trunc);
    out.write(bytes.data(),static_cast<std::streamsize>(bytes.size()));
    out.flush();
    const bool ok=out.good();
    out.close();
    return ok && !out.fail() && flushFile(path);
}
inline std::string readFile(const std::filesystem::path& path,size_t limit=MaxPayload) {
    const auto size=std::filesystem::file_size(path);
    if(size>limit) throw std::runtime_error("oversized checkpoint file");
    std::string bytes(static_cast<size_t>(size),'\0');
    std::ifstream in(path,std::ios::binary);
    if(!in.read(bytes.data(),static_cast<std::streamsize>(bytes.size())))
        throw std::runtime_error("checkpoint read failed");
    return bytes;
}
struct Snapshot {
    std::string id;
    std::array<std::string,3> payload; // state, GNN learner, curriculum
};
class Store {
    std::filesystem::path root_;
    inline static uint64_t sequence_{0};
    static constexpr const char* Names[3]={"state.bin","motor.bin","skills.bin"};
    struct Lock {
#ifdef _WIN32
        HANDLE handle{INVALID_HANDLE_VALUE};
        explicit Lock(const std::filesystem::path& path) {
            handle=CreateFileW(path.c_str(),GENERIC_READ|GENERIC_WRITE,0,nullptr,
                               OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
            if(handle==INVALID_HANDLE_VALUE) throw std::runtime_error("checkpoint writer busy");
        }
        ~Lock(){CloseHandle(handle);}
#else
        int handle{-1};
        explicit Lock(const std::filesystem::path& path) {
            handle=open(path.c_str(),O_CREAT|O_RDWR,0600);
            if(handle<0) throw std::runtime_error("checkpoint lock failed");
            if(flock(handle,LOCK_EX|LOCK_NB)!=0) {close(handle);throw std::runtime_error("checkpoint writer busy");}
        }
        ~Lock(){flock(handle,LOCK_UN);close(handle);}
#endif
    };
public:
    std::string error;
    explicit Store(std::filesystem::path root):root_(std::move(root)) {}
    std::string bestId() const {
        try {
            const auto id=readFile(root_/"best.txt",128);
            if(id.rfind("cp-",0)!=0||id.find_first_not_of("cp-0123456789")!=std::string::npos) return {};
            return id;
        } catch(const std::exception&) { return {}; }
    }
    bool pinBest(const std::string& id) {
        if(id.rfind("cp-",0)!=0||id.find_first_not_of("cp-0123456789")!=std::string::npos) return false;
        Snapshot candidate;
        if(!read(root_/id,candidate)) return false;
        try {
            if(!writeFile(root_/"best.pending",id)) return false;
#ifdef _WIN32
            if(!MoveFileExW((root_/"best.pending").c_str(),(root_/"best.txt").c_str(),
                           MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)) return false;
#else
            std::filesystem::rename(root_/"best.pending",root_/"best.txt");
#endif
            return true;
        } catch(const std::exception& e) { error=e.what(); return false; }
    }
    std::vector<std::filesystem::path> candidates() const {
        std::vector<std::filesystem::path> result;
        std::error_code ec;
        if(!std::filesystem::is_directory(root_,ec)) return result;
        for(const auto& entry:std::filesystem::directory_iterator(root_)) {
            const auto name=entry.path().filename().string();
            if(entry.is_directory() && name.rfind("cp-",0)==0 &&
               name.find(".pending")==std::string::npos) result.push_back(entry.path());
        }
        std::sort(result.rbegin(),result.rend());
        return result;
    }
    bool read(const std::filesystem::path& directory,Snapshot& snapshot) {
        try {
            std::istringstream manifest(readFile(directory/"manifest.txt",4096));
            std::string magic;
            int version=0;
            if(!(manifest>>magic>>version)||magic!="GTANGEL_LEARNING"||version!=1)
                throw std::runtime_error("unsupported checkpoint manifest");
            Snapshot trial; trial.id=directory.filename().string();
            for(size_t i=0;i<3;++i) {
                std::string name; uint64_t size=0, hash=0;
                if(!(manifest>>name>>size>>hash)||name!=Names[i]||size>MaxPayload)
                    throw std::runtime_error("invalid checkpoint manifest entry");
                trial.payload[i]=readFile(directory/name);
                if(trial.payload[i].size()!=size||checksum(trial.payload[i])!=hash)
                    throw std::runtime_error("checkpoint checksum mismatch");
            }
            manifest>>std::ws;
            if(!manifest.eof()) throw std::runtime_error("unexpected manifest entry");
            snapshot=std::move(trial);
            error.clear();
            return true;
        } catch(const std::exception& e) { error=e.what(); return false; }
    }
    bool save(const std::array<std::string,3>& payload,std::string& id,const std::string& expectedLatest={}) {
        std::filesystem::path pending;
        try {
            for(const auto& bytes:payload)
                if(bytes.empty()||bytes.size()>MaxPayload) throw std::runtime_error("invalid checkpoint payload size");
            std::filesystem::create_directories(root_);
            Lock lock(root_/"writer.lock");
            if(!expectedLatest.empty()) {
                const auto current=candidates();
                if(current.empty()||current.front().filename().string()!=expectedLatest)
                    throw std::runtime_error("checkpoint advanced during evaluation");
            }
            const auto now=std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
            std::ostringstream name;
            name<<"cp-"<<std::setw(20)<<std::setfill('0')<<now<<"-";
#ifdef _WIN32
            name<<GetCurrentProcessId();
#else
            name<<getpid();
#endif
            name<<"-"<<++sequence_;
            const auto final=root_/name.str();
            pending=root_/(name.str()+".pending");
            if(!std::filesystem::create_directory(pending)) throw std::runtime_error("checkpoint generation exists");
            std::ostringstream manifest; manifest<<"GTANGEL_LEARNING 1\n";
            for(size_t i=0;i<3;++i) {
                if(!writeFile(pending/Names[i],payload[i])) throw std::runtime_error("checkpoint write/flush failed");
                manifest<<Names[i]<<" "<<payload[i].size()<<" "<<checksum(payload[i])<<"\n";
            }
            if(!writeFile(pending/"manifest.txt",manifest.str())) throw std::runtime_error("manifest flush failed");
            Snapshot verified;
            if(!read(pending,verified)) throw std::runtime_error(error);
            std::filesystem::rename(pending,final); // Same-volume publication of the complete generation.
            id=name.str();
            error.clear();
            // Retain five complete valid generations. Invalid and pending files
            // are never mistaken for recovery points.
            size_t kept=0; const auto best=bestId();
            for(const auto& candidate:candidates()) {
                Snapshot old;
                if(read(candidate,old) && ++kept>5 && old.id!=best) {
                    std::error_code ec; std::filesystem::remove_all(candidate,ec);
                }
            }
            error.clear();
            return true;
        } catch(const std::exception& e) {
            error=e.what();
            if(!pending.empty()) { std::error_code ec; std::filesystem::remove_all(pending,ec); }
            return false;
        }
    }
};
struct Autosave {
    uint64_t lastSuccessMs{0}, lastAttemptMs{0};
    bool due(uint64_t now,bool dirty) const {
        return dirty && now-lastSuccessMs>=60000 && now-lastAttemptMs>=10000;
    }
    void attempted(uint64_t now,bool success) {
        lastAttemptMs=now;
        if(success) lastSuccessMs=now;
    }
};
} // namespace gtlearn

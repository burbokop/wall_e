#ifndef COMPILER_INFO_H
#define COMPILER_INFO_H

#include <string>

namespace wall_e {

struct compiler_info {
    const std::string compiler_name;
    const int version_major;
    const int version_minor;
    const int patchlevel;
    compiler_info(std::string compiler_name, int version_major, int version_minor, int patchlevel)
        : compiler_name(compiler_name),
          version_major(version_major),
          version_minor(version_minor),
          patchlevel(patchlevel) {}

    bool is_gcc() const;
    bool is_clang() const;
    std::string version_str() const;
    std::string to_string() const;
};

std::ostream& operator<<(std::ostream& stream, const compiler_info& cinfo);

#ifdef __clang__
#define WALL_E_COMPILER_INFO compiler_info("clang++", __clang_major__, __clang_minor__, __clang_patchlevel__);
#else
#define WALL_E_COMPILER_INFO compiler_info("g++", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#endif

extern const compiler_info cxx_info;
inline const compiler_info inline_cxx_info = WALL_E_COMPILER_INFO;

}

#endif // COMPILER_INFO_H

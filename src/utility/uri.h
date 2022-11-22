#ifndef wall_e_URI_H
#define wall_e_URI_H

#include "collections.h"

#include <filesystem>

namespace wall_e {

namespace uri {
    wall_e::opt<std::filesystem::path> to_path(std::string uri);
}

}

#endif // URI_H

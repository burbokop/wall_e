#include "uri.h"


wall_e::opt<std::filesystem::path> wall_e::uri::to_path(std::string uri) {
    const std::string scheme = "file://";
    if(uri.starts_with(scheme)) {
        uri.erase(uri.begin(), uri.begin() + scheme.size());
#ifdef _WIN32
        if(uri.size() > 2 && (uri[0] == '/' || uri[0] == '\\') && uri[2] == ':') {
            uri.erase(uri.begin(), uri.begin() + 1);
        }
#endif
        return std::filesystem::path(uri).make_preferred();
    } else {
        return std::nullopt;
    }
}

#include "XboxInternals/Iso/XexExecutable.h"
#include <algorithm>
#include <cctype>
#include <regex>
#include <fstream>
#include <cstring>

namespace XboxInternals::Iso {

static std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    return s;
}

void XexExecutable::parseFromFilename(const std::string& filename) {
    info_ = XexInfo();
    std::string name = filename;
    // Extract final path component if a path is passed
    auto pos = name.find_last_of("/\\");
    if (pos != std::string::npos) name = name.substr(pos + 1);

    // Lowercase copy for pattern matching
    std::string low = toLower(name);

    // Common TitleID pattern: 8 hex digits (e.g., 415607BE)
    std::regex r_titleid("([0-9a-f]{8})", std::regex_constants::icase);
    std::smatch m;
    if (std::regex_search(low, m, r_titleid) && m.size() > 1) {
        info_.titleId = m.str(1);
    }

    // Simpler manual search fallback
    size_t dpos = low.find("disc");
    if (dpos != std::string::npos) {
        // attempt to read a digit after 'disc'
        for (size_t i = dpos + 4; i < low.size(); ++i) {
            if (std::isdigit(static_cast<unsigned char>(low[i]))) {
                info_.discNumber = static_cast<uint8_t>(low[i] - '0');
                break;
            }
        }
    }

    // Try to infer media id from filename (4-8 hex group near start)
    std::regex r_media("([0-9a-f]{4,8})", std::regex_constants::icase);
    if (std::regex_search(low, m, r_media) && m.size() > 1) {
        info_.mediaId = m.str(1);
    }
}

bool XexExecutable::parseFromFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) return false;

    // Read XEX2 header (simplified - just extract game name from string table)
    char magic[4];
    file.read(magic, 4);
    
    // Check for XEX2 magic "XEX2"
    if (std::memcmp(magic, "XEX2", 4) != 0) return false;

    // Skip to approximate location of string table (this is simplified)
    // Real implementation would parse optional headers to find execution info
    file.seekg(0x18, std::ios::beg);
    uint32_t certOffset = 0;
    file.read(reinterpret_cast<char*>(&certOffset), 4);
    
    // Try to extract title ID from certificate area (offset 0x18 in cert)
    if (certOffset > 0 && certOffset < 0x10000) {
        file.seekg(certOffset + 0x18, std::ios::beg);
        uint32_t titleIdNum = 0;
        file.read(reinterpret_cast<char*>(&titleIdNum), 4);
        
        // Convert to hex string
        char buf[16];
        std::snprintf(buf, sizeof(buf), "%08X", titleIdNum);
        info_.titleId = buf;
    }

    // Placeholder: extract game name from assumed string location
    // Real parsing would find the execution info optional header
    info_.gameName = "Unknown Game";
    
    return true;
}

} // namespace XboxInternals::Iso

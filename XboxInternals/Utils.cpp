#include "Utils.h"


std::string Utils::NormalizeFilePath(std::string path, char cur_separater, char replacement_separator)
{
    for (DWORD i = 0; i < path.length(); i++)
        if (path.at(i) == cur_separater)
            path.at(i) = replacement_separator;
    return path;
}

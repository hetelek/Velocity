#ifndef UTILS_H
#define UTILS_H

#include "winnames.h"

#include <string>

namespace Utils
{
    std::string NormalizeFilePath(std::string path, char cur_separater='/', char replacement_separator='\\');
}

#endif // UTILS_H

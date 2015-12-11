#ifndef UTILS_H
#define UTILS_H

#include "winnames.h"
#include "XboxInternals_global.h"

#include <string>

namespace Utils
{
    std::string XBOXINTERNALSSHARED_EXPORT NormalizeFilePath(std::string path, char cur_separater='/', char replacement_separator='\\');
}

#endif // UTILS_H

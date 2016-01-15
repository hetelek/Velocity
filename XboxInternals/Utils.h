#ifndef UTILS_H
#define UTILS_H

#include "winnames.h"
#include "XboxInternals_global.h"

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>

#include <dirent.h>
#include <stdio.h>

namespace Utils
{
    std::string XBOXINTERNALSSHARED_EXPORT NormalizeFilePath(std::string path, char cur_separater='/', char replacement_separator='\\');

    std::string XBOXINTERNALSSHARED_EXPORT ConvertToHexString(BYTE *buffer, DWORD len, std::ios_base&(letterCase)(std::ios_base&) = std::uppercase);
    std::string XBOXINTERNALSSHARED_EXPORT ConvertToHexString(UINT64 value, std::ios_base&(letterCase)(std::ios_base&) = std::uppercase);

    std::vector<std::string> FilesInDirectory(std::string directoryPath);

    UINT64 RoundToNearestHex1000(UINT64 num);

    bool CreateLocalDirectory(std::string filePath);

    std::string GetTemporaryFileName();

    bool DeleteLocalFile(std::string path);
}

#endif // UTILS_H

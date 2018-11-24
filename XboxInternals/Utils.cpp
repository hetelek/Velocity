#include "Utils.h"


std::string Utils::NormalizeFilePath(std::string path, char cur_separater, char replacement_separator)
{
    for (DWORD i = 0; i < path.length(); i++)
        if (path.at(i) == cur_separater)
            path.at(i) = replacement_separator;
    return path;
}


std::string Utils::ConvertToHexString(BYTE *buffer, DWORD len, std::ios_base&(letterCase)(std::ios_base&))
{
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < len; i++)
        ss << letterCase << std::hex << std::setw(2) << (int)buffer[i];
    return ss.str();
}


std::string Utils::ConvertToHexString(UINT64 value, std::ios_base&(letterCase)(std::ios_base&))
{
    std::ostringstream ss;
    ss << letterCase << std::hex << value;
    return ss.str();
}


std::vector<std::string> Utils::FilesInDirectory(std::string directoryPath)
{
    std::vector<std::string> toReturn;

    DIR *dir;
    struct dirent *ent;
    dir = opendir(directoryPath.c_str());
    if (dir != NULL)
    {
        // load the stuff that's always at the begining . ..
#ifdef __win32
        readdir(dir);
        readdir(dir);
#endif

        // load only the files, don't want the folders
        while ((ent = readdir(dir)) != NULL)
        {
            std::string fullName = directoryPath;
            fullName += ent->d_name;
            if (opendir(fullName.c_str()) == NULL)
                toReturn.push_back(fullName);
        }
        closedir (dir);
    }
    else
        throw std::string("Utils: Error opening directory\n");

    return toReturn;
}


UINT64 Utils::RoundToNearestHex1000(UINT64 num)
{
    return (num + 0xFFF) & ~0xFFF;
}


bool Utils::CreateLocalDirectory(std::string filePath)
{
#ifdef _WIN32
    return (bool)CreateDirectoryA(filePath.c_str(), NULL);
#endif
}


std::string Utils::GetTemporaryFileName()
{
    std::string tempFileName;
#ifdef _WIN32
    // Opening a file using the path returned by tmpnam() may result in a "permission denied" error on Windows
    // Not sure why it happens but tweaking the manifest/UAC properties makes a difference.
    char *tempFileName_c = _tempnam(NULL, NULL);
    if (!tempFileName_c)
        throw std::string("Utils: Failed to generate temporary file name.\n");
    tempFileName = std::string(tempFileName_c);
    free(tempFileName_c);
    tempFileName_c = NULL;
#else
    char tempFileName_c[L_tmpnam];
    if (!tmpnam(tempFileName_c))
        throw std::string("Utils: Failed to generate temporary file name.\n");
    tempFileName = std::string(tempFileName_c);
#endif

    return tempFileName;
}


bool Utils::DeleteLocalFile(std::string path)
{
#ifdef _WIN32
    return (bool)DeleteFileA(path.c_str());
#endif
}

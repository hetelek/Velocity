#ifndef XCONTENTPACKAGE_H
#define XCONTENTPACKAGE_H

#include "XContentHeader.h"
#include "Utils.h"

#include "XboxInternals_global.h"

class XBOXINTERNALSSHARED_EXPORT IXContentHeader
{
public:
    static FileSystem GetFileSystem(std::string file);

    XContentHeader *metaData;

    std::string GetFatxFilePath();
};

#endif // XCONTENTPACKAGE_H

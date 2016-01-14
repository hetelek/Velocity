#ifndef XUIZ_H
#define XUIZ_H

#include "IO/BaseIO.h"
#include "IO/FileIO.h"

#include <string>
#include <vector>

#include "XboxInternals_global.h"

#define XUIZ_MAGIC 				0x5855495A		// 'XUIZ'
#define XUIZ_HEADER_SIZE		0x16
#define XUIZ_COPY_BUFFER_SIZE	0x100000

struct XuizFileEntry
{
    DWORD size;
    DWORD offset;
    DWORD realAddress;
    std::string path;
};

class XBOXINTERNALSSHARED_EXPORT Xuiz
{
public:
    Xuiz(std::string filePath);
    Xuiz(BaseIO *io);
    ~Xuiz();

    std::vector<XuizFileEntry> GetFiles() const;

    void ExtractFile(std::string outFile, XuizFileEntry *fileEntry) const;

    void ExtractFile(std::string outFile, std::string xuizFilePath) const;

    XuizFileEntry *GetFileEntry(std::string xuizFilePath) const;

private:
    BaseIO *io;
    bool freeIO;
    std::vector<XuizFileEntry> files;

    void Parse();
};

#endif // XUIZ_H

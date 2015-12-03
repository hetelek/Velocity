#ifndef FATXINDEXABLEMULTIFILEIO_H
#define FATXINDEXABLEMULTIFILEIO_H

#include <string>
#include <vector>
#include <algorithm>

#include "IndexableMultiFileIO.h"
#include "BaseIO.h"
#include "Fatx/FatxDrive.h"

using std::string;
using std::wstring;
using std::vector;

class XBOXINTERNALSSHARED_EXPORT FatxIndexableMultiFileIO : public IndexableMultiFileIO
{
public:
    FatxIndexableMultiFileIO(string fileDirectory, FatxDrive *drive);
    virtual ~FatxIndexableMultiFileIO();

protected:
    // get all the file names in the directory
    virtual void loadDirectories(string path);

    virtual BaseIO* openFile(string path);

private:
    FatxDrive *drive;
};

#endif // FATXINDEXABLEMULTIFILEIO_H

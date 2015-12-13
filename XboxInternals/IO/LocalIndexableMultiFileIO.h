#ifndef MULTIFILEIO_H
#define MULTIFILEIO_H

#include <iostream>
#include <vector>
#include <algorithm>

#include "FileIO.h"
#include "BaseIO.h"
#include "IndexableMultiFileIO.h"
#include "Utils.h"
#include "XboxInternals_global.h"

using std::string;
using std::wstring;
using std::vector;

class XBOXINTERNALSSHARED_EXPORT LocalIndexableMultiFileIO : public IndexableMultiFileIO
{
public:
    LocalIndexableMultiFileIO(string fileDirectory);
    virtual ~LocalIndexableMultiFileIO();

protected:
    // get all the file names in the directory
    virtual void loadDirectories(string path);

    virtual BaseIO* openFile(string path);
};

#endif // MULTIFILEIO_H

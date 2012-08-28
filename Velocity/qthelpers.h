#ifndef QTHELPERS_H
#define QTHELPERS_H

// qt
#include <QString>
#include <QDesktopServices>

// other
#include "winnames.h"
#include <ctype.h>

class QtHelpers
{
public:
    static QString ByteArrayToString(BYTE *buffer, DWORD len, bool spacesBetween);

    static DWORD ParseHexString(QString string);

    static void ParseHexStringBuffer(QString bytes, BYTE *outBuffer, DWORD len);

    static QString DesktopLocation();

    static bool VerifyHexString(QString str);

    static bool VerifyDecimalString(QString str);

    static bool VerifyHexStringBuffer(QString bytes);
};

#endif // QTHELPERS_H

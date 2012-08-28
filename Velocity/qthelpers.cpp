#include "qthelpers.h"

QString QtHelpers::ByteArrayToString(BYTE *buffer, DWORD len, bool spacesBetween)
{
    if (spacesBetween)
    {
        QString builder = "";
        for (int i = 0; i < len; i++)
            builder += QString("%1").arg((BYTE)buffer[i], 2, 16, QChar('0')).toUpper() + " ";
        return builder;
    }
    else
    {
        QString builder = "";
        for (int i = 0; i < len; i++)
            builder += QString("%1").arg((BYTE)buffer[i], 2, 16, QChar('0')).toUpper();
        return builder;
    }
}

DWORD QtHelpers::ParseHexString(QString string)
{
    string.replace("0x", "");
    return string.toULong(0, 16);
}

void QtHelpers::ParseHexStringBuffer(QString bytes, BYTE *outBuffer, DWORD len)
{
    bytes = bytes.trimmed().replace(" ", "");

    if (len != (bytes.length() / 2))
        throw QString("QT: Error converting hex string to byte array, length mismatch.\n");

    for (DWORD i = 0; i < len; i++)
    {
        QString temp = bytes.mid(i * 2, 2);
        outBuffer[i] = temp.toInt(0, 16);
    }
}

QString QtHelpers::DesktopLocation()
{
    return QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
}

bool QtHelpers::VerifyHexStringBuffer(QString bytes)
{
    return VerifyHexString(bytes.replace(" ", ""));
}

bool QtHelpers::VerifyHexString(QString str)
{
    str = str.replace("0x", "");
    for (DWORD i = 0; i < str.length(); i++)
        if (!isxdigit(str.at(i).toAscii()))
            return false;
    return true;
}

bool QtHelpers::VerifyDecimalString(QString str)
{
    for (DWORD i = 0; i< str.length(); i++)
        if (!str.at(i).isDigit())
            return false;
    return true;
}

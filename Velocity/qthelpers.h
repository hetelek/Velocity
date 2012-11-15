#ifndef QTHELPERS_H
#define QTHELPERS_H

// qt
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QDesktopServices>
#include <QCoreApplication>
#include <QLabel>
#include <QPushButton>
#include <QFont>
#include <QComboBox>
#include <QLineEdit>
#include <QTreeWidget>
#include <QPlainTextEdit>
#include <QHeaderView>
#include <QCheckBox>

// other
#include "winnames.h"
#include <Stfs/StfsConstants.h>
#include <ctype.h>

#ifdef _WIN32
    #include <direct.h>
#endif

enum VelocityDropAction
{
    OpenInPackageViewer,
    RehashAndResign,
    OpenInProfileEditor
};

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

    static QString GetKVPath(ConsoleType type, QWidget *parent = 0);

    static bool ParseVersionString(QString version, Version *out);

    static QString ExecutingDirectory();

    static void GenAdjustWidgetAppearanceToOS(QWidget *rootWidget);
};

#endif // QTHELPERS_H

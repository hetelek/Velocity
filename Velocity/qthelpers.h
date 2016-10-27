#ifndef QTHELPERS_H
#define QTHELPERS_H

// qt
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QDesktopServices>
#include <QCoreApplication>
#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QFont>
#include <QComboBox>
#include <QLineEdit>
#include <QTreeWidget>
#include <QPlainTextEdit>
#include <QHeaderView>
#include <QCheckBox>
#include <QProgressBar>
#include <QStringList>
#include <QMdiArea>

// other
#include "winnames.h"
#include "Stfs/StfsConstants.h"
#include "Stfs/XContentHeader.h"
#include "Fatx/FatxDrive.h"
#include "nightcharts.h"
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

    static QString ToHexString(UINT64 num);

    static void ParseHexStringBuffer(QString bytes, BYTE *outBuffer, int len);

    static QString DesktopLocation();

    static bool VerifyHexString(QString str);

    static bool VerifyDecimalString(QString str);

    static bool VerifyHexStringBuffer(QString bytes);

    static std::string GetKVPath(ConsoleType type, QWidget *parent = 0);

    static bool ParseVersionString(QString version, Version *out);

    static QString ExecutingDirectory();

    static void GenAdjustWidgetAppearanceToOS(QWidget *rootWidget);

    static void SearchTreeWidget(QTreeWidget *widget, QLineEdit *searchWidget, QString searchString);

    static QTreeWidgetItem *GetRootLevelTreeWidgetItem(QTreeWidgetItem *item);

    static void HideAllItems(QTreeWidgetItem *parent);

    static void ShowAllItems(QTreeWidgetItem *parent);

    static void CollapseAllChildren(QTreeWidgetItem *item);

    static void GetFileIcon(DWORD magic, QString fileName, QIcon &icon, QTreeWidgetItem &item, FileSystem fileSystem = FileSystemSTFS);
    
    static void AddSubWindow(QMdiArea *mdiArea, QWidget *widget);

    static QStringList StdStringArrayToQStringList(std::vector<std::string> strings);

    static void DrawFreeMemoryGraph(FatxDrive *drive, QLabel *graph, QColor backgroundColor, QLabel *freeMemLegendColor,
                                    QLabel *freeMemLegend, QLabel *usedMemLengendColor, QLabel *usedMemLegend, bool contentOnly,
                                    void(*updateUI)(void*, bool));
                                    
private:
    class SubWindowEvents : public QObject
    {
        public:
            SubWindowEvents(QObject* parent);

        protected:
            bool eventFilter(QObject *obj, QEvent *event);
    };
};

#endif // QTHELPERS_H

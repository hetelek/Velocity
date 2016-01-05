#ifndef MULTIPROGRESSDIALOG_H
#define MULTIPROGRESSDIALOG_H

// qt
#include <QDialog>
#include <QMessageBox>
#include <QDir>
#include "qthelpers.h"

// xbox
#include "Stfs/XContentHeader.h"
#include "Disc/Svod.h"
#include "IO/SvodIO.h"
#include "Stfs/StfsPackage.h"
#include "Fatx/FatxConstants.h"
#include "Fatx/FatxDrive.h"
#include "Fatx/XContentDevice.h"
#include "Utils.h"

enum Operation
{
    OpExtract,
    OpReplace,
    OpInject,
    OpBackup,
    OpRestore,
    OpExtractAll
};

struct StfsExtractEntry
{
    StfsFileEntry *entry;
    QString path;
};

struct FatxFriendlyInjectEntry
{
    QString localPath;
    bool isDataFile;
    QString fatxDataFilePath;
};

namespace Ui {
class MultiProgressDialog;
}

void updateProgress(void *form, DWORD curProgress, DWORD total);

class MultiProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MultiProgressDialog(Operation op, FileSystem fileSystem, void *device, QString outDir, QList<void*> internalFiles,
                                 QWidget *parent = 0, QStringList rootPaths = QStringList(), FatxFileEntry *parentEntry = NULL);
    ~MultiProgressDialog();

    void start();

private:
    Ui::MultiProgressDialog *ui;
    FileSystem system;
    void *device;
    QString outDir;
    QList<void*> internalFiles;
    int fileIndex;
    DWORD overallProgress;
    DWORD overallProgressTotal;
    DWORD prevProgress;
    QStringList rootPaths;
    Operation op;
    FatxFileEntry *parentEntry;

    void operateOnNextFile();

    friend void updateProgress(void *form, DWORD curProgress, DWORD total);
};

#endif // MULTIPROGRESSDIALOG_H

#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

// qt
#include <QDialog>
#include <QMessageBox>

// xbox360
#include "Stfs/StfsPackage.h"

// other
#include "stfsworkerthread.h"

namespace Ui {
class ProgressDialog;
}

class ProgressDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ProgressDialog(StfsPackage *package, QList<QString> filesToExtract, QList<QString> outPaths, bool injecting = false, QWidget *parent = 0);
    void startWorking();
    ~ProgressDialog();

private slots:
    void updateProgress(DWORD blocksExtracted, DWORD totalBlocksToExtract);

private:
    Ui::ProgressDialog *ui;

    StfsPackage *package;
    QList<QString> filesToExtract;
    QList<QString> outPaths;
    QList<FileEntry> fileEntries;
    DWORD overallProgress;
    DWORD totalBlocksToExtract;
    DWORD filesExtracted;
    DWORD prevPrgress;

    void extractNextFile();
    void startExtracting();
    void startInjecting();

    bool injecting;
};

#endif // PROGRESSDIALOG_H

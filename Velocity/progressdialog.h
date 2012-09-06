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
    explicit ProgressDialog(StfsPackage *package, QList<QString> filesToExtract, QList<QString> outPaths, QWidget *parent = 0);
    void startExtracting();
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

    void extractNextFile();
};

#endif // PROGRESSDIALOG_H

#ifndef MULTIPROGRESSDIALOG_H
#define MULTIPROGRESSDIALOG_H

// qt
#include <QDialog>
#include <QMessageBox>

// xbox
#include "Stfs/XContentHeader.h"
#include "Disc/svod.h"
#include "IO/SvodIO.h"
#include "Stfs/StfsPackage.h"

namespace Ui {
class MultiProgressDialog;
}

void updateProgress(void *form, DWORD curProgress, DWORD total);

class MultiProgressDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit MultiProgressDialog(FileSystem fileSystem, void *device, QString outDir, QList<void*> filesToExtract, QWidget *parent = 0);
    ~MultiProgressDialog();

    void start();
    
private:
    Ui::MultiProgressDialog *ui;
    FileSystem system;
    void *device;
    QString outDir;
    QList<void*> filesToExtract;
    int fileIndex;
    DWORD overallProgress;
    DWORD overallProgressTotal;
    DWORD prevProgress;

    void extractNextFile();

    friend void updateProgress(void *form, DWORD curProgress, DWORD total);
};

#endif // MULTIPROGRESSDIALOG_H

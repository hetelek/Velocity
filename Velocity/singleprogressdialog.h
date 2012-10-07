#ifndef SINGLEPROGRESSDIALOG_H
#define SINGLEPROGRESSDIALOG_H

// qt
#include <QDialog>

// xbox360
#include "Stfs/StfsPackage.h"

// other
#include "stfsworkerthread.h"

namespace Ui {
class SingleProgressDialog;
}

class SingleProgressDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SingleProgressDialog(StfsPackage *package, QString externalFile, QString packageFilePath, StfsJob job, FileEntry *entry = NULL, QWidget *parent = 0);
    void startJob();
    ~SingleProgressDialog();

private slots:
    void onProgressUpdated(DWORD blocksReplaced, DWORD totalBlockCount);
    
private:
    Ui::SingleProgressDialog *ui;

    StfsPackage *package;
    StfsJob job;
    FileEntry *entry;
    QString externalFile;
    QString packageFilePath;
};

#endif // SINGLEPROGRESSDIALOG_H

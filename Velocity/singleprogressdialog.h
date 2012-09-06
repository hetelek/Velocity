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
    explicit SingleProgressDialog(StfsPackage *package, QString externalFile, QString packageFilePath, QWidget *parent = 0);
    void startReplace();
    ~SingleProgressDialog();

private slots:
    void onProgressUpdated(DWORD blocksReplaced, DWORD totalBlockCount);
    
private:
    Ui::SingleProgressDialog *ui;

    StfsPackage *package;
    QString externalFile;
    QString packageFilePath;
};

#endif // SINGLEPROGRESSDIALOG_H

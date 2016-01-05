#ifndef SINGLEPROGRESSDIALOG_H
#define SINGLEPROGRESSDIALOG_H

// qt
#include <QDialog>
#include "qthelpers.h"
#include "multiprogressdialog.h"

// xbox360
#include "Stfs/StfsPackage.h"
#include "Disc/ISO.h"

void UpdateProgress(void *arg, DWORD cur, DWORD total);

namespace Ui {
class SingleProgressDialog;
}

class SingleProgressDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SingleProgressDialog(FileSystem system, void *device, Operation op, QString internalPath, QString externalPath, void *outEntry = NULL, QWidget *parent = 0);
    void start();
    ~SingleProgressDialog();
    
private:
    Ui::SingleProgressDialog *ui;
    Operation op;
    FileSystem system;
    void *device;
    void *outEntry;
    QString internalPath, externalPath;

    friend void UpdateProgress(void *arg, DWORD cur, DWORD total);
};

#endif // SINGLEPROGRESSDIALOG_H

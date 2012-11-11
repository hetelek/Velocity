#ifndef FATXPATHGENDIALOG_H
#define FATXPATHGENDIALOG_H

// qt
#include <QDialog>
#include <QMessageBox>
#include <QMdiSubWindow>
#include "qthelpers.h"

// xbox360
#include "Stfs/StfsPackage.h"

namespace Ui {
class FATXPathGenDialog;
}

class FATXPathGenDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit FATXPathGenDialog(StfsPackage *package, QWidget *parent = 0);
    ~FATXPathGenDialog();
    
private slots:
    void on_pushButton_clicked();

private:
    Ui::FATXPathGenDialog *ui;

    StfsPackage *package;
};

#endif // FATXPATHGENDIALOG_H

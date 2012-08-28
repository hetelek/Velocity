#ifndef LICENSINGDATADIALOG_H
#define LICENSINGDATADIALOG_H

// qt
#include <QDialog>
#include <QMessageBox>
#include "qthelpers.h"

// xbox360
#include "Stfs/StfsDefinitions.h"

namespace Ui {
class LicensingDataDialog;
}

class LicensingDataDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit LicensingDataDialog(LicenseEntry *entryTable, QWidget *parent = 0);
    ~LicensingDataDialog();
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::LicensingDataDialog *ui;
    LicenseEntry *entryTable;
};

#endif // LICENSINGDATADIALOG_H

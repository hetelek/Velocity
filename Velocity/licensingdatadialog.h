#ifndef LICENSINGDATADIALOG_H
#define LICENSINGDATADIALOG_H

// qt
#include <QDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QComboBox>
#include <QStatusBar>
#include "qthelpers.h"

// xbox360
#include "Stfs/StfsDefinitions.h"

struct LicenseTypeStruct
{
    QString name;
    LicenseType value;
};

namespace Ui {
class LicensingDataDialog;
}

class LicensingDataDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit LicensingDataDialog(QStatusBar *statusBar, LicenseEntry *entryTable, bool unlockable, QWidget *parent = 0);
    ~LicensingDataDialog();
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_btnUnlock_clicked();

private:
    Ui::LicensingDataDialog *ui;
    LicenseEntry *entryTable;
    QStatusBar *statusBar;

    QComboBox *cmbxLicenseTypes[16];
};

#endif // LICENSINGDATADIALOG_H

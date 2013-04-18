#ifndef FLASHDRIVECONFIGDATADIALOG_H
#define FLASHDRIVECONFIGDATADIALOG_H

// qt
#include <QDialog>
#include <QStatusBar>
#include "qthelpers.h"

// xbox
#include "Fatx/FatxConstants.h"

// forms
#include "certificatedialog.h"


namespace Ui {
class FlashDriveConfigDataDialog;
}

class FlashDriveConfigDataDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit FlashDriveConfigDataDialog(FlashDriveConfigurationData *configData, QStatusBar *statusBar, QWidget *parent = 0);
    ~FlashDriveConfigDataDialog();
    
private slots:
    void on_btnCertificate_clicked();

private:
    Ui::FlashDriveConfigDataDialog *ui;
    QStatusBar *statusBar;
    FlashDriveConfigurationData *configData;
};

#endif // FLASHDRIVECONFIGDATADIALOG_H

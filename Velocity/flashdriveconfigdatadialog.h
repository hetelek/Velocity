#ifndef FLASHDRIVECONFIGDATADIALOG_H
#define FLASHDRIVECONFIGDATADIALOG_H

// qt
#include <QDialog>

// xbox
#include "FATX/FatxConstants.h"


namespace Ui {
class FlashDriveConfigDataDialog;
}

class FlashDriveConfigDataDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit FlashDriveConfigDataDialog(FlashDriveConfigurationData *data, QWidget *parent = 0);
    ~FlashDriveConfigDataDialog();
    
private:
    Ui::FlashDriveConfigDataDialog *ui;
};

#endif // FLASHDRIVECONFIGDATADIALOG_H

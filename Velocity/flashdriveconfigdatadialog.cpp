#include "flashdriveconfigdatadialog.h"
#include "ui_flashdriveconfigdatadialog.h"

FlashDriveConfigDataDialog::FlashDriveConfigDataDialog(FlashDriveConfigurationData *data, QWidget *parent) :
    QDialog(parent), ui(new Ui::FlashDriveConfigDataDialog)
{
    ui->setupUi(this);


}

FlashDriveConfigDataDialog::~FlashDriveConfigDataDialog()
{
    delete ui;
}

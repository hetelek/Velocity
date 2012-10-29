#include "stfstoolsdialog.h"
#include "ui_stfstoolsdialog.h"

StfsToolsDialog::StfsToolsDialog(StfsPackage *package, QWidget *parent) : QDialog(parent), ui(new Ui::StfsToolsDialog), package(package)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    QtHelpers::GenAdjustWidgetAppearanceToOS(this);

    if (package->metaData->volumeDescriptor.allocatedBlockCount == 0)
        ui->spnBlock->setMaximum(0);
    else
        ui->spnBlock->setMaximum(package->metaData->volumeDescriptor.allocatedBlockCount - 1);

    ui->lblAddr->setText("0x" + QString::number(package->BlockToAddress(0), 16).toUpper());
    ui->lblHashAddr->setText("0x" + QString::number(package->GetHashAddressOfBlock(0), 16).toUpper());
}

StfsToolsDialog::~StfsToolsDialog()
{
    delete ui;
}

void StfsToolsDialog::on_spnBlock_valueChanged(int arg1)
{
    ui->lblAddr->setText("0x" + QString::number(package->BlockToAddress(arg1), 16).toUpper());
    ui->lblHashAddr->setText("0x" + QString::number(package->GetHashAddressOfBlock(arg1), 16).toUpper());
}

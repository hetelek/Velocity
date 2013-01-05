#include "stfstoolsdialog.h"
#include "ui_stfstoolsdialog.h"

StfsToolsDialog::StfsToolsDialog(StfsPackage *package, QWidget *parent) : QDialog(parent), ui(new Ui::StfsToolsDialog), package(package)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    QtHelpers::GenAdjustWidgetAppearanceToOS(this);

    if (package->metaData->stfsVolumeDescriptor.allocatedBlockCount == 0)
        ui->spnBlock->setMaximum(0);
    else
        ui->spnBlock->setMaximum(package->metaData->stfsVolumeDescriptor.allocatedBlockCount - 1);

    on_spnBlock_valueChanged(0);
}

StfsToolsDialog::~StfsToolsDialog()
{
    delete ui;
}

void StfsToolsDialog::on_spnBlock_valueChanged(int arg1)
{
    DWORD blockNum = package->BlockToAddress(arg1);
    DWORD hashAddr = package->GetHashAddressOfBlock(arg1);

    ui->lblAddr->setText(QString::number(blockNum) + " (0x" + QString::number(blockNum, 16).toUpper() + ")");
    ui->lblHashAddr->setText(QString::number(hashAddr) + " (0x" + QString::number(hashAddr, 16).toUpper() + ")");
}

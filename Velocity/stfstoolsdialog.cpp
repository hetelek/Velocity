#include "stfstoolsdialog.h"
#include "ui_stfstoolsdialog.h"

StfsToolsDialog::StfsToolsDialog(StfsPackage *package, QWidget *parent) : QDialog(parent), ui(new Ui::StfsToolsDialog), package(package)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    ui->spnBlock->setMaximum(package->metaData->volumeDescriptor.allocatedBlockCount - 1);

    ui->lblAddr->setText("0x" + QString::number(package->BlockToAddress(0), 16).toUpper());
    ui->lblHashAddr->setText("0x" + QString::number(package->GetHashAddressOfBlock(0), 16).toUpper());
}

StfsToolsDialog::~StfsToolsDialog()
{
    delete ui;
}

void StfsToolsDialog::on_pushButton_clicked()
{
    ui->lblAddr->setText("0x" + QString::number(package->BlockToAddress(ui->spnBlock->value()), 16).toUpper());
    ui->lblHashAddr->setText("0x" + QString::number(package->GetHashAddressOfBlock(ui->spnBlock->value()), 16).toUpper());
}

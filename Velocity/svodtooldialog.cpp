#include "svodtooldialog.h"
#include "ui_svodtooldialog.h"

SvodToolDialog::SvodToolDialog(SVOD *svod, QWidget *parent) :
    QDialog(parent), ui(new Ui::SvodToolDialog), svod(svod)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    QtHelpers::GenAdjustWidgetAppearanceToOS(this);

    ui->spinBox->setMinimum(svod->metaData->svodVolumeDescriptor.dataBlockOffset * 2);
    ui->spinBox->setMaximum(svod->GetSectorCount());
}

SvodToolDialog::~SvodToolDialog()
{
    delete ui;
}

void SvodToolDialog::on_spinBox_valueChanged(int arg1)
{
    DWORD addr, index;
    svod->SectorToAddress(arg1, &addr, &index);
    ui->lblAddress->setText("0x" + QString::number(addr, 16).toUpper());
    ui->lblFileIndex->setText(QString::number(index));
}

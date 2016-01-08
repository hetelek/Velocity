#include "isosectordialog.h"
#include "ui_isosectordialog.h"

IsoSectorDialog::IsoSectorDialog(ISO *iso, QWidget *parent) :
    QDialog(parent), ui(new Ui::IsoSectorDialog), iso(iso)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    ui->spnSector->setMaximum(0);
    ui->spnSector->setMaximum(iso->GetTotalSectors());

    // update the sector address
    on_spnSector_valueChanged(0);
}

IsoSectorDialog::~IsoSectorDialog()
{
    delete ui;
}

void IsoSectorDialog::on_spnSector_valueChanged(int arg1)
{
    DWORD sector = ui->spnSector->value();
    UINT64 sectorAddress = iso->SectorToAddress(sector);
    QString sectorAddressTxt = "0x" + QString::fromStdString(Utils::ConvertToHexString(sectorAddress));
    ui->lblAddress->setText(sectorAddressTxt);
}

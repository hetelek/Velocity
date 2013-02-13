#include "addressconverterdialog.h"
#include "ui_addressconverterdialog.h"

AddressConverterDialog::AddressConverterDialog(Xdbf *xdbf, QWidget *parent) : QDialog(parent), ui(new Ui::AddressConverterDialog), xdbf(xdbf)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    QtHelpers::GenAdjustWidgetAppearanceToOS(this);

    ui->spnRealAddr->setValue(xdbf->GetRealAddress(0));
    ui->spnRealAddr->setMinimum(ui->spnRealAddr->value());

    setFixedSize(sizeHint());
}

AddressConverterDialog::~AddressConverterDialog()
{
    delete ui;
}
void AddressConverterDialog::on_spnSpecifier_editingFinished()
{
    ui->spnRealAddr->setValue(xdbf->GetRealAddress(ui->spnSpecifier->value()));
}

void AddressConverterDialog::on_spnRealAddr_editingFinished()
{
    ui->spnSpecifier->setValue(xdbf->GetSpecifier(ui->spnRealAddr->value()));
}

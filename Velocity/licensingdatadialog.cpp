#include "licensingdatadialog.h"
#include "ui_licensingdatadialog.h"

LicensingDataDialog::LicensingDataDialog(LicenseEntry *entryTable, QWidget *parent) : QDialog(parent), ui(new Ui::LicensingDataDialog), entryTable(entryTable)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    ui->tableWidget->horizontalHeader()->resizeSection(0, 125);

    // load all the license entries
    for (DWORD i = 0; i < 0x10; i++)
    {
        ui->tableWidget->insertRow(i);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem("0x" + QString::number(entryTable[i].licenseID, 16).toUpper()));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem("0x" + QString::number(entryTable[i].bits, 16).toUpper()));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem("0x" + QString::number(entryTable[i].flags, 16).toUpper()));
    }
}

LicensingDataDialog::~LicensingDataDialog()
{
    delete ui;
}

void LicensingDataDialog::on_pushButton_clicked()
{
    // verify all the values
    for (DWORD i = 0; i < 0x10; i++)
    {
        if (!QtHelpers::VerifyHexString(ui->tableWidget->item(i, 0)->text()))
        {
            QMessageBox::warning(this, "Invalid Value", "The License ID in row " + QString::number(i + 1) + " must be all hexadecimal digits.\n");
            return;
        }
        if (!QtHelpers::VerifyHexString(ui->tableWidget->item(i, 1)->text()))
        {
            QMessageBox::warning(this, "Invalid Value", "The Bits in row " + QString::number(i + 1) + " must be all hexadecimal digits.\n");
            return;
        }
        if (!QtHelpers::VerifyHexString(ui->tableWidget->item(i, 2)->text()))
        {
            QMessageBox::warning(this, "Invalid Value", "The Flags in row " + QString::number(i + 1) + " must be all hexadecimal digits.\n");
            return;
        }
    }

    // update all the license entries
    for (DWORD i = 0; i < 0x10; i++)
    {
        entryTable[i].licenseID = ui->tableWidget->item(i, 0)->text().replace("0x", "").toULongLong(0, 16);
        entryTable[i].bits = QtHelpers::ParseHexString(ui->tableWidget->item(i, 1)->text());
        entryTable[i].flags = QtHelpers::ParseHexString(ui->tableWidget->item(i, 2)->text());
    }

    this->close();
}

void LicensingDataDialog::on_pushButton_2_clicked()
{
    this->close();
}

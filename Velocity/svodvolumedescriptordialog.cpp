#include "svodvolumedescriptordialog.h"
#include "ui_svodvolumedescriptordialog.h"

SvodVolumeDescriptorDialog::SvodVolumeDescriptorDialog(QStatusBar *statusBar, SvodVolumeDescriptor *descriptor, QWidget *parent) :
    QDialog(parent), ui(new Ui::SvodVolumeDescriptorDialog), descriptor(descriptor), statusBar(statusBar)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    ui->tableWidget->setItem(0, 0, new QTableWidgetItem("0x" + QString::number(descriptor->size, 16).toUpper()));
    ui->tableWidget->setItem(1, 0, new QTableWidgetItem(QString::number(descriptor->blockCacheElementCount)));
    ui->tableWidget->setItem(2, 0, new QTableWidgetItem(QString::number(descriptor->workerThreadProcessor)));
    ui->tableWidget->setItem(3, 0, new QTableWidgetItem(QString::number(descriptor->workerThreadPriority)));
    ui->tableWidget->setItem(4, 0, new QTableWidgetItem(QtHelpers::ByteArrayToString(descriptor->rootHash, 0x14, false)));
    ui->tableWidget->setItem(5, 0, new QTableWidgetItem("0x" + QString::number(descriptor->flags, 16)));
    ui->tableWidget->setItem(6, 0, new QTableWidgetItem(QString::number(descriptor->dataBlockCount)));
    ui->tableWidget->setItem(7, 0, new QTableWidgetItem(QString::number(descriptor->dataBlockOffset)));

    statusBar->showMessage("Successfully loaded volume descriptor", 3000);
}

SvodVolumeDescriptorDialog::~SvodVolumeDescriptorDialog()
{
    delete ui;
}

void SvodVolumeDescriptorDialog::on_pushButton_2_clicked()
{
    close();
}

void SvodVolumeDescriptorDialog::on_pushButton_clicked()
{
    // verify all of the values
    if (!QtHelpers::VerifyHexString(ui->tableWidget->item(0, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The struct size must be all hexadecimal digits.\n");
        return;
    }
    else if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(1, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The block cache element must be all digits.\n");
        return;
    }
    else if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(2, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The worker thread processor must be all digits.\n");
        return;
    }
    else if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(3, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The worker thread priority must be all digits.\n");
        return;
    }
    else if (!QtHelpers::VerifyHexStringBuffer(ui->tableWidget->item(4, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The root hash must be all hexadecimal digits.\n");
        return;
    }
    else if (!QtHelpers::VerifyHexString(ui->tableWidget->item(5, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The device features must be all hexadecimal digits.\n");
        return;
    }
    else if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(6, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The data block count must be all hexadecimal digits.\n");
        return;
    }
    else if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(7, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The data block offset must be all hexadecimal digits.\n");
        return;
    }

    // update the struct
    descriptor->size = QtHelpers::ParseHexString(ui->tableWidget->item(0, 0)->text());
    descriptor->blockCacheElementCount = ui->tableWidget->item(1, 0)->text().toInt();
    descriptor->workerThreadProcessor = ui->tableWidget->item(2, 0)->text().toInt();
    descriptor->workerThreadPriority = ui->tableWidget->item(3, 0)->text().toInt();

    QtHelpers::ParseHexStringBuffer(ui->tableWidget->item(4, 0)->text(), descriptor->rootHash, 0x14);

    descriptor->flags = QtHelpers::ParseHexString(ui->tableWidget->item(5, 0)->text());
    descriptor->dataBlockCount = ui->tableWidget->item(4, 0)->text().toInt();
    descriptor->dataBlockOffset = ui->tableWidget->item(7, 0)->text().toInt();

    statusBar->showMessage("Volume Descriptor updated successfully", 3000);

    close();
}

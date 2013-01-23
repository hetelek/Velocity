#include "stfsvolumedescriptordialog.h"
#include "ui_stfsvolumedescriptordialog.h"

StfsVolumeDescriptorDialog::StfsVolumeDescriptorDialog(QStatusBar *statusBar, StfsVolumeDescriptor *desc, QWidget *parent) :
    QDialog(parent), ui(new Ui::StfsVolumeDescriptorDialog), desc(desc), statusBar(statusBar)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    // load struct size
    ui->tableWidget->setItem(0, 0, new QTableWidgetItem("0x" + QString::number(desc->size, 16).toUpper()));

    // load top table
    cmbxHashTable = new QComboBox(this);
    cmbxHashTable->addItem("Top Table");
    cmbxHashTable->addItem("Bottom Table");
    ui->tableWidget->setCellWidget(1, 0, cmbxHashTable);
    cmbxHashTable->setCurrentIndex((desc->blockSeperation >> 1) & 1);

    // load package type
    cmbxPackageType = new QComboBox(this);
    cmbxPackageType->addItem("Female");
    cmbxPackageType->addItem("Male");
    ui->tableWidget->setCellWidget(2, 0, cmbxPackageType);
    cmbxPackageType->setCurrentIndex((~desc->blockSeperation) & 1);
    connect(cmbxPackageType, SIGNAL(currentIndexChanged(int)), this, SLOT(packageTypeChanged(int)));

    if (cmbxPackageType->currentIndex() == 0)
    {
        cmbxHashTable->setCurrentIndex(0);
        cmbxHashTable->setEnabled(false);
    }

    // load the file block stuff
    ui->tableWidget->setItem(3, 0, new QTableWidgetItem(QString::number(desc->fileTableBlockCount)));
    ui->tableWidget->setItem(4, 0, new QTableWidgetItem(QString::number(desc->fileTableBlockNum)));

    // load the top hash table hash
    ui->tableWidget->setItem(5, 0, new QTableWidgetItem(QtHelpers::ByteArrayToString(desc->topHashTableHash, 20, false)));

    // load block counts
    ui->tableWidget->setItem(6, 0, new QTableWidgetItem(QString::number(desc->allocatedBlockCount)));
    ui->tableWidget->setItem(7, 0, new QTableWidgetItem(QString::number(desc->unallocatedBlockCount)));

    statusBar->showMessage("Volume Descriptor loaded successfully", 3000);
}

StfsVolumeDescriptorDialog::~StfsVolumeDescriptorDialog()
{
    delete cmbxHashTable;
    delete cmbxPackageType;
    delete ui;
}

void StfsVolumeDescriptorDialog::on_pushButton_2_clicked()
{
    this->close();
}

void StfsVolumeDescriptorDialog::packageTypeChanged(int index)
{
    if (index == 0)
    {
        cmbxHashTable->setCurrentIndex(0);
        cmbxHashTable->setEnabled(false);
    }
    else
        cmbxHashTable->setEnabled(true);
}

void StfsVolumeDescriptorDialog::on_pushButton_clicked()
{
    // verfiy everything
    if (!QtHelpers::VerifyHexString(ui->tableWidget->item(0, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The Struct Size must be all hexadecimal digits.\n");
        return;
    }
    if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(3, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The File Table Block Count must be all digits.\n");
        return;
    }
    if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(4, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The File Table Block Number must be all digits.\n");
        return;
    }
    if (ui->tableWidget->item(5, 0)->text().length() != 40 || !QtHelpers::VerifyHexStringBuffer(ui->tableWidget->item(5, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The Top Hash Table Hash must be 40 hexadecimal digits.\n");
        return;
    }
    if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(6, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The Total Allocated Block Count must be all digits.\n");
        return;
    }
    if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(7, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The Total Unallocated Block Count must be all digits.\n");
        return;
    }

    // update the struct size
    desc->size = QtHelpers::ParseHexString(ui->tableWidget->item(0, 0)->text());

    // update the block seperation
    desc->blockSeperation = (cmbxHashTable->currentIndex() << 1) | ((~cmbxPackageType->currentIndex()) & 1);

    // update the file block stuff
    desc->fileTableBlockCount = ui->tableWidget->item(3, 0)->text().toULong();
    desc->fileTableBlockNum = ui->tableWidget->item(4, 0)->text().toULong();

    // update the top hash table hash
    QtHelpers::ParseHexStringBuffer(ui->tableWidget->item(5, 0)->text(), desc->topHashTableHash, 20);

    // update the block counts
    desc->allocatedBlockCount = ui->tableWidget->item(6, 0)->text().toULong();
    desc->unallocatedBlockCount = ui->tableWidget->item(7, 0)->text().toULong();

    statusBar->showMessage("Volume Descriptor saved successfully", 3000);

    this->close();
}

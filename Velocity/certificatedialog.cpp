#include "certificatedialog.h"
#include "ui_certificatedialog.h"

CertificateDialog::CertificateDialog(QStatusBar *statusBar, Certificate *cert, QWidget *parent, bool readOnly) :
    QDialog(parent), ui(new Ui::CertificateDialog), cert(cert), statusBar(statusBar)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    if (readOnly)
        ui->pushButton->setEnabled(false);

    ui->tableWidget->setItem(0, 0, new QTableWidgetItem("0x" + QString::number(cert->publicKeyCertificateSize, 16).toUpper()));

    // set the console id
    ui->tableWidget->setItem(1, 0, new QTableWidgetItem(QtHelpers::ByteArrayToString(cert->ownerConsoleID, 5, false)));

    // set the part number
    ui->tableWidget->setItem(2, 0, new QTableWidgetItem(QString::fromStdString(cert->ownerConsolePartNumber)));

    // set the console type
    cmbxConsoleType = new QComboBox(this);
    cmbxConsoleType->addItem("Devkit");
    cmbxConsoleType->addItem("Retail");
    ui->tableWidget->setCellWidget(3, 0, cmbxConsoleType);
    cmbxConsoleType->setCurrentIndex(cert->ownerConsoleType - 1);

    // set up the console type flags
    cmbxTestKit = new QComboBox(this);
    cmbxTestKit->addItem("No");
    cmbxTestKit->addItem("Yes");
    ui->tableWidget->setCellWidget(4, 0, cmbxTestKit);
    cmbxTestKit->setCurrentIndex((cert->consoleTypeFlags & TestKit) >> 30);

    cmbxRecoveryGenerated = new QComboBox(this);
    cmbxRecoveryGenerated->addItem("No");
    cmbxRecoveryGenerated->addItem("Yes");
    ui->tableWidget->setCellWidget(5, 0, cmbxRecoveryGenerated);
    cmbxRecoveryGenerated->setCurrentIndex((cert->consoleTypeFlags & RecoveryGenerated) >> 31);

    // set the date of generation
    ui->tableWidget->setItem(6, 0, new QTableWidgetItem(QString::fromStdString(cert->dateGeneration)));

    // set the public exponent
    ui->tableWidget->setItem(7, 0, new QTableWidgetItem("0x" + QString::number(cert->publicExponent, 16).toUpper()));

    // set the public modulus
    ui->txtPubModulus->setPlainText(QtHelpers::ByteArrayToString(cert->publicModulus, 0x80, true));

    // set the certificate signature
    ui->txtCertSig->setPlainText(QtHelpers::ByteArrayToString(cert->certificateSignature, 0x100, true));

    // set the signature
    ui->txtSig->setPlainText(QtHelpers::ByteArrayToString(cert->signature, 0x80, true));

    statusBar->showMessage("Certificate loaded successfully", 3000);
}

CertificateDialog::~CertificateDialog()
{
    delete cmbxConsoleType;
    delete ui;
}

void CertificateDialog::on_pushButton_2_clicked()
{
    this->close();
}

void CertificateDialog::on_pushButton_clicked()
{
    // verify that all of the values are the correct length
    if (!QtHelpers::VerifyHexString(ui->tableWidget->item(0, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The Public Key Certificate Size must be all hexadecimal digits.\n");
        return;
    }
    if (ui->tableWidget->item(1, 0)->text().trimmed().length() != 10 || !QtHelpers::VerifyHexStringBuffer(ui->tableWidget->item(1, 0)->text().trimmed()))
    {
        QMessageBox::warning(this, "Invalid Value", "The ConsoleID must be 10 hexadecimal digits long.\n");
        return;
    }
    if (ui->tableWidget->item(2, 0)->text().length() > 0x14)
    {
        QMessageBox::warning(this, "Invalid Value", "The length of the Console Part Number must be less than 20.\n");
        return;
    }
    if (ui->tableWidget->item(6, 0)->text().length() != 8)
    {
        QMessageBox::warning(this, "Invalid Value", "The length of the Date of Generation must be 8.\n");
        return;
    }
    if (!QtHelpers::VerifyHexString(ui->tableWidget->item(7, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The Public Exponent must be all hexadecimal digits.\n");
        return;
    }

    cert->publicKeyCertificateSize = QtHelpers::ParseHexString(ui->tableWidget->itemAt(0, 0)->text());
    QtHelpers::ParseHexStringBuffer(ui->tableWidget->item(1, 0)->text(), cert->ownerConsoleID, 5);
    cert->ownerConsolePartNumber = ui->tableWidget->item(2, 0)->text().toStdString();
    cert->consoleTypeFlags = (ConsoleTypeFlags)((cmbxTestKit->currentIndex() << 30) | (cmbxRecoveryGenerated->currentIndex() << 31));
    cert->ownerConsoleType = (ConsoleType)(cmbxConsoleType->currentIndex() + 1);
    cert->dateGeneration = ui->tableWidget->item(6, 0)->text().toStdString();
    cert->publicExponent = QtHelpers::ParseHexString(ui->tableWidget->item(7, 0)->text());

    statusBar->showMessage("Certificate saved successfully", 3000);

    this->close();
}

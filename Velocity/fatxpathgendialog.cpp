#include "fatxpathgendialog.h"
#include "ui_fatxpathgendialog.h"

FATXPathGenDialog::FATXPathGenDialog(StfsPackage *package, QWidget *parent) :
    QDialog(parent), ui(new Ui::FATXPathGenDialog), package(package)
{
    ui->setupUi(this);

    // the general path for packages is: Data\Content\ProfileID\TitleID\ContentType
    ui->txtPath->setText("Data\\Content\\" +
            QtHelpers::ByteArrayToString(package->metaData->profileID, 8, false) + "\\" +
            QString("%1").arg(package->metaData->titleID, 8, 16, QChar('0')).toUpper() + "\\" +
            QString("%1").arg(package->metaData->contentType, 8, 16, QChar('0')).toUpper() + "\\");    
}

FATXPathGenDialog::~FATXPathGenDialog()
{
    package->Close();
    delete package;
    delete ui;
}

void FATXPathGenDialog::on_pushButton_clicked()
{
    QMdiSubWindow *subWin = qobject_cast<QMdiSubWindow*>(this->parent());
    if (subWin)
        subWin->hide();

    close();
}



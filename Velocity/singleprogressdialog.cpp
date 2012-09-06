#include "singleprogressdialog.h"
#include "ui_singleprogressdialog.h"

SingleProgressDialog::SingleProgressDialog(StfsPackage *package, QString externalFile, QString packageFilePath, QWidget *parent) :
    QDialog(parent), ui(new Ui::SingleProgressDialog), package(package), externalFile(externalFile), packageFilePath(packageFilePath)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
}

void SingleProgressDialog::startReplace()
{
    StfsWorkerThread thread(package, Replace, NULL, externalFile, this, packageFilePath);

    connect(&thread, SIGNAL(progressUpdated(DWORD,DWORD)), this, SLOT(onProgressUpdated(DWORD, DWORD)));

    thread.run();
}

void SingleProgressDialog::onProgressUpdated(DWORD blocksReplaced, DWORD totalBlockCount)
{
    ui->progressBar->setMaximum(totalBlockCount);
    ui->progressBar->setValue(blocksReplaced);

    if (blocksReplaced == totalBlockCount)
        this->close();

    QApplication::processEvents();
}

SingleProgressDialog::~SingleProgressDialog()
{
    delete ui;
}

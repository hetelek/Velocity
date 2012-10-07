#include "singleprogressdialog.h"
#include "ui_singleprogressdialog.h"

SingleProgressDialog::SingleProgressDialog(StfsPackage *package, QString externalFile, QString packageFilePath, StfsJob job, FileEntry *entry, QWidget *parent) :
    QDialog(parent), ui(new Ui::SingleProgressDialog), package(package), externalFile(externalFile), packageFilePath(packageFilePath), job(job), entry(entry)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    if (job == Inject)
    {
        setWindowTitle("Injecting File");
        ui->lblIcon->setPixmap(QPixmap(":/Images/add.png"));
    }
}

void SingleProgressDialog::startJob()
{
    StfsWorkerThread thread(package, job, entry, externalFile, this, packageFilePath);

    connect(&thread, SIGNAL(progressUpdated(DWORD,DWORD)), this, SLOT(onProgressUpdated(DWORD, DWORD)));

    thread.run();
}

void SingleProgressDialog::onProgressUpdated(DWORD blocksReplaced, DWORD totalBlockCount)
{
    ui->progressBar->setMaximum(totalBlockCount);
    ui->progressBar->setValue(blocksReplaced);

    QApplication::processEvents();

    if (blocksReplaced == totalBlockCount)
        close();
}

SingleProgressDialog::~SingleProgressDialog()
{
    delete ui;
}

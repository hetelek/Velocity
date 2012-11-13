#include "progressdialog.h"
#include "ui_progressdialog.h"

ProgressDialog::ProgressDialog(StfsPackage *package, QList<QString> filesToExtract, QList<QString> outPaths, QWidget *parent) :
    QDialog(parent),  ui(new Ui::ProgressDialog), package(package), filesToExtract(filesToExtract), outPaths(outPaths), overallProgress(0), totalBlocksToExtract(0), filesExtracted(0), prevPrgress(0)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    // update the ui
    setWindowTitle("Extracting File(s)");

}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}

void ProgressDialog::startExtracting()
{
    // get all the stfs file entries
    for (int i = 0; i < filesToExtract.size(); i++)
    {
        FileEntry temp = package->GetFileEntry(filesToExtract.at(i).toStdString());
        if (temp.blocksForFile == 0)
            totalBlocksToExtract++;
        else
            totalBlocksToExtract += temp.blocksForFile;
        fileEntries.append(temp);
    }

    ui->progressBar_2->setMaximum(totalBlocksToExtract);

    extractNextFile();
}

void ProgressDialog::extractNextFile()
{
    // create a new thread
    StfsWorkerThread thread(package, Extract, const_cast<FileEntry*>(&fileEntries.at(filesExtracted)), outPaths.at(filesExtracted), this, "");
    ui->groupBox->setTitle(filesToExtract.at(filesExtracted));
    ui->groupBox_2->setTitle("Overall Progress, File " + QString::number(filesExtracted + 1) + " of " + QString::number(filesToExtract.size()));
    prevPrgress = 0;

    connect(&thread, SIGNAL(progressUpdated(DWORD, DWORD)), this, SLOT(updateProgress(DWORD,DWORD)));

    thread.run();
}

void ProgressDialog::updateProgress(DWORD blocksExtracted, DWORD totalBlockCount)
{
    // update single file progress bar
    ui->progressBar->setMaximum(totalBlockCount);
    ui->progressBar->setValue(blocksExtracted);

    overallProgress += (blocksExtracted - prevPrgress);

    // update overall progress bar
    ui->progressBar_2->setValue(overallProgress);

    prevPrgress = blocksExtracted;

    if (overallProgress == totalBlocksToExtract)
        close();
    else if (blocksExtracted == totalBlockCount)
    {
        QMutex mutex;
        mutex.lock();

        filesExtracted++;

        mutex.unlock();

        extractNextFile();
    }

    QApplication::processEvents();
}

#include "multiprogressdialog.h"
#include "ui_multiprogressdialog.h"

MultiProgressDialog::MultiProgressDialog(FileSystem fileSystem, void *device, QString outDir, QList<void*> filesToExtract, QWidget *parent) :
    QDialog(parent),ui(new Ui::MultiProgressDialog), system(fileSystem), device(device), outDir(outDir), filesToExtract(filesToExtract), fileIndex(0), overallProgress(0), overallProgressTotal(0)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
}

MultiProgressDialog::~MultiProgressDialog()
{
    delete ui;
}

void MultiProgressDialog::start()
{
    // calculate the total overall progress

    switch(system)
    {
        case FileSystemSTFS:
            break;
        case FileSystemSVOD:
            for (DWORD i = 0; i < filesToExtract.size(); i++)
            {
                GDFXFileEntry *entry = reinterpret_cast<GDFXFileEntry*>(filesToExtract.at(i));
                overallProgressTotal += (entry->size + 0xFFFF) / 0x10000;
            }
            ui->progressBar_2->setMaximum(overallProgressTotal);
            break;
    }

    extractNextFile();
}

void MultiProgressDialog::extractNextFile()
{
    // make sure there's another file to extract
    if (fileIndex == filesToExtract.size())
    {
        close();
        return;
    }

    switch(system)
    {
        case FileSystemSTFS:
            break;
        case FileSystemSVOD:

            // get the file entry
            GDFXFileEntry *entry = reinterpret_cast<GDFXFileEntry*>(filesToExtract.at(fileIndex++));

            setWindowTitle("Extracting " + QString::fromStdString(entry->name));

            // get the file from the device
            SVOD *svod = reinterpret_cast<SVOD*>(device);
            SvodIO io = svod->GetSvodIO(*entry);

            try
            {
                // extract the file
                io.SaveFile(outDir.toStdString() + entry->name, updateProgress, this);
                io.Close();
            }
            catch (string error)
            {
                QMessageBox::critical(this, "", QString::fromStdString(error));
            }

            break;
    }
}

void updateProgress(void *form, DWORD curProgress, DWORD total)
{
    // get the dialog back
    MultiProgressDialog *dialog = reinterpret_cast<MultiProgressDialog*>(form);

    // update the ui
    dialog->ui->progressBar->setValue(curProgress);
    dialog->ui->progressBar->setMaximum(total);

    dialog->ui->progressBar_2->setValue(dialog->overallProgress++);

    if (curProgress == total)
        dialog->extractNextFile();

    QApplication::processEvents();
}

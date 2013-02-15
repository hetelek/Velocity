#include "multiprogressdialog.h"
#include "ui_multiprogressdialog.h"

MultiProgressDialog::MultiProgressDialog(FileSystem fileSystem, void *device, QString outDir, QList<void*> filesToExtract, QWidget *parent) :
    QDialog(parent),ui(new Ui::MultiProgressDialog), system(fileSystem), device(device), outDir(outDir), filesToExtract(filesToExtract),
    fileIndex(0), overallProgress(0), overallProgressTotal(0), prevProgress(0)
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
            for (DWORD i = 0; i < filesToExtract.size(); i++)
            {
                StfsFileEntry *entry = reinterpret_cast<StfsFileEntry*>(filesToExtract.at(i));
                if (entry->blocksForFile == 0)
                    overallProgressTotal++;
                else
                    overallProgressTotal += entry->blocksForFile;
            }
            break;
        case FileSystemSVOD:
            for (DWORD i = 0; i < filesToExtract.size(); i++)
            {
                GdfxFileEntry *entry = reinterpret_cast<GdfxFileEntry*>(filesToExtract.at(i));
                overallProgressTotal += (entry->size + 0xFFFF) / 0x10000;
            } 
            break;
    }

    ui->progressBar_2->setMaximum(overallProgressTotal);
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
    prevProgress = 0;

    switch(system)
    {
        case FileSystemSTFS:
        {

            // get the file entry
            StfsFileEntry *entry = reinterpret_cast<StfsFileEntry*>(filesToExtract.at(fileIndex++));

            setWindowTitle("Extracting " + QString::fromStdString(entry->name));

            try
            {
                StfsPackage *package = reinterpret_cast<StfsPackage*>(device);
                package->ExtractFile(entry, ((filesToExtract.size() != 1) ? (outDir.replace("\\", "/").toStdString() + entry->name) : outDir.toStdString()), updateProgress, this);
            }
            catch (string error)
            {
                QMessageBox::critical(this, "", "An error occurred while extracting files.\n\n" + QString::fromStdString(error));
            }

            break;
        }
        case FileSystemSVOD:
        {

            // get the file entry
            GdfxFileEntry *entry = reinterpret_cast<GdfxFileEntry*>(filesToExtract.at(fileIndex++));

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
                QMessageBox::critical(this, "", "An error occurred while extracting files.\n\n" + QString::fromStdString(error));
            }

            break;
        }
    }
}

void updateProgress(void *form, DWORD curProgress, DWORD total)
{
    // get the dialog back
    MultiProgressDialog *dialog = reinterpret_cast<MultiProgressDialog*>(form);

    // update the ui
    dialog->ui->progressBar->setValue(curProgress);
    dialog->ui->progressBar->setMaximum(total);

    dialog->overallProgress += (curProgress - dialog->prevProgress);
    dialog->ui->progressBar_2->setValue(dialog->overallProgress);
    dialog->prevProgress = curProgress;

    if (curProgress == total)
        dialog->extractNextFile();

    QApplication::processEvents();
}

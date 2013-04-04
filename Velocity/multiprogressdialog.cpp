#include "multiprogressdialog.h"
#include "ui_multiprogressdialog.h"

MultiProgressDialog::MultiProgressDialog(Operation op, FileSystem fileSystem, void *device, QString outDir, QList<void *> internalFiles, QWidget *parent, QString rootPath, FatxFileEntry *parentEntry) :
    QDialog(parent),ui(new Ui::MultiProgressDialog), system(fileSystem), device(device), outDir(outDir), internalFiles(internalFiles),
    fileIndex(0), overallProgress(0), overallProgressTotal(0), prevProgress(0), rootPath(rootPath), op(op), parentEntry(parentEntry)
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
            for (DWORD i = 0; i < internalFiles.size(); i++)
            {
                StfsFileEntry *entry = reinterpret_cast<StfsFileEntry*>(internalFiles.at(i));
                if (entry->blocksForFile == 0)
                    overallProgressTotal++;
                else
                    overallProgressTotal += entry->blocksForFile;
            }
            break;
        case FileSystemSVOD:
            for (DWORD i = 0; i < internalFiles.size(); i++)
            {
                GdfxFileEntry *entry = reinterpret_cast<GdfxFileEntry*>(internalFiles.at(i));
                overallProgressTotal += (entry->size + 0xFFFF) / 0x10000;
            } 
            break;
        case FileSystemFATX:
            overallProgressTotal = 0;
            ui->progressBar_2->setTextVisible(false);
            break;
    }

    ui->progressBar_2->setMaximum(overallProgressTotal);
    extractNextFile();
}

void MultiProgressDialog::extractNextFile()
{
    // make sure there's another file to extract
    if (fileIndex == internalFiles.size())
    {
        close();
        return;
    }
    prevProgress = 0;

    switch(system)
    {
        case FileSystemSTFS:
        {
            if (op != OpExtract)
                throw string("MultiProgressDialog: Invalid operation for file system.\n");

            // get the file entry
            StfsFileEntry *entry = reinterpret_cast<StfsFileEntry*>(internalFiles.at(fileIndex++));

            setWindowTitle("Extracting " + QString::fromStdString(entry->name));

            try
            {
                StfsPackage *package = reinterpret_cast<StfsPackage*>(device);
                package->ExtractFile(entry, ((internalFiles.size() != 1) ? (outDir.replace("\\", "/").toStdString() + entry->name) : outDir.toStdString()), updateProgress, this);
            }
            catch (string error)
            {
                QMessageBox::critical(this, "", "An error occurred while extracting files.\n\n" + QString::fromStdString(error));
            }

            break;
        }
        case FileSystemSVOD:
        {
            if (op != OpExtract)
                throw string("MultiProgressDialog: Invalid operation for file system.\n");

            // get the file entry
            GdfxFileEntry *entry = reinterpret_cast<GdfxFileEntry*>(internalFiles.at(fileIndex++));

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
        case FileSystemFATX:
        {
            // update groupbox text
            ui->groupBox_2->setTitle("Overall Progress - " + QString::number(fileIndex + 1) + " of " + QString::number(internalFiles.size()));

            if (op == OpExtract)
            {
                // get the file entry
                FatxFileEntry *entry = reinterpret_cast<FatxFileEntry*>(internalFiles.at(fileIndex++));

                if (entry->fileAttributes & FatxDirectory)
                    extractNextFile();

                setWindowTitle("Extracting " + QString::fromStdString(entry->name));

                // get the file from the device
                FatxDrive *drive = reinterpret_cast<FatxDrive*>(device);
                FatxIO io = drive->GetFatxIO(entry);

                try
                {
                    // make all the directories needed
                    QString temp = QString::fromStdString(entry->path);
                    QString dirPath = QDir::toNativeSeparators(outDir + temp.replace(rootPath, ""));
                    QDir saveDir(dirPath);

                    if (!saveDir.exists())
                        saveDir.mkpath(dirPath);

                    // extract the file
                    io.SaveFile(dirPath.toStdString() + entry->name, updateProgress, this);
                }
                catch (string error)
                {
                    QMessageBox::critical(this, "", "An error occurred while extracting files.\n\n" + QString::fromStdString(error));
                }
            }
            else if (op == OpInject)
            {
                try
                {
                    // get the file from the device
                    FatxDrive *drive = reinterpret_cast<FatxDrive*>(device);

                    QString *fileName = reinterpret_cast<QString*>(internalFiles.at(fileIndex++));
                    QFileInfo fileInfo(*fileName);
                    QString cleanName = *fileName;

                    // set up the parent entry
                    FatxFileEntry *pEntry = parentEntry;
                    if (rootPath != "")
                    {
                        // fix the path seperators
                        rootPath = rootPath.replace("/", "\\");
                        *fileName = fileName->replace("/", "\\");

                        // get the FATX file path
                        QString fatxPath = QString::fromStdString(parentEntry->path + parentEntry->name) + fileName->replace(rootPath, "").mid(0, fileName->replace(rootPath, "").lastIndexOf("\\"));
                        pEntry = drive->GetFileEntry(fatxPath.toStdString());

                        // if the directory on the device doesn't exist, then we need to create it
                        if (pEntry == NULL)
                        {
                            drive->CreateFolder(drive->GetFileEntry(fatxPath.mid(0, fatxPath.lastIndexOf("\\")).toStdString()), fatxPath.mid(fatxPath.lastIndexOf("\\") + 1).toStdString());
                            pEntry = drive->GetFileEntry(fatxPath.toStdString());
                        }
                    }

                    // check if the file already exists
                    if (drive->FileExists(pEntry, fileInfo.fileName().toStdString()))
                    {
                        int button = QMessageBox::question(this, "File Alread Exists", "The file " + fileInfo.fileName() +
                                              " already exists in this directory. Would you like to replace the current one?",
                                              QMessageBox::Yes, QMessageBox::No);

                        if (button == QMessageBox::Yes)
                        {
                            FatxIO file = drive->GetFatxIO(drive->GetFileEntry(pEntry->path + pEntry->name + "\\" + fileInfo.fileName().toStdString()));
                            file.ReplaceFile(cleanName.toStdString(), updateProgress, this);
                        }
                    }
                    else
                    {
                        drive->InjectFile(pEntry, fileInfo.fileName().toStdString(), cleanName.toStdString(), updateProgress, this);
                    }
                }
                catch (string error)
                {
                    QMessageBox::critical(this, "", "An error occurred while copying files to the xbox drive.\n\n" + QString::fromStdString(error));
                }
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

    if (dialog->system != FileSystemFATX)
    {
        dialog->overallProgress += (curProgress - dialog->prevProgress);
        dialog->ui->progressBar_2->setValue(dialog->overallProgress);
        dialog->prevProgress = curProgress;
    }

    if (curProgress == total)
        dialog->extractNextFile();

    QApplication::processEvents();
}

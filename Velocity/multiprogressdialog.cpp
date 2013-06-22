#include "multiprogressdialog.h"
#include "ui_multiprogressdialog.h"

MultiProgressDialog::MultiProgressDialog(Operation op, FileSystem fileSystem, void *device, QString outDir, QList<void *> internalFiles, QWidget *parent, QString rootPath, FatxFileEntry *parentEntry) :
    QDialog(parent),ui(new Ui::MultiProgressDialog), system(fileSystem), device(device), outDir(outDir), internalFiles(internalFiles),
    fileIndex(0), overallProgress(0), overallProgressTotal(0), prevProgress(0), rootPath(rootPath), op(op), parentEntry(parentEntry)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    QtHelpers::GenAdjustWidgetAppearanceToOS(this);

    switch (op)
    {
        case OpInject:
            ui->lblIcon->setPixmap(QPixmap(":/Images/inject.png"));
            break;
        case OpExtract:
            ui->lblIcon->setPixmap(QPixmap(":/Images/extract.png"));
            break;
    }
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
        case FileSystemFriendlyFATX:
            overallProgressTotal = 0;
            ui->progressBar_2->setTextVisible(false);
            break;
    }

    ui->progressBar_2->setMaximum(overallProgressTotal);
    operateOnNextFile();
}

void MultiProgressDialog::operateOnNextFile()
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
            // get the file entry
            StfsExtractEntry *entry = reinterpret_cast<StfsExtractEntry*>(internalFiles.at(fileIndex++));

            setWindowTitle("Extracting " + QString::fromStdString(entry->entry->name));

            try
            {
                // make all the directories needed
                QString dirPath = QDir::toNativeSeparators(outDir + entry->path).replace("\\", "/");
                std::string dirPathStd = dirPath.toStdString();

                if (internalFiles.size() != 1)
                {
                    QDir saveDir(dirPath);

                    if (!saveDir.exists())
                        saveDir.mkpath(dirPath);

                    dirPathStd += entry->entry->name;
                }

                StfsPackage *package = reinterpret_cast<StfsPackage*>(device);
                package->ExtractFile(entry->entry, dirPathStd, updateProgress, this);

                delete entry;
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
            if (op == OpExtract)
            {
                for (int i = 0; i < internalFiles.size(); i++)
                {
                    // update groupbox text
                    ui->groupBox_2->setTitle("Overall Progress - " + QString::number(i + 1) + " of " + QString::number(internalFiles.size()));

                    // get the file entry
                    FatxFileEntry *entry = reinterpret_cast<FatxFileEntry*>(internalFiles.at(i));

                    if (entry->fileAttributes & FatxDirectory)
                        continue;

                    setWindowTitle("Copying " + QString::fromStdString(entry->name));

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

                    // reset the progress
                    prevProgress = 0;
                }
            }
            else if (op == OpInject)
            {
                bool failed = false;
                try
                {
                    for (int i = 0; i < internalFiles.size(); i++)
                    {
                        // update groupbox text
                        ui->groupBox_2->setTitle("Overall Progress - " + QString::number(i + 1) + " of " + QString::number(internalFiles.size()));

                        // get the file from the device
                        FatxDrive *drive = reinterpret_cast<FatxDrive*>(device);

                        QString *fileName = reinterpret_cast<QString*>(internalFiles.at(i));
                        QFileInfo fileInfo(*fileName);
                        QString cleanName = *fileName;

                        setWindowTitle("Copying " + fileInfo.fileName());

                        // set up the parent entry
                        FatxFileEntry *pEntry = parentEntry;
                        if (rootPath != "")
                        {
                            // fix the path seperators
                            rootPath = rootPath.replace("/", "\\");
                            *fileName = fileName->replace("/", "\\");

                            // get the FATX file path
                            QString fatxPath = QString::fromStdString(parentEntry->path + parentEntry->name) + fileName->replace(rootPath, "").mid(0, fileName->replace(rootPath, "").lastIndexOf("\\"));
                            pEntry = drive->CreatePath(fatxPath.toStdString());
                        }

                        // check if the file already exists
                        if (drive->FileExists(pEntry, fileInfo.fileName().toStdString()))
                        {
                            int button = QMessageBox::question(this, "File Already Exists", "The file " + fileInfo.fileName() +
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


                        // reset the progress
                        prevProgress = 0;
                    }
                }
                catch (string error)
                {
                    failed = true;
                    QMessageBox::critical(this, "", "An error occurred while copying files to the drive.\n\n" + QString::fromStdString(error));
                }
            }

            // cleanup
            close();
            return;
        }

        case FileSystemFriendlyFATX:
        {
            if (op == OpExtract)
            {
                for (int i = 0; i < internalFiles.size(); i++)
                {
                    // update groupbox text
                    ui->groupBox_2->setTitle("Overall Progress - " + QString::number(i + 1) + " of " + QString::number(internalFiles.size()));

                    // get the file entry
                    std::string *file = reinterpret_cast<std::string*>(internalFiles.at(i));
                    std::string fileName = file->substr(file->find_last_of('\\') + 1);

                    setWindowTitle("Copying " + QString::fromStdString(fileName));

                    // get the file from the device
                    XContentDevice *drive = reinterpret_cast<XContentDevice*>(device);

                    try
                    {
                        // extract the file
                        drive->CopyFileToLocalDisk(outDir.toStdString() + "/" + fileName, *file, updateProgress, this);
                    }
                    catch (string error)
                    {
                        QMessageBox::critical(this, "", "An error occurred while extracting files.\n\n" + QString::fromStdString(error));
                    }

                    // reset the progress
                    prevProgress = 0;
                }
            }
            else if (op == OpInject)
            {
                try
                {
                    for (int i = 0; i < internalFiles.size(); i++)
                    {
                        // update groupbox text
                        ui->groupBox_2->setTitle("Overall Progress - " + QString::number(i + 1) + " of " + QString::number(internalFiles.size()));

                        std::string *file = reinterpret_cast<std::string*>(internalFiles.at(i));

                        QFileInfo fileInfo(QString::fromStdString(*file));
                        setWindowTitle("Copying " + fileInfo.baseName());

                        // put the file on the device
                        XContentDevice *drive = reinterpret_cast<XContentDevice*>(device);
                        drive->CopyFileToDevice(*file, updateProgress, this);
                    }
                }
                catch (string error)
                {
                    QMessageBox::critical(this, "", "An error occurred while copying files to the device.\n\n" + QString::fromStdString(error));
                }
            }

            close();
            return;
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

    if (dialog->system != FileSystemFATX && dialog->system != FileSystemFriendlyFATX)
    {
        dialog->overallProgress += (curProgress - dialog->prevProgress);
        dialog->ui->progressBar_2->setValue(dialog->overallProgress);
        dialog->prevProgress = curProgress;

        if (curProgress == total)
            dialog->operateOnNextFile();
    }

    QApplication::processEvents();
}

#include "multiprogressdialog.h"
#include "ui_multiprogressdialog.h"

MultiProgressDialog::MultiProgressDialog(Operation op, FileSystem fileSystem, void *device, QString outDir, QList<void *> internalFiles, QWidget *parent, QStringList rootPaths, FatxFileEntry *parentEntry) :
    QDialog(parent),ui(new Ui::MultiProgressDialog), system(fileSystem), device(device), outDir(outDir), internalFiles(internalFiles),
    fileIndex(0), overallProgress(0), overallProgressTotal(0), prevProgress(0), rootPaths(rootPaths), op(op), parentEntry(parentEntry)
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
            for (int i = 0; i < internalFiles.size(); i++)
            {
                StfsFileEntry *entry = reinterpret_cast<StfsFileEntry*>(internalFiles.at(i));
                if (entry->blocksForFile == 0)
                    overallProgressTotal++;
                else
                    overallProgressTotal += entry->blocksForFile;
            }
            break;
        case FileSystemSVOD:
            for (int i = 0; i < internalFiles.size(); i++)
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
                        QString rootPath = "";
                        if (rootPaths.size() > i)
                            rootPath = rootPaths.at(i);

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
                        if (rootPaths.size() > i)
                        {
                            QString rootPath = rootPaths.at(i);

                            // fix the path srootPath != ""eperators
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
                        QString rootPath = "";
                        if (rootPaths.size() > i)
                            rootPath = rootPaths.at(i);

                        // make all the directories needed
                        QString relativeDirectory = QString::fromStdString(*file);
                        relativeDirectory.chop(fileName.size());
                        QString dirPath = QDir::toNativeSeparators(outDir + "/" + relativeDirectory.replace(rootPath, ""));
                        QDir saveDir(dirPath);

                        if (!saveDir.exists())
                            saveDir.mkpath(dirPath);

                        // get the correct local file path
                        std::string localFilePath = dirPath.toStdString();
                        localFilePath = Utils::NormalizeFilePath(localFilePath, '\\', '/');
                        if (localFilePath.at(localFilePath.size() - 1) != '/')
                            localFilePath += "/";
                        localFilePath += fileName;

                        // extract the file
                        drive->CopyFileToLocalDisk(localFilePath, *file, updateProgress, this);
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
                    // determine the total number of files that need to be copied to the device, SVOD systems may have data files to copy
                    QList<FatxFriendlyInjectEntry> files;
                    for (size_t i = 0; i < internalFiles.size(); i++)
                    {
                        // get the original file
                        std::string *file = reinterpret_cast<std::string*>(internalFiles.at(i));

                        FatxFriendlyInjectEntry fileInfo;
                        fileInfo.localPath = QString::fromStdString(*file);
                        fileInfo.isDataFile = false;

                        // check to see if it's an SVOD system
                        if (IXContentHeader::GetFileSystem(*file) == FileSystemSVOD)
                        {
                            SVOD svod(*file);
                            std::string fatxDataFilePath = svod.GetFatxFilePath() + svod.GetContentName() + ".data\\";

                            // get all the data files (if this file has any)
                            std::vector<std::string> dataFilePaths = SVOD::GetDataFilePaths(*file);
                            for (size_t x = 0; x < dataFilePaths.size(); x++)
                            {
                                FatxFriendlyInjectEntry dataFileInfo;
                                dataFileInfo.localPath = QString::fromStdString(dataFilePaths.at(x));
                                dataFileInfo.isDataFile = true;
                                dataFileInfo.fatxDataFilePath = QString::fromStdString(fatxDataFilePath);

                                files.push_back(dataFileInfo);
                            }
                        }

                        // this has to be added after the content data files so that when the root descriptor is added the content device
                        // can load the SVOD system off the device
                        files.push_back(fileInfo);
                    }

                    for (size_t i = 0; i < files.size(); i++)
                    {
                        // update groupbox text
                        ui->groupBox_2->setTitle("Overall Progress - " + QString::number(i + 1) + " of " + QString::number(files.size()));

                        QString filePath = files.at(i).localPath;
                        bool isDataFile = files.at(i).isDataFile;
                        QString fatxDataFilePath = files.at(i).fatxDataFilePath;

                        QFileInfo fileInfo(filePath);
                        setWindowTitle("Copying " + fileInfo.baseName());

                        // put the file on the device
                        XContentDevice *drive = reinterpret_cast<XContentDevice*>(device);
                        if (isDataFile)
                            drive->CopyFileToRawDevice(filePath.toStdString(), fileInfo.baseName().toStdString(), fatxDataFilePath.toStdString(), updateProgress, this);
                        else
                            drive->CopyFileToDevice(filePath.toStdString(), updateProgress, this);
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

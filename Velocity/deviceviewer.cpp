#include "deviceviewer.h"
#include "ui_deviceviewer.h"

#include <QDebug>

DeviceViewer::DeviceViewer(QStatusBar *statusBar, QList<QAction *> gpdActions, QList<QAction *> gameActions, QWidget *parent) :
    QDialog(parent), ui(new Ui::DeviceViewer), currentDrive(NULL), parentEntry(NULL),
    gpdActions(gpdActions), gameActions(gameActions), statusBar(statusBar), drivesLoaded(false)
  {
    ui->setupUi(this);

    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 3);

    ui->treeWidget->header()->setDefaultSectionSize(100);
    ui->treeWidget->header()->resizeSection(0, 250);
    ui->treeWidget->sortItems(1, Qt::AscendingOrder);

    // setup the context menus
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    // setup treewdiget for drag and drop
    setAcceptDrops(true);
    ui->treeWidget->setAcceptDrops(true);
    connect(ui->treeWidget, SIGNAL(dragDropped(QDropEvent*)), this, SLOT(onDragDropped(QDropEvent*)));
    connect(ui->treeWidget, SIGNAL(dragEntered(QDragEnterEvent*)), this, SLOT(onDragEntered(QDragEnterEvent*)));
    connect(ui->treeWidget, SIGNAL(dragLeft(QDragLeaveEvent*)), this, SLOT(onDragLeft(QDragLeaveEvent*)));

    progressBar = new QProgressBar(this);
    progressBar->setMaximumHeight(statusBar->height() - 5);
    progressBar->setMinimumWidth(statusBar->width());
    progressBar->setTextVisible(false);
    progressBar->setVisible(false);
    statusBar->addWidget(progressBar);
}

DeviceViewer::~DeviceViewer()
{
    for (size_t i = 0; i < loadedDrives.size(); i++)
        delete loadedDrives.at(i);

    delete ui;
}

void DeviceViewer::DrawMemoryGraph()
{
    if (!drivesLoaded)
    {
        progressBar->setVisible(true);
        progressBar->setMinimum(0);
        progressBar->setMaximum(0);
    }

    QtHelpers::DrawFreeMemoryGraph(currentDrive, ui->imgPiechart, ui->imgPiechart->palette().background().color(),
                                   ui->imgFreeMem, ui->lblFeeMemory, ui->imgUsedMem, ui->lblUsedSpace, false, updateUI);
}

void DeviceViewer::showContextMenu(QPoint point)
{
    QPoint globalPos = ui->treeWidget->mapToGlobal(point);
    QMenu contextMenu;

    QList<QTreeWidgetItem*> items = ui->treeWidget->selectedItems();

    foreach (QTreeWidgetItem *item, items)
        if (item->data(5, Qt::UserRole).toBool())
            return;

    if (parentEntry == NULL || parentEntry->name == "Drive Root")
        return;

    if (items.size() >= 1)
    {
        contextMenu.addAction(QPixmap(":/Images/extract.png"), "Copy Selected to Local Disk");
        contextMenu.addAction(QPixmap(":/Images/add.png"), "Copy File(s) Here");
        contextMenu.addAction(QPixmap(":/Images/NewFolder.png"), "Copy Folder Here");
        contextMenu.addAction(QPixmap(":/Images/FolderFileIcon.png"), "Create Folder Here");

        contextMenu.addSeparator();
        contextMenu.addAction(QPixmap(":/Images/delete.png"), "Delete Selected");
        contextMenu.addSeparator();

        if (items.size() == 1)
        {
            contextMenu.addAction(QPixmap(":/Images/rename.png"), "Rename");
            contextMenu.addAction(QPixmap(":/Images/properties.png"), "View Properties");
        }
    }
    else
    {
        contextMenu.addAction(QPixmap(":/Images/add.png"), "Copy File(s) Here");
        contextMenu.addAction(QPixmap(":/Images/NewFolder.png"), "Copy Folder Here");
        contextMenu.addAction(QPixmap(":/Images/FolderFileIcon.png"), "Create Folder Here");
    }

    contextMenu.addSeparator();
    contextMenu.addAction(QPixmap(":/Images/refresh.png"), "Refresh");

    QAction *selectedItem = contextMenu.exec(globalPos);
    if(selectedItem == NULL)
        return;

    try
    {
        if (selectedItem->text() == "Copy Selected to Local Disk")
        {
            QList<void*> filesToExtract;

            // get the entries
            for (int i = 0; i < items.size(); i++)
            {
                FatxFileEntry *entry = items.at(i)->data(0, Qt::UserRole).value<FatxFileEntry*>();
                if (entry->fileAttributes & FatxDirectory)
                    GetSubFilesFATX(entry, filesToExtract);
                else
                    filesToExtract.push_back(entry);
            }

            // get the save path
            QString path = QFileDialog::getExistingDirectory(this, "Save Location", QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));

            if (path.isEmpty())
                return;

            QStringList rootPaths;
            QString rootPath = QString::fromStdString(directoryChain.last()->path + directoryChain.last()->name + "\\");
            rootPaths.push_back(rootPath);

            // save the file to the local disk
            MultiProgressDialog *dialog = new MultiProgressDialog(OpExtract, FileSystemFATX, currentDrive, path + "/", filesToExtract, this, rootPaths);
            dialog->setModal(true);
            dialog->show();
            dialog->start();
        }
        else if (selectedItem->text() == "View Properties")
        {
            FatxFileEntry *entry = items.at(0)->data(0, Qt::UserRole).value<FatxFileEntry*>();
            FatxFileDialog dialog(currentDrive, entry, entry->partition->clusterSize, items.at(0)->data(1, Qt::UserRole).toString(), this);
            dialog.exec();

            items.at(0)->setText(0, QString::fromStdString(entry->name));
        }
        else if (selectedItem->text() == "Copy File(s) Here")
        {
            QStringList toInjectPaths = QFileDialog::getOpenFileNames(this, "Choose file(s) to copy...",
                    QtHelpers::DesktopLocation());
            if (toInjectPaths.size() == 0)
                return;

            QList<void*> files;
            for (int i = 0; i < toInjectPaths.size(); i++)
                files.push_back(const_cast<void*>((void*)&toInjectPaths.at(i)));

            InjectFiles(files, "");
        }
        else if (selectedItem->text() == "Copy Folder Here")
        {
            QString folder = QFileDialog::getExistingDirectory(this, "Choose a folder to copy...", QtHelpers::DesktopLocation());
            if (folder == "")
                return;

            QList<void*> files;
            GetSubFilesLocal(folder, files);

            InjectFiles(files, QFileInfo(folder).path());
        }
        else if (selectedItem->text() == "Delete Selected")
        {
            // we don't want the user doing anything else while this operation is being completed
            SetWidgetsEnabled(false);
            ui->btnBack->setEnabled(false);

            // start the progress bar
            progressBar->setVisible(true);
            progressBar->setMaximum(0);


            // delete the entries
            for (int i = 0; i < items.size(); i++)
            {
                FatxFileEntry *entry = items.at(i)->data(0, Qt::UserRole).value<FatxFileEntry*>();
                currentDrive->RemoveFile(entry, updateUIDelete, this);

                QApplication::processEvents();
                DrawMemoryGraph();

                delete items.at(i);
            }

            // hide the progress bar
            progressBar->setVisible(false);
            progressBar->setMaximum(1);

            ui->btnBack->setEnabled(true);
            SetWidgetsEnabled(true);
        }
        else if (selectedItem->text() == "Create Folder Here")
        {
            bool ok;
            QString text = QInputDialog::getText(this, "New Folder", "Folder name:", QLineEdit::Normal, "New Folder", &ok, windowFlags() & ~Qt::WindowContextHelpButtonHint);
            if (ok && !text.isEmpty())
            {
                if (text.length() > FATX_ENTRY_MAX_NAME_LENGTH)
                    QMessageBox::critical(this, "Invalid Filename", "The inputted name is invalid.");
                else
                {
                    FatxFileEntry *entry = currentDrive->CreateFolder(parentEntry, text.toStdString());

                    // setup the tree widget item
                    QTreeWidgetItem *entryItem = new QTreeWidgetItem(ui->treeWidget);
                    entryItem->setData(0, Qt::UserRole, QVariant::fromValue(entry));
                    entryItem->setData(5, Qt::UserRole, QVariant(false));

                    // show the directory indicator
                    entryItem->setIcon(0, QIcon(":/Images/FolderFileIcon.png"));

                    // setup the text
                    entryItem->setText(0, QString::fromStdString(entry->name));

                    MSTime createdtime = DWORDToMSTime(entry->creationDate);

                    QDate date;
                    date.setDate(createdtime.year, createdtime.month, createdtime.monthDay);

                    entryItem->setText(2, date.toString(Qt::DefaultLocaleShortDate));
                }
            }
        }
        else if (selectedItem->text() == "Rename")
        {
            QString name, label = "";

            do
            {
                name = QInputDialog::getText(this, "New File Name", label, QLineEdit::Normal, items.at(0)->text(0), NULL, windowFlags() & ~Qt::WindowContextHelpButtonHint);
                label = "<font color=\"red\">Invalid File Name</font>";
            }
            while (!FatxDrive::ValidFileName(name.toStdString()));

            if (name == "")
                return;

            FatxFileEntry *fileEntry = items.at(0)->data(0, Qt::UserRole).value<FatxFileEntry*>();
            FatxIO io = currentDrive->GetFatxIO(fileEntry);

            fileEntry->name = name.toStdString();
            io.WriteEntryToDisk();

            items.at(0)->setText(0, name);
        }
        else if (selectedItem->text() == "Refresh")
        {
            LoadFolderAll(parentEntry);
        }
    }
    catch (std::string error)
    {
        SetWidgetsEnabled(true);
        progressBar->setVisible(false);
        progressBar->setMaximum(1);

        QMessageBox::warning(this, "Problem", "The operation failed.\n\n" + QString::fromStdString(error));
    }
}

void DeviceViewer::InjectFiles(QList<void*> files, QString rootPath)
{
    MultiProgressDialog *dialog = new MultiProgressDialog(OpInject, FileSystemFATX, currentDrive, "", files, this, QStringList(rootPath), parentEntry);
    dialog->setModal(true);
    dialog->show();
    dialog->start();

    DrawMemoryGraph();

    LoadFolderAll(parentEntry);
}

void DeviceViewer::DrawHeader(QString driveName)
{
    DrawMemoryGraph();
    if (currentDrive->GetFatxDriveType() == FatxHarddrive)
        ui->btnSecurityBlob->setText("Security Blob");
    else
        ui->btnSecurityBlob->setText("Config Data");
    ui->txtDriveName->setText(driveName);

    ui->imgDrive->setPixmap(((currentDrive->GetFatxDriveType() == FatxHarddrive) ? QPixmap(":/Images/harddrive.png") : QPixmap(":/Images/usb drive.png")));
}

void DeviceViewer::LoadDrives()
{
    // clear all the items
    ui->treeWidget->clear();

    for (size_t i = 0; i < loadedDrives.size(); i++)
        delete loadedDrives.at(i);

    try
    {
        loadedDrives = FatxDriveDetection::GetAllFatxDrives();
        if (loadedDrives.size() < 1)
        {
            statusBar->showMessage("No drives detected", 3000);
            return;
        }

        for (size_t i = 0; i < loadedDrives.size(); i++)
        {
            QTreeWidgetItem *driveItem = new QTreeWidgetItem(ui->treeWidget_2);
            driveItem->setData(0, Qt::UserRole, QVariant::fromValue(loadedDrives.at(i)));

            if (loadedDrives.at(i)->GetFatxDriveType() == FatxHarddrive)
            {
                driveItem->setIcon(0, QIcon(":/Images/harddrive.png"));
                driveItem->setText(0, "Hard Drive");
            }
            else
            {
                driveItem->setIcon(0, QIcon(":/Images/usb drive.png"));
                driveItem->setText(0, "Flash Drive");
            }

            // load the partion information
            std::vector<Partition*> parts = loadedDrives.at(i)->GetPartitions();
            for (size_t j = 0; j < parts.size(); j++)
            {
                QTreeWidgetItem *secondItem = new QTreeWidgetItem(driveItem);
                secondItem->setText(0, QString::fromStdString(parts.at(j)->name));
                secondItem->setIcon(0, QIcon(":/Images/partition.png"));
                secondItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
                secondItem->setData(0, Qt::UserRole, QVariant::fromValue(parts.at(j)));
                secondItem->setData(5, Qt::UserRole, QVariant::fromValue(true));
                secondItem->setData(4, Qt::UserRole, QVariant::fromValue(-1));
            }

            // load the name of the drive
            FatxFileEntry *nameEntry = loadedDrives.at(i)->GetFileEntry("Drive:\\Content\\name.txt");
            QString name = (loadedDrives.at(i)->GetFatxDriveType() == FatxHarddrive) ? "Hard Drive" : "Flash Drive";
            if (nameEntry)
            {
                FatxIO nameFile = loadedDrives.at(i)->GetFatxIO(nameEntry);
                nameFile.SetPosition(0);

                // make sure that it starts with 0xFEFF
                if (nameFile.ReadWord() == 0xFEFF)
                    name = QString::fromStdWString(nameFile.ReadWString((nameEntry->fileSize > 0x36) ? 26 : (nameEntry->fileSize - 2) / 2));
            }

            driveItem->setText(0, name);
            previousName = name;
        }

        currentDrive = loadedDrives.at(0);
        currentDriveItem = ui->treeWidget_2->topLevelItem(0);
        LoadPartitions();
        drivesLoaded = true;

        DrawHeader(ui->treeWidget_2->topLevelItem(0)->text(0));
        SetWidgetsEnabled(true);
        statusBar->showMessage("Drive(s) loaded successfully", 3000);
    }
    catch (std::string error)
    {
        QMessageBox::warning(this, "Problem Loading", "The drive failed to load.\n\n" + QString::fromStdString(error));
    }
}

void DeviceViewer::SetWidgetsEnabled(bool enabled)
{
    ui->btnPartitions->setEnabled(enabled);
    ui->btnSecurityBlob->setEnabled(enabled);
    ui->txtDriveName->setEnabled(enabled);
    ui->txtPath->setEnabled(enabled);
    ui->treeWidget->setEnabled(enabled);
    ui->treeWidget_2->setEnabled(enabled);
    ui->lblFeeMemory->setEnabled(enabled);
    ui->lblUsedSpace->setEnabled(enabled);
    ui->imgFreeMem->setEnabled(enabled);
    ui->imgUsedMem->setEnabled(enabled);
    ui->imgPiechart->setEnabled(enabled);
    ui->btnBackup->setEnabled(enabled);
    ui->btnRestore->setEnabled(enabled);
    ui->btnShowAll->setEnabled(enabled);
    ui->txtSearch->setEnabled(enabled);
}

void DeviceViewer::on_treeWidget_doubleClicked(const QModelIndex &index)
{
    try
    {
        // get the item
        QTreeWidgetItem *item = (QTreeWidgetItem*)index.internalPointer();

        // set the current parent
        FatxFileEntry *currentParent = GetFatxFileEntry(item);

        QString type = item->data(1, Qt::UserRole).toString();
        if (type == "STFS")
        {
            FatxIO io = currentDrive->GetFatxIO(currentParent);
            StfsPackage package(&io);

            PackageViewer viewer(statusBar, &package, gpdActions, gameActions, this, false);
            viewer.exec();

            package.Close();
            io.Close();
        }
        else if (type == "SVOD")
        {
            std::string rootFilePath = currentParent->path + currentParent->name;

            SVOD *svodSystem = new SVOD(rootFilePath, currentDrive);
            SvodDialog dialog(svodSystem, statusBar, this);
            dialog.exec();
        }
        else if (type == "XEX")
        {
            FatxIO io = currentDrive->GetFatxIO(currentParent);
            Xbox360Executable *xex = new Xbox360Executable(&io);

            // the dialog will free xex
            XexDialog dialog(xex, this);
            dialog.exec();
        }

        if ((currentParent->fileAttributes & FatxDirectory) == 0)
            return;

        parentEntry = currentParent;

        LoadFolderAll(currentParent);
    }
    catch (std::string error)
    {
        QMessageBox::warning(this, "Problem Loading", "The folder failed to load.\n\n" + QString::fromStdString(error));
    }
}

FatxFileEntry* DeviceViewer::GetFatxFileEntry(QTreeWidgetItem *item)
{
    FatxFileEntry *currentParent;
    if (item->data(5, Qt::UserRole).toBool())
    {
        Partition *part = item->data(0, Qt::UserRole).value<Partition*>();
        currentParent = &part->root;
    }
    else
        currentParent = item->data(0, Qt::UserRole).value<FatxFileEntry*>();

    return currentParent;
}

void DeviceViewer::LoadFolderAll(FatxFileEntry *folder)
{
    try
    {
        if (directoryChain.at(directoryChain.size() - 1) != folder)
            directoryChain.push_back(folder);

        ui->btnBack->setEnabled(directoryChain.size() > 1);

        progressBar->setVisible(true);
        progressBar->setMaximum(0);

        ui->treeWidget->clear();
        FatxDrive *prevDrive = currentDrive;
        currentDrive = folder->partition->drive;

        // reload the header if the drive changed
        if (currentDrive != prevDrive)
        {
            DrawHeader(ui->treeWidget_2->currentItem()->parent()->text(0));
            currentDriveItem = ui->treeWidget_2->currentItem()->parent();
        }

        currentDrive->GetChildFileEntries(folder, updateUI);

        for (size_t i = 0; i < folder->cachedFiles.size(); i++)
        {
            // get the entry
            FatxFileEntry *entry = &folder->cachedFiles.at(i);

            // don't show if it's deleted
            if (entry->nameLen == FATX_ENTRY_DELETED)
                continue;

            // setup the tree widget item
            QTreeWidgetItem *entryItem = new QTreeWidgetItem(ui->treeWidget);
            entryItem->setData(0, Qt::UserRole, QVariant::fromValue(entry));
            entryItem->setData(5, Qt::UserRole, QVariant(false));

            // show the indicator if it's a directory
            if (entry->fileAttributes & FatxDirectory)
                entryItem->setIcon(0, QIcon(":/Images/FolderFileIcon.png"));
            else
            {
                QIcon fileIcon;

                // get the file magic and file system (either SVOD or STFS, both have the same magic)
                currentDrive->GetFileEntryMagic(entry);

                QtHelpers::GetFileIcon(entry->magic, QString::fromStdString(entry->name), fileIcon, *entryItem, entry->fileSystem);

                entryItem->setIcon(0, fileIcon);
                entryItem->setText(1, QString::fromStdString(ByteSizeToString(entry->fileSize)));
            }

            // setup the text
            entryItem->setText(0, QString::fromStdString(entry->name));

            MSTime createdtime = DWORDToMSTime(entry->creationDate);

            QDate date;
            date.setDate(createdtime.year, createdtime.month, createdtime.monthDay);

            entryItem->setText(2, date.toString(Qt::DefaultLocaleShortDate));

            if (i % 25 == 0)
                QApplication::processEvents();
        }

        progressBar->setVisible(false);
        progressBar->setMaximum(1);

        parentEntry = folder;

        ui->txtPath->setText(QString::fromStdString(folder->path + folder->name + "\\"));
    }
    catch (std::string error)
    {
        QMessageBox::warning(this, "Problem Loading", "The folder failed to load.\n\n" + QString::fromStdString(error));
    }
}

void DeviceViewer::LoadFolderTree(QTreeWidgetItem *item)
{
    try
    {
        // get the FatxFile entry and make sure it's a directory
        FatxFileEntry *folder = GetFatxFileEntry(item);
        if ((folder->fileAttributes & FatxDirectory) == 0)
            return;

        currentDrive->GetChildFileEntries(folder);

        for (DWORD i = 0; i < folder->cachedFiles.size(); i++)
        {
            // if it isn't a folder then don't bother loading it
            FatxFileEntry *entry = &folder->cachedFiles.at(i);
            if ((entry->fileAttributes & FatxDirectory) == 0 || entry->nameLen == FATX_ENTRY_DELETED)
                continue;

            QTreeWidgetItem *subFolder = new QTreeWidgetItem(item);
            subFolder->setIcon(0, QIcon(":/Images/FolderFileIcon.png"));
            subFolder->setText(0, QString::fromStdString(entry->name));

            subFolder->setData(0, Qt::UserRole, QVariant::fromValue(entry));
            subFolder->setData(4, Qt::UserRole, QVariant(item->data(4, Qt::UserRole).toInt() + 1));
            subFolder->setData(5, Qt::UserRole, QVariant::fromValue(false));

            subFolder->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
        }

        if (item->childCount() == 0)
            item->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
    }
    catch (std::string error)
    {
        QMessageBox::warning(this, "Problem Loading", "The folder failed to load.\n\n" + QString::fromStdString(error));
    }
}

void DeviceViewer::on_btnBack_clicked()
{
    int index = directoryChain.size() - 2;
    FatxFileEntry *entry = directoryChain.at(index);
    parentEntry = entry;

    if (entry->name == "Drive Root")
        LoadPartitions();
    else
        LoadFolderAll(entry);
    directoryChain.removeLast();
    directoryChain.removeLast();

    ui->btnBack->setEnabled(directoryChain.size() > 1);
}

void DeviceViewer::LoadPartitions()
{
    ui->treeWidget->clear();

    ui->txtPath->setText("Drive:\\");

    // load partitions
    std::vector<Partition*> parts = currentDrive->GetPartitions();
    for (size_t i = 0; i < parts.size(); i++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
        item->setData(5, Qt::UserRole, QVariant(true));
        item->setIcon(0, QIcon(":/Images/partition.png"));

        item->setText(0, QString::fromStdString(parts.at(i)->name));
        item->setText(1, QString::fromStdString(ByteSizeToString(parts.at(i)->size)));
        item->setData(0, Qt::UserRole, QVariant::fromValue(parts.at(i)));
    }

    FatxFileEntry *entry = new FatxFileEntry;
    entry->name = "Drive Root";
    directoryChain.push_back(entry);
}

void DeviceViewer::GetSubFilesFATX(FatxFileEntry *parent, QList<void *> &entries)
{
    if ((parent->fileAttributes & FatxDirectory) == 0)
    {
        entries.push_back(parent);
        return;
    }

    currentDrive->GetChildFileEntries(parent);

    for (DWORD i = 0; i < parent->cachedFiles.size(); i++)
        if (parent->cachedFiles.at(i).nameLen != FATX_ENTRY_DELETED)
            GetSubFilesFATX(&parent->cachedFiles.at(i), entries);
}

void DeviceViewer::GetSubFilesLocal(QString parent, QList<void*> &files)
{
    QDir dir(parent);
    foreach (QFileInfo file, dir.entryInfoList(QDir::Files))
        files.push_back(new QString(file.filePath()));

    QFileInfoList dirs = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs);
    for (int i = 0; i < dirs.size(); i++)
        GetSubFilesLocal(dirs.at(i).filePath(), files);
}

void DeviceViewer::on_treeWidget_2_itemExpanded(QTreeWidgetItem *item)
{
    if (!item->parent())
    {
        currentDrive = item->data(0, Qt::UserRole).value<FatxDrive*>();
        currentDriveItem = item;
        DrawHeader(item->text(0));
        LoadPartitions();
        return;
    }

    if (item->childCount() > 0)
        return;

    LoadFolderTree(item);
}

void DeviceViewer::on_treeWidget_2_itemClicked(QTreeWidgetItem *item, int column)
{
    if (!item->parent())
    {
        currentDrive = item->data(0, Qt::UserRole).value<FatxDrive*>();
        parentEntry = NULL;
        currentDriveItem = item;
        DrawHeader(item->text(0));
        LoadPartitions();
        return;
    }

    FatxFileEntry *entry = GetFatxFileEntry(item);
    LoadFolderAll(entry);
}

void DeviceViewer::on_btnSecurityBlob_clicked()
{
    if (currentDrive->GetFatxDriveType() == FatxHarddrive)
    {
        SecuritySectorDialog dialog(currentDrive, this);
        dialog.exec();
    }
    else
    {
        FlashDriveConfigDataDialog dialog(&currentDrive->configurationData, statusBar, this);
        dialog.exec();
    }
}

void DeviceViewer::on_btnPartitions_clicked()
{
    std::vector<Partition*> partitions = currentDrive->GetPartitions();

    PartitionDialog dialog(partitions, this);
    dialog.exec();
}

void DeviceViewer::onDragEntered(QDragEnterEvent *event)
{
    if (parentEntry != NULL && parentEntry->name != "Drive Root" && event->mimeData()->hasFormat("text/uri-list"))
    {
        event->acceptProposedAction();
        statusBar->showMessage("Copy file(s) here");
    }
}

void DeviceViewer::onDragDropped(QDropEvent *event)
{
#ifdef _WIN32
    int removeSize = 8;
#else
    int removeSize = 7;
#endif

    statusBar->showMessage("");

    QList<QUrl> filePaths = event->mimeData()->urls();
    if (filePaths.size() == 0)
        return;

    QString rootPath = QFileInfo(filePaths.at(0).toString().mid(removeSize)).absolutePath();

    // fix the file name to remove the "file:///" at the beginning
    QList<void*> files;
    for (int i = 0; i < filePaths.size(); i++)
    {
        QString rawPath = filePaths.at(i).toString().mid(removeSize);
        if (QFileInfo(rawPath).isDir())
            GetSubFilesLocal(rawPath, files);
        else
            files.push_back(new QString(rawPath));
    }

    InjectFiles(files, rootPath);
}

void DeviceViewer::onDragLeft(QDragLeaveEvent *event)
{
    statusBar->showMessage("");
}

void DeviceViewer::on_txtPath_returnPressed()
{
    FatxFileEntry *parent = currentDrive->GetFileEntry(ui->txtPath->text().toStdString());
    if (parent == NULL || !(parent->fileAttributes & FatxDirectory))
        QMessageBox::critical(this, "Error", "Velocity can't find " + ui->txtPath->text() + ". Check the spelling and try again.");
    else
        LoadFolderAll(parent);
}

void updateUIDelete(void *arg)
{
    DeviceViewer *viewer = reinterpret_cast<DeviceViewer*>(arg);
    viewer->DrawMemoryGraph();

    QApplication::processEvents();
}

void DeviceViewer::on_btnBackup_clicked()
{
    QString savePath = QFileDialog::getSaveFileName(this, "Create a backup for your device", QtHelpers::DesktopLocation() + "/Drive Backup.bin");

    if (savePath == "")
        return;

    SingleProgressDialog *dialog = new SingleProgressDialog(FileSystemFATX, currentDrive, OpBackup, "", savePath, NULL, this);
    dialog->setModal(true);
    dialog->show();
    dialog->start();
}

void DeviceViewer::on_btnRestore_clicked()
{
    QString openPath = QFileDialog::getOpenFileName(this, "Choose a backup to restore from", QtHelpers::DesktopLocation() + "/Drive Backup.bin");

    if (openPath == "")
        return;

    SingleProgressDialog *dialog = new SingleProgressDialog(FileSystemFATX, currentDrive, OpRestore, "", openPath, NULL, this);
    dialog->setModal(true);
    dialog->show();
    dialog->start();

    /////////////////////////////////////////////
    // reload the drive information in the GUI //
    /////////////////////////////////////////////

    // reload the drive name
    FatxFileEntry *nameEntry = currentDrive->GetFileEntry("Drive:\\Content\\name.txt");
    QString name = (currentDrive->GetFatxDriveType() == FatxHarddrive) ? "Hard Drive" : "Flash Drive";
    if (nameEntry)
    {
        FatxIO nameFile = currentDrive->GetFatxIO(nameEntry);
        nameFile.SetPosition(0);

        // make sure that it starts with 0xFEFF
        if (nameFile.ReadWord() == 0xFEFF)
            name = QString::fromStdWString(nameFile.ReadWString((nameEntry->fileSize > 0x36) ? 26 : (nameEntry->fileSize - 2) / 2));
    }
    currentDriveItem->setText(0, name);
    DrawHeader(name);
    directoryChain.clear();
    LoadPartitions();

    statusBar->showMessage("Successfully reloaded storage device", 3000);
}

void DeviceViewer::on_txtSearch_textChanged(const QString &arg1)
{
    QtHelpers::SearchTreeWidget(ui->treeWidget, ui->txtSearch, arg1);
}

void DeviceViewer::on_btnShowAll_clicked()
{
    ui->txtSearch->setText("");
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); i++)
        ui->treeWidget->topLevelItem(i)->setHidden(false);
    ui->txtSearch->setStyleSheet("");
}

void DeviceViewer::on_txtDriveName_editingFinished()
{
    QString newName = ui->txtDriveName->text();

    // no need to update it again
    if (previousName == newName)
        return;

    try
    {
        currentDrive->SetDriveName(newName.toStdWString());
        currentDriveItem->setText(0, newName);
        previousName = newName;
    }
    catch (std::string error)
    {
        // restore the original name
        ui->txtDriveName->setText(previousName);
        QMessageBox::warning(this, "Problem Renaming", "The drive could not be renamed.\n\n" + QString::fromStdString(error));
    }
}

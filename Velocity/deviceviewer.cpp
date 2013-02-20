#include "deviceviewer.h"
#include "ui_deviceviewer.h"

DeviceViewer::DeviceViewer(QWidget *parent) :
    QDialog(parent), ui(new Ui::DeviceViewer), currentIndex(-1)
{
    ui->setupUi(this);
    currentDrive = NULL;

    ui->treeWidget->header()->setDefaultSectionSize(100);
    ui->treeWidget->header()->resizeSection(0, 250);

    // setup the context menus
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showRemoveContextMenu(QPoint)));
}

DeviceViewer::~DeviceViewer()
{
    if (currentDrive)
        delete currentDrive;

    delete ui;
}

void DeviceViewer::on_pushButton_clicked()
{
    // clear all the items
    ui->treeWidget->clear();

    if (currentDrive)
        currentDrive->Close();

    try
    {
        // open the drive
        currentDrive = new FatxDrive(ui->txtPath->text().toStdWString());

        // load the partion information
        std::vector<Partition*> parts = currentDrive->GetPartitions();
        for (DWORD i = 0; i < parts.size(); i++)
        {
            QTreeWidgetItem *secondItem = new QTreeWidgetItem(ui->treeWidget_2);
            secondItem->setText(0, QString::fromStdString(parts.at(i)->name));
            secondItem->setIcon(0, QIcon(":/Images/partition.png"));
            secondItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
            secondItem->setData(0, Qt::UserRole, QVariant::fromValue(parts.at(i)));
            secondItem->setData(5, Qt::UserRole, QVariant::fromValue(true));
            secondItem->setData(4, Qt::UserRole, QVariant::fromValue(-1));
        }
        ui->btnPartitions->setEnabled(true);
        ui->btnSecurityBlob->setEnabled(true);
    }
    catch (std::string error)
    {
        QMessageBox::warning(this, "Problem Loading", "The drive failed to load.\n\n" + QString::fromStdString(error));
    }

    LoadPartitions();
}

void DeviceViewer::showRemoveContextMenu(QPoint point)
{
    QPoint globalPos = ui->treeWidget->mapToGlobal(point);
    QMenu contextMenu;

    QList<QTreeWidgetItem*> items = ui->treeWidget->selectedItems();
    if (items.size() < 1)
        return;

    foreach (QTreeWidgetItem *item, items)
        if (item->data(5, Qt::UserRole).toBool())
            return;

    contextMenu.addAction(QPixmap(":/Images/extract.png"), "Copy Selected to Local Disk");
    contextMenu.addAction(QPixmap(":/Images/properties.png"), "View Properties");

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
                    GetSubFiles(entry, filesToExtract);
                else
                    filesToExtract.push_back(entry);
            }

            // get the save path
            QString path = QFileDialog::getExistingDirectory(this, "Save Location", QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));

            if (path.isEmpty())
                return;

            // save the file to the local disk
            MultiProgressDialog *dialog = new MultiProgressDialog(FileSystemFATX, currentDrive, path + "/", filesToExtract, this, ui->txtPath->text());
            dialog->setModal(true);
            dialog->show();
            dialog->start();
        }
        else if (selectedItem->text() == "View Properties")
        {
            /*FatxFileEntry *parent = items.at(0)->data(0, Qt::UserRole).value<FatxFileEntry*>();
            FatxFileEntry child;
            child.name = "hetelek";
            child.creationDate = 0x4252B24A;
            child.lastWriteDate = 0x4252B24A;
            child.lastAccessDate = 0x4252B24A;
            child.fileAttributes = 0;
            child.fileSize = 0x43;
            currentDrive->CreateFileEntry(parent, &child);*/

            FatxFileEntry *entry = items.at(0)->data(0, Qt::UserRole).value<FatxFileEntry*>();
            FatxFileDialog dialog(entry, entry->partition->clusterSize, items.at(0)->data(1, Qt::UserRole).toString(), this);
            dialog.exec();
        }
    }
    catch (std::string error)
    {
        QMessageBox::warning(this, "Problem Extracting", "The file failed to extract.\n\n" + QString::fromStdString(error));
    }
}

void DeviceViewer::on_treeWidget_doubleClicked(const QModelIndex &index)
{
    try
    {
        // get the item
        QTreeWidgetItem *item = (QTreeWidgetItem*)index.internalPointer();

        // set the current parent
        FatxFileEntry *currentParent = GetFatxFileEntry(item);

        if ((currentParent->fileAttributes & FatxDirectory) == 0)
            return;

        currentIndex++;
        LoadFolderAll(currentParent);
        ui->btnBack->setEnabled(currentIndex >= 0);
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
        ui->treeWidget->clear();
        currentDrive->GetChildFileEntries(folder);

        for (int i = 0; i < folder->cachedFiles.size(); i++)
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

                currentDrive->GetFileEntryMagic(entry);

                QtHelpers::GetFileIcon(entry->magic, QString::fromStdString(entry->name), fileIcon, *entryItem);

                entryItem->setIcon(0, fileIcon);
                entryItem->setText(1, QString::fromStdString(ByteSizeToString(entry->fileSize)));
            }

            // setup the text
            entryItem->setText(0, QString::fromStdString(entry->name));

            MSTime createdtime = DWORDToMSTime(entry->creationDate);

            QDate date;
            date.setDate(createdtime.year, createdtime.month, createdtime.monthDay);

            entryItem->setText(2, date.toString(Qt::DefaultLocaleShortDate));
        }

        if (currentIndex == directoryChain.size())
            directoryChain.append(folder);
        else
            directoryChain[currentIndex] = folder;

        QString path = "Drive:\\" + QString::fromStdString(directoryChain.at(0)->partition->name) + "\\";
        for (DWORD i = 1; i <= currentIndex; i++)
            path += QString::fromStdString(directoryChain.at(i)->name) + "\\";
        ui->txtPath->setText(path);
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
            if ((entry->fileAttributes & FatxDirectory) == 0)
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
    if (currentIndex == 0)
        LoadPartitions();
    else
        LoadFolderAll(directoryChain.at(--currentIndex));

    ui->btnBack->setEnabled(currentIndex >= 0);
}

void DeviceViewer::LoadPartitions()
{
    ui->treeWidget->clear();

    ui->txtPath->setText("Drive:\\");

    // load partitions
    std::vector<Partition*> parts = currentDrive->GetPartitions();
    for (int i = 0; i < parts.size(); i++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
        item->setData(5, Qt::UserRole, QVariant(true));
        item->setIcon(0, QIcon(":/Images/partition.png"));

        item->setText(0, QString::fromStdString(parts.at(i)->name));
        item->setText(1, QString::fromStdString(ByteSizeToString(parts.at(i)->size)));
        item->setData(0, Qt::UserRole, QVariant::fromValue(parts.at(i)));
    }

    currentIndex = -1;
}

void DeviceViewer::GetSubFiles(FatxFileEntry *parent, QList<void *> &entries)
{
    if ((parent->fileAttributes & FatxDirectory) == 0)
    {
        entries.push_back(parent);
        return;
    }

    currentDrive->GetChildFileEntries(parent);

    for (DWORD i = 0; i < parent->cachedFiles.size(); i++)
        GetSubFiles(&parent->cachedFiles.at(i), entries);
}

void DeviceViewer::on_treeWidget_2_itemExpanded(QTreeWidgetItem *item)
{
    LoadFolderTree(item);
}

void DeviceViewer::on_treeWidget_2_itemClicked(QTreeWidgetItem *item, int column)
{
    currentIndex = item->data(4, Qt::UserRole).toInt() + 1;
    FixDirectoryChain(item->parent(), currentIndex);

    FatxFileEntry *entry = GetFatxFileEntry(item);
    LoadFolderAll(entry);
    ui->btnBack->setEnabled(currentIndex >= 0);
}

void DeviceViewer::FixDirectoryChain(QTreeWidgetItem *currentItem, int index)
{
    if (index == -1 || currentItem == NULL)
        return;

    FatxFileEntry *entry = GetFatxFileEntry(currentItem);
    FixDirectoryChain(currentItem->parent(), index - 1);

    if (index >= directoryChain.size())
        directoryChain.append(entry);
    else
        directoryChain[index] = entry;
}

void DeviceViewer::on_btnSecurityBlob_clicked()
{
    SecuritySectorDialog dialog(currentDrive, this);
    dialog.exec();
}

void DeviceViewer::on_btnPartitions_clicked()
{
    PartitionDialog dialog(currentDrive->GetPartitions(), this);
    dialog.exec();
}

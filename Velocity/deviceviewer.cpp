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

        // load the security blob
        ui->lblFirmwareRevision->setText(QString::fromStdString(currentDrive->securityBlob.firmwareRevision).trimmed());
        ui->lblModelNumber->setText(QString::fromStdString(currentDrive->securityBlob.modelNumber).trimmed());
        ui->lblSerialNumber->setText(QString::fromStdString(currentDrive->securityBlob.serialNumber).trimmed());
        ui->lblSectors->setText("0x" + QString::number(currentDrive->securityBlob.userAddressableSectors, 16).toUpper());
        ui->lblValidSignature->setText(((currentDrive->securityBlob.validSignature) ? "Yes" : "No"));

        // load the partion information
        std::vector<Partition*> parts = currentDrive->GetPartitions();
        for (DWORD i = 0; i < parts.size(); i++)
            ui->comboBox->addItem(QString::fromStdString(parts.at(i)->name));

        ui->comboBox->setCurrentIndex(0);
        ui->comboBox->setEnabled(true);
        ui->btnClusterTool->setEnabled(true);
        ui->btnExtractSecuritySector->setEnabled(true);
        ui->btnReplaceSecuritySector->setEnabled(true);
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

    bool hasFolders = false, hasFiles = false, hasPartitions = false;

    QList<QTreeWidgetItem*> items = ui->treeWidget->selectedItems();
    if (items.size() < 1)
        return;

    for (int i = 0; i < items.count(); i++)
    {
        if (items.at(i)->text(3).isEmpty())
            hasPartitions = true;
        else if (items.at(i)->text(1).isEmpty())
            hasFolders = true;
        else
            hasFiles = true;
    }

    if (hasPartitions)
        return;

    if (hasFiles && !hasFolders)
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
                filesToExtract.push_back(items.at(i)->data(0, Qt::UserRole).value<FatxFileEntry*>());

            // get the save path
            QString path = QFileDialog::getExistingDirectory(this, "Save Location", QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));

            if (path.isEmpty())
                return;

            // save the file to the local disk
            MultiProgressDialog *dialog = new MultiProgressDialog(FileSystemFATX, currentDrive, path + "/", filesToExtract, this);
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
            FatxFileDialog dialog(entry, entry->partition->clusterSize, items.at(0)->data(1, Qt::UserRole).toString(), ui->txtPath->text(), this);
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

        // remove all current child items
        qDeleteAll(item->takeChildren());

        // set the current parent
        FatxFileEntry *currentParent;
        if (item->data(5, Qt::UserRole).toBool())
        {
            Partition *part = item->data(0, Qt::UserRole).value<Partition*>();
            currentParent = &part->root;
        }
        else
            currentParent = item->data(0, Qt::UserRole).value<FatxFileEntry*>();

        if ((currentParent->fileAttributes & FatxDirectory) == 0)
            return;

        currentIndex++;
        LoadFolder(currentParent);
        ui->btnBack->setEnabled(currentIndex >= 0);
    }
    catch (std::string error)
    {
        QMessageBox::warning(this, "Problem Loading", "The folder failed to load.\n\n" + QString::fromStdString(error));
    }
}

void DeviceViewer::LoadFolder(FatxFileEntry *folder)
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

                DWORD magic = 0;

                if (entry->fileSize >= 0x4)
                {
                    FatxIO io = currentDrive->GetFatxIO(entry);
                    magic = io.ReadDword();
                }

                QtHelpers::GetFileIcon(magic, QString::fromStdString(entry->name), fileIcon, *entryItem);

                entryItem->setIcon(0, fileIcon);
                entryItem->setText(1, QString::fromStdString(ByteSizeToString(entry->fileSize)));
            }

            // setup the text
            entryItem->setText(0, QString::fromStdString(entry->name));
            entryItem->setText(2, "0x" + QString::number(entry->startingCluster, 16).toUpper());

            MSTime createdtime = DWORDToMSTime(entry->creationDate);

            QDate date;
            date.setDate(createdtime.year, createdtime.month, createdtime.monthDay);

            entryItem->setText(3, date.toString(Qt::DefaultLocaleShortDate));
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
void DeviceViewer::on_btnBack_clicked()
{
    if (currentIndex == 0)
        LoadPartitions();
    else
        LoadFolder(directoryChain.at(--currentIndex));

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

void DeviceViewer::on_comboBox_currentIndexChanged(int index)
{
    if (index < 0)
        return;

    std::vector<Partition*> parts = currentDrive->GetPartitions();
    Partition* part = parts.at(index);
    ui->lblPartID->setText("0x" + QString::number(part->partitionId, 16).toUpper());
    ui->lblPartSize->setText(QString::fromStdString(ByteSizeToString(part->size)));
    ui->lblPartClusterSize->setText("0x" + QString::number(part->clusterSize, 16).toUpper());
    ui->lblPartFirstClusterAddr->setText("0x" + QString::number(part->clusterStartingAddress, 16).toUpper());
    ui->lblPartSectorsPerCluster->setText(QString::number(part->sectorsPerCluster));
    ui->lblPartRootDirCluster->setText("0x" + QString::number(part->rootDirectoryCluster, 16).toUpper());
}

void DeviceViewer::on_btnClusterTool_clicked()
{
    ClusterToolDialog dialog(currentDrive->GetPartitions().at(ui->comboBox->currentIndex()), this);
    dialog.exec();
}

void DeviceViewer::on_btnExtractSecuritySector_clicked()
{
    QString savePath = QFileDialog::getSaveFileName(this, "Choose a place to save the security blob...", QtHelpers::DesktopLocation() + "/Security Blob.bin");
    if (savePath == "")
        return;

    try
    {
        currentDrive->ExtractSecurityBlob(savePath.toStdString());
        QMessageBox::information(this, "Success", "Successfully saved the security blob.");
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Error", "An error occurred while extracting the security blob.\n\n" + QString::fromStdString(error));
    }
}

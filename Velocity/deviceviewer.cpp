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
        currentDrive = new FatxDrive(ui->lineEdit->text().toStdWString());

        // load the device information
        ui->lblFirmwareRevision->setText(QString::fromStdString(currentDrive->securityBlob.firmwareRevision).trimmed());
        ui->lblModelNumber->setText(QString::fromStdString(currentDrive->securityBlob.modelNumber).trimmed());
        ui->lblSerialNumber->setText(QString::fromStdString(currentDrive->securityBlob.serialNumber).trimmed());
        ui->lblSectors->setText("0x" + QString::number(currentDrive->securityBlob.userAddressableSectors, 16).toUpper());
        ui->lblValidSignature->setText(((currentDrive->securityBlob.validSignature) ? "Yes" : "No"));
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

    contextMenu.addAction(QPixmap(":/Images/extract.png"), "Copy Selected to Local Disk");

    QAction *selectedItem = contextMenu.exec(globalPos);
    if(selectedItem == NULL)
        return;

    QList <QTreeWidgetItem*> items = ui->treeWidget->selectedItems();

    try
    {
        if (selectedItem->text() == "Copy Selected to Local Disk")
        {
            if (items.size() < 1)
                return;

            // get the entry
            FatxFileEntry *entry = items.at(0)->data(0, Qt::UserRole).value<FatxFileEntry*>();

            // get the save path
            QString path = QFileDialog::getSaveFileName(this, "Save Location", QDesktopServices::storageLocation(QDesktopServices::DesktopLocation) + "/" + QString::fromStdString(entry->name));

            if (path.isEmpty())
                return;

            // save the entry to disk
            FatxIO io = currentDrive->GetFatxIO(entry);
            io.SaveFile(path.toStdString());

            QMessageBox::information(this, "Success", "Successfully copied the file(s).\n");
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
                FatxIO io = currentDrive->GetFatxIO(entry);
                io.SetPosition(0);
                QtHelpers::GetFileIcon(io.ReadDword(), QString::fromStdString(entry->name), fileIcon, *entryItem);

                entryItem->setIcon(0, fileIcon);
            }

            // setup the text
            entryItem->setText(0, QString::fromStdString(entry->name));
            entryItem->setText(1, QString::fromStdString(ByteSizeToString(entry->fileSize)));
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

    // load partitions
    std::vector<Partition*> parts = currentDrive->GetPartitions();
    for (int i = 0; i < parts.size(); i++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
        item->setData(5, Qt::UserRole, QVariant(true));
        item->setIcon(0, QIcon(":/Images/partition.png"));

        item->setText(0, QString::fromStdString(parts.at(i)->name));
        item->setData(0, Qt::UserRole, QVariant::fromValue(parts.at(i)));
    }

    currentIndex = -1;
}

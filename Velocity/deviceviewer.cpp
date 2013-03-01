#include "deviceviewer.h"
#include "ui_deviceviewer.h"

#include <QDebug>

DeviceViewer::DeviceViewer(QWidget *parent) :
    QDialog(parent), ui(new Ui::DeviceViewer)
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
        UINT64 totalFreeSpace = 0;
        UINT64 totalSpace = 0;

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

            totalFreeSpace += currentDrive->GetFreeMemory(parts.at(i));
            totalSpace += (UINT64)parts.at(i)->clusterCount * parts.at(i)->clusterSize;
        }

        // calculate the percentage
        float freeMemPercentage = (((float)totalFreeSpace * 100.0) / totalSpace);

        // draw the insano piechart
        QPixmap chart(750, 500);
        chart.fill(ui->imgPiechart->palette().background().color());
        QPainter painter(&chart);
        Nightcharts pieChart;
        pieChart.setType(Nightcharts::Dpie);
        pieChart.setCords(25, 1, 700, 425);
        pieChart.setFont(QFont());
        pieChart.addPiece("Used Space", QColor(0, 0, 254), 100.0 - freeMemPercentage);
        pieChart.addPiece("Free Space", QColor(255, 0, 254), freeMemPercentage);
        pieChart.draw(&painter);

        ui->imgPiechart->setPixmap(chart);

        // setup the legend
        QPixmap freeMemClr(16, 16);
        freeMemClr.fill(QColor(255, 0, 254));
        ui->imgFreeMem->setPixmap(freeMemClr);
        ui->lblFeeMemory->setText(QString::fromStdString(ByteSizeToString(totalFreeSpace)) + " of Free Space");

        QPixmap usedMemClr(16, 16);
        usedMemClr.fill(QColor(0, 0, 254));
        ui->imgUsedMem->setPixmap(usedMemClr);
        ui->lblUsedSpace->setText(QString::fromStdString(ByteSizeToString(totalSpace - totalFreeSpace)) + " of Used Space");

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
    contextMenu.addAction(QPixmap(":/Images/add.png"), "Copy File(s) Here");

    contextMenu.addSeparator();
    contextMenu.addAction(QPixmap(":/Images/delete.png"), "Delete Selected");
    contextMenu.addSeparator();

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
            MultiProgressDialog *dialog = new MultiProgressDialog(FileSystemFATX, currentDrive, path + "/", filesToExtract, this, QString::fromStdString(directoryChain.last()->path + directoryChain.last()->name + "\\"));
            dialog->setModal(true);
            dialog->show();
            dialog->start();
        }
        else if (selectedItem->text() == "View Properties")
        {
            FatxFileEntry *entry = items.at(0)->data(0, Qt::UserRole).value<FatxFileEntry*>();
            FatxFileDialog dialog(entry, entry->partition->clusterSize, items.at(0)->data(1, Qt::UserRole).toString(), this);
            dialog.exec();
        }
        else if (selectedItem->text() == "Copy File(s) Here")
        {
            FatxFileEntry *entry = items.at(0)->data(0, Qt::UserRole).value<FatxFileEntry*>();

            QStringList toInjectPath = QFileDialog::getOpenFileNames(this);
            if (toInjectPath.size() == 0)
                return;

            for (int i = 0; i < toInjectPath.size(); i++)
            {
                QString filePath = toInjectPath.at(i);
                QFileInfo info(filePath);

                // save the file to the local disk
                SingleProgressDialog *dialog = new SingleProgressDialog(FileSystemFATX, currentDrive, OpInject, info.fileName(), filePath, entry, this);
                dialog->setModal(true);
                dialog->show();
                dialog->start();
            }

            QMessageBox::information(this, "Copied Files", "All files have been successfully copied to the harddrive.");
        }
        else if (selectedItem->text() == "Delete Selected")
        {
            // delete the entries
            for (int i = 0; i < items.size(); i++)
            {
                FatxFileEntry *entry = items.at(i)->data(0, Qt::UserRole).value<FatxFileEntry*>();
                currentDrive->DeleteFile(entry);

                delete items.at(i);
            }
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
    for (int i = 0; i < parts.size(); i++)
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

void DeviceViewer::GetSubFiles(FatxFileEntry *parent, QList<void *> &entries)
{
    if ((parent->fileAttributes & FatxDirectory) == 0)
    {
        entries.push_back(parent);
        return;
    }

    currentDrive->GetChildFileEntries(parent);

    for (DWORD i = 0; i < parent->cachedFiles.size(); i++)
        if (parent->cachedFiles.at(i).nameLen != FATX_ENTRY_DELETED)
            GetSubFiles(&parent->cachedFiles.at(i), entries);
}

void DeviceViewer::on_treeWidget_2_itemExpanded(QTreeWidgetItem *item)
{
    if (item->childCount() > 0)
        return;

    LoadFolderTree(item);
}

void DeviceViewer::on_treeWidget_2_itemClicked(QTreeWidgetItem *item, int column)
{
    FatxFileEntry *entry = GetFatxFileEntry(item);
    LoadFolderAll(entry);
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


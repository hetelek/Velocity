#include "deviceviewer.h"
#include "ui_deviceviewer.h"

DeviceViewer::DeviceViewer(QWidget *parent) : QDialog(parent), ui(new Ui::DeviceViewer)
{
    ui->setupUi(this);
    currentDrive = NULL;

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
    }
    catch (std::string error)
    {
        QMessageBox::warning(this, "Problem Loading", "The drive failed to load.\n\n" + QString::fromStdString(error));
    }

    // load partitions
    std::vector<Partition*> parts = currentDrive->GetPartitions();
    for (int i = 0; i < parts.size(); i++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
        item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
        item->setIcon(0, QIcon(":/Images/partition.png"));

        item->setText(0, QString::fromStdString(parts.at(i)->name));
        item->setData(0, Qt::UserRole, QVariant::fromValue(parts.at(i)));
    }
}

void DeviceViewer::on_treeWidget_expanded(const QModelIndex &index)
{
    try
    {
        // get the item
        QTreeWidgetItem *item = (QTreeWidgetItem*)index.internalPointer();

        // remove all current child items
        qDeleteAll(item->takeChildren());

        // set the current parent
        FatxFileEntry *currentParent;
        if (!item->parent())
        {
            Partition *part = item->data(0, Qt::UserRole).value<Partition*>();
            currentParent = &part->root;
        }
        else
            currentParent = item->data(0, Qt::UserRole).value<FatxFileEntry*>();

        currentDrive->GetChildFileEntries(currentParent);

        for (int i = 0; i < currentParent->cachedFiles.size(); i++)
        {
            // get the entry
            FatxFileEntry *entry = &currentParent->cachedFiles.at(i);

            // don't show if it's deleted
            if (entry->nameLen == FATX_ENTRY_DELETED)
                continue;

            // setup the tree widget item
            QTreeWidgetItem *entryItem = new QTreeWidgetItem(item);
            entryItem->setData(0, Qt::UserRole, QVariant::fromValue(entry));

            // show the indicator if it's a directory
            if (entry->fileAttributes & FatxDirectory)
            {
                entryItem->setIcon(0, QIcon(":/Images/FolderFileIcon.png"));
                entryItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
            }
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
            entryItem->setText(2, "0x" + QString::number(entry->startingCluster, 16));

            // add it to the tree widget
            item->addChild(entryItem);
        }
    }
    catch (std::string error)
    {
        QMessageBox::warning(this, "Problem Loading", "The folder failed to load.\n\n" + QString::fromStdString(error));
    }
}

void DeviceViewer::showRemoveContextMenu(QPoint point)
{
    QPoint globalPos = ui->treeWidget->mapToGlobal(point);
    QMenu contextMenu;

    contextMenu.addAction(QPixmap(":/Images/extract.png"), "Extract Selected");

    QAction *selectedItem = contextMenu.exec(globalPos);
    if(selectedItem == NULL)
        return;

    QList <QTreeWidgetItem*> items = ui->treeWidget->selectedItems();

    try
    {
        if (selectedItem->text() == "Extract Selected")
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
        }
    }
    catch (std::string error)
    {
        QMessageBox::warning(this, "Problem Extracting", "The file failed to extract.\n\n" + QString::fromStdString(error));
    }
}

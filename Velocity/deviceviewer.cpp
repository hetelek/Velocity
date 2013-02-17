#include "deviceviewer.h"
#include "ui_deviceviewer.h"

DeviceViewer::DeviceViewer(QWidget *parent) : QDialog(parent), ui(new Ui::DeviceViewer)
{
    ui->setupUi(this);
    currentDrive = NULL;
}

DeviceViewer::~DeviceViewer()
{
    if (currentDrive)
        delete currentDrive;

    delete ui;
}

void DeviceViewer::on_pushButton_clicked()
{
    currentDrive = new FatxDrive(ui->lineEdit->text().toStdWString());

    std::vector<Partition*> parts = currentDrive->GetPartitions();

    // load partitions
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
    QTreeWidgetItem *item = (QTreeWidgetItem*)index.internalPointer();

    qDeleteAll(item->takeChildren());

    FatxFileEntry *currentParent;
    if (!item->parent())
    {
        qDebug() << "loading partition";

        Partition *part = item->data(0, Qt::UserRole).value<Partition*>();
        currentParent = &part->root;
    }
    else
    {
        currentParent = item->data(0, Qt::UserRole).value<FatxFileEntry*>();
    }

    currentDrive->GetChildFileEntries(currentParent);

    for (int i = 0; i < currentParent->cachedFiles.size(); i++)
    {
        FatxFileEntry *entry = &currentParent->cachedFiles.at(i);

        if (entry->nameLen == FATX_ENTRY_DELETED)
            continue;

        QTreeWidgetItem *entryItem = new QTreeWidgetItem(item);
        entryItem->setData(0, Qt::UserRole, QVariant::fromValue(entry));

        if (entry->fileAttributes & FatxDirectory)
        {
            entryItem->setIcon(0, QIcon(":/Images/FolderFileIcon.png"));
            entryItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
        }
        else
            entryItem->setIcon(0, QIcon(":/Images/DefaultFileIcon.png"));

        entryItem->setText(0, QString::fromStdString(entry->name));
        entryItem->setText(1, QString::fromStdString(ByteSizeToString(entry->fileSize)));
        entryItem->setText(2, "0x" + QString::number(entry->startingCluster, 16));

        item->addChild(entryItem);
    }
}

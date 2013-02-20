#include "partitiondialog.h"
#include "ui_partitiondialog.h"

PartitionDialog::PartitionDialog(std::vector<Partition*> partitions, QWidget *parent) :
    QDialog(parent), ui(new Ui::PartitionDialog), partitions(partitions)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    for (DWORD i = 0; i < partitions.size(); i++)
        ui->comboBox->addItem(QString::fromStdString(partitions.at(i)->name));

    on_comboBox_currentIndexChanged(0);
}

PartitionDialog::~PartitionDialog()
{
    delete ui;
}

void PartitionDialog::on_comboBox_currentIndexChanged(int index)
{
    Partition *part = partitions.at(index);
    ui->lblPartID->setText("0x" + QString::number(part->partitionId, 16).toUpper());
    ui->lblPartSize->setText(QString::fromStdString(ByteSizeToString(part->size)));
    ui->lblPartClusterSize->setText("0x" + QString::number(part->clusterSize, 16).toUpper());
    ui->lblPartFirstClusterAddr->setText("0x" + QString::number(part->clusterStartingAddress, 16).toUpper());
    ui->lblPartSectorsPerCluster->setText(QString::number(part->sectorsPerCluster));
    ui->lblPartRootDirCluster->setText("0x" + QString::number(part->rootDirectoryCluster, 16).toUpper());
}

void PartitionDialog::on_btnClusterTool_clicked()
{
    Partition *part = partitions.at(ui->comboBox->currentIndex());
    ClusterToolDialog dialog(part, this);
    dialog.exec();
}

#include "partitiondialog.h"
#include "ui_partitiondialog.h"

PartitionDialog::PartitionDialog(std::vector<Partition*> &partitions, QWidget *parent) :
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

    // calculate the percentage
    float freeMemPercentage = (part->freeMemory * 100.0) / ((UINT64)part->clusterCount * part->clusterSize);

    // draw the chart
    QPixmap chart(391, 221);
    chart.fill(ui->imgPiechart->palette().background().color());
    QPainter painter(&chart);
    Nightcharts pieChart;
    pieChart.setType(Nightcharts::Dpie);
    pieChart.setCords(10, 10, 200, 150);
    pieChart.setFont(QFont("Arial", 48));
    pieChart.addPiece("Used Space", QColor(0, 0, 254), 100.0 - freeMemPercentage);
    pieChart.addPiece("Free Space", QColor(255, 0, 254), freeMemPercentage);
    pieChart.setLegendCords(235, 50);
    pieChart.setLegendType(Nightcharts::Vertical);
    pieChart.draw(&painter);
    pieChart.drawLegend(&painter);

    chart.copy();

    ui->imgPiechart->setPixmap(chart);
}

void PartitionDialog::on_btnClusterTool_clicked()
{
    Partition part = *partitions.at(ui->comboBox->currentIndex());
    ClusterToolDialog dialog(part, this);
    dialog.exec();
}

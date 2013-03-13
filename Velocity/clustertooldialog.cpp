#include "clustertooldialog.h"
#include "ui_clustertooldialog.h"

ClusterToolDialog::ClusterToolDialog(Partition &partition, QWidget *parent) :
    QDialog(parent), ui(new Ui::ClusterToolDialog), part(partition)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    ui->spinBox->setMaximum(partition.clusterCount - 1);
    on_spinBox_valueChanged(1);
}

ClusterToolDialog::~ClusterToolDialog()
{
    delete ui;
}

void ClusterToolDialog::on_spinBox_valueChanged(int cluster)
{
    ui->lblAddress->setText("0x" + QString::number(FatxIO::ClusterToOffset(&part, cluster), 16).toUpper());
}

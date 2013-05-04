#ifndef PARTITIONDIALOG_H
#define PARTITIONDIALOG_H

// qt
#include <QDialog>

// dialogs
#include "clustertooldialog.h"

// xbox
#include "Stfs/StfsDefinitions.h"
#include "Fatx/FatxConstants.h"

// other
#include "nightcharts.h"

namespace Ui {
class PartitionDialog;
}

class PartitionDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PartitionDialog(std::vector<Partition*> &partitions, QWidget *parent = 0);
    ~PartitionDialog();
    
private slots:
    void on_comboBox_currentIndexChanged(int index);

    void on_btnClusterTool_clicked();

private:
    Ui::PartitionDialog *ui;
    std::vector<Partition*> &partitions;

};

#endif // PARTITIONDIALOG_H

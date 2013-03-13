#ifndef CLUSTERTOOLDIALOG_H
#define CLUSTERTOOLDIALOG_H

// qt
#include <QDialog>

#include "Fatx/FatxConstants.h"
#include "IO/FatxIO.h"

namespace Ui {
class ClusterToolDialog;
}

class ClusterToolDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ClusterToolDialog(Partition &partition, QWidget *parent = 0);
    ~ClusterToolDialog();
    
private slots:
    void on_spinBox_valueChanged(int cluster);

private:
    Ui::ClusterToolDialog *ui;
    Partition &part;
};

#endif // CLUSTERTOOLDIALOG_H

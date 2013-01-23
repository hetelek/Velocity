#ifndef VOLUMEDESCRIPTORDIALOG_H
#define VOLUMEDESCRIPTORDIALOG_H

// qt
#include <QDialog>
#include <QComboBox>
#include <QMessageBox>
#include <QStatusBar>
#include "qthelpers.h"

// xbox360
#include "Stfs/StfsDefinitions.h"

namespace Ui {
class StfsVolumeDescriptorDialog;
}

class StfsVolumeDescriptorDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit StfsVolumeDescriptorDialog(QStatusBar *statusBar, StfsVolumeDescriptor *desc, QWidget *parent = 0);
    ~StfsVolumeDescriptorDialog();
    
private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void packageTypeChanged(int index);

private:
    Ui::StfsVolumeDescriptorDialog *ui;
    StfsVolumeDescriptor *desc;
    QStatusBar *statusBar;

    QComboBox *cmbxHashTable;
    QComboBox *cmbxPackageType;
};

#endif // VOLUMEDESCRIPTORDIALOG_H

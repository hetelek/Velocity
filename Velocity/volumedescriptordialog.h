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
class VolumeDescriptorDialog;
}

class VolumeDescriptorDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit VolumeDescriptorDialog(QStatusBar *statusBar, VolumeDescriptor *desc, QWidget *parent = 0);
    ~VolumeDescriptorDialog();
    
private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void packageTypeChanged(int index);

private:
    Ui::VolumeDescriptorDialog *ui;
    VolumeDescriptor *desc;
    QStatusBar *statusBar;

    QComboBox *cmbxHashTable;
    QComboBox *cmbxPackageType;
};

#endif // VOLUMEDESCRIPTORDIALOG_H

#ifndef VOLUMEDESCRIPTORDIALOG_H
#define VOLUMEDESCRIPTORDIALOG_H

// qt
#include <QDialog>
#include <QComboBox>
#include <QMessageBox>
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
    explicit VolumeDescriptorDialog(VolumeDescriptor *desc, QWidget *parent = 0);
    ~VolumeDescriptorDialog();
    
private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void packageTypeChanged(int index);

private:
    Ui::VolumeDescriptorDialog *ui;
    VolumeDescriptor *desc;

    QComboBox *cmbxHashTable;
    QComboBox *cmbxPackageType;
};

#endif // VOLUMEDESCRIPTORDIALOG_H

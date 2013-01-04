#ifndef SVODVOLUMEDESCRIPTORDIALOG_H
#define SVODVOLUMEDESCRIPTORDIALOG_H

// qt
#include <QDialog>
#include <QStatusBar>
#include <QMessageBox>
#include "qthelpers.h"

// xbox
#include "Stfs/XContentHeader.h"

namespace Ui {
class SvodVolumeDescriptorDialog;
}

class SvodVolumeDescriptorDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SvodVolumeDescriptorDialog(QStatusBar *statusBar, SvodVolumeDescriptor *descriptor, QWidget *parent = 0);
    ~SvodVolumeDescriptorDialog();
    
private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::SvodVolumeDescriptorDialog *ui;
    SvodVolumeDescriptor *descriptor;
    QStatusBar *statusBar;
};

#endif // SVODVOLUMEDESCRIPTORDIALOG_H

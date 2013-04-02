#ifndef SECURITYSECTORDIALOG_H
#define SECURITYSECTORDIALOG_H

// qt
#include <QDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "qthelpers.h"

// xbox
#include "Fatx/FatxDrive.h"

namespace Ui {
class SecuritySectorDialog;
}

class SecuritySectorDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SecuritySectorDialog(FatxDrive *drive, QWidget *parent = 0);
    ~SecuritySectorDialog();
    
private slots:
    void on_btnExtractSecuritySector_clicked();

    void on_btnReplaceSecuritySector_clicked();

private:
    Ui::SecuritySectorDialog *ui;
    FatxDrive *drive;
};

#endif // SECURITYSECTORDIALOG_H

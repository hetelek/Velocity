#ifndef CERTIFICATEDIALOG_H
#define CERTIFICATEDIALOG_H

// qt
#include <QDialog>
#include <QComboBox>
#include <QMessageBox>
#include <QStatusBar>
#include "qthelpers.h"

// xbox360
#include <Stfs/StfsDefinitions.h>

namespace Ui {
class CertificateDialog;
}

class CertificateDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CertificateDialog(QStatusBar *statusBar, Certificate *cert, QWidget *parent, bool readOnly = false);
    ~CertificateDialog();
    
private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::CertificateDialog *ui;
    Certificate *cert;
    QStatusBar *statusBar;

    QComboBox *cmbxConsoleType;
    QComboBox *cmbxTestKit;
    QComboBox *cmbxRecoveryGenerated;
};

#endif // CERTIFICATEDIALOG_H

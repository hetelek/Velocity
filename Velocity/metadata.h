#ifndef METADATA_H
#define METADATA_H

// forms
#include "certificatedialog.h"
#include "volumedescriptordialog.h"
#include "licensingdatadialog.h"
#include "transferflagsdialog.h"

// qt
#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include "qthelpers.h"

// xbox360
#include "Stfs/StfsPackage.h"

namespace Ui {
class Metadata;
}

class Metadata : public QDialog
{
    Q_OBJECT
    
public:
    explicit Metadata(StfsPackage *package, QWidget *parent = 0);
    ~Metadata();
    
private:
    Ui::Metadata *ui;
    StfsPackage *package;

    QComboBox *cmbxMagic;
    QPushButton *btnCertificate;
    QPushButton *btnVolDesc;
    QPushButton *btnLicense;
    QPushButton *btnTransFlags;

private slots:
    void btnCertificateClicked();
    void btnVolDescClicked();
    void btnLicenseClicked();
    void btnTransFlagsClicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};

#endif // METADATA_H

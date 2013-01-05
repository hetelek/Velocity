#ifndef METADATA_H
#define METADATA_H

// forms
#include "certificatedialog.h"
#include "stfsvolumedescriptordialog.h"
#include "licensingdatadialog.h"
#include "transferflagsdialog.h"
#include "svodvolumedescriptordialog.h"

// qt
#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QDateTimeEdit>
#include <QStatusBar>
#include "qthelpers.h"

// xbox360
#include "Stfs/StfsPackage.h"
#include "Stfs/StfsConstants.h"

namespace Ui {
class Metadata;
}

struct AssetSubcategoryStruct
{
    QString name;
    AssetSubcategory value;
};

struct OnlineResumeStateStruct
{
    QString name;
    OnlineContentResumeState value;
};

class Metadata : public QDialog
{
    Q_OBJECT
    
public:
    explicit Metadata(QStatusBar *statusBar, XContentHeader *header, bool pec, QWidget *parent = 0);
    ~Metadata();
    
private:
    Ui::Metadata *ui;
    XContentHeader *header;
    bool pec;
    QStatusBar *statusBar;

    QComboBox *cmbxMagic;
    QComboBox *cmbxSubcategory;
    QComboBox *cmbxSkeletonVersion;
    QComboBox *cmbxInstallerType;
    QComboBox *cmbxResumeState;

    QDateTimeEdit *lastModified;

    QPushButton *btnCertificate;
    QPushButton *btnVolDesc;
    QPushButton *btnLicense;
    QPushButton *btnTransFlags;
    DWORD offset;

private slots:
    void btnCertificateClicked();
    void btnVolDescClicked();
    void btnLicenseClicked();
    void btnTransFlagsClicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};

#endif // METADATA_H

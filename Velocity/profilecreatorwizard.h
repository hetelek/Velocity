#ifndef PROFILECREATORWIZARD_H
#define PROFILECREATORWIZARD_H

// qt
#include <QWizard>
#include <QMessageBox>
#include "qthelpers.h"

// xbox360
#include "Stfs/StfsPackage.h"
#include "Stfs/StfsMetaData.h"
#include "GPD/DashboardGPD.h"

namespace Ui {
class ProfileCreatorWizard;
}

class ProfileCreatorWizard : public QWizard
{
    Q_OBJECT
    
public:
    explicit ProfileCreatorWizard(QWidget *parent = 0);
    ~ProfileCreatorWizard();
    
private slots:
    void onCurrentIdChanged(int id);

    void on_txtGamertag_textChanged(const QString &arg1);

    void onFinished(int status);

private:
    Ui::ProfileCreatorWizard *ui;
};

#endif // PROFILECREATORWIZARD_H

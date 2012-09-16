#ifndef PROFILECREATORWIZARD_H
#define PROFILECREATORWIZARD_H

// qt
#include <QWizard>
#include <QMessageBox>
#include <QBuffer>
#include <QDir>
#include <QUuid>
#include "qthelpers.h"

// xbox360
#include "Stfs/StfsPackage.h"
#include "Stfs/StfsMetaData.h"
#include "GPD/DashboardGPD.h"
#include "Account/Account.h"

// other
#include <stdlib.h>
#include <time.h>

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

    void on_pushButton_clicked();

private:
    Ui::ProfileCreatorWizard *ui;
    UINT64 profileID;

    bool verifyGamertag(QString gamertag);
};

#endif // PROFILECREATORWIZARD_H

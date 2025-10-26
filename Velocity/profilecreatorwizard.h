#ifndef PROFILECREATORWIZARD_H
#define PROFILECREATORWIZARD_H

// qt
#include <QWizard>
#include <QMessageBox>
#include <QBuffer>
#include <QDir>
#include <QUuid>
#include <QStatusBar>
#include "qthelpers.h"

// xbox360
#include <XboxInternals/Stfs/StfsPackage.h>
#include <XboxInternals/Stfs/XContentHeader.h>
#include <XboxInternals/Gpd/DashboardGpd.h>
#include <XboxInternals/Account/Account.h>

// other
#include <stdlib.h>
#include <time.h>

#include "Binaries/femaleAvatar.h"

namespace Ui
{
class ProfileCreatorWizard;
}

class ProfileCreatorWizard : public QWizard
{
    Q_OBJECT

public:
    explicit ProfileCreatorWizard(QStatusBar *statusBar, QWidget *parent = 0);
    ~ProfileCreatorWizard();

private slots:
    void onCurrentIdChanged(int id);

    void on_txtGamertag_textChanged(const QString &arg1);

    void onFinished(int status);

    void on_pushButton_clicked();

    void on_radioButton_clicked(bool checked);

    void on_radioButton_2_clicked(bool checked);

private:
    Ui::ProfileCreatorWizard *ui;
    UINT64 profileID;
    QStatusBar *statusBar;
    ConsoleType consoleType;

    bool verifyGamertag(QString gamertag);
};

#endif // PROFILECREATORWIZARD_H



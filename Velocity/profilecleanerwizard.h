#ifndef PROFILECLEANERWIZARD_H
#define PROFILECLEANERWIZARD_H

// qt
#include <QWizard>
#include <QMessageBox>
#include <QUuid>
#include <QDebug>
#include "qthelpers.h"

// xbox360
#include "Stfs/StfsPackage.h"
#include "Gpd/DashboardGpd.h"
#include "Gpd/GameGpd.h"

enum CleanOperation
{
    Dust,
    Sweep,
    Purify
};

namespace Ui {
class ProfileCleanerWizard;
}

class ProfileCleanerWizard : public QWizard
{
    Q_OBJECT
    
public:
    explicit ProfileCleanerWizard(QWidget *parent = 0);
    ~ProfileCleanerWizard();

private slots:
    void onCurrentIdChanged(int id);
    
    void on_pushButton_clicked();

    void on_radioButton_toggled(bool checked);

    void on_radioButton_2_toggled(bool checked);

    void on_radioButton_3_toggled(bool checked);

private:
    Ui::ProfileCleanerWizard *ui;
    bool profileOpened;
    StfsPackage *profile;
    QString directory;
    QString profilePath;
    DWORD initialSize;

    CleanOperation op;

    void clean();
    void extractAll(StfsFileListing *f, QString parentDirectory);
    void deleteAllRecursive(QDir directory);
    void injectAll(StfsPackage *profile, QDir currentDirectory, QString currentStfsDir);
};

#endif // PROFILECLEANERWIZARD_H

#ifndef ACHIEVEMENTCREATIONWIZARD_H
#define ACHIEVEMENTCREATIONWIZARD_H

// qt
#include <QWizard>
#include <QMessageBox>
#include <QFileDialog>
#include "qthelpers.h"

// xbox360
#include <Gpd/GameGpd.h>

// other
#include <iostream>

using namespace std;

namespace Ui {
class AchievementCreationWizard;
}

class AchievementCreationWizard : public QWizard
{
    Q_OBJECT
    
public:
    explicit AchievementCreationWizard(wstring gameName, QImage *achThumbnail, AchievementEntry *entry, QWidget *parent = 0);
    ~AchievementCreationWizard();
    
private slots:
    void on_comboBox_currentIndexChanged(int index);

    void onCurrentIdChanged(int id);

    void on_pushButton_clicked();

    void onFinished(int result);

    void on_txtName_textChanged(const QString &arg1);

    void on_txtLockDesc_textChanged();

    void on_txtUnlockDesc_textChanged();

private:
    Ui::AchievementCreationWizard *ui;
    AchievementEntry *achievement;
    QImage *achThumbnail;
};

#endif // ACHIEVEMENTCREATIONWIZARD_H

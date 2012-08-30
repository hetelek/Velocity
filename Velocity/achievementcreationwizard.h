#ifndef ACHIEVEMENTCREATIONWIZARD_H
#define ACHIEVEMENTCREATIONWIZARD_H

// qt
#include <QWizard>

// xbox360
#include <GPD/GameGPD.h>

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
    explicit AchievementCreationWizard(wstring gameName, AchievementEntry *entry, QWidget *parent = 0);
    ~AchievementCreationWizard();
    
private slots:
    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::AchievementCreationWizard *ui;
    AchievementEntry *achievement;
};

#endif // ACHIEVEMENTCREATIONWIZARD_H

#include "achievementcreationwizard.h"
#include "ui_achievementcreationwizard.h"

AchievementCreationWizard::AchievementCreationWizard(wstring gameName, AchievementEntry *entry, QWidget *parent) :QWizard(parent), ui(new Ui::AchievementCreationWizard), achievement(entry)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    ui->label_3->setFont(QFont("Arial", 10));
    ui->lblAchievementType->setFont(QFont("Arial", 10));

    ui->label_3->setText("Welcome to the achievement creation wizard! This wizard will allow you to create an achievement for the game " + QString::fromStdWString(gameName) + ". The achievement you create will only be obtainable through modding, and since it isn't an official achievement, your friends on XboxLIVE will not be able to see it.\r\n\r\nClick next to begin.");
}

AchievementCreationWizard::~AchievementCreationWizard()
{
    delete ui;
}

void AchievementCreationWizard::on_comboBox_currentIndexChanged(int index)
{
    switch (index)
    {
        case 0:
            ui->lblAchievementType->setText("The player has completed a game. For example, finishing the single-player campaign or finishing Dynasty Mode.");
            break;
        case 1:
            ui->lblAchievementType->setText("The player has increased their level. For example, becoming an Expert in a multiplayer game.");
            break;
        case 2:
            ui->lblAchievementType->setText("The player has unlocked a new game feature. For example, unlocking a new map or weapon type.");
            break;
        case 3:
            ui->lblAchievementType->setText("The player has completed a special goal in the game. For example, scoring 3 goals in a single soccer match.");
            break;
        case 4:
            ui->lblAchievementType->setText("The player has gained an award in a tournament-level event. For example, winning 3rd place in the FIFA World Cup.");
            break;
        case 5:
            ui->lblAchievementType->setText("The player has reached a certain point or completed a specific portion of the game. For example, completing the 3rd level of the game.");
            break;
        case 6:
            ui->lblAchievementType->setText("The player has done something that falls outside the other achievement types. ");
            break;
    }
}

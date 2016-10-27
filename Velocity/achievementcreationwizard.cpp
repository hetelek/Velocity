#include "achievementcreationwizard.h"
#include "ui_achievementcreationwizard.h"

AchievementCreationWizard::AchievementCreationWizard(wstring gameName, QImage *achThumbnail, AchievementEntry *entry, QWidget *parent) : QWizard(parent), ui(new Ui::AchievementCreationWizard), achievement(entry), achThumbnail(achThumbnail)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    achievement->structSize = 0;

    ui->label_3->setFont(QFont("Arial", 10));
    ui->lblAchievementType->setFont(QFont("Arial", 10));
    ui->lblAchievementType_3->setFont(QFont("Arial", 10));

    connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(onCurrentIdChanged(int)));
    connect(this, SIGNAL(finished(int)), this, SLOT(onFinished(int)));

    ui->lblAchievementType_3->setText("You have successfully created a custom achievement. Click finish to add the achievement to " + QString::fromStdWString(gameName) + ".");
    ui->label_3->setText("Welcome to the achievement creation wizard! This wizard will allow you to create an achievement for " + QString::fromStdWString(gameName) + ". The achievement you create will only be obtainable through modding, and since it isn't an official achievement, your friends on XboxLIVE will not be able to see it.\r\n\r\nClick next to begin.");
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

void AchievementCreationWizard::onCurrentIdChanged(int id)
{
    switch (id)
    {
        // disable next button until the user enters text for all the fields
        case 1:
            button(QWizard::NextButton)->setEnabled(false);
            break;
        // disable next button until the user chooses a thumbnail for the achievement
        case 2:
            button(QWizard::NextButton)->setEnabled(false);
            break;
    }
}

void AchievementCreationWizard::on_pushButton_clicked()
{
    QString imagePath = QFileDialog::getOpenFileName(this, "Choose a thumbnail for your achievement", QtHelpers::DesktopLocation(), "*.png");

    if (imagePath == "")
        return;

    QImage thumbnail(imagePath);

    // verify the dimensions
    if (thumbnail.height() != 64 || thumbnail.width() != 64)
    {
        QMessageBox::warning(this, "Invalid Image", "The thumbnail image for the achievement must 64x64 pixels.");
        return;
    }

    ui->imgThumbnail->setPixmap(QPixmap::fromImage(thumbnail));
    button(QWizard::NextButton)->setEnabled(true);
}

void AchievementCreationWizard::onFinished(int result)
{
    if (result == 1)
    {
        // build up the achievement entry
        achievement->name = ui->txtName->text().toStdWString();
        achievement->lockedDescription = ui->txtLockDesc->toPlainText().toStdWString();
        achievement->unlockedDescription = ui->txtUnlockDesc->toPlainText().toStdWString();
        achievement->gamerscore = ui->spnGamerscore->value();
        achievement->flags = (ui->cmbxType->currentIndex() + 1) | (ui->cmbxSecret->currentIndex() << 3);
        achievement->unlockTime = 0;
        achievement->unlockTimeMilliseconds = 0;
        achievement->structSize = 0x1C;

        // set the thumbnail image
        *achThumbnail = ui->imgThumbnail->pixmap()->toImage();
    }
}

void AchievementCreationWizard::on_txtName_textChanged(const QString & /* arg1 */)
{
    button(QWizard::NextButton)->setEnabled(ui->txtName->text() != "" && ui->txtLockDesc->toPlainText() != "" && ui->txtUnlockDesc->toPlainText() != "");
}

void AchievementCreationWizard::on_txtLockDesc_textChanged()
{
    button(QWizard::NextButton)->setEnabled(ui->txtName->text() != "" && ui->txtLockDesc->toPlainText() != "" && ui->txtUnlockDesc->toPlainText() != "");
}

void AchievementCreationWizard::on_txtUnlockDesc_textChanged()
{
    button(QWizard::NextButton)->setEnabled(ui->txtName->text() != "" && ui->txtLockDesc->toPlainText() != "" && ui->txtUnlockDesc->toPlainText() != "");
}

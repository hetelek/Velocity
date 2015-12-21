#include "profileselectiondialog.h"
#include "ui_profileselectiondialog.h"

ProfileSelectionDialog::ProfileSelectionDialog(std::vector<XContentDeviceProfile> *profiles, QString fileName, QWidget *parent) :
    QDialog(parent), ui(new Ui::ProfileSelectionDialog), profiles(profiles), hasSelectedProfile(false)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    setWindowTitle("Owner of " + fileName);

    // add all the profiles to the dialog
    for (size_t i = 0; i < profiles->size(); i++)
    {
        XContentDeviceProfile profile = profiles->at(i);

        // set the gamertag
        QTreeWidgetItem *profileItem = new QTreeWidgetItem(ui->treeWidget);
        QString gamertag = QString::fromStdWString(profile.GetName());
        profileItem->setText(0, gamertag);

        if (gamertag == "Unknown Profile")
            profileItem->setHidden(true);

        // set the thumbnail
        QByteArray imageBuff((char*)profile.GetThumbnail(), profile.GetThumbnailSize());
        profileItem->setIcon(0, QIcon(QPixmap::fromImage(QImage::fromData(imageBuff))));
    }

    QString instructions = ui->lblDirections->text();
    instructions.replace("FILE_NAME", fileName);
    ui->lblDirections->setText(instructions);
}

ProfileSelectionDialog::~ProfileSelectionDialog()
{
    delete ui;
}

XContentDeviceProfile ProfileSelectionDialog::GetSelectedProfile()
{
    return selectedProfile;
}

bool ProfileSelectionDialog::HasSelectedProfile()
{
    return hasSelectedProfile;
}

void ProfileSelectionDialog::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if (current == NULL)
    {
        hasSelectedProfile = false;
    }
    else
    {
        selectedProfile = profiles->at(ui->treeWidget->currentIndex().row());
        hasSelectedProfile = true;
    }
}


void ProfileSelectionDialog::on_pushButton_clicked()
{
    close();
}

void ProfileSelectionDialog::on_pushButton_2_clicked()
{
    hasSelectedProfile = false;
    close();
}

void ProfileSelectionDialog::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    selectedProfile = profiles->at(ui->treeWidget->currentIndex().row());
    hasSelectedProfile = true;
    close();
}

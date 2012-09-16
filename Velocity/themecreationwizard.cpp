#include "themecreationwizard.h"
#include "ui_themecreationwizard.h"

ThemeCreationWizard::ThemeCreationWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::ThemeCreationWizard)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    // populate the sphere color combobox
    for (DWORD i = 0; i < 16; i++)
    {
        ui->cmbxSphereColor->insertItem(i, colorNames[i]);
        ui->cmbxSphereColor->setItemData(i, colors[i], Qt::DecorationRole);
    }

    connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(onCurrentIdChanged(int)));
}

ThemeCreationWizard::~ThemeCreationWizard()
{

}

void ThemeCreationWizard::onCurrentIdChanged(int index)
{
    switch (index)
    {
        case 2:
            button(QWizard::NextButton)->setEnabled(ui->txtName->text() != "");
            break;
    }
}

void ThemeCreationWizard::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Choose a thumbnail image", QtHelpers::DesktopLocation(), "PNG File (*.png)");

    if (fileName == "")
        return;

    QPixmap thumbnail(fileName);

    if (thumbnail.isNull() || thumbnail.width() != 64 || thumbnail.height() != 64)
    {
        QMessageBox::warning(this, "Invalid Image", "Thumbnail must be a 64x64 PNG image.");
        return;
    }

    ui->imgThumbnail->setPixmap(thumbnail);
}

void ThemeCreationWizard::openWallpaper(QLabel *imageViewer)
{
    QString fileName = QFileDialog::getOpenFileName(this, "Choose a wallpaper image", QtHelpers::DesktopLocation(), "PNG File (*.png);;JPEG File(*.jpeg);;JPG File (*jpg)");

    if (fileName == "")
        return;

    QPixmap thumbnail(fileName);

    if (thumbnail.isNull() || thumbnail.width() != 1280 || thumbnail.height() != 720)
    {
        QMessageBox::warning(this, "Invalid Image", "Wallpaper must be a 1280x720 PNG image.");
        return;
    }

    imageViewer->setPixmap(thumbnail);
    imageViewer->setScaledContents(true);
}


void ThemeCreationWizard::on_pushButton_2_clicked()
{
    openWallpaper(ui->imgWallpaper1);
}

void ThemeCreationWizard::on_pushButton_3_clicked()
{
    openWallpaper(ui->imgWallpaper2);
}

void ThemeCreationWizard::on_pushButton_4_clicked()
{
    openWallpaper(ui->imgWallpaper3);
}

void ThemeCreationWizard::on_pushButton_5_clicked()
{
    openWallpaper(ui->imgWallpaper4);
}

void ThemeCreationWizard::on_txtName_textChanged(const QString &arg1)
{
    button(QWizard::NextButton)->setEnabled(ui->txtName->text() != "");
}

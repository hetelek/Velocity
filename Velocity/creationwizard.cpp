#include "creationwizard.h"
#include "ui_creationwizard.h"

CreationWizard::CreationWizard(QString *fileName, QWidget *parent) :
    QWizard(parent), ui(new Ui::CreationWizard), fileName(fileName), consoleType(Retail), magic(CON)
{
    connect(this, SIGNAL(currentIdChanged(int)), SLOT(onCurrentIdChanged(int)));
    connect(this, SIGNAL(finished(int)), this, SLOT(onFinished(int)));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QtHelpers::GenAdjustWidgetAppearanceToOS(this);

    magic = CON;
    ui->setupUi(this);
#ifdef __WIN32
    ui->label_4->setFont(QFont("Arial", 10));
#elif __APPLE__
    QFont f = ui->label_4->font();
    f.setPointSize(12);
    ui->label_4->setFont(f);

    ui->label_3->setText("<font size=3>" + ui->label_3->text() + "</font>");
    ui->label_14->setText("<font size=3>" + ui->label_14->text() + "</font>");
    ui->label_4->setText("<font size=3>" + ui->label_4->text() + "</font>");
    ui->label_6->setText("<font size=3>" + ui->label_6->text() + "</font>");
    ui->label_7->setText("<font size=3>" + ui->label_7->text() + "</font>");
    ui->label_8->setText("<font size=3>" + ui->label_8->text() + "</font>");
    ui->lblSavePath->setText("<font size=3>" + ui->lblSavePath->text() + "</font>");
#endif
}

CreationWizard::~CreationWizard()
{
    delete ui;
}

void CreationWizard::onCurrentIdChanged(int id)
{
    switch (id)
    {
    case 3:
        ui->lwContentTypes->clear();
        // populate content type list

        if (magic == CON)
        {
            ui->lwContentTypes->addItem("Cache File");
            ui->lwContentTypes->addItem("Gamer Picture");
            ui->lwContentTypes->addItem("License Store");
            ui->lwContentTypes->addItem("Profile");
            ui->lwContentTypes->addItem("Publisher");
            ui->lwContentTypes->addItem("Saved Game");
            ui->lwContentTypes->addItem("Theme");
            ui->lwContentTypes->addItem("Xbox Saved Game");
            ui->lwContentTypes->addItem("XNA");
        }
        else if (magic == LIVE)
        {
            ui->lwContentTypes->addItem("App");
            ui->lwContentTypes->addItem("Arcade Game");
            ui->lwContentTypes->addItem("Avatar Item");
            ui->lwContentTypes->addItem("Community Game");
            ui->lwContentTypes->addItem("Game Demo");
            ui->lwContentTypes->addItem("Gamer Picture");
            ui->lwContentTypes->addItem("Game Trailer");
            ui->lwContentTypes->addItem("Game Video");
            ui->lwContentTypes->addItem("IPTV Pause Buffer ");
            ui->lwContentTypes->addItem("Installer");
            ui->lwContentTypes->addItem("License Store");
            ui->lwContentTypes->addItem("Market Place Content");
            ui->lwContentTypes->addItem("Movie");
            ui->lwContentTypes->addItem("Music Video");
            ui->lwContentTypes->addItem("Podcast Video");
            ui->lwContentTypes->addItem("Storage Download");
            ui->lwContentTypes->addItem("Video");
            ui->lwContentTypes->addItem("Viral Video");
            ui->lwContentTypes->addItem("Xbox Download");
            ui->lwContentTypes->addItem("Xbox Original Game");
            ui->lwContentTypes->addItem("Xbox360 Title");
        }
        else
        {
            ui->lwContentTypes->addItem("Avatar Asset Pack");
            ui->lwContentTypes->addItem("Theme");
        }
        ui->lwContentTypes->setCurrentItem(ui->lwContentTypes->item(0));
        ui->lwContentTypes->item(0)->setSelected(true);
        break;

    case 4:
        button(QWizard::NextButton)->setEnabled(false);
        break;
    case 6:
        ui->lblSavePath->setText(QtHelpers::DesktopLocation().replace("\\", "/") + "/" + ui->txtDisplayName->text());
        break;
    }
}

void CreationWizard::on_txtDisplayName_textChanged(const QString & /* arg1 */)
{
    button(QWizard::NextButton)->setEnabled(ui->txtDisplayName->text() != "" && ui->txtTitleID->text().length() == 8 && QtHelpers::VerifyHexString(ui->txtTitleID->text()));
}

void CreationWizard::openImage(QLabel *img)
{
    QString imgPath = QFileDialog::getOpenFileName(this, "Open a 64x64 thumbnail", QtHelpers::DesktopLocation(), "*.png");

    if (imgPath == "")
        return;

    QImage image(imgPath);

    if (image.width() != 64 && image.height() != 64)
    {
        QMessageBox::warning(this, "Invalid Image", "The image must be a 64x64 PNG image.");
        return;
    }

    img->setPixmap(QPixmap::fromImage(image));
}

void CreationWizard::on_btnOpenThumbnail_clicked()
{
    openImage(ui->imgThumbnail);
}

void CreationWizard::on_btnOpenTitleThumbnail_clicked()
{
    openImage(ui->imgTitleThumbnail);
}

void CreationWizard::on_txtTitleID_textChanged(const QString & /* arg1 */)
{
    if (ui->txtTitleID->text().length() != 8 || !QtHelpers::VerifyHexString(ui->txtTitleID->text()))
        ui->txtTitleID->setStyleSheet("color: rgb(255, 1, 1);");
    else
        ui->txtTitleID->setStyleSheet("");

    button(QWizard::NextButton)->setEnabled(ui->txtDisplayName->text() != "" && ui->txtTitleID->text().length() == 8 && QtHelpers::VerifyHexString(ui->txtTitleID->text()));
}

void CreationWizard::on_pushButton_clicked()
{
    QString savePath = QFileDialog::getSaveFileName(this, "Choose a place to create the pacakge", ui->lblSavePath->text());
    if (savePath == "")
        return;

    ui->lblSavePath->setText(savePath);
}

void CreationWizard::onFinished(int status)
{
    *fileName = "error";
    if (status != 1)
        return;

    try
    {
        StfsPackage package(ui->lblSavePath->text().toStdString(), ((magic != CON) ? StfsPackageFemale : 0) | StfsPackageCreate);

        // set the metadata
        package.metaData->magic = magic;
        package.metaData->certificate.ownerConsoleType = consoleType;
        package.metaData->contentType = (ContentType)getContentType();
        package.metaData->titleID = QtHelpers::ParseHexString(ui->txtTitleID->text());
        package.metaData->displayName = ui->txtDisplayName->text().toStdWString();
        package.metaData->titleName = ui->txtTitleName->text().toStdWString();

        // set the thumbnails
        QByteArray ba1;
        QBuffer buffer1(&ba1);
        buffer1.open(QIODevice::WriteOnly);
        ui->imgThumbnail->pixmap()->save(&buffer1, "PNG");
        package.metaData->thumbnailImage = (BYTE*)ba1.data();
        package.metaData->thumbnailImageSize = ba1.length();

        QByteArray ba2;
        QBuffer buffer2(&ba2);
        buffer2.open(QIODevice::WriteOnly);
        ui->imgTitleThumbnail->pixmap()->save(&buffer2, "PNG");
        package.metaData->titleThumbnailImage = (BYTE*)ba2.data();
        package.metaData->titleThumbnailImageSize = ba2.length();

        package.metaData->WriteMetaData();

        // fix the pacakge
        package.Rehash();
        if (package.metaData->magic == CON)
            package.Resign(QtHelpers::GetKVPath(package.metaData->certificate.ownerConsoleType, this));
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Error", "An error occured while creating the package.\n\n" + QString::fromStdString(error));
        return;
    }

    *fileName = ui->lblSavePath->text();
}

DWORD CreationWizard::getContentType()
{
    QString contentType = ui->lwContentTypes->currentItem()->text().trimmed();
    if (contentType == "App")
        return 0x7000;
    else if (contentType == "Arcade Game")
        return 0xD0000;
    else if (contentType == "Avatar Item")
        return 0x9000;
    else if (contentType == "Avatar Asset Pack")
        return 0x8000;
    else if (contentType == "Cache File")
        return 0x40000;
    else if (contentType == "Community Game")
        return 0x2000000;
    else if (contentType == "Game Demo")
        return 0x80000;
    else if (contentType == "Gamer Picture")
        return 0x20000;
    else if (contentType == "Gamer Title")
        return 0xA0000;
    else if (contentType == "Game Trailer")
        return 0xC0000;
    else if (contentType == "Game Video")
        return 0x400000;
    else if (contentType == "Installed Game")
        return 0x4000;
    else if (contentType == "Installer")
        return 0xB0000;
    else if (contentType == "IPTV PauseBuffer")
        return 0x2000;
    else if (contentType == "License Store")
        return 0xF0000;
    else if (contentType == "Market Place Content")
        return 2;
    else if (contentType == "Movie")
        return 0x100000;
    else if (contentType == "Music Video")
        return 0x300000;
    else if (contentType == "Podcast Video")
        return 0x500000;
    else if (contentType == "Profile")
        return 0x10000;
    else if (contentType == "Publisher")
        return 3;
    else if (contentType == "Saved Game")
        return 1;
    else if (contentType == "Storage Download")
        return 0x50000;
    else if (contentType == "Theme")
        return 0x30000;
    else if (contentType == "Video")
        return 0x200000;
    else if (contentType == "Viral Video")
        return 0x600000;
    else if (contentType == "Xbox Download")
        return 0x70000;
    else if (contentType == "Xbox Original Game")
        return 0x5000;
    else if (contentType == "Xbox Saved Game")
        return 0x60000;
    else if (contentType == "Xbox360 Title")
        return 0x1000;
    else if (contentType == "XNA")
        return 0xE0000;
    return 0;
}

void CreationWizard::on_radioButton_clicked(bool checked)
{
    if (checked)
        consoleType = Retail;
}

void CreationWizard::on_radioButton_2_clicked(bool checked)
{
    if (checked)
        consoleType = DevKit;
}

void CreationWizard::on_radioButton_3_clicked(bool checked)
{
    if (checked)
        magic = CON;
}

void CreationWizard::on_radioButton_4_clicked(bool checked)
{
    if (checked)
        magic = LIVE;
}

void CreationWizard::on_radioButton_5_clicked(bool checked)
{
    if (checked)
        magic = PIRS;
}

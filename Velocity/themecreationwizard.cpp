#include "themecreationwizard.h"
#include "ui_themecreationwizard.h"

ThemeCreationWizard::ThemeCreationWizard(QWidget *parent) :
    QWizard(parent), ui(new Ui::ThemeCreationWizard), imagesLoaded(0)
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
    connect(this, SIGNAL(finished(int)), this, SLOT(onFinished(int)));
}

ThemeCreationWizard::~ThemeCreationWizard()
{

}

void ThemeCreationWizard::onFinished(int status)
{
    if (status == 0)
        return;
    try
    {
        // create a new file
        FileIO io(ui->lblSavePath->text().toStdString(), true);

        // null out the first 0xD000 bytes
        BYTE tempBuffer[0x1000] = {0};
        for (DWORD i = 0; i < 0xD; i++)
            io.write(tempBuffer, 0x1000);
        io.setPosition(0xA014);
        io.write((DWORD)0x80FFFFFF);

        // set up the metadata for the theme
        StfsMetaData metadata(&io, MetadataDontFreeThumbnails | MetadataSkipRead);
        metadata.magic = CON;
        metadata.certificate.ownerConsoleType = (ui->cmbxType->currentIndex() == 0) ? Retail : DevKit;
        metadata.certificate.consoleTypeFlags = 0;

        memset(metadata.licenseData, 0, sizeof(LicenseEntry) * 0x10);
        metadata.licenseData[0].type = Unrestricted;
        metadata.licenseData[0].data = 0xFFFFFFFFFFFF;

        metadata.headerSize = 0x971A;
        metadata.contentType = Theme;
        metadata.metaDataVersion = 2;
        metadata.contentSize = 0;
        metadata.mediaID = 0;
        metadata.version = 0;
        metadata.baseVersion = 0;
        metadata.titleID = 0xFFFE07D1;
        metadata.platform = 0;
        metadata.executableType = 0;
        metadata.discNumber = 0;
        metadata.discInSet = 0;
        metadata.savegameID = 0;
        memset(metadata.consoleID, 0, 5);
        memset(metadata.profileID, 0, 8);

        // volume descriptor
        metadata.volumeDescriptor.size = 0x24;
        metadata.volumeDescriptor.blockSeperation = 0;
        metadata.volumeDescriptor.fileTableBlockCount = 1;
        metadata.volumeDescriptor.fileTableBlockNum = 0;
        metadata.volumeDescriptor.allocatedBlockCount = 1;
        metadata.volumeDescriptor.unallocatedBlockCount = 0;

        metadata.dataFileCount = 0;
        metadata.dataFileCombinedSize = 0;
        metadata.seasonNumber = 0;
        metadata.episodeNumber = 0;
        memset(metadata.seasonID, 0, 0x10);
        memset(metadata.seriesID, 0, 0x10);
        memset(metadata.deviceID, 0, 0x14);
        metadata.displayName = ui->txtName->text().toStdWString();
        metadata.displayDescription = L"";
        metadata.publisherName = L"";
        metadata.titleName = L"Xbox360 Dashboard";
        metadata.transferFlags = 0x40;

        // set gamerpicture
        QByteArray ba1;
        QBuffer buffer1(&ba1);
        buffer1.open(QIODevice::WriteOnly);
        ui->imgThumbnail->pixmap()->save(&buffer1, "PNG");
        metadata.thumbnailImage = ba1.data();
        metadata.thumbnailImageSize = ba1.length();

        // set title thumbnail image
        QByteArray ba2;
        QBuffer buffer2(&ba2);
        buffer2.open(QIODevice::WriteOnly);
        QPixmap(":/Images/defaultTitleImage.png").save(&buffer2, "PNG");
        metadata.titleThumbnailImage = ba2.data();
        metadata.titleThumbnailImageSize = ba2.length();

        metadata.WriteMetaData();
        io.close();

        StfsPackage theme(ui->lblSavePath->text().toStdString(), false);

        // inject the wallpapers
        theme.InjectFile(wallpaper1.toStdString(), "Wallpaper1");
        theme.InjectFile(wallpaper2.toStdString(), "Wallpaper2");
        theme.InjectFile(wallpaper3.toStdString(), "Wallpaper3");
        theme.InjectFile(wallpaper4.toStdString(), "Wallpaper4");

        // create the parameters.ini file
        QString paramsFilePath = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");
        QFile params(paramsFilePath);
        params.open(QIODevice::Truncate | QIODevice::WriteOnly);

        // write the correct information to it
        QTextStream txtStream(&params);
        txtStream << "SphereColor=" << ui->cmbxSphereColor->currentIndex() << "\r\nAvatarLightingDirectional=0,0,0,0\r\nAvatarLightingAmbient=0\r\n";

        // close the file
        txtStream.flush();
        params.close();

        // inject the params file to the theme package
        theme.InjectFile(paramsFilePath.toStdString(), "parameters.ini");

        // create the dash style file
        QString dashStyleFilePath = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");
        FileIO ioD(dashStyleFilePath.toStdString(), true);
        ioD.write((DWORD)0);
        ioD.close();

        // inject the file
        theme.InjectFile(dashStyleFilePath.toStdString(), "DashStyle");

        // fix the package
        theme.Rehash();
        theme.Resign(QtHelpers::GetKVPath(metadata.certificate.ownerConsoleType, this));

        // delete the temp files
        QFile::remove(paramsFilePath);
        QFile::remove(dashStyleFilePath);
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Error", "An error occured while creating the theme.\n\n" + QString::fromStdString(error));
    }
}

void ThemeCreationWizard::onCurrentIdChanged(int index)
{
    switch (index)
    {
        case 2:
            button(QWizard::NextButton)->setEnabled(ui->txtName->text() != "");
            break;
        case 4:
            button(QWizard::NextButton)->setEnabled(imagesLoaded & 1);
            break;
        case 5:
            button(QWizard::NextButton)->setEnabled(imagesLoaded & 2);
            break;
        case 6:
            button(QWizard::NextButton)->setEnabled(imagesLoaded & 4);
            break;
        case 7:
            button(QWizard::NextButton)->setEnabled(imagesLoaded & 8);
            break;
        case 8:
            ui->lblSavePath->setText(QtHelpers::DesktopLocation() + "\\" + ui->txtName->text());
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

void ThemeCreationWizard::openWallpaper(QLabel *imageViewer, QString *saveStr)
{
    QString fileName = QFileDialog::getOpenFileName(this, "Choose a wallpaper image", QtHelpers::DesktopLocation(), "JPG File (*jpg)");

    if (fileName == "")
        return;

    QPixmap thumbnail(fileName);

    if (thumbnail.isNull() || thumbnail.width() != 1280 || thumbnail.height() != 720)
    {
        QMessageBox::warning(this, "Invalid Image", "Wallpaper must be a 1280x720 JPG image.");
        return;
    }

    *saveStr = fileName;

    imageViewer->setPixmap(thumbnail);
    imageViewer->setScaledContents(true);
    imagesLoaded |= (1 << (this->currentId() - 4));
    button(QWizard::NextButton)->setEnabled(true);
}


void ThemeCreationWizard::on_pushButton_2_clicked()
{
    openWallpaper(ui->imgWallpaper1, &wallpaper1);
}

void ThemeCreationWizard::on_pushButton_3_clicked()
{
    openWallpaper(ui->imgWallpaper2, &wallpaper2);
}

void ThemeCreationWizard::on_pushButton_4_clicked()
{
    openWallpaper(ui->imgWallpaper3, &wallpaper3);
}

void ThemeCreationWizard::on_pushButton_5_clicked()
{
    openWallpaper(ui->imgWallpaper4, &wallpaper4);
}

void ThemeCreationWizard::on_txtName_textChanged(const QString &arg1)
{
    button(QWizard::NextButton)->setEnabled(ui->txtName->text() != "");
}

void ThemeCreationWizard::on_pushButton_6_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Choose a place to save your theme", QtHelpers::DesktopLocation() + "\\" + ui->txtName->text());

    if (filePath == "")
        return;

    ui->lblSavePath->setText(filePath);
}

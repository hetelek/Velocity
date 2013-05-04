#include "themecreationwizard.h"
#include "ui_themecreationwizard.h"

ThemeCreationWizard::ThemeCreationWizard(QStatusBar *statusBar, QWidget *parent) :
    QWizard(parent), ui(new Ui::ThemeCreationWizard), statusBar(statusBar), imagesLoaded(0), consoleType(Retail)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    QtHelpers::GenAdjustWidgetAppearanceToOS(this);

#ifdef __APPLE__
    ui->label_3->setText("<font size=3>" + ui->label_3->text() + "</font>");
    ui->label_5->setText("<font size=3>" + ui->label_5->text() + "</font>");
    ui->label_7->setText("<font size=3>" + ui->label_7->text() + "</font>");
    ui->lblSavePath->setText("<font size=3>" + ui->lblSavePath->text() + "</font>");
#elif __WIN32
    ui->label_3->setFont(QFont("MS Shell Dlg 2", 10));
#endif

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
    // free all the images allocated on the heap
    for (int i = 0; i < allocatedImages.size(); i++)
        delete allocatedImages.at(i);
}

void ThemeCreationWizard::onFinished(int status)
{
    if (status == 0)
        return;
    try
    {
        StfsPackage *theme = new StfsPackage(ui->lblSavePath->text().toStdString(), StfsPackageCreate);

        // create a new file
        theme->metaData->magic = CON;
        theme->metaData->certificate.ownerConsoleType = consoleType;
        theme->metaData->certificate.consoleTypeFlags = (ConsoleTypeFlags)0;

        theme->metaData->contentType = Theme;
        theme->metaData->metaDataVersion = 2;
        theme->metaData->titleID = 0xFFFE07D1;

        // had to do some glitch hacks in order to get this to work. If I don't do this, the OS gives an 'Unknown Signal'
        std::wstring *w = new std::wstring(ui->txtName->text().toStdWString());
        memcpy(&theme->metaData->displayName, w, sizeof(std::wstring));

        theme->metaData->transferFlags = 0x40;

        // set gamerpicture
        QByteArray ba1;
        QBuffer buffer1(&ba1);
        buffer1.open(QIODevice::WriteOnly);
        ui->imgThumbnail->pixmap()->save(&buffer1, "PNG");
        theme->metaData->thumbnailImage = (BYTE*)ba1.data();
        theme->metaData->thumbnailImageSize = ba1.length();

        // set title thumbnail image
        QByteArray ba2;
        QBuffer buffer2(&ba2);
        buffer2.open(QIODevice::WriteOnly);
        QPixmap(":/Images/defaultTitleImage.png").save(&buffer2, "PNG");
        theme->metaData->titleThumbnailImage = (BYTE*)ba2.data();
        theme->metaData->titleThumbnailImageSize = ba2.length();

        theme->metaData->WriteMetaData();
        theme->Rehash();

        // inject the wallpapers
        injectImage(theme, &wallpaper1, "Wallpaper1");
        injectImage(theme, &wallpaper2, "Wallpaper2");
        injectImage(theme, &wallpaper3, "Wallpaper3");
        injectImage(theme, &wallpaper4, "Wallpaper4");

        // create the parameters.ini file
        QString paramsFilePath = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");
        QFile params(paramsFilePath);
        params.open(QIODevice::Truncate | QIODevice::WriteOnly);

        // Write the correct information to it
        QTextStream txtStream(&params);
        txtStream << "SphereColor=" << ui->cmbxSphereColor->currentIndex() << "\r\nAvatarLightingDirectional=0,0,0,0\r\nAvatarLightingAmbient=0\r\n";

        // close the file
        txtStream.flush();
        params.close();

        // inject the params file to the theme package
        theme->InjectFile(paramsFilePath.toStdString(), "parameters.ini");

        // create the dash style file
        QString dashStyleFilePath = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");
        FileIO ioD(dashStyleFilePath.toStdString(), true);
        ioD.Write((DWORD)0);
        ioD.Close();

        // inject the file
        theme->InjectFile(dashStyleFilePath.toStdString(), "DashStyle");

        // fix the package
        theme->Rehash();
        theme->Resign(QtHelpers::GetKVPath(theme->metaData->certificate.ownerConsoleType, this));

        // delete the temp files
        QFile::remove(paramsFilePath);
        QFile::remove(dashStyleFilePath);

        statusBar->showMessage("Theme created successfully", 3000);

        delete theme;
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Error", "An error occured while creating the theme.\n\n" + QString::fromStdString(error));
    }
}

void ThemeCreationWizard::injectImage(StfsPackage *theme, QImage *image, QString fileName)
{
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    image->scaled(1280, 720).save(&buffer, "JPG");

    theme->InjectData((BYTE*)ba.data(), ba.length(), fileName.toStdString());
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
            ui->lblSavePath->setText( QtHelpers::DesktopLocation().replace("\\", "/") + "/" + ui->txtName->text());
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

void ThemeCreationWizard::openWallpaper(QLabel *imageViewer, QImage *saveImage)
{
    QString fileName = QFileDialog::getOpenFileName(this, "Choose a wallpaper image", QtHelpers::DesktopLocation(), "Image Files (*.jpg *.jpeg *.png)");

    if (fileName == "")
        return;

    QPixmap *thumbnail = new QPixmap(fileName);
    if (thumbnail->isNull())
    {
        QMessageBox::warning(this, "Invalid Image", "The requested image could not be loaded successfully.");
        return;
    }

    *saveImage = QImage(fileName);
    allocatedImages.push_back(thumbnail);

    imageViewer->setPixmap(*thumbnail);
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

void ThemeCreationWizard::on_txtName_textChanged(const QString & /* arg1 */)
{
    button(QWizard::NextButton)->setEnabled(ui->txtName->text() != "");
}

void ThemeCreationWizard::on_pushButton_6_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Choose a place to save your theme", QtHelpers::DesktopLocation().replace("\\", "/") + "/" + ui->txtName->text());

    if (filePath == "")
        return;

    ui->lblSavePath->setText(filePath);
}

void ThemeCreationWizard::on_radioButton_clicked(bool checked)
{
    if (checked)
        consoleType = Retail;
}

void ThemeCreationWizard::on_radioButton_2_clicked(bool checked)
{
    if (checked)
        consoleType = DevKit;
}

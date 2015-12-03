#include "devicecontentviewer.h"
#include "ui_devicecontentviewer.h"

DeviceContentViewer::DeviceContentViewer(QStatusBar *statusBar, QWidget *parent) :
    statusBar(statusBar), currentPackage(NULL), QDialog(parent), ui(new Ui::DeviceContentViewer)
{
    ui->setupUi(this);

    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    progressBar = new QProgressBar(this);
    progressBar->setMaximumHeight(statusBar->height() - 5);
    progressBar->setMinimumWidth(statusBar->width());
    progressBar->setTextVisible(false);
    progressBar->setVisible(false);
    statusBar->addWidget(progressBar);
}

DeviceContentViewer::~DeviceContentViewer()
{
    for (int i = 0; i < devices.size(); i++)
        delete devices.at(i);

    delete ui;
}

void DeviceContentViewer::LoadDevices()
{
    // load all of the FATX drives as XContentDevices
    std::vector<FatxDrive*> drives = FatxDriveDetection::GetAllFatxDrives();
    for (int i = 0; i < drives.size(); i++)
    {
        XContentDevice *device = new XContentDevice(drives.at(i));
        if (!device->LoadDevice(DisplayProgress, this))
            continue;

        devices.push_back(device);
    }

    LoadDevicesp();
}

void DeviceContentViewer::LoadSharedItemCategory(QString category, std::vector<XContentDeviceSharedItem> *items, QTreeWidgetItem *parent, QString iconPath)
{
    QTreeWidgetItem *categoryItem = new QTreeWidgetItem(parent);
    categoryItem->setText(0, category);
    categoryItem->setIcon(0, QIcon(QPixmap(iconPath)));

    // load all the category's items
    for (int i = 0; i < items->size(); i++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(categoryItem);
        XContentDeviceItem content = items->at(i);

        item->setText(0, QString::fromStdWString(content.GetName()));

        item->setData(0, Qt::UserRole, QVariant::fromValue(content.content));
        item->setData(1, Qt::UserRole, QVariant(QString::fromStdString(content.GetPathOnDevice())));
        item->setData(2, Qt::UserRole, QVariant(QString::fromStdString(content.GetRawName())));

        // set the icon to the STFS package's thumbnail
        if (content.GetThumbnail() != NULL)
        {
            QByteArray imageBuff((char*)content.GetThumbnail(), content.GetThumbnailSize());
            item->setIcon(0, QIcon(QPixmap::fromImage(QImage::fromData(imageBuff))));
        }
        else
        {
            item->setIcon(0, QIcon(":/Images/watermark.png"));
        }
    }
}

void DeviceContentViewer::LoadDevicesp()
{
    ClearSidePanel();
    for (int i = 0; i < devices.size(); i++)
    {
        XContentDevice *device = devices.at(i);

        QTreeWidgetItem *deviceItem = new QTreeWidgetItem(ui->treeWidget);
        deviceItem->setText(0, QString::fromStdWString(device->GetName()));

        // set the appropriate icon
        if (device->GetDeviceType() == FatxHarddrive)
            deviceItem->setIcon(0, QIcon(QPixmap(":/Images/harddrive.png")));
        else
            deviceItem->setIcon(0, QIcon(QPixmap(":/Images/usb drive.png")));

        // load the profiles
        for (int x = 0; x < device->profiles->size(); x++)
        {
            QTreeWidgetItem *profileItem = new QTreeWidgetItem(deviceItem);
            XContentDeviceProfile profile = device->profiles->at(x);

            QString profileName = QString::fromStdWString(profile.GetName());
            if (profileName == "")
                profileName = "Unknown Profile";
            profileItem->setText(0, profileName);

            profileItem->setData(0, Qt::UserRole, QVariant::fromValue(profile.content));
            profileItem->setData(1, Qt::UserRole, QVariant(QString::fromStdString(profile.GetPathOnDevice())));
            profileItem->setData(2, Qt::UserRole, QVariant(QString::fromStdString(profile.GetRawName())));
            profileItem->setData(3, Qt::UserRole, QVariant((quint32)profile.GetFileSize()));

            // set the icon to the gamerpicture as long as it isn't null
            QByteArray imageBuff((char*)profile.GetThumbnail(), profile.GetThumbnailSize());
            QPixmap gamerThumb = QPixmap::fromImage(QImage::fromData(imageBuff));
            if (profile.GetThumbnail() != NULL && !gamerThumb.isNull())
                profileItem->setIcon(0, QIcon(gamerThumb));
            else
                profileItem->setIcon(0, QIcon(QPixmap(":/Images/HiddenAchievement.png")));

            // load all the titles for this profile
            for (int y = 0; y < profile.titles.size(); y++)
            {
                QTreeWidgetItem *titleItem = new QTreeWidgetItem(profileItem);
                XContentDeviceTitle title = profile.titles.at(y);

                titleItem->setText(0, QString::fromStdWString(title.GetName()));

                // set the title thumbnail as long as it isn't null, if it is then use a default image
                QByteArray imageBuff((char*)title.GetThumbnail(), title.GetThumbnailSize());
                QPixmap titleThumb = QPixmap::fromImage(QImage::fromData(imageBuff));
                if (title.GetThumbnail() != NULL && !titleThumb.isNull())
                    titleItem->setIcon(0, QIcon(titleThumb));
                else
                    titleItem->setIcon(0, QIcon(QPixmap(":/Images/watermark.png")));

                // load all the saves for this title
                for (int z = 0; z < title.titleSaves.size(); z++)
                {
                    QTreeWidgetItem *saveItem = new QTreeWidgetItem(titleItem);
                    XContentDeviceItem save = title.titleSaves.at(z);

                    saveItem->setData(0, Qt::UserRole, QVariant::fromValue(save.content));
                    saveItem->setData(1, Qt::UserRole, QVariant(QString::fromStdString(save.GetPathOnDevice())));
                    saveItem->setData(2, Qt::UserRole, QVariant(QString::fromStdString(save.GetRawName())));
                    saveItem->setData(3, Qt::UserRole, QVariant((unsigned int)save.GetFileSize()));

                    saveItem->setText(0, QString::fromStdWString(save.GetName()));

                    // set the icon to the STFS package's thumbnail as long as the image isn't null
                    QByteArray imageBuff((char*)save.GetThumbnail(), save.GetThumbnailSize());
                    QPixmap saveThumb = QPixmap::fromImage(QImage::fromData(imageBuff));
                    if (save.GetThumbnail() != NULL && !saveThumb.isNull())
                        saveItem->setIcon(0, QIcon(saveThumb));
                    else
                    {
                        profileItem->setIcon(0, QIcon(QPixmap(":/Images/watermark.png")));
                    }
                }
            }
        }

        QTreeWidgetItem *sharedItemFolder = new QTreeWidgetItem(deviceItem);
        sharedItemFolder->setText(0, "Shared Items");
        sharedItemFolder->setIcon(0, QIcon(QPixmap(":/Images/FolderFileIcon.png")));

        // load the shared items
        LoadSharedItemCategory("Games", device->games, sharedItemFolder, ":/Images/xboxcontroller.png");
        LoadSharedItemCategory("DLC", device->dlc, sharedItemFolder, ":/Images/xboxglobe.png");
        LoadSharedItemCategory("Demos", device->demos, sharedItemFolder, ":/Images/xboxcircles.png");
        LoadSharedItemCategory("Videos", device->videos, sharedItemFolder, ":/Images/film.png");
        LoadSharedItemCategory("Themes", device->themes, sharedItemFolder, ":/Images/thememedium.png");
        LoadSharedItemCategory("Gamer Pictures", device->gamerPictures, sharedItemFolder, ":/Images/gamerpicture.png");
        LoadSharedItemCategory("Avatar Items", device->avatarItems, sharedItemFolder, ":/Images/profile.png");
        LoadSharedItemCategory("System Items", device->systemItems, sharedItemFolder, ":/Images/preferences.png");
    }
}

void DeviceContentViewer::ClearSidePanel()
{
    ui->imgTumbnail->setPixmap(QPixmap(":/Images/watermark.png"));
    ui->imgTitleThumbnail->setPixmap(QPixmap(":/Images/watermark.png"));

    ui->lblRawName->setText("...");
    ui->lblTitleID->setText("...");
    ui->lblTitleName->setText("...");
    ui->lblPackageType->setText("...");
    ui->lblFileSize->setText("...");

    ui->btnOpenIn->setEnabled(false);
    ui->btnViewPackage->setEnabled(false);

    currentPackage = NULL;
}

void DeviceContentViewer::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    IXContentHeader *package = item->data(0, Qt::UserRole).value<IXContentHeader*>();
    if (package == NULL)
        return;

    StfsPackage *stfsPackage = reinterpret_cast<StfsPackage*>(package);
    if (package->metaData->contentType == Profile)
    {
        bool ok;
        ProfileEditor editor(statusBar, stfsPackage, false, &ok, this);
        if (ok)
            editor.exec();
    }
    else
    {
        PackageViewer viewer(statusBar, stfsPackage, QList<QAction*>(), QList<QAction*>(), this, false);
        viewer.exec();
    }
}

void DeviceContentViewer::showContextMenu(const QPoint &pos)
{
    // make sure that all the items the user has selected can be extracted
    bool canExtract = true;
    for (int i = 0; i < ui->treeWidget->selectedItems().size(); i++)
        if (ui->treeWidget->selectedItems().at(i)->data(1, Qt::UserRole).toString() == "")
            canExtract = false;

    QPoint globalPos = ui->treeWidget->mapToGlobal(pos);
    QMenu contextMenu;

    // if the user doesn't have any items selected, then we can't extract anything
    if (ui->treeWidget->selectedItems().size() != 0 && canExtract)
    {
        contextMenu.addAction(QPixmap(":/Images/extract.png"), "Copy Selected to Local Disk");
        contextMenu.addAction(QPixmap(":/Images/delete.png"), "Delete Selected Items");
    }

    contextMenu.addAction(QPixmap(":/Images/add.png"), "Copy File(s) Here");

    QAction *selectedItem = contextMenu.exec(globalPos);
    if (selectedItem == NULL)
        return;

    if (selectedItem->text() == "Copy Selected to Local Disk")
    {
        QString saveDir = QFileDialog::getExistingDirectory(this, "Choose a place to extract the files to...", QtHelpers::DesktopLocation());
        if (saveDir == "")
            return;

        QList<void*> filesToExtract;
        for (int i = 0; i < ui->treeWidget->selectedItems().size(); i++)
        {
            QString path = ui->treeWidget->selectedItems().at(i)->data(1, Qt::UserRole).toString();
            filesToExtract.push_back(new std::string(path.toStdString()));
        }

        MultiProgressDialog *dialog = new MultiProgressDialog(OpExtract, FileSystemFriendlyFATX, devices.at(0), saveDir, filesToExtract, this);
        dialog->setModal(true);
        dialog->show();
        dialog->start();

        // delete all of the allocated strings
        for (int i = 0; i < filesToExtract.size(); i++)
            delete (std::string*)filesToExtract.at(i);
    }
    else if (selectedItem->text() == "Copy File(s) Here")
    {
        QStringList files = QFileDialog::getOpenFileNames(this, "", QtHelpers::DesktopLocation());
        if (files.size() == 0)
            return;

        QList<void*> filesToInject;
        for (int i = 0; i < files.size(); i++)
            filesToInject.push_back(new std::string(files.at(i).toStdString()));

        MultiProgressDialog *dialog = new MultiProgressDialog(OpInject, FileSystemFriendlyFATX, devices.at(0), "", filesToInject, this);
        dialog->setModal(true);
        dialog->show();
        dialog->start();

        // delete all of the allocated strings
        for (int i = 0; i < filesToInject.size(); i++)
            delete (std::string*)filesToInject.at(i);

        ui->treeWidget->clear();
        LoadDevicesp();
    }
    else if (selectedItem->text() == "Delete Selected Items")
    {
        try
        {
            while (ui->treeWidget->selectedItems().size() > 0)
            {
                QTreeWidgetItem *selectedItem = ui->treeWidget->selectedItems().at(0);

                IXContentHeader *package = selectedItem->data(0, Qt::UserRole).value<IXContentHeader*>();
                std::string path = selectedItem->data(1, Qt::UserRole).toString().toStdString();

                devices.at(0)->DeleteFile(package, path);

                // remove the item from the tree widget
                delete selectedItem;
            }
        }
        catch (std::string error)
        {
            QMessageBox::critical(this, "Error", "An error occurred while deleting files.\n\n" + QString::fromStdString(error));
        }
    }
}

void DeviceContentViewer::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if (current == NULL)
        return;

    IXContentHeader *package = current->data(0, Qt::UserRole).value<IXContentHeader*>();
    if (package == NULL)
    {
        ClearSidePanel();
        return;
    }

    // load the thumbnail image, if it's null then load a default image
    QByteArray thumbnailBuff((char*)package->metaData->thumbnailImage, package->metaData->thumbnailImageSize);
    QPixmap thumbnailImage = QPixmap::fromImage(QImage::fromData(thumbnailBuff));
    if (package->metaData->thumbnailImage != NULL && !thumbnailImage.isNull())
        ui->imgTumbnail->setPixmap(thumbnailImage);
    else
        ui->imgTumbnail->setPixmap(QPixmap(":/Images/watermark.png"));


    // load the title thumbnail image, if it's null then load a default image
    QByteArray titleThumbnailBuff((char*)package->metaData->titleThumbnailImage, package->metaData->titleThumbnailImageSize);
    QPixmap titleThumbnailImage = QPixmap::fromImage(QImage::fromData(titleThumbnailBuff));
    if (package->metaData->titleThumbnailImage != NULL && !titleThumbnailImage.isNull())
        ui->imgTitleThumbnail->setPixmap(titleThumbnailImage);
    else
        ui->imgTitleThumbnail->setPixmap(QPixmap(":/Images/watermark.png"));

    // since the name could be longer than the label, we're going to do some work so
    // that instead of making the side panel huge, we get ... at the end of the string
    // and it cuts the rest off
    QFontMetrics fm = QFontMetrics(ui->lblRawName->font());
    QString fittedRawName = fm.elidedText(current->data(2, Qt::UserRole).toString(), Qt::ElideRight, ui->lblRawName->width());
    ui->lblRawName->setText(fittedRawName);


    ui->lblTitleID->setText(QString::number(package->metaData->titleID, 16).toUpper());
    ui->lblTitleName->setText(QString::fromStdWString(package->metaData->titleName));
    ui->lblPackageType->setText(QString::fromStdString(ContentTypeToString(package->metaData->contentType)));
    ui->lblFileSize->setText(QString::fromStdString(ByteSizeToString(current->data(3, Qt::UserRole).toUInt())));

    currentPackage = package;

    ui->btnViewPackage->setEnabled(true);
}

void DeviceContentViewer::on_btnViewPackage_clicked()
{
    if (currentPackage == NULL)
    {
        ui->btnViewPackage->setEnabled(false);
        return;
    }

    IXContentHeader *content = ui->treeWidget->currentItem()->data(0, Qt::UserRole).value<IXContentHeader*>();

    if (content->metaData->fileSystem == FileSystemSTFS)
    {
        StfsPackage *stfsPackage = reinterpret_cast<StfsPackage*>(currentPackage);
        PackageViewer viewer(statusBar, stfsPackage, QList<QAction*>(), QList<QAction*>(), this, false);
        viewer.exec();
    }
    else if (content->metaData->fileSystem == FileSystemSVOD)
    {
        SVOD *svodSystem = reinterpret_cast<SVOD*>(currentPackage);
        SvodDialog viewer(svodSystem, statusBar, this);
        viewer.exec();
    }
}

void DeviceContentViewer::resizeEvent(QResizeEvent *)
{
    if (currentPackage == NULL || ui->treeWidget->currentItem() == NULL)
        return;

    // we need to re-calculate the string so that it fits properly in the newly sized form
    QFontMetrics fm = QFontMetrics(ui->lblRawName->font());
    QString fittedRawName = fm.elidedText(ui->treeWidget->currentItem()->data(2, Qt::UserRole).toString(), Qt::ElideRight, ui->frame->width());
    ui->lblRawName->setText(fittedRawName);
}

void DisplayProgress(void *arg, bool finished)
{
    DeviceContentViewer *contentViewer = static_cast<DeviceContentViewer*>(arg);
    if (!finished)
    {
        contentViewer->progressBar->setVisible(true);
        contentViewer->progressBar->setMinimum(0);
        contentViewer->progressBar->setMaximum(0);
        contentViewer->setWindowTitle("Device Content Viewer - Loading Device(s)");
    }
    else
    {
        contentViewer->progressBar->setVisible(false);
        contentViewer->progressBar->setMaximum(1);
        contentViewer->ui->treeWidget->setEnabled(true);
        contentViewer->setWindowTitle("Device Content Viewer");
    }
    QApplication::processEvents();
}

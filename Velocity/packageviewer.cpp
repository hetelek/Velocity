#include "packageviewer.h"
#include "ui_packageviewer.h"

PackageViewer::PackageViewer(QStatusBar *statusBar, StfsPackage *package, QList<QAction *> gpdActions, QList<QAction *> gameActions, QWidget *parent, bool disposePackage) :
    QDialog(parent),ui(new Ui::PackageViewer), package(package), disposePackage(disposePackage), parent (parent), statusBar(statusBar),  gpdActions(gpdActions),
    gameActions(gameActions), openInMenu(NULL)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    QtHelpers::GenAdjustWidgetAppearanceToOS(this);

    ui->treeWidget->header()->setDefaultSectionSize(75);
    ui->treeWidget->header()->resizeSection(0, 200);
    ui->treeWidget->header()->resizeSection(2, 100);

    ui->imgTile->setPixmap(QPixmap());

    // load all of the data
    if (package->IsPEC())
    {
        ui->lblMagic->setText("Magic: <i>N/A</i>");
        ui->lblDisplayName->setText("Display Name: <i>N/A</i>");
        ui->lblTitleName->setText("Title ID: <i>N/A</i>");
        ui->lblType->setText("Type: PEC");

        ui->txtDeviceID->setEnabled(false);
        ui->txtProfileID->setEnabled(false);
    }
    else
    {
        ui->lblMagic->setText("Magic: 0x" + QString::number(package->metaData->magic, 16).toUpper() + " (" + QString::fromStdString(MagicToString(package->metaData->magic)) + ")");
        ui->lblDisplayName->setText("Display Name: " + QString::fromStdWString(package->metaData->displayName));

        if (package->metaData->magic == LIVE || package->metaData->magic == PIRS)
            ui->btnFix->setText("Rehash");

        try
        {
            ui->lblType->setText(QString::fromStdString("Package Type: " + ContentTypeToString(package->metaData->contentType)));
        }
        catch (...)
        {
            ui->lblType->setText("Package Type: <i>Unknown</i>");
        }
        ui->lblTitleName->setText("Title ID: " + QString::number(package->metaData->titleID, 16).toUpper());

        QString builder = "";
        for (int i = 0; i < 8; i++)
            builder += QString("%1").arg((BYTE)package->metaData->profileID[i], 2, 16, QChar('0')).toUpper();

        ui->txtProfileID->setText(builder.toUpper());

        builder = "";
        for (int i = 0; i < 20; i++)
            builder += QString("%1").arg((BYTE)package->metaData->deviceID[i], 2, 16, QChar('0')).toUpper();

        ui->txtDeviceID->setText(builder.toUpper());

        // set the thumbnail
        QByteArray imageBuff((char*)package->metaData->thumbnailImage, (size_t)package->metaData->thumbnailImageSize);
        ui->imgTile->setPixmap(QPixmap::fromImage(QImage::fromData(imageBuff)));

        if (package->metaData->magic != CON)
        {
            ui->txtProfileID->setEnabled(false);
            ui->txtDeviceID->setEnabled(false);
        }

        openInMenu = new QMenu(this);
        connect(openInMenu, SIGNAL(aboutToShow()), this, SLOT(aboutToShow()));

        if (package->metaData->contentType == Profile)
        {
            profileEditor = new QAction("Profile Editor", this);
            gameAdder = new QAction("Game Adder", this);

            connect(openInMenu, SIGNAL(triggered(QAction*)), this, SLOT(onOpenInSelected(QAction*)));

            openInMenu->addAction(profileEditor);
            openInMenu->addAction(gameAdder);
        }
    }

    listing = package->GetFileListing();
    PopulateTreeWidget(&listing);

    if (package->metaData->contentType == SavedGame)
    {
        for (int x = 0; x < gameActions.size(); x++)
        {
            DWORD titleId = gameActions.at(x)->property("titleid").toUInt();
            if (titleId == package->metaData->titleID)
            {
                gameActions.at(x)->setProperty("package", QVariant::fromValue(package));
                openInMenu->addAction(gameActions.at(x));
            }
        }
    }


    if (openInMenu != NULL && openInMenu->actions().size() > 0)
    {
        ui->btnOpenIn->setEnabled(true);
        ui->btnOpenIn->setMenu(openInMenu);
    }

    // setup the context menus
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showRemoveContextMenu(QPoint)));

    ui->imgTile->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->imgTile, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showSaveImageContextMenu(QPoint)));
}

PackageViewer::~PackageViewer()
{
    for (int i = 0; i < gpdActions.size(); i++)
        if (gpdActions.at(i)->property("package").isValid())
            gpdActions.at(i)->setProperty("package", QVariant::Invalid);

    for (int i = 0; i < gameActions.size(); i++)
        if (gameActions.at(i)->property("package").isValid())
            gameActions.at(i)->setProperty("package", QVariant::Invalid);

    if (disposePackage)
    {
        package->Close();
        delete package;
    }
    delete ui;
}

void PackageViewer::PopulateTreeWidget(StfsFileListing *entry, QTreeWidgetItem *parent)
{
    bool isRootEntry = entry->folder.entryIndex == 0xFFFF;

    for (unsigned int i = 0; i < entry->folderEntries.size(); i++)
    {
        QTreeWidgetItem *folder;
        if (entry->folderEntries.at(i).folder.pathIndicator != 0xFFFF)
            folder = new QTreeWidgetItem(parent);
        else
            folder = new QTreeWidgetItem(ui->treeWidget);

        folder->setText(0, QString::fromStdString(entry->folderEntries.at(i).folder.name));
        folder->setIcon(0, QIcon(":/Images/FolderFileIcon.png"));
        folder->setText(1, "0 bytes");
        folder->setText(2, "N/A");
        folder->setText(3, "N/A");

        PopulateTreeWidget(&entry->folderEntries.at(i), folder);
    }

    // add all files
    for (DWORD i = 0; i < entry->fileEntries.size(); i++)
    {
        QTreeWidgetItem *fileEntry;
        if (!isRootEntry)
            fileEntry = new QTreeWidgetItem(parent);
        else
            fileEntry = new QTreeWidgetItem(ui->treeWidget);

        QString path;
        GetPackagePath(fileEntry, &path);
        SetIcon(entry->fileEntries.at(i).name, &entry->fileEntries.at(i), fileEntry);

        QString name = QString::fromStdString(entry->fileEntries.at(i).name);
        fileEntry->setText(0, name);
        fileEntry->setText(1, QString::fromStdString(ByteSizeToString(entry->fileEntries.at(i).fileSize)));
        fileEntry->setText(2, "0x" + QString::number(package->BlockToAddress(entry->fileEntries.at(i).startingBlockNum), 16).toUpper());
        fileEntry->setText(3, "0x" + QString::number(entry->fileEntries.at(i).startingBlockNum, 16).toUpper());

        if (!package->IsPEC() && package->metaData->contentType == Profile)
        {
            for (int x = 0; x < gpdActions.size(); x++)
            {
                if (QString::number(gpdActions.at(x)->property("titleid").toUInt(), 16).toUpper() + ".Gpd" == name.toUpper())
                {
                    gpdActions.at(x)->setProperty("package", QVariant::fromValue(package));
                    openInMenu->addAction(gpdActions.at(x));
                }
            }
        }
    }
}

void PackageViewer::GetPackagePath(QTreeWidgetItem *item, QString *out, bool folderOnly)
{
    bool hasParent = item->parent() != NULL;

    if (!hasParent && folderOnly)
        return;

    QString slash = "";
    if (hasParent)
        slash = "\\";

    out->push_front(slash + item->text(0));

    if (hasParent)
        GetPackagePath(item->parent(), out);
}

void PackageViewer::SetIcon(string name, StfsFileEntry *entry, QTreeWidgetItem *item)
{
    try
    {
        QIcon toSet;
        QtHelpers::GetFileIcon(package->GetFileMagic(*entry), QString::fromStdString(name), toSet, *item);
        item->setIcon(0, toSet);
    }
    catch (string error)
    {
        QMessageBox::critical(this, "", QString::fromStdString(error));
    }
}

void PackageViewer::on_btnFix_clicked()
{
    bool success = true, resigned = false;

    // verify the profile and device id
    if ((!QtHelpers::VerifyHexStringBuffer(ui->txtProfileID->text()) || ui->txtProfileID->text().length() != 16) && !package->IsPEC())
    {
        QMessageBox::warning(this, "Invalid Value", "The profile ID must be 16 hexadecimal digits.");
        return;
    }
    if ((!QtHelpers::VerifyHexStringBuffer(ui->txtDeviceID->text()) || ui->txtDeviceID->text().length() != 40) && !package->IsPEC())
    {
        QMessageBox::warning(this, "Invalid Value", "The device ID must be 40 hexadecimal digits.");
        return;
    }

    // save the ids if it's not PEC
    if (!package->IsPEC())
    {
        QtHelpers::ParseHexStringBuffer(ui->txtProfileID->text(), package->metaData->profileID, 8);
        QtHelpers::ParseHexStringBuffer(ui->txtDeviceID->text(), package->metaData->deviceID, 20);
    }

    try
    {
        package->Rehash();
    }
    catch (string error)
    {
        success = false;
        QMessageBox::critical(this, "Error", "Failed to rehash the package.\n\n" + QString::fromStdString(error));
    }

    if (package->metaData->magic == CON || package->IsPEC())
    {
        try
        {
            string path = QtHelpers::GetKVPath(package->metaData->certificate.ownerConsoleType, this);

            if (path != "")
            {
                package->Resign(path);
                resigned = true;
            }
        }
        catch (string error)
        {
            success = false;
            QMessageBox::critical(this, "Error", "Failed to resign the package.\n\n" + QString::fromStdString(error));
        }
    }

    if (success)
    {
        QString resignedStr = (resigned ? " and resigned" : "");
        statusBar->showMessage("Rehashed" + resignedStr + " successfully", 3000);
        QMessageBox::information(this, "Success", "The package has successfully been rehashed" + resignedStr + ".");
    }
}

void PackageViewer::on_btnViewAll_clicked()
{
    Metadata meta(statusBar, package->metaData, package->IsPEC(), this);
    meta.exec();

    if (package->metaData->magic == LIVE || package->metaData->magic == PIRS)
        ui->btnFix->setText("Rehash");
    else
        ui->btnFix->setText("Rehash/Resign");
}

void PackageViewer::showSaveImageContextMenu(QPoint point)
{
    if (ui->imgTile->pixmap()->isNull())
        return;

    QPoint globalPos = ui->imgTile->mapToGlobal(point);
    QMenu contextMenu;

    contextMenu.addAction(QPixmap(":/Images/save.png"), "Save Image");
    QAction *selectedItem = contextMenu.exec(globalPos);

    if (selectedItem == NULL)
        return;
    else if (selectedItem->text() == "Save Image")
    {
        QString imageSavePath = QFileDialog::getSaveFileName(this, "Choose a location to save the thumbnail", QtHelpers::DesktopLocation() + "\\thumbnail.png", "*.png");
        if (imageSavePath == "")
            return;

        ui->imgTile->pixmap()->save(imageSavePath, "PNG");

        statusBar->showMessage("Thumbnail image saved successfully", 3000);
    }
}

void PackageViewer::on_txtSearch_textChanged(const QString &arg1)
{
    QtHelpers::SearchTreeWidget(ui->treeWidget, ui->txtSearch, arg1);
}

void PackageViewer::onOpenInSelected(QAction *action)
{
    if (action)
    {
        if (action == gameAdder)
        {
            bool ok;
            GameAdderDialog dialog(package, this, false, &ok);
            if (ok)
            {
                dialog.exec();

                ui->treeWidget->clear();
                listing = package->GetFileListing();
                PopulateTreeWidget(&listing);
            }
        }
        else if (action == profileEditor)
        {
            ProfileEditor editor(statusBar, package, false, this);

            if(editor.isOk())
                editor.exec();
        }
    }
}

void PackageViewer::aboutToShow()
{
    for (int i = 0; i < gpdActions.size(); i++)
        gpdActions.at(i)->setProperty("fromPackageViewer", QVariant(true));
    for (int i = 0; i < gameActions.size(); i++)
        gameActions.at(i)->setProperty("fromPackageViewer", QVariant(true));
}

void PackageViewer::GetSubFilesStfs(StfsFileListing *parent, QList<void *> &entries, QString currentPath)
{
    for (size_t i = 0; i < parent->fileEntries.size(); i++)
    {
        StfsExtractEntry *exEntry = new StfsExtractEntry;
        exEntry->entry = &parent->fileEntries.at(i);
        exEntry->path = currentPath;
        entries.push_back(exEntry);
    }

    for (size_t i = 0; i < parent->folderEntries.size(); i++)
        GetSubFilesStfs(&parent->folderEntries.at(i), entries, currentPath + QString::fromStdString(parent->folderEntries.at(i).folder.name) + "/");
}

void PackageViewer::showRemoveContextMenu(QPoint point)
{
    int amount = ui->treeWidget->selectedItems().length();

    QPoint globalPos = ui->treeWidget->mapToGlobal(point);
    QMenu contextMenu;

    contextMenu.addAction(QPixmap(":/Images/extract.png"), "Extract All");
    contextMenu.addSeparator();

    bool isFolder = false;
    if (amount == 0)
    {
        contextMenu.addAction(QPixmap(":/Images/add.png"), "Inject Here");
    }
    else
    {
        isFolder = ui->treeWidget->selectedItems()[0]->text(3) == "N/A";

        if (!isFolder)
        {
            contextMenu.addAction(QPixmap(":/Images/extract.png"), "Extract Selected");
            contextMenu.addAction(QPixmap(":/Images/delete.png"), "Remove Selected");
        }

        if (amount == 1)
        {
            contextMenu.addSeparator();
            contextMenu.addAction(QPixmap(":/Images/rename.png"), "Rename Selected");

            if (!isFolder)
                contextMenu.addAction(QPixmap(":/Images/replace.png"), "Replace File");
            else
            {
                contextMenu.addSeparator();
                contextMenu.addAction(QPixmap(":/Images/add.png"), "Inject Here");
            }
        }

        contextMenu.addSeparator();
        contextMenu.addAction(QPixmap(":/Images/properties.png"), "View Properties");
    }

    QAction *selectedItem = contextMenu.exec(globalPos);
    if(selectedItem == NULL)
        return;

    QList <QTreeWidgetItem*> items = ui->treeWidget->selectedItems();
    int totalCount = items.count(), successCount = 0;

    if (selectedItem->text() == "Extract Selected")
    {
        bool multiple = totalCount > 1;

        QString path;
        if (multiple)
            path = QFileDialog::getExistingDirectory(this, "Save Location", QtHelpers::DesktopLocation()) + "/";
        else
            path = QFileDialog::getSaveFileName(this, "Save Location", QtHelpers::DesktopLocation() + "/" + ui->treeWidget->selectedItems()[0]->text(0));

        if (path.isEmpty())
            return;


        QList<void*> outFiles;
        QList<StfsFileEntry> sillyNess;
        for (int i = 0; i < totalCount; i++)
        {
            QString packagePath;
            GetPackagePath(items.at(i), &packagePath);

            StfsExtractEntry *entry = new StfsExtractEntry;
            entry->path = "";

            sillyNess.append(package->GetFileEntry(packagePath.toStdString()));
            entry->entry = const_cast<StfsFileEntry*>(&sillyNess.at(i));
            outFiles.append(const_cast<void*>((void*)entry));
        }

        try
        {
            MultiProgressDialog *dialog = new MultiProgressDialog(OpExtract, FileSystemSTFS, package, path.replace("\\", "/"), outFiles, this);
            dialog->setModal(true);
            dialog->show();
            dialog->start();

            successCount++;
        }
        catch (string error)
        {
            QMessageBox::critical(this, "Error", "Failed to extract file.\n\n" + QString::fromStdString(error));
            return;
        }

        statusBar->showMessage("Selected files extracted successfully", 3000);

    }
    else if (selectedItem->text() == "Extract All")
    {
        QString path = QFileDialog::getExistingDirectory(this, "Save Location", QtHelpers::DesktopLocation()) + "/";

        QList<void*> entries;
        GetSubFilesStfs(&listing, entries);

        try
        {
            MultiProgressDialog *dialog = new MultiProgressDialog(OpExtract, FileSystemSTFS, package, path.replace("\\", "/"), entries, this);
            dialog->setModal(true);
            dialog->show();
            dialog->start();
        }
        catch (string error)
        {
            QMessageBox::critical(this, "Error", "Failed to extract file.\n\n" + QString::fromStdString(error));
            return;
        }

        statusBar->showMessage("All files extracted successfully", 3000);
    }
    else if (selectedItem->text() == "Remove Selected")
    {
        for (int i = 0; i < totalCount; i++)
        {
            QString packagePath;
            GetPackagePath(items.at(i), &packagePath);
            try
            {
                package->RemoveFile(packagePath.toStdString());
                delete items.at(i);
                successCount++;
            }
            catch (string error)
            {
                QMessageBox::critical(this, "Error", "Failed to delete file.\n\n" + QString::fromStdString(error));
            }
        }

        if (successCount == totalCount)
            statusBar->showMessage("Selected files removed successfully", 3000);
        else
            QMessageBox::warning(this, "Warning", "Some files could not be deleted.\n\nSuccessful Deletions: " + QString::number(successCount) + " / " + QString::number(totalCount));
    }
    else if (selectedItem->text() == "Rename Selected")
    {
        QString packagePath;
        GetPackagePath(items.at(0), &packagePath);
        try
        {
            string newName = items.at(0)->text(0).toStdString();

            RenameDialog dialog(this, &newName);
            dialog.exec();

            if (newName == items.at(0)->text(0).toStdString())
                return;

            package->RenameFile(newName, packagePath.toStdString());
            items.at(0)->setText(0, QString::fromStdString(newName));
            statusBar->showMessage("File renamed successfully", 3000);
        }
        catch (string error)
        {
            QMessageBox::critical(this, "Error", "Failed to rename file.\n\n" + QString::fromStdString(error));
        }
    }
    else if (selectedItem->text() == "Replace File")
    {
        QString packagePath;
        GetPackagePath(items.at(0), &packagePath);

        QString path = QFileDialog::getOpenFileName(this, "New File", QtHelpers::DesktopLocation() + "/" + ui->treeWidget->selectedItems()[0]->text(0));
        if (path.isEmpty())
            return;

        try
        {
            SingleProgressDialog *dialog = new SingleProgressDialog(FileSystemSTFS, package, OpReplace, packagePath, path, NULL, this);
            dialog->setModal(true);
            dialog->show();
            dialog->start();

        }
        catch(string error)
        {
            QMessageBox::critical(this, "Error", "Failed to replace file.\n\n" + QString::fromStdString(error));
        }

        statusBar->showMessage("File replaced successfully", 3000);
    }
    else if (selectedItem->text() == "Inject Here")
    {
        QString packagePath;

        QString path = QFileDialog::getOpenFileName(this, "New File", QtHelpers::DesktopLocation());
        if (path.isEmpty())
            return;

        if (totalCount == 1)
        {
            GetPackagePath(items.at(0), &packagePath, true);
            packagePath += "\\";

            if (isFolder && items.at(0)->parent() == NULL)
                packagePath.append(items.at(0)->text(0) + "\\");
        }

        QFileInfo pathInfo(path);
        packagePath.append(pathInfo.fileName());

        try
        {
            StfsFileEntry *injectedEntry = new StfsFileEntry;
            SingleProgressDialog *dialog = new SingleProgressDialog(FileSystemSTFS, package, OpInject, packagePath, path, injectedEntry, this);
            dialog->setModal(true);
            dialog->show();
            dialog->start();

            listing = package->GetFileListing();

            QTreeWidgetItem *fileEntry;
            if (injectedEntry->pathIndicator != 0xFFFF)
                fileEntry = new QTreeWidgetItem(items.at(0));
            else
                fileEntry = new QTreeWidgetItem(ui->treeWidget);

            QString filePath;
            GetPackagePath(fileEntry, &filePath);
            SetIcon(injectedEntry->name, injectedEntry, fileEntry);

            fileEntry->setText(0, QString::fromStdString(injectedEntry->name));
            fileEntry->setText(1, QString::fromStdString(ByteSizeToString(injectedEntry->fileSize)));
            fileEntry->setText(2, "0x" + QString::number(package->BlockToAddress(injectedEntry->startingBlockNum), 16).toUpper());
            fileEntry->setText(3, "0x" + QString::number(injectedEntry->startingBlockNum, 16).toUpper());

            statusBar->showMessage("File injected successfully", 3000);
        }
        catch(string error)
        {
            QMessageBox::critical(this, "Error", "Failed to replace file.\n\n" + QString::fromStdString(error));
        }
    }
    else if (selectedItem->text() == "View Properties")
    {
        try
        {
            QString packagePath;
            GetPackagePath(items.at(0), &packagePath);

            StfsFileEntry entry = package->GetFileEntry(packagePath.toStdString(), true);
            bool folder = entry.flags & 2;

            bool changed = false;
            PropertiesDialog dialog(&entry, packagePath, &changed, items.at(0)->icon(0), items.at(0)->childCount() != 0, this);
            dialog.exec();

            if (changed)
            {
                package->GetFileEntry(packagePath.toStdString(), true, &entry);
                listing = package->GetFileListing();

                items.at(0)->setText(0, QString::fromStdString(entry.name));

                bool newStatus = (entry.flags & 2);
                if (folder != newStatus)
                {
                    if (newStatus)
                    {
                        items.at(0)->setIcon(0, QIcon(":/Images/FolderFileIcon.png"));
                        items.at(0)->setText(1, "0");
                        items.at(0)->setText(2, "N/A");
                        items.at(0)->setText(3, "N/A");
                    }
                    else
                    {
                        QString filePath;
                        GetPackagePath(items.at(0), &filePath);
                        SetIcon(entry.name, &entry, items.at(0));
                        items.at(0)->setText(0, QString::fromStdString(entry.name));
                        items.at(0)->setText(1, "0x" + QString::number(entry.fileSize, 16).toUpper());
                        items.at(0)->setText(2, "0x" + QString::number(package->BlockToAddress(entry.startingBlockNum), 16).toUpper());
                        items.at(0)->setText(3, "0x" + QString::number(entry.startingBlockNum, 16).toUpper());
                    }
                }
            }
        }
        catch(string error)
        {
            QMessageBox::critical(this, "Error", "Failed to open file.\n\n" + QString::fromStdString(error));
        }
    }
}

void PackageViewer::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int /*column*/)
{
    // make sure that the item clicked isn't a folder
    if (item->text(2) == "N/A")
        return;

    // make sure the file double clicked on is a gpd
    int index = item->text(0).lastIndexOf(".");
    QString extension;
    if (index != -1)
        extension = item->text(0).mid(index).toLower();

    if (item->data(1, Qt::UserRole).toString() == "Xdbf")
    {
        try
        {
            // get the path of the file in the package
            QString packagePath;
            GetPackagePath(item, &packagePath);

            // verify the magic
            if (package->GetFileMagic(packagePath.toStdString()) != 0x58444246)
            {
                statusBar->showMessage("Invalid Gpd", 3000);
                return;
            }

            // get a temporary file name
            QString tempName = (QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", ""));
            string tempNameStd = tempName.toStdString();

            // extract the file to a temporary location
            package->ExtractFile(packagePath.toStdString(), tempNameStd);

            // parse the gpd
            GpdBase *gpd = new GpdBase(tempNameStd);
            statusBar->showMessage("Gpd parsed successfully", 3000);

            bool changed;
            XdbfDialog dialog(statusBar, gpd, &changed, this);
            dialog.exec();

            if(changed)
                package->ReplaceFile(tempNameStd, packagePath.toStdString());

            gpd->Close();

            // delete the temp file
            QFile::remove(tempName);
        }
        catch(string error)
        {
            QMessageBox::critical(this, "Error", "Failed to open the Gpd.\n\n" + QString::fromStdString(error));
        }
    }
    else if (item->data(1, Qt::UserRole).toString() == "STRB")
    {
        // get the path of the file in the package
        QString packagePath;
        GetPackagePath(item, &packagePath);

        // verify the magic
        if (package->GetFileMagic(packagePath.toStdString()) != 0x53545242)
            return;

        // get a temporary file name
        string tempName = (QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "")).toStdString();

        // extract the file to a temporary location
        package->ExtractFile(packagePath.toStdString(), tempName);

        // show the avatar asset dialog
        AvatarAsset *asset = new AvatarAsset(tempName);
        statusBar->showMessage("STRB file parsed successfully", 3000);

        StrbDialog dialog(asset, this);
        dialog.exec();

        // delete the temp file
        remove(tempName.c_str());
    }
    else if (item->data(1, Qt::UserRole).toString() == "Image")
    {
        // get a temporary file name
        string tempName = (QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "")).toStdString();

        // extract the file to a temporary location
        QString imagePath;
        GetPackagePath(item, &imagePath);

        package->ExtractFile(imagePath.toStdString(), tempName);

        // display the image
        QImage image(QString::fromStdString(tempName));
        if (image.isNull())
            return;

        ImageDialog dialog(image, this);
        dialog.exec();

        // delete the temp file
        QFile::remove(QString::fromStdString(tempName));
    }
    else if (item->data(1, Qt::UserRole).toString() == "PEC")
    {
        try
        {
            // get a temporary file name
            string tempName = (QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "")).toStdString();

            // extract the file to a temporary location
            QString packagePath;
            GetPackagePath(item, &packagePath);

            package->ExtractFile(packagePath.toStdString(), tempName);

            StfsPackage pec (tempName, StfsPackagePEC);
            PackageViewer dialog(statusBar, &pec, gpdActions, gameActions, this, false);
            dialog.exec();

            pec.Close();

            // replace the PEC with the modified one
            package->ReplaceFile(tempName, packagePath.toStdString());

            // delete the temp file
            remove(tempName.c_str());
        }
        catch(string error)
        {
            QMessageBox::critical(this, "Error", "Failed to open PEC file.\n\n" + QString::fromStdString(error));
        }
    }
    else if (item->data(1, Qt::UserRole).toString() == "STFS")
    {
        QString packagePath;
        GetPackagePath(item, &packagePath);

        try
        {
            // get a temporary file name
            string tempName = (QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "")).toStdString();

            // extract the file to a temporary location
            package->ExtractFile(packagePath.toStdString(), tempName);

            StfsPackage pack(tempName);
            PackageViewer dialog(statusBar, &pack, gpdActions, gameActions, this, false);
            dialog.exec();

            pack.Close();

            // replace
            package->ReplaceFile(tempName, packagePath.toStdString());

            // delete the temporary file
            QFile::remove(QString::fromStdString(tempName));
        }
        catch(string error)
        {
            QMessageBox::critical(this, "Error", "Failed to open Stfs Package.\n\n" + QString::fromStdString(error));
        }
    }
    else if (item->data(1, Qt::UserRole).toString() == "XEX")
    {
        QString packagePath;
        GetPackagePath(item, &packagePath);

        try
        {
            StfsIO *io = package->GetStfsIO(packagePath.toStdString());
            Xbox360Executable *xex = new Xbox360Executable(io);

            // the dialog will free xex
            XexDialog dialog(xex, this);
            dialog.exec();
        }
        catch(string error)
        {
            QMessageBox::critical(this, "Error", "Failed to open XEX.\n\n" + QString::fromStdString(error));
        }
    }
}

void PackageViewer::on_btnStfsTools_clicked()
{
    StfsToolsDialog dialog(package, this);
    dialog.exec();
}

void PackageViewer::on_btnShowAll_clicked()
{
    ui->txtSearch->setText("");
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); i++)
        QtHelpers::ShowAllItems(ui->treeWidget->topLevelItem(i));
}

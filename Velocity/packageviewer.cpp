#include "packageviewer.h"
#include "ui_packageviewer.h"

PackageViewer::PackageViewer(StfsPackage *package, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PackageViewer),
    package(package),
    parent (parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    disposePackage = true;
    ui->treeWidget->header()->setDefaultSectionSize(75);
    ui->treeWidget->header()->resizeSection(0, 200);
    ui->treeWidget->header()->resizeSection(2, 100);

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
        ui->lblMagic->setText(QString::fromStdString("Magic: " + MagicToString(package->metaData->magic)));
        ui->lblDisplayName->setText("Display Name: " + QString::fromStdWString(package->metaData->displayName));

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

        if (package->metaData->contentType == Profile)
            ui->btnProfileEditor->setEnabled(true);
    }

    listing = package->GetFileListing();
    PopulateTreeWidget(&listing);

    // setup the context menus
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showRemoveContextMenu(QPoint)));

    ui->imgTile->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->imgTile, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showSaveImageContextMenu(QPoint)));
}

PackageViewer::~PackageViewer()
{
    if (disposePackage)
        delete package;
    delete ui;
}

void PackageViewer::PopulateTreeWidget(FileListing *entry, QTreeWidgetItem *parent)
{
    QTreeWidgetItem *item;

    bool isRootEntry = entry->folder.entryIndex == 0xFFFF;
    bool isRoot = entry->folder.pathIndicator == 0xFFFF;

    if (!isRootEntry)
    {
        if (!isRoot)
            item = new QTreeWidgetItem(parent);
        else
            item = new QTreeWidgetItem(ui->treeWidget);

        item->setText(0, QString::fromStdString(entry->folder.name));
        item->setIcon(0, QIcon(":/Images/FolderFileIcon.png"));
        item->setText(1, "0");
        item->setText(2, "N/A");
        item->setText(3, "N/A");
    }

    for (DWORD i = 0; i < entry->fileEntries.size(); i++)
    {
        QTreeWidgetItem *fileEntry;
        if (!isRootEntry)
            fileEntry = new QTreeWidgetItem(item);
        else
            fileEntry = new QTreeWidgetItem(ui->treeWidget);

        SetIcon(entry->fileEntries.at(i).name, fileEntry);

        fileEntry->setText(0, QString::fromStdString(entry->fileEntries.at(i).name));
        fileEntry->setText(1, "0x" + QString::number(entry->fileEntries.at(i).fileSize, 16).toUpper());
        fileEntry->setText(2, "0x" + QString::number(package->BlockToAddress(entry->fileEntries.at(i).startingBlockNum), 16).toUpper());
        fileEntry->setText(3, "0x" + QString::number(entry->fileEntries.at(i).startingBlockNum, 16).toUpper());
    }

    for (int i = 0; i < entry->folderEntries.size(); i++)
        PopulateTreeWidget(&entry->folderEntries.at(i), item);
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

void PackageViewer::SetIcon(string name, QTreeWidgetItem *item)
{
    DWORD index = name.rfind(".");
    string extension = "";
    if (index != string::npos)
        extension = name.substr(index);

    if (extension == ".gpd" || extension == ".fit")
        item->setIcon(0, QIcon(":/Images/GpdFileIcon.png"));
    else if (name == "Account")
        item->setIcon(0, QIcon(":/Images/AccountFileIcon.png"));
    else if (name == "PEC")
        item->setIcon(0, QIcon(":/Images/PecFileIcon.png"));
    else if (name == "Account")
        item->setIcon(0, QIcon(":/Images/AccountFileIcon.png"));
    else if (extension == ".png" || extension == ".jpg" || extension == ".jpeg")
        item->setIcon(0, QIcon(":/Images/ImageFileIcon.png"));
    else
        item->setIcon(0, QIcon(":/Images/DefaultFileIcon.png"));
}

void PackageViewer::on_btnFix_clicked()
{
    bool success = true, resigned = false;
    try
    {
        package->Rehash();
    }
    catch (string error)
    {
        success = false;
        QMessageBox::critical(this, "Error", "Failed to rehash the package.\n\n" + QString::fromStdString(error));
    }

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

    if (success)
    {
        QString resignedStr = (resigned ? " and resigned" : "");
        QMessageBox::information(this, "Success", "The package has successfully been rehashed" + resignedStr + "!");
    }
}

void PackageViewer::on_btnViewAll_clicked()
{
    Metadata meta(package);
    meta.exec();
}

void PackageViewer::showSaveImageContextMenu(QPoint point)
{
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

        QMessageBox::information(this, "Success", "Successfully saved the thumbnail image.");
    }
}

void PackageViewer::on_txtSearch_textChanged(const QString &arg1)
{
    QList<QTreeWidgetItem*> itemsMatched = ui->treeWidget->findItems(ui->txtSearch->text(), Qt::MatchContains | Qt::MatchRecursive);

    // hide all the items
    for (DWORD i = 0; i < ui->treeWidget->topLevelItemCount(); i++)
        ui->treeWidget->setItemHidden(ui->treeWidget->topLevelItem(i), true);

    if (itemsMatched.count() == 0)
    {
        ui->txtSearch->setStyleSheet("color: rgb(255, 1, 1);");
        showAllItems();
        return;
    }

    ui->txtSearch->setStyleSheet("");
    // add all the matched ones to the list
    for (DWORD i = 0; i < itemsMatched.count(); i++)
    {
        // show all the item's parents
        QTreeWidgetItem *parent = itemsMatched.at(i)->parent();
        while (parent != NULL)
        {
            ui->treeWidget->setItemHidden(parent, false);
            parent->setExpanded(true);
            parent = parent->parent();
        }

        // show the item itself
        ui->treeWidget->setItemHidden(itemsMatched.at(i), false);
    }
}

void PackageViewer::showRemoveContextMenu(QPoint point)
{
    int amount = ui->treeWidget->selectedItems().length();
    if (amount < 1)
            return;

    bool isFolder = ui->treeWidget->selectedItems()[0]->text(3) == "N/A";
    QPoint globalPos = ui->treeWidget->mapToGlobal(point);

    QMenu contextMenu;

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
    }

    contextMenu.addSeparator();
    contextMenu.addAction(QPixmap(":/Images/add.png"), "Inject Here");
    contextMenu.addSeparator();
    contextMenu.addAction(QPixmap(":/Images/properties.png"), "View Properties");

    QAction *selectedItem = contextMenu.exec(globalPos);
    if(selectedItem == NULL)
        return;

    QList <QTreeWidgetItem*> items = ui->treeWidget->selectedItems();
    int totalCount = items.count(), successCount = 0;

    if (totalCount < 1)
        return;

    if (selectedItem->text() == "Extract Selected")
    {
        bool multiple = totalCount > 1;

        QString path;
        if (multiple)
            path = QFileDialog::getExistingDirectory(this, "Save Location", QtHelpers::DesktopLocation());
        else
            path = QFileDialog::getSaveFileName(this, "Save Location", QtHelpers::DesktopLocation() + "/" + ui->treeWidget->selectedItems()[0]->text(0));

        if (path.isEmpty())
            return;


        QList<QString> packagePaths;
        QList<QString> outPaths;
        for (int i = 0; i < totalCount; i++)
        {
            QString packagePath;
            GetPackagePath(items.at(i), &packagePath);
            packagePaths.append(packagePath);

            QString final = path;
            if (multiple)
                final += "\\" + items.at(i)->text(0);
            outPaths.append(final);
        }

        try
        {
            ProgressDialog *dialog = new ProgressDialog(package, packagePaths, outPaths, this);
            dialog->setModal(true);
            dialog->show();
            dialog->startExtracting();

            successCount++;
        }
        catch (string error)
        {
            QMessageBox::critical(this, "Error", "Failed to extract file.\n\n" + QString::fromStdString(error));
            return;
        }

        QMessageBox::information(this, "Success", "All selected files have been extracted.");

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
            QMessageBox::information(this, "Success", "All selected files have been deleted.");
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
            QMessageBox::information(this, "Success", "The file has been renamed.");
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
            SingleProgressDialog *dialog = new SingleProgressDialog(package, path, packagePath, this);
            dialog->setModal(true);
            dialog->show();
            dialog->startReplace();

        }
        catch(string error)
        {
            QMessageBox::critical(this, "Error", "Failed to replace file.\n\n" + QString::fromStdString(error));
        }

        QMessageBox::information(this, "Success", "Successfully replaced file.");
    }
    else if (selectedItem->text() == "Inject Here")
    {
        QString packagePath;
        GetPackagePath(items.at(0), &packagePath, true);

        QString path = QFileDialog::getOpenFileName(this, "New File", QtHelpers::DesktopLocation());
        if (path.isEmpty())
            return;

        QFileInfo pathInfo(path);

        if (isFolder && items.at(0)->parent() == NULL)
            packagePath.append(items.at(0)->text(0) + "\\");

        packagePath.append(pathInfo.fileName());

        try
        {
            FileEntry injectedEntry = package->InjectFile(path.toStdString(), packagePath.toStdString());
            listing = package->GetFileListing();

            QTreeWidgetItem *fileEntry;
            if (injectedEntry.pathIndicator != 0xFFFF)
                fileEntry = new QTreeWidgetItem(items.at(0));
            else
                fileEntry = new QTreeWidgetItem(ui->treeWidget);

            SetIcon(injectedEntry.name, fileEntry);

            fileEntry->setText(0, QString::fromStdString(injectedEntry.name));
            fileEntry->setText(1, "0x" + QString::number(injectedEntry.fileSize, 16).toUpper());
            fileEntry->setText(2, "0x" + QString::number(package->BlockToAddress(injectedEntry.startingBlockNum), 16).toUpper());
            fileEntry->setText(3, "0x" + QString::number(injectedEntry.startingBlockNum, 16).toUpper());

            QMessageBox::information(this, "Success", "The file has been injected.");
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

            FileEntry entry = package->GetFileEntry(packagePath.toStdString(), true);
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
                        SetIcon(entry.name, items.at(0));
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

void PackageViewer::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    // make sure the file double clicked on is a gpd
    int index = item->text(0).lastIndexOf(".");
    QString extension;
    if (index != string::npos)
        extension = item->text(0).mid(index).toLower();

    if (extension == ".gpd" || extension == ".fit")
    {
        try
        {
            // get a temporary file name
            QString tempName = (QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", ""));
            string tempNameStd = tempName.toStdString();

            // extract the file to a temporary location
            QString packagePath;
            GetPackagePath(item, &packagePath);

            package->ExtractFile(packagePath.toStdString(), tempNameStd);

            // parse the gpd
            GPDBase *gpd = new GPDBase(tempNameStd);
            bool changed;
            XdbfDialog dialog(gpd, &changed, this);
            dialog.exec();

            if(changed)
                package->ReplaceFile(tempNameStd, packagePath.toStdString());

            delete gpd;

            // delete the temp file
            QFile::remove(tempName);
        }
        catch(string error)
        {
            QMessageBox::critical(this, "Error", "Failed to open the GPD.\n\n" + QString::fromStdString(error));
        }
    }
    else if (extension == ".bin")
    {
        // get a temporary file name
        string tempName = (QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "")).toStdString();

        // extract the file to a temporary location
        QString packagePath;
        GetPackagePath(item, &packagePath);

        package->ExtractFile(packagePath.toStdString(), tempName);

        // verify that it's an STRB file
        FileIO io(tempName, false);
        if (io.readDword() == 0x53545242)
        {
            io.close();

            // show the avatar asset dialog
            AvatarAsset *asset = new AvatarAsset(tempName);
            StrbDialog dialog(asset, this);
            dialog.exec();

            // delete the temp file
            remove(tempName.c_str());
        }
        else
            io.close();
    }
    else if (extension == ".png" || extension == ".jpg" || extension == ".jpeg")
    {
        // get a temporary file name
        string tempName = (QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "")).toStdString();

        // extract the file to a temporary location
        QString imagePath;
        GetPackagePath(item, &imagePath);

        package->ExtractFile(imagePath.toStdString(), tempName);

        // display the image
        ImageDialog dialog(QImage(QString::fromStdString(tempName)), this);
        dialog.exec();

        // delete the temp file
        QFile::remove(QString::fromStdString(tempName));
    }
    else if (item->text(0) == "PEC")
    {
        try
        {
            // get a temporary file name
            string tempName = (QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "")).toStdString();

            // extract the file to a temporary location
            QString packagePath;
            GetPackagePath(item, &packagePath);

            package->ExtractFile(packagePath.toStdString(), tempName);

            StfsPackage *pec = new StfsPackage(tempName, true);
            PackageViewer dialog(pec, this);
            dialog.exec();

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
}

void PackageViewer::on_btnProfileEditor_clicked()
{
    ProfileEditor *editor = new ProfileEditor(package, true);
    editor->show();
    disposePackage = false;
    delete ui;
}

void PackageViewer::on_btnStfsTools_clicked()
{
    StfsToolsDialog dialog(package, this);
    dialog.exec();
}

void PackageViewer::on_btnShowAll_clicked()
{
    ui->txtSearch->setText("");
    showAllItems();
}

void PackageViewer::showAllItems()
{
    // show all the items and collapse them all
    for (DWORD i = 0; i < ui->treeWidget->topLevelItemCount(); i++)
    {
        collapseAllChildren(ui->treeWidget->topLevelItem(i));
        ui->treeWidget->setItemHidden(ui->treeWidget->topLevelItem(i), false);
    }
}

void PackageViewer::collapseAllChildren(QTreeWidgetItem *item)
{
    item->setExpanded(false);

    // collapse all children
    for (DWORD i = 0; i < item->childCount(); i++)
        collapseAllChildren(item->child(i));
}

#include "svoddialog.h"
#include "ui_svoddialog.h"

SvodDialog::SvodDialog(SVOD *svod, QStatusBar *statusBar, QWidget *parent, bool preserveSvod) :
    QDialog(parent), ui(new Ui::SvodDialog), svod(svod), statusBar(statusBar), preserveSvod(preserveSvod)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    QtHelpers::GenAdjustWidgetAppearanceToOS(this);

    loadListing(NULL, &svod->root);

    ui->treeWidget->header()->setDefaultSectionSize(75);
    ui->treeWidget->header()->resizeSection(0, 250);
    ui->treeWidget->header()->resizeSection(3, 20);

    ui->btnResign->setEnabled(svod->metaData->magic == CON);

    // setup the context menu
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showFileContextMenu(QPoint)));

    ui->lblDisplayName->setText(QString::fromStdWString(svod->metaData->displayName));
    ui->lblTitleID->setText(QString::number(svod->metaData->titleID, 16).toUpper());
    ui->lblSectorOffset->setText("0x" + QString::number(svod->metaData->svodVolumeDescriptor.dataBlockOffset * 2, 16).toUpper());
    ui->lblType->setText(QString::fromStdString(ContentTypeToString(svod->metaData->contentType)));

    QByteArray imageBuff((char*)svod->metaData->thumbnailImage, (size_t)svod->metaData->thumbnailImageSize);
    ui->imgThumbnail->setPixmap(QPixmap::fromImage(QImage::fromData(imageBuff)));

    statusBar->showMessage("SVOD system parsed successfully", 3000);
}

SvodDialog::~SvodDialog()
{
    if (!preserveSvod)
        delete svod;
    delete ui;
}

void SvodDialog::loadListing(QTreeWidgetItem *parent, vector<GdfxFileEntry> *files)
{
    DWORD addr, index;

    for (DWORD i = 0; i < files->size(); i++)
    {
        QTreeWidgetItem *item;
        if (parent == NULL)
            item = new QTreeWidgetItem(ui->treeWidget);
        else
            item = new QTreeWidgetItem(parent);

        svod->SectorToAddress(files->at(i).sector, &addr, &index);


        QIcon icon;
        SvodIO io = svod->GetSvodIO(files->at(i));
        QtHelpers::GetFileIcon(io.ReadDword(), QString::fromStdString(files->at(i).name), icon, *item);

        item->setIcon(0, icon);
        item->setText(0, QString::fromStdString(files->at(i).name));
        item->setText(1, QString::fromStdString(ByteSizeToString(files->at(i).size)));
        item->setText(2, "0x" + QString::number(addr, 16).toUpper());
        item->setText(3, QString::number(index));
        item->setData(0, Qt::UserRole, QVariant::fromValue(&files->at(i)));

        if (files->at(i).attributes & GdfxDirectory)
        {
            item->setIcon(0, QIcon(":/Images/FolderFileIcon.png"));
            loadListing(item, &files->at(i).files);
        }
    }
}

void SvodDialog::showFileContextMenu(QPoint pos)
{
    if (ui->treeWidget->currentIndex().row() == -1)
        return;

    GdfxFileEntry *entry = ui->treeWidget->currentItem()->data(0, Qt::UserRole).value<GdfxFileEntry*>();

    if (entry->attributes & GdfxDirectory)
        return;

    QPoint globalPos = ui->treeWidget->mapToGlobal(pos);
    QMenu contextMenu;
    contextMenu.addAction(QIcon(":/Images/extract.png"), "Extract");
    if (ui->treeWidget->selectedItems().count() == 1)
    {
        contextMenu.addAction(QIcon(":/Images/replace.png"), "Replace");
        contextMenu.addAction(QIcon(":/Images/properties.png"), "View Properties");
    }

    QAction *selectedItem = contextMenu.exec(globalPos);
    if(selectedItem == NULL)
        return;

    if (selectedItem->text() == "View Properties")
    {
        SvodFileInfoDialog dialog(svod, entry, ui->treeWidget->currentItem()->data(1, Qt::UserRole).toString(), this);
        dialog.exec();
    }
    else if (selectedItem->text() == "Extract")
    {
        // get a directory to save the files to
        QString savePath = QFileDialog::getExistingDirectory(this, "Choose a place to save the file...", QtHelpers::DesktopLocation() + "/" + QString::fromStdString(entry->name));
        if (savePath == "")
            return;

        // get a list of all the files to extract
        QList<void*> list;
        foreach (QTreeWidgetItem *item, ui->treeWidget->selectedItems())
        {
            GdfxFileEntry *entry = item->data(0, Qt::UserRole).value<GdfxFileEntry*>();
            list.push_back(entry);
        }

        MultiProgressDialog *dialog = new MultiProgressDialog(OpExtract, FileSystemSVOD, svod, savePath + "/", list, this);
        dialog->setModal(true);
        dialog->show();
        dialog->start();
    }
    else if (selectedItem->text() == "Replace")
    {
        // open a file
        QString filePath = QFileDialog::getOpenFileName(this, "Choose a modifed version to repalce...", QtHelpers::DesktopLocation());
        if (filePath == "")
            return;

        SingleProgressDialog *dialog = new SingleProgressDialog(FileSystemSVOD, svod, OpReplace, QString::fromStdString(entry->filePath + entry->name), filePath, NULL, this);
        dialog->setModal(true);
        dialog->show();
        dialog->start();
    }
}

void SvodDialog::on_btnViewAll_clicked()
{
    Metadata dialog(statusBar, svod->metaData, false, this);
    dialog.exec();
}

void SvodDialog::on_pushButton_clicked()
{
    SvodToolDialog dialog(svod, this);
    dialog.exec();
}

void SvodDialog::on_txtSearch_textChanged(const QString &arg1)
{
    QtHelpers::SearchTreeWidget(ui->treeWidget, ui->txtSearch, arg1);
}

void SvodDialog::on_btnShowAll_clicked()
{
    ui->txtSearch->setText("");
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); i++)
        QtHelpers::ShowAllItems(ui->treeWidget->topLevelItem(i));
}

void SvodDialog::on_pushButton_3_clicked()
{
    try
    {
        svod->Rehash(UpdateProgress, this);
        QMessageBox::information(this, "Success", "Successfully rehashed the system.");
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Error", "An error occurred while rehashing the system.\n\n" + QString::fromStdString(error));
    }
}

void UpdateProgress(DWORD cur, DWORD total, void *arg)
{
    SvodDialog *dialog = reinterpret_cast<SvodDialog*>(arg);

    if (cur < total)
        dialog->statusBar->showMessage("Rehashing files " + QString::number(cur) + "/" + QString::number(total));
    else
        dialog->statusBar->showMessage("Successfully rehashed the system", 3000);

    QApplication::processEvents();
}

void SvodDialog::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    GdfxFileEntry *entry = ui->treeWidget->currentItem()->data(0, Qt::UserRole).value<GdfxFileEntry*>();
    QString tempName = (QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", ""));

    // the other file types that are supported have no buisness being in an SVOD system, so we'll just leave those out
    if (item->data(1, Qt::UserRole).toString() == "Image")
    {
        svod->GetSvodIO(*entry).SaveFile(tempName.toStdString());

        ImageDialog dialog(QImage(tempName), this);
        dialog.exec();

        QFile::remove(tempName);
    }
    else if (item->data(1, Qt::UserRole).toString() == "STFS")
    {
        SvodIO io = svod->GetSvodIO(*entry);

        try
        {
            StfsPackage package(&io);

            PackageViewer viewer(statusBar, &package, QList<QAction*>(), QList<QAction*>(), this, false);
            viewer.exec();

            package.Close();
        }
        catch (string error)
        {
            QMessageBox::critical(this, "Error", "An error occurred while loading the STFS package.\n\n" + QString::fromStdString(error));
        }

        QFile::remove(tempName);
    }
    else if (item->data(1, Qt::UserRole).toString() == "XEX")
    {
        try
        {
            SvodIO io = svod->GetSvodIO(*entry);
            Xbox360Executable *xex = new Xbox360Executable(&io);

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

void SvodDialog::on_btnResign_clicked()
{
    QString kvPath = QFileDialog::getOpenFileName(this, "Choose a kv for resigning", QtHelpers::DesktopLocation());
    if (kvPath == "")
        return;

    // open kv
    FileIO kvIO(kvPath.toStdString());
    kvIO.SetPosition(0, ios_base::end);

    // read in the console id of the certificate in the kv
    DWORD adder = 0;
    if (kvIO.GetPosition() == 0x4000)
        adder = 0x10;
    kvIO.SetPosition(0x9BA + adder);
    BYTE consoleID[5];
    kvIO.ReadBytes(consoleID, 5);
    kvIO.Close();

    // make sure the console ids match
    if (memcmp(consoleID, svod->metaData->certificate.ownerConsoleID, 5) != 0)
    {
        QMessageBox::StandardButton btn = (QMessageBox::StandardButton)QMessageBox::question(this, "Continue?",
                                                                "The KeyVault provided is not from the console where this SVOD system was signed. This system will show up as corrupt on the original console. Unless you know what you're doing, choose no.\n\nAre you sure that you want to contiure?",
                                                                QMessageBox::No, QMessageBox::Yes);
        if (btn != QMessageBox::Yes)
            return;
    }

    try
    {
        svod->Resign(kvPath.toStdString());
        QMessageBox::information(this, "Success", "Successfuly resigned the system.");
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Error", "An error occurred while resigning the system.\n\n" + QString::fromStdString(error));
    }
}

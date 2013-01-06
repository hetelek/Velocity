#include "svoddialog.h"
#include "ui_svoddialog.h"

SvodDialog::SvodDialog(SVOD *svod, QStatusBar *statusBar, QWidget *parent) :
    QDialog(parent), ui(new Ui::SvodDialog), svod(svod), statusBar(statusBar), changing(true)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    loadListing(NULL, &svod->root);

    ui->treeWidget->header()->setDefaultSectionSize(75);
    ui->treeWidget->header()->resizeSection(0, 200);

    ui->btnResign->setEnabled(svod->metadata->magic == CON);

    // setup the context menu
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showFileContextMenu(QPoint)));

    ui->lblDisplayName->setText(QString::fromStdWString(svod->metadata->displayName));
    ui->lblTitleID->setText(QString::number(svod->metadata->titleID, 16).toUpper());
    ui->lblSectorOffset->setText("0x" + QString::number(svod->metadata->svodVolumeDescriptor.dataBlockOffset * 2, 16).toUpper());
    switch (svod->metadata->contentType)
    {
        case GameOnDemand:
            ui->comboBox->setCurrentIndex(0);
            break;
        case InstalledGame:
            ui->comboBox->setCurrentIndex(1);
            break;
    }

    QByteArray imageBuff((char*)svod->metadata->thumbnailImage, (size_t)svod->metadata->thumbnailImageSize);
    ui->imgThumbnail->setPixmap(QPixmap::fromImage(QImage::fromData(imageBuff)));

    statusBar->showMessage("SVOD system parsed successfully", 3000);
}

SvodDialog::~SvodDialog()
{
    delete svod;
    delete ui;
}

void SvodDialog::loadListing(QTreeWidgetItem *parent, vector<GDFXFileEntry> *files)
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

        QString extension = QString::fromStdString(files->at(i).name).mid(QString::fromStdString(files->at(i).name).lastIndexOf(".") + 1);
        if (extension == "xex")
            item->setIcon(0, QIcon(":/Images/XEXFileIcon.png"));
        else if (extension == "png" || extension == "jpg" || extension == "jpeg" || extension == "bmp")
            item->setIcon(0, QIcon(":/Images/ImageFileIcon.png"));
        else
            item->setIcon(0, QIcon(":/Images/DefaultFileIcon.png"));
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

    GDFXFileEntry *entry = ui->treeWidget->currentItem()->data(0, Qt::UserRole).value<GDFXFileEntry*>();

    QPoint globalPos = ui->treeWidget->mapToGlobal(pos);
    QMenu contextMenu;
    contextMenu.addAction(QIcon(":/Images/extract.png"), "Extract");
    contextMenu.addAction(QIcon(":/Images/replace.png"), "Replace");
    contextMenu.addAction(QIcon(":/Images/properties.png"), "View Properties");

    QAction *selectedItem = contextMenu.exec(globalPos);
    if(selectedItem == NULL)
        return;

    if (selectedItem->text() == "View Properties")
    {
        SvodFileInfoDialog dialog(svod, entry, this);
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
            GDFXFileEntry *entry = item->data(0, Qt::UserRole).value<GDFXFileEntry*>();
            list.push_back(entry);
        }

        MultiProgressDialog *dialog = new MultiProgressDialog(FileSystemSVOD, svod, savePath + "/", list, this);
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

        SvodIO io = svod->GetSvodIO(*entry);
        io.OverwriteFile(filePath.toStdString());
        io.Close();
    }
}

void SvodDialog::on_btnViewAll_clicked()
{
    Metadata dialog(statusBar, svod->metadata, false, this);
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

void SvodDialog::on_comboBox_currentIndexChanged(int index)
{
    if (changing)
    {
        changing = false;
        return;
    }

    // game on demand
    if (index == 0)
    {
        QMessageBox::StandardButton button = QMessageBox::warning(this, "Warning",
                             "Once converted, this will only work on consoles with signature checks removed such as JTAGs, RGHs or DevKits. Would you like to continue?",
                             QMessageBox::Yes, QMessageBox::No);

        if (button == QMessageBox::Yes)
        {
            ui->btnResign->setEnabled(false);
            svod->metadata->magic = PIRS;
            svod->metadata->contentType = GameOnDemand;
            svod->metadata->displayDescription = L"";
            svod->metadata->WriteMetaData();
        }
        else
        {
            changing = true;
            ui->comboBox->setCurrentIndex(1);
        }
    }
    // installed game
    else
    {
        QMessageBox::StandardButton button = QMessageBox::warning(this, "Warning",
                             "Once converted, this can only be used the KeyVault for the console is provided. Otherwise this can be played on consoles with signature checks removed such as JTAGs, RGHs or DevKits. Would you like to continue?",
                             QMessageBox::Yes, QMessageBox::No);

        if (button == QMessageBox::Yes)
        {
            ui->btnResign->setEnabled(true);
            svod->metadata->magic = CON;
            svod->metadata->contentType = InstalledGame;
            svod->metadata->displayDescription = L"This is an installed game. To play, insert the original game disc.";
            svod->metadata->WriteMetaData();
        }
        else
        {
            changing = true;
            ui->comboBox->setCurrentIndex(0);
        }
    }
}

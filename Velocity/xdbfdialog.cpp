#include "xdbfdialog.h"
#include "ui_xdbfdialog.h"

XdbfDialog::XdbfDialog(QStatusBar *statusBar, GpdBase *gpd, bool *modified, QWidget *parent) :
    QDialog(parent), ui(new Ui::XdbfDialog), gpd(gpd), modified(modified), statusBar(statusBar)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->treeWidget->header()->resizeSection(0, 260);

    loadEntries();

    // display the gpd name
    for (DWORD i = 0; i < gpd->strings.size(); i++)
        if (gpd->strings.at(i).entry.id == TitleInformation)
        {
            if (gpd->strings.at(i).ws != L"")
                setWindowTitle("Xdbf Viewer - " + QString::fromStdWString(gpd->strings.at(i).ws));
            break;
        }

    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
}

void XdbfDialog::addEntriesToTable(vector<XdbfEntry> entries, QString type)
{
    for (DWORD i = 0; i < entries.size(); i++)
    {
        // create an item
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
        item->setText(0, QString::fromStdString(XdbfHelpers::IDtoString(entries.at(i).id)));
        item->setText(1, "0x" + QString::number(gpd->xdbf->GetRealAddress(entries.at(i).addressSpecifier), 16).toUpper());
        item->setText(2, "0x" + QString::number(entries.at(i).length, 16).toUpper());
        item->setText(3, type);

        // add it to the table
        ui->treeWidget->insertTopLevelItem(ui->treeWidget->topLevelItemCount(), item);
    }
}

void XdbfDialog::loadEntries()
{
    // dispay all the entries
    addEntriesToTable(gpd->xdbf->achievements.entries, "Achievement");
    addEntriesToTable(gpd->xdbf->images, "Image");
    addEntriesToTable(gpd->xdbf->settings.entries, "Setting");
    addEntriesToTable(gpd->xdbf->titlesPlayed.entries, "Title");
    addEntriesToTable(gpd->xdbf->strings, "String");
    addEntriesToTable(gpd->xdbf->avatarAwards.entries, "Avatar Award");
}

void XdbfDialog::showContextMenu(QPoint p)
{
    if (ui->treeWidget->selectedItems().count() < 1)
        return;

    QPoint globalPos = ui->treeWidget->mapToGlobal(p);
    QMenu contextMenu;

    contextMenu.addAction(QPixmap(":/Images/extract.png"), "Extract Selected");
    if (ui->treeWidget->selectedItems().count() == 1)
        contextMenu.addAction(QPixmap(":/Images/replace.png"), "Replace Entry");
    contextMenu.addSeparator();
    contextMenu.addAction(QPixmap(":/Images/convert.png"), "Address Converter");
    contextMenu.addSeparator();
    contextMenu.addAction(QPixmap(":/Images/clean.png"), "Clean");

    QAction *selectedItem = contextMenu.exec(globalPos);

    if (selectedItem == NULL)
        return;
    else if (selectedItem->text() == "Extract Selected")
    {
        QList<QTreeWidgetItem*> items = ui->treeWidget->selectedItems();

        // get the out path
        QString path;
        if (items.count() > 1)
            path = QFileDialog::getExistingDirectory(this, "Extract Directory", QtHelpers::DesktopLocation()) + "/";
        else
            path = QFileDialog::getSaveFileName(this, "Choose a place to extract the entry", QtHelpers::DesktopLocation() + "/" + ui->treeWidget->currentItem()->text(0));

        if (path == "" || path == "\\")
            return;

        try
        {
            for (int i = 0; i < items.count(); i++)
            {
                int index = -1;
                for (int x = 0; x < ui->treeWidget->topLevelItemCount(); x++)
                    if (ui->treeWidget->topLevelItem(x) == items.at(i))
                    {
                        index = x;
                        break;
                    }

                Entry e = indexToEntry(index);
                XdbfEntry xentry;

                // get the xdbf entry
                if (e.type == Achievement)
                    xentry = gpd->xdbf->achievements.entries.at(e.index);
                else if (e.type == Image)
                    xentry = gpd->xdbf->images.at(e.index);
                else if (e.type == Setting)
                    xentry = gpd->xdbf->settings.entries.at(e.index);
                else if (e.type == Title)
                    xentry = gpd->xdbf->titlesPlayed.entries.at(e.index);
                else if (e.type == String)
                    xentry = gpd->xdbf->strings.at(e.index);
                else if (e.type == AvatarAward)
                    xentry = gpd->xdbf->avatarAwards.entries.at(e.index);

                // extract the entry into memory
                BYTE *entryBuff = new BYTE[xentry.length];
                gpd->xdbf->ExtractEntry(xentry, entryBuff);

                QString outPath = path;
                if (items.count() > 1)
                    outPath += items.at(i)->text(0);

                // Write the data to a file
                FileIO io(outPath.toStdString(), true);
                io.Write(entryBuff, xentry.length);
                io.Close();

                // free the temporary memory
                delete[] entryBuff;
            }

            statusBar->showMessage("Selected entries extracted successfully", 3000);
        }
        catch (string error)
        {
            QMessageBox::critical(this, "Error", "An error occurred while extracting.\n\n" + QString::fromStdString(error));
        }
    }
    else if (selectedItem->text() == "Replace Entry")
    {
        QString entryPath = QFileDialog::getOpenFileName(this, "Open the entry to replace the current with", QtHelpers::DesktopLocation() + "/" + ui->treeWidget->currentItem()->text(0));

        if (entryPath == "")
            return;

        try
        {
            Entry e = indexToEntry(ui->treeWidget->currentIndex().row());
            XdbfEntry xentry;

            // get the xdbf entry
            if (e.type == Achievement)
                xentry = gpd->xdbf->achievements.entries.at(e.index);
            else if (e.type == Image)
                xentry = gpd->xdbf->images.at(e.index);
            else if (e.type == Setting)
                xentry = gpd->xdbf->settings.entries.at(e.index);
            else if (e.type == Title)
                xentry = gpd->xdbf->titlesPlayed.entries.at(e.index);
            else if (e.type == String)
                xentry = gpd->xdbf->strings.at(e.index);
            else if (e.type == AvatarAward)
                xentry = gpd->xdbf->avatarAwards.entries.at(e.index);

            // open the file and get the length
            FileIO io(entryPath.toStdString());
            io.SetPosition(0, ios_base::end);
            DWORD fileLen = io.GetPosition();

            // allocate enough memory for the buffer
            BYTE *entryBuff = new BYTE[fileLen];

            // read in the file
            io.SetPosition(0);
            io.ReadBytes(entryBuff, fileLen);

            xentry.length = fileLen;
            gpd->xdbf->ReWriteEntry(xentry, entryBuff);

            // cleanup
            delete[] entryBuff;
            io.Close();
            if (modified != NULL)
                *modified = true;

            // get the xdbf entry again so we can update the UI
            if (e.type == Achievement)
                xentry = gpd->xdbf->achievements.entries.at(e.index);
            else if (e.type == Image)
                xentry = gpd->xdbf->images.at(e.index);
            else if (e.type == Setting)
                xentry = gpd->xdbf->settings.entries.at(e.index);
            else if (e.type == Title)
                xentry = gpd->xdbf->titlesPlayed.entries.at(e.index);
            else if (e.type == String)
                xentry = gpd->xdbf->strings.at(e.index);
            else if (e.type == AvatarAward)
                xentry = gpd->xdbf->avatarAwards.entries.at(e.index);

            // update the entry in the UI
            QTreeWidgetItem *item = ui->treeWidget->selectedItems().at(0);
            item->setText(1, "0x" + QString::number(gpd->xdbf->GetRealAddress(xentry.addressSpecifier), 16).toUpper());
            item->setText(2, "0x" + QString::number(xentry.length, 16).toUpper());

            statusBar->showMessage("Entry replaced successfully", 3000);
        }
        catch (string error)
        {
            QMessageBox::critical(this, "Error", "An error occurred while replacing.\n\n" + QString::fromStdString(error));
        }
    }
    else if (selectedItem->text() == "Address Converter")
    {
        AddressConverterDialog dialog(gpd->xdbf, this);
        dialog.exec();
    }
    else if (selectedItem->text() == "Clean")
    {
        int btn = QMessageBox::question(this, "Continue?", "Cleaning the Gpd will remove all of the unused memory that is in the file. This could potentially reduce the size of the Gpd.\n\nDo you want to continue?", QMessageBox::Yes, QMessageBox::No);

        if (btn != QMessageBox::Yes)
          return;

        // clean the Gpd
        try
        {
            gpd->xdbf->Clean();
        }
        catch (std::string error)
        {
            QMessageBox::critical(this, "Clean Error", "An error occured while cleaning the Gpd.\n\n" + QString::fromStdString(error));
            return;
        }

        // reload the listing
        ui->treeWidget->clear();
        loadEntries();

        if (modified != NULL)
            *modified = true;

        statusBar->showMessage("Cleaned Gpd successfully", 3000);
    }
}

XdbfDialog::~XdbfDialog()
{
    gpd->Close();
    delete gpd;
    delete ui;
}

Entry XdbfDialog::indexToEntry(DWORD index)
{
    Entry toReturn;

    DWORD count = 0;
    if (index < (count += gpd->xdbf->achievements.entries.size()))
    {
        toReturn.index = index;
        toReturn.type = Achievement;
    }
    else if (index < (count += gpd->xdbf->images.size()))
    {
        toReturn.index = index - gpd->xdbf->achievements.entries.size();
        toReturn.type = Image;
    }
    else if (index < (count += gpd->xdbf->settings.entries.size()))
    {
        toReturn.index = index - (count - gpd->xdbf->settings.entries.size());
        toReturn.type = Setting;
    }
    else if (index < (count += gpd->xdbf->titlesPlayed.entries.size()))
    {
        toReturn.index = index - (count - gpd->xdbf->titlesPlayed.entries.size());
        toReturn.type = Title;
    }
    else if (index < (count += gpd->xdbf->strings.size()))
    {
        toReturn.index = index - (count - gpd->xdbf->strings.size());
        toReturn.type = String;
    }
    else if (index < (count += gpd->xdbf->avatarAwards.entries.size()))
    {
        toReturn.index = index - (count - gpd->xdbf->avatarAwards.entries.size());
        toReturn.type = AvatarAward;
    }

    return toReturn;
}

void XdbfDialog::on_treeWidget_doubleClicked(const QModelIndex &index)
{
    Entry e = indexToEntry(index.row());
    switch (e.type)
    {
        case String:
        {
            QString str = QString::fromStdWString(gpd->strings.at(e.index).ws);
            if (str.trimmed() != "")
                QMessageBox::about(this, "Setting", "<html><center><h3>Unicode String</h3><br />" + str + "</center></html>");
            else
                QMessageBox::about(this, "Setting", "<html><center><h3>Unicode String</h3><br /><i>Empty</i></center></html>");
            break;
        }
        case Setting:
        {
            SettingEntry setting = gpd->settings.at(e.index);

            switch (setting.type)
            {
                case Int32:
                    QMessageBox::about(this, "Setting", "<html><center><h3>Int32 Setting</h3><br />" + QString::number(setting.int32) + "</center></html>");
                    break;
                case Int64:
                    QMessageBox::about(this, "Setting", "<html><center><h3>Int64 Setting</h3><br />" + QString::number(setting.int64) + "</center></html>");
                    break;
                case Float:
                    QMessageBox::about(this, "Setting", "<html><center><h3>Float Setting</h3><br />" + QString::number(setting.floatData) + "</center></html>");
                    break;
                case Double:
                    QMessageBox::about(this, "Setting", "<html><center><h3>Double Setting</h3><br />" + QString::number(setting.doubleData) + "</center></html>");
                    break;
                case UnicodeString:
                {
                    QString str = QString::fromStdWString(*setting.str);
                    if (str.trimmed() != "")
                        QMessageBox::about(this, "Setting", "<html><center><h3>String Setting</h3><br />" + str + "</center></html>");
                    else
                        QMessageBox::about(this, "Setting", "<html><center><h3>String Setting</h3><br /><i>Empty</i></center></html>");
                    break;
                }
                case TimeStamp:
                    QMessageBox::about(this, "Setting", "<html><center><h3>Timestamp Setting</h3><br />" + QDateTime::fromTime_t(setting.timeStamp).toString() + "</center></html>");
                    break;
                default:
                    statusBar->showMessage("Cannot view this type of entry", 3000);
                    break;
            }
            break;
        }
        case Image:
        {
            QByteArray imageBuff((char*)gpd->images.at(e.index).image, (size_t)gpd->images.at(e.index).length);

            ImageDialog dialog(QImage::fromData(imageBuff), this);
            dialog.exec();
            break;
        }
        default:
            statusBar->showMessage("Cannot view this type of entry", 3000);
            break;
    }
}

#include "xdbfdialog.h"
#include "ui_xdbfdialog.h"

XdbfDialog::XdbfDialog(GPDBase *gpd, bool *modified, QWidget *parent) : QDialog(parent), ui(new Ui::XdbfDialog), gpd(gpd), modified(modified)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->treeWidget->header()->resizeSection(0, 260);

    // dispay all the entries
    addEntriesToTable(gpd->xdbf->achievements.entries, "Achievement");
    addEntriesToTable(gpd->xdbf->images, "Image");
    addEntriesToTable(gpd->xdbf->settings.entries, "Setting");
    addEntriesToTable(gpd->xdbf->titlesPlayed.entries, "Title");
    addEntriesToTable(gpd->xdbf->strings, "String");
    addEntriesToTable(gpd->xdbf->avatarAwards.entries, "Avatar Award");

    // display the gpd name
    for (DWORD i = 0; i < gpd->strings.size(); i++)
        if (gpd->strings.at(i).entry.id == TitleInformation)
        {
            if (gpd->strings.at(i).ws != L"")
                setWindowTitle("XDBF Viewer - " + QString::fromStdWString(gpd->strings.at(i).ws));
            break;
        }
}

void XdbfDialog::addEntriesToTable(vector<XDBFEntry> entries, QString type)
{
    for (DWORD i = 0; i < entries.size(); i++)
    {
        // create an item
        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText(0, QString::fromStdString(XDBFHelpers::IDtoString(entries.at(i).id)));
        item->setText(1, "0x" + QString::number(gpd->xdbf->GetRealAddress(entries.at(i).addressSpecifier), 16).toUpper());
        item->setText(2, "0x" + QString::number(entries.at(i).length, 16).toUpper());
        item->setText(3, type);

        // add it to the table
        ui->treeWidget->insertTopLevelItem(ui->treeWidget->topLevelItemCount(), item);
    }
}

XdbfDialog::~XdbfDialog()
{
    delete ui;
}

Entry XdbfDialog::indexToEntry(int index)
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
            QMessageBox::about(this, "Setting", "<html><center><h3>Unicode String</h3><br />" + QString::fromStdWString(gpd->strings.at(e.index).ws) + "</center></html>");
            break;
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
                    QMessageBox::about(this, "Setting", "<html><center><h3>String Setting</h3><br />" + QString::fromStdWString(*setting.str) + "</center></html>");
                    break;
                case TimeStamp:
                    QMessageBox::about(this, "Setting", "<html><center><h3>Timestamp Setting</h3><br />" + QDateTime::fromTime_t(setting.timeStamp).toString() + "</center></html>");
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
    }
}

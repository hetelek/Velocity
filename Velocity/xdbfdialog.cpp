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

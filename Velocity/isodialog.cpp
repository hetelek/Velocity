#include "isodialog.h"
#include "ui_isodialog.h"

ISODialog::ISODialog(ISO *iso, QWidget *parent) :
    QDialog(parent), ui(new Ui::ISODialog), iso(iso)
{
    ui->setupUi(this);

    LoadFileListing();

    ui->treeWidget->setColumnWidth(0, 300);
}

ISODialog::~ISODialog()
{
    delete ui;
    delete iso;
}

void ISODialog::LoadFileListing()
{
    iso->GetFileListing();
    LoadDirectory(ui->treeWidget, iso->root, true);
}

void ISODialog::LoadDirectory(QObject *parent, std::vector<GdfxFileEntry> directoryContents, bool root)
{
    // create a tree widget for all the items, and recursively call for directories
    for (size_t i = 0; i < directoryContents.size(); i++)
    {
        GdfxFileEntry curEntry = directoryContents.at(i);

        // it's necessary to differentiate the parent type
        QTreeWidgetItem *item;
        if (root)
            item = new QTreeWidgetItem((QTreeWidget*)parent);
        else
            item = new QTreeWidgetItem((QTreeWidgetItem*)parent);

        item->setText(0, QString::fromStdString(curEntry.name));
        item->setText(1, QtHelpers::ToHexString(curEntry.address));
        item->setText(2, QtHelpers::ToHexString(curEntry.sector));
        item->setText(3, QString::fromStdString(ByteSizeToString(curEntry.size)));

        if (curEntry.attributes & GdfxDirectory)
            LoadDirectory((QObject*)item, curEntry.files);
    }
}

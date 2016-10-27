#include "transferflagsdialog.h"
#include "ui_transferflagsdialog.h"

TransferFlagsDialog::TransferFlagsDialog(QStatusBar *statusBar, BYTE *flags, QWidget *parent) :
    QDialog(parent), ui(new Ui::TransferFlagsDialog), flags(flags), statusBar(statusBar)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    ui->treeWidget->header()->resizeSection(0, 130);

    Flag temp = { QString("Unknown"), 0x01 };
    availableFlags.push_back(temp);

    temp.name = "Unknown";
    temp.value = 0x02;
    availableFlags.push_back(temp);

    temp.name = "Deep Link Supported";
    temp.value = 0x04;
    availableFlags.push_back(temp);

    temp.name = "Disable Network Storage";
    temp.value = 0x08;
    availableFlags.push_back(temp);

    temp.name = "Kinect Enabled";
    temp.value = 0x10;
    availableFlags.push_back(temp);

    temp.name = "Move Only Transfer";
    temp.value = 0x20;
    availableFlags.push_back(temp);

    temp.name = "Device Transfer";
    temp.value = 0x40;
    availableFlags.push_back(temp);

    temp.name = "Profile Transfer";
    temp.value = 0x80;
    availableFlags.push_back(temp);

    DWORD added = 0;
    BYTE tempFlag = *flags;
    for (DWORD i = 0; i < 8; i++)
    {
        if (tempFlag & 1)
        {
            QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
            item->setText(0, availableFlags.at(i - added).name);
            item->setText(1, "0x" + QString::number(availableFlags.at(i - added).value, 16).toUpper());
            ui->treeWidget->insertTopLevelItem(added, item);

            availableFlags.erase(availableFlags.begin() + (i - added));
            added++;
        }
        tempFlag >>= 1;
    }

    for (DWORD i = 0; i < availableFlags.size(); i++)
        ui->comboBox->addItem(availableFlags.at(i).name);

    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showRemoveContextMenu(QPoint)));

    statusBar->showMessage("Transfer flags loaded successfully", 3000);
}

void TransferFlagsDialog::showRemoveContextMenu(QPoint pos)
{
    if (ui->treeWidget->selectedItems().length() == 0)
           return;

   QPoint globalPos =ui->treeWidget->mapToGlobal(pos);

   QMenu contextMenu;
   QIcon icon;
   icon.addPixmap(QPixmap::fromImage(QImage(":/Images/delete.png")));
   contextMenu.addAction(icon, "Remove Flag");

   QAction *selectedItem = contextMenu.exec(globalPos);
   if (selectedItem)
   {
       QTreeWidgetItem *item = ui->treeWidget->selectedItems().at(0);
       ui->comboBox->addItem(item->text(0));

       // add it to the available
       Flag flag = { item->text(0), (BYTE)QtHelpers::ParseHexString(item->text(1)) };
       availableFlags.push_back(flag);

       delete item;
   }
}

TransferFlagsDialog::~TransferFlagsDialog()
{
    delete ui;
}

void TransferFlagsDialog::on_pushButton_clicked()
{
    if (ui->comboBox->currentIndex() < 0)
        return;

    // add the flag to the list
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
    item->setText(0, availableFlags.at(ui->comboBox->currentIndex()).name);
    item->setText(1, "0x" + QString::number(availableFlags.at(ui->comboBox->currentIndex()).value, 16));
    ui->treeWidget->insertTopLevelItem(0, item);

    // remove it from the availble flags, and the combobox
    availableFlags.erase(availableFlags.begin() + ui->comboBox->currentIndex());
    ui->comboBox->removeItem(ui->comboBox->currentIndex());
}

void TransferFlagsDialog::on_pushButton_2_clicked()
{
    this->close();
}

void TransferFlagsDialog::on_pushButton_3_clicked()
{
    *flags = 0;

    for (int i = 0; i <  ui->treeWidget->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = ui->treeWidget->topLevelItem(i);
        *flags |= QtHelpers::ParseHexString(item->text(1));
    }

    statusBar->showMessage("Transfer flags saved successfully", 3000);
    this->close();
}

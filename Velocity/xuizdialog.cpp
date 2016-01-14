#include "xuizdialog.h"
#include "ui_xuizdialog.h"

XuizDialog::XuizDialog(QStatusBar *statusBar, Xuiz *xuiz, QWidget *parent) :
    QDialog(parent), ui(new Ui::XuizDialog), statusBar(statusBar), xuiz(xuiz)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    // set up hte context menu
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    // load all the files in the table
    std::vector<XuizFileEntry> files = xuiz->GetFiles();
    for (size_t i = 0; i < files.size(); i++)
    {
        XuizFileEntry file = files.at(i);
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);

        item->setText(0, QString::fromStdString(file.path));
        item->setText(1, QString::fromStdString(ByteSizeToString(file.size)));
    }
}

XuizDialog::~XuizDialog()
{
    delete ui;
}

void XuizDialog::showContextMenu(QPoint point)
{
    QPoint globalPos = ui->treeWidget->mapToGlobal(point);
    QMenu contextMenu;

    if (ui->treeWidget->selectedItems().size() == 1)
        contextMenu.addAction(QPixmap(":/Images/extract.png"), "Extract");

    QAction *selectedItem = contextMenu.exec(globalPos);
    if (selectedItem == NULL)
        return;

    if (selectedItem->text() == "Extract")
    {
        QString xuizFilePath = ui->treeWidget->selectedItems().at(0)->text(0);
        QString fileName = xuizFilePath;

        int lastSlashIndex = fileName.lastIndexOf('\\');
        if (lastSlashIndex != -1)
            fileName = fileName.mid(lastSlashIndex + 1);

        QString outPath = QFileDialog::getSaveFileName(this, "Choose a place to extract the file to", QtHelpers::DesktopLocation() + "/" + fileName);
        if (!outPath.isEmpty())
        {
            try
            {
                xuiz->ExtractFile(outPath.toStdString(), xuizFilePath.toStdString());

                statusBar->showMessage("Successfully extracted file", 5000);
            }
            catch (std::string error)
            {
                QMessageBox::critical(this, "Extracting Error", "An error occurred while extracted a file.\n\n" +
                                      QString::fromStdString(error));
            }
        }
    }
}

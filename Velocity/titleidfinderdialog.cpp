#include "titleidfinderdialog.h"
#include "ui_titleidfinderdialog.h"

TitleIdFinderDialog::TitleIdFinderDialog(QStatusBar *statusBar, QWidget *parent) :
    QDialog(parent), ui(new Ui::TitleIdFinderDialog), statusBar(statusBar)
{
    ui->setupUi(this);
    finder = new TitleIdFinder("", this);

    connect(finder, SIGNAL(SearchFinished(QList<TitleData>)), this, SLOT(onRequestFinished(QList<TitleData>)));

    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeWidget->header()->resizeSection(0, 300);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(on_pushButton_clicked()));
}

TitleIdFinderDialog::~TitleIdFinderDialog()
{
    delete ui;
}

void TitleIdFinderDialog::on_pushButton_clicked()
{
    finder->SetGameName(ui->lineEdit->text());
    finder->StartSearch();
    statusBar->showMessage("Searching...", 0x7FFFFFFF);
}

void TitleIdFinderDialog::onRequestFinished(QList<TitleData> matches)
{
    ui->treeWidget->clear();
    statusBar->showMessage("Search returned " + QString::number(matches.length()) + " result(s)", 3000);

    for (int i = 0; i < matches.length(); i++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);

        QString newStr = ((QString*)&matches.at(i).titleName)->replace("&#174;", "®").replace("&#39;", "'").replace("&amp;","&").replace("&gt;",">").replace("&lt;","<").replace("â", "").replace("¢", "");
        item->setText(0, newStr);
        item->setText(1, QString::number(matches.at(i).titleID, 16).toUpper());

        ui->treeWidget->addTopLevelItem(item);
    }
}

void TitleIdFinderDialog::showContextMenu(QPoint p)
{
    if (ui->treeWidget->selectedItems().count() == 0)
        return;

    QPoint globalPos = ui->treeWidget->mapToGlobal(p);
    QMenu contextMenu;

    contextMenu.addAction(QPixmap(":/Images/id.png"), "Copy Title ID");
    contextMenu.addAction(QPixmap(":/Images/xbox.png"), "View on Xbox.com");
    QAction *selectedItem = contextMenu.exec(globalPos);

    if (selectedItem == NULL)
        return;
    else if (selectedItem->text() == "Copy Title ID")
    {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(ui->treeWidget->selectedItems().at(0)->text(1));
        statusBar->showMessage("Copied Successfully", 3000);
    }
    else if (selectedItem->text() == "View on Xbox.com")
    {
        QDesktopServices::openUrl(QUrl("http://marketplace.xbox.com/Product/66acd000-77fe-1000-9115-d802" + ui->treeWidget->selectedItems().at(0)->text(1).toLower()));
    }
}

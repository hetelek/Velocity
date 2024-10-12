#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    setFixedSize(sizeHint());
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QtHelpers::GenAdjustWidgetAppearanceToOS(this);
}

About::~About()
{
    delete ui;
}


void About::onSongFinished()
{
}

void About::on_label_3_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link));
}

void About::on_pushButton_2_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/hetelek/Velocity"));
}

void About::on_pushButton_clicked()
{
    QDesktopServices::openUrl(QUrl("http://free60.org/Main_Page"));
}

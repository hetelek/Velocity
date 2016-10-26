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

#ifndef __linux
    if (!QFile::exists(QtHelpers::ExecutingDirectory() + "Developers.mp3"))
        return;

    developers = new Phonon::MediaObject(this);
    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);

    connect(developers, SIGNAL(finished()), this, SLOT(onSongFinished()));

    Phonon::createPath(developers, audioOutput);

    developers->setCurrentSource(Phonon::MediaSource(QtHelpers::ExecutingDirectory() + "Developers.mp3"));
    developers->play();
#endif
}

About::~About()
{
    delete ui;
}


void About::onSongFinished()
{
#ifndef __linux
    // for (;;) let's listen to it one more time;
    developers->clear();
    developers->setCurrentSource(Phonon::MediaSource(QtHelpers::ExecutingDirectory() + "Developers.mp3"));
    developers->play();
#endif
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

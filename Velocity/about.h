#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include <QUrl>
#include <QFile>
#include <QMessageBox>
#include "qthelpers.h"

#ifndef __linux
    #include <phonon/MediaObject>
    #include <phonon/AudioOutput>
#endif

namespace Ui {
class About;
}

class About : public QDialog
{
    Q_OBJECT
    
public:
    explicit About(QWidget *parent = 0);
    ~About();
    
private slots:
    void on_label_3_linkActivated(const QString &link);

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void onSongFinished();

private:
    Ui::About *ui;

#ifndef __linux
    Phonon::MediaObject *developers;
    Phonon::AudioOutput *audioOutput;
#endif
};

#endif // ABOUT_H

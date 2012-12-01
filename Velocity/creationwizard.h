#ifndef CREATIONWIZARD_H
#define CREATIONWIZARD_H

// qt
#include <QWizard>
#include <QMessageBox>
#include <QFileDialog>
#include <QBuffer>
#include "qthelpers.h"

// xbox 360
#include "Stfs/StfsPackage.h"

namespace Ui {
class CreationWizard;
}

class CreationWizard : public QWizard
{
    Q_OBJECT

public:
    explicit CreationWizard(QString *fileName = 0, QWidget *parent = NULL);
    ~CreationWizard();

private slots:
    void onCurrentIdChanged(int id);

    void on_txtDisplayName_textChanged(const QString &arg1);

    void on_btnOpenThumbnail_clicked();

    void on_btnOpenTitleThumbnail_clicked();

    void on_txtTitleID_textChanged(const QString &arg1);

    void on_pushButton_clicked();

    void onFinished(int status);

    void on_radioButton_clicked(bool checked);

    void on_radioButton_2_clicked(bool checked);

    void on_radioButton_3_clicked(bool checked);

    void on_radioButton_4_clicked(bool checked);

    void on_radioButton_5_clicked(bool checked);

private:
    Ui::CreationWizard *ui;
    Magic magic;
    ContentType type;
    QString *fileName;
    ConsoleType consoleType;

    void openImage(QLabel *img);
    DWORD getContentType();
};

#endif // CREATIONWIZARD_H

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
    explicit CreationWizard(QString *fileName, QWidget *parent = 0);
    ~CreationWizard();

private slots:
    void onCurrentIdChanged(int id);

    void on_txtDisplayName_textChanged(const QString &arg1);

    void on_btnOpenThumbnail_clicked();

    void on_cmbxMagic_currentIndexChanged(int index);

    void on_btnOpenTitleThumbnail_clicked();

    void on_txtTitleID_textChanged(const QString &arg1);

    void on_pushButton_clicked();

    void onFinished(int status);

private:
    Ui::CreationWizard *ui;
    Magic magic;
    ContentType type;
    QString *fileName;

    void openImage(QLabel *img);
    DWORD getContentType();
};

#endif // CREATIONWIZARD_H

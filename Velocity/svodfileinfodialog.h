#ifndef SVODFILEINFODIALOG_H
#define SVODFILEINFODIALOG_H

// qt
#include <QDialog>

// xbox
#include "Disc/gdfx.h"
#include "Disc/svod.h"
#include "Stfs/StfsDefinitions.h"

namespace Ui {
class SvodFileInfoDialog;
}

class SvodFileInfoDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SvodFileInfoDialog(SVOD *svod, GDFXFileEntry *entry, QString type, QWidget *parent = 0);
    ~SvodFileInfoDialog();
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::SvodFileInfoDialog *ui;
    GDFXFileEntry *entry;
    SVOD *svod;
    QString type;

    QString getFileType(QString fileName);
};

#endif // SVODFILEINFODIALOG_H

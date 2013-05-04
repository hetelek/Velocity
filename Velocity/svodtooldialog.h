#ifndef SVODTOOLDIALOG_H
#define SVODTOOLDIALOG_H

// qt
#include <QDialog>
#include "qthelpers.h"

// xbox
#include "Disc/Svod.h"

namespace Ui {
class SvodToolDialog;
}

class SvodToolDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SvodToolDialog(SVOD *svod, QWidget *parent = 0);
    ~SvodToolDialog();
    
private slots:
    void on_spinBox_valueChanged(int arg1);

private:
    Ui::SvodToolDialog *ui;
    SVOD *svod;
};

#endif // SVODTOOLDIALOG_H

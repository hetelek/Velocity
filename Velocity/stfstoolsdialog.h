#ifndef STFSTOOLSDIALOG_H
#define STFSTOOLSDIALOG_H

#include <QDialog>
#include "qthelpers.h"

#include "Stfs/StfsPackage.h"

namespace Ui {
class StfsToolsDialog;
}

class StfsToolsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit StfsToolsDialog(StfsPackage *packge, QWidget *parent = 0);
    ~StfsToolsDialog();
    
private slots:

    void on_spnBlock_valueChanged(int arg1);

private:
    Ui::StfsToolsDialog *ui;
    StfsPackage *package;
};

#endif // STFSTOOLSDIALOG_H

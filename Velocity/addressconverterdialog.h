#ifndef ADDRESSCONVERTERDIALOG_H
#define ADDRESSCONVERTERDIALOG_H

#include <QDialog>
#include "qthelpers.h"

#include "Gpd/Xdbf.h"

namespace Ui {
class AddressConverterDialog;
}

class AddressConverterDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddressConverterDialog(Xdbf *xdbf, QWidget *parent = 0);
    ~AddressConverterDialog();
    
private slots:
    void on_spnSpecifier_editingFinished();

    void on_spnRealAddr_editingFinished();

private:
    Ui::AddressConverterDialog *ui;
    Xdbf *xdbf;
};

#endif // ADDRESSCONVERTERDIALOG_H

#ifndef XDBFDIALOG_H
#define XDBFDIALOG_H

// qt
#include <QDialog>

// xbox360
#include "GPD/XDBF.h"
#include "GPD/XDBFDefininitions.h"
#include "GPD/XDBFHelpers.h"
#include "GPD/GPDBase.h"

namespace Ui {
class XdbfDialog;
}

class XdbfDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit XdbfDialog(GPDBase *gpd, bool *modified = NULL, QWidget *parent = 0);
    ~XdbfDialog();
    
private:
    Ui::XdbfDialog *ui;
    GPDBase *gpd;
    bool *modified;

    void addEntriesToTable(vector<XDBFEntry> entries, QString type);
};

#endif // XDBFDIALOG_H

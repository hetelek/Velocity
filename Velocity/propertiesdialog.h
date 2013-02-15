#ifndef PROPERTIESDIALOG_H
#define PROPERTIESDIALOG_H

#include <QDialog>
#include <QDate>
#include <Stfs/StfsPackage.h>
#include "qthelpers.h"

namespace Ui {
class PropertiesDialog;
}

class PropertiesDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PropertiesDialog(StfsFileEntry *entry, QString location, bool *changed, QIcon icon, bool hasChildren, QWidget *parent = 0);
    ~PropertiesDialog();
    
private slots:
    void on_btnCancel_clicked();

    void on_leName_textChanged(const QString &arg1);

    void on_cbConsecutive_toggled(bool checked);

    void on_cbFolder_toggled(bool checked);

    void on_btnOK_clicked();

    void on_btnApply_clicked();

private:
    void updateEntry();

    Ui::PropertiesDialog *ui;
    bool *changed;
    StfsFileEntry *entry;
};

#endif // PROPERTIESDIALOG_H

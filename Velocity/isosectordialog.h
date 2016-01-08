#ifndef ISOSECTORDIALOG_H
#define ISOSECTORDIALOG_H

#include <QDialog>

#include "Disc/ISO.h"
#include "Utils.h"

namespace Ui {
class IsoSectorDialog;
}

class IsoSectorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IsoSectorDialog(ISO *iso, QWidget *parent = 0);
    ~IsoSectorDialog();

private slots:
    void on_spnSector_valueChanged(int arg1);

private:
    Ui::IsoSectorDialog *ui;
    ISO *iso;
};

#endif // ISOSECTORDIALOG_H

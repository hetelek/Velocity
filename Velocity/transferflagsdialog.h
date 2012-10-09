#ifndef TRANSFERFLAGSDIALOG_H
#define TRANSFERFLAGSDIALOG_H

// qt
#include <QDialog>
#include <QMenu>
#include <QStatusBar>
#include "qthelpers.h"

// other
#include "winnames.h"
#include <iostream>
#include <vector>

using namespace std;

namespace Ui {
class TransferFlagsDialog;
}

struct Flag
{
    QString name;
    BYTE value;
};

class TransferFlagsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TransferFlagsDialog(QStatusBar *statusBar, BYTE *flags, QWidget *parent = 0);
    ~TransferFlagsDialog();
    
private slots:
    void on_pushButton_clicked();

    void showRemoveContextMenu(QPoint pos);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::TransferFlagsDialog *ui;
    BYTE *flags;
    QStatusBar *statusBar;

    vector<Flag> availableFlags;
    vector<Flag> usedFlags;
};

#endif // TRANSFERFLAGSDIALOG_H

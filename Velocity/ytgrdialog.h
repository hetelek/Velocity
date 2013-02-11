#ifndef YTGRDIALOG_H
#define YTGRDIALOG_H

// qt
#include <QDialog>
#include <QDateTime>
#include <QStatusBar>
#include "qthelpers.h"

// xbox360
#include "AvatarAsset/YTGR.h"

namespace Ui {
class YtgrDialog;
}

class YtgrDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit YtgrDialog(YTGR *ytgr, QStatusBar *statusBar, QWidget *parent = 0);
    ~YtgrDialog();
    
private:
    Ui::YtgrDialog *ui;
    YTGR *ytgr;
};

#endif // YTGRDIALOG_H

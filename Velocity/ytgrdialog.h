#ifndef YtgrDIALOG_H
#define YtgrDIALOG_H

// qt
#include <QDialog>
#include <QDateTime>
#include <QStatusBar>
#include "qthelpers.h"

// xbox360
#include "AvatarAsset/Ytgr.h"

namespace Ui {
class YtgrDialog;
}

class YtgrDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit YtgrDialog(Ytgr *ytgr, QStatusBar *statusBar, QWidget *parent = 0);
    ~YtgrDialog();
    
private:
    Ui::YtgrDialog *ui;
    Ytgr *ytgr;
};

#endif // YtgrDIALOG_H

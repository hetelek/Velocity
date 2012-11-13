#ifndef TITLEIDFINDERDIALOG_H
#define TITLEIDFINDERDIALOG_H

// qt
#include <QDialog>
#include <QMenu>
#include <QClipboard>
#include <QDesktopServices>
#include <QUrl>
#include <QStatusBar>

// other
#include "titleidfinder.h"

namespace Ui {
class TitleIdFinderDialog;
}

class TitleIdFinderDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TitleIdFinderDialog(QStatusBar *statusBar, QWidget *parent = 0);
    ~TitleIdFinderDialog();
    
private slots:
    void on_pushButton_clicked();
    void onRequestFinished(QList<TitleData>);
    void showContextMenu(QPoint p);

private:
    Ui::TitleIdFinderDialog *ui;

    TitleIdFinder *finder;
    QStatusBar *statusBar;
};

#endif // TITLEIDFINDERDIALOG_H

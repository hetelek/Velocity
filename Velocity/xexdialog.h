#ifndef XEXDIALOG_H
#define XEXDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include "qthelpers.h"

#include "boxartretriever.h"

#include "Xex/Xex.h"
#include "Utils.h"

namespace Ui {
class XexDialog;
}

class XexDialog : public QDialog
{
    Q_OBJECT

public:
    explicit XexDialog(Xbox360Executable *xex, QWidget *parent = 0);
    ~XexDialog();

private slots:
    void on_pushButton_clicked();

    void onLargeBoxArtRetrieved(QPixmap boxArt);

    void showContextMenu(QPoint point);

private:
    Ui::XexDialog *ui;
    Xbox360Executable *xex;
    BoxArtRetriever *boxArtRetriever;

    void AddExecutableProperty(QString name, DWORD value);

    void AddExecutableProperty(QString name, QString value);
};

#endif // XEXDIALOG_H

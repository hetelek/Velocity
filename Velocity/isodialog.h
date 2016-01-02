#ifndef ISODIALOG_H
#define ISODIALOG_H

#include <QDialog>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "qthelpers.h"

#include "Disc/ISO.h"

namespace Ui {
class ISODialog;
}

class ISODialog : public QDialog
{
    Q_OBJECT

public:
    explicit ISODialog(ISO *iso, QWidget *parent = 0);
    ~ISODialog();

private:
    Ui::ISODialog *ui;
    ISO *iso;

    void LoadFileListing();

    void LoadDirectory(QObject *parent, std::vector<GdfxFileEntry> directoryContents, bool root=false);
};

#endif // ISODIALOG_H

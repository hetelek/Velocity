#ifndef FATXFILEDIALOG_H
#define FATXFILEDIALOG_H

//qt
#include <QDialog>
#include <QDateTime>

#include "Fatx/FatxConstants.h"
#include "Stfs/StfsDefinitions.h"

namespace Ui {
class FatxFileDialog;
}

class FatxFileDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit FatxFileDialog(FatxFileEntry *entry, DWORD clusterSize, QString type, QWidget *parent);
    ~FatxFileDialog();
    
private:
    Ui::FatxFileDialog *ui;
    FatxFileEntry *entry;
    QString type;

    QString msTimeToString(DWORD time);

    QString getFileType(QString fileName);
};

#endif // FATXFILEDIALOG_H

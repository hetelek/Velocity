#ifndef FATXFILEDIALOG_H
#define FATXFILEDIALOG_H

//qt
#include <QDialog>
#include <QDateTime>

#include <XboxInternals/Fatx/FatxConstants.h>
#include <XboxInternals/IO/FatxIO.h>
#include <XboxInternals/Fatx/FatxDrive.h>
#include <XboxInternals/Stfs/StfsDefinitions.h>

namespace Ui
{
class FatxFileDialog;
}

class FatxFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FatxFileDialog(FatxDrive *drive, FatxFileEntry *entry, DWORD clusterSize, QString type,
            QWidget *parent);
    ~FatxFileDialog();

private slots:
    void on_btnOK_clicked();
    void on_btnCancel_clicked();

private:
    Ui::FatxFileDialog *ui;
    FatxFileEntry *entry;
    QString type;
    FatxDrive *drive;

    QString msTimeToString(DWORD time);
    QString getFileType(QString fileName);
    void WriteEntryBack();
    UINT64 calculateDirectorySize(FatxFileEntry *directory, DWORD clusterSize);
};

#endif // FATXFILEDIALOG_H



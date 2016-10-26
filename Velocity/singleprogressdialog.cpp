#include "singleprogressdialog.h"
#include "ui_singleprogressdialog.h"

SingleProgressDialog::SingleProgressDialog(FileSystem system, void *device, Operation op, QString internalPath, QString externalPath, void *outEntry, QWidget *parent) :
    QDialog(parent), ui(new Ui::SingleProgressDialog), system(system), device(device), op(op), internalPath(internalPath), externalPath(externalPath), outEntry(outEntry)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    QtHelpers::GenAdjustWidgetAppearanceToOS(this);

    switch (op)
    {
        case OpInject:
            setWindowTitle("Injecting File");
            ui->lblIcon->setPixmap(QPixmap(":/Images/add.png"));
            break;
        case OpReplace:
            setWindowTitle("Replacing File");
            ui->lblIcon->setPixmap(QPixmap(":/Images/replace.png"));
            break;
        case OpBackup:
            setWindowTitle("Creating Backup");
            ui->lblIcon->setPixmap(QPixmap(":/Images/save.png"));
            break;
        case OpRestore:
            setWindowTitle("Restoring from Backup");
            ui->lblIcon->setPixmap(QPixmap(":/Images/restore.png"));
            break;
        case OpExtractAll:
            setWindowTitle("Extracting All Files");
            ui->lblIcon->setPixmap(QPixmap(":/Images/extract.png"));
            break;
        case OpExtract:
            setWindowTitle("Extracting File");
            ui->lblIcon->setPixmap(QPixmap(":/Images/extract.png"));
            break;
    }
}

void SingleProgressDialog::start()
{
    try
    {
        switch (system)
        {
            case FileSystemSTFS:
            {
                StfsPackage *package = reinterpret_cast<StfsPackage*>(device);
                if (op == OpReplace)
                    package->ReplaceFile(externalPath.toStdString(), internalPath.toStdString(), UpdateProgress, this);
                else if (op == OpInject)
                {
                    StfsFileEntry *entry = reinterpret_cast<StfsFileEntry*>(outEntry);
                    *entry = package->InjectFile(externalPath.toStdString(), internalPath.toStdString(), UpdateProgress, this);
                }
                break;
            }
            case FileSystemSVOD:
            {
                if (op != OpReplace)
                    throw std::string("Invalid SVOD operation.\n");
                else
                {
                    SVOD *svod = reinterpret_cast<SVOD*>(device);
                    SvodIO io = svod->GetSvodIO(internalPath.toStdString());

                    io.OverWriteFile(externalPath.toStdString(), UpdateProgress, this);
                }
                break;
            }
            case FileSystemFATX:
            {
                FatxDrive *drive = reinterpret_cast<FatxDrive*>(device);
                if (op == OpInject)
                {
                    try
                    {
                        FatxFileEntry *parent = reinterpret_cast<FatxFileEntry*>(outEntry);
                        drive->InjectFile(parent, internalPath.toStdString(), externalPath.toStdString(), UpdateProgress, this);
                    }
                    catch (string error)
                    {
                        QMessageBox::critical(this, "", "An error occurred while copy the file to your device.\n\n" + QString::fromStdString(error));
                    }
                }
                else if (op == OpBackup)
                {
                    try
                    {
                        FatxDrive *drive = reinterpret_cast<FatxDrive*>(device);
                        drive->CreateBackup(externalPath.toStdString(), UpdateProgress, this);
                    }
                    catch (string error)
                    {
                        QMessageBox::critical(this, "", "An error occurred while creating a backup for your device.\n\n" + QString::fromStdString(error));
                    }
                }
                else if (op == OpRestore)
                {
                    try
                    {
                        FatxDrive *drive = reinterpret_cast<FatxDrive*>(device);
                        drive->RestoreFromBackup(externalPath.toStdString(), UpdateProgress, this);
                    }
                    catch (string error)
                    {
                        QMessageBox::critical(this, "", "An error occurred while restoring your device from a backup.\n\n" + QString::fromStdString(error));
                    }
                }
                break;
            }
            case FileSystemISO:
            {
                ISO *iso = reinterpret_cast<ISO*>(device);
                if (op == OpExtractAll)
                {
                    iso->ExtractAll(externalPath.toStdString(), UpdateProgress, this);
                }
                else if (op == OpExtract)
                {
                    iso->ExtractFile(externalPath.toStdString(), internalPath.toStdString(), UpdateProgress, this);
                }
                break;
            }
        }
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Error", "An error occured during the operation.\n\n" + QString::fromStdString(error));
        close();
    }
}

SingleProgressDialog::~SingleProgressDialog()
{
    delete ui;
}

void UpdateProgress(void *arg, DWORD cur, DWORD total)
{
    SingleProgressDialog *dialog = reinterpret_cast<SingleProgressDialog*>(arg);
    dialog->ui->progressBar->setMaximum(total);
    dialog->ui->progressBar->setValue(cur);

    if (cur == total)
        dialog->close();

    QApplication::processEvents();
}

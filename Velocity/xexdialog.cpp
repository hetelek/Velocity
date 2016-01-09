#include "xexdialog.h"
#include "ui_xexdialog.h"

XexDialog::XexDialog(Xbox360Executable *xex, QWidget *parent) :
    QDialog(parent), ui(new Ui::XexDialog), xex(xex)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    // load the system import libraries
    std::vector<std::string> systemImportLibraries = xex->GetSystemImportLibraries();
    for (size_t i = 0; i < systemImportLibraries.size(); i++)
    {
        QString systemImportLibraryName = QString::fromStdString(systemImportLibraries.at(i));
        ui->lstSystemImportLibraries->addItem(systemImportLibraryName);
    }

    // load the module flags
    DWORD moduleFlags = xex->GetModuleFlags();
    ui->chkTitleModule->setChecked(!!(moduleFlags & ModuleTitle));
    ui->chkDeltaPatch->setChecked(!!(moduleFlags & ModulePatchDelta));
    ui->chkDLL->setChecked(!!(moduleFlags & ModuleDLL));
    ui->chkExportsToTitle->setChecked(!!(moduleFlags & ModuleExportsToTitle));
    ui->chkFullPatch->setChecked(!!(moduleFlags & ModulePatchFull));
    ui->chkSystemDebugger->setChecked(!!(moduleFlags & ModuleSystemDebugger));
    ui->chkUserMode->setChecked(!!(moduleFlags & ModuleUserMode));

    // load the ratings
    QString esrbRatingIconPath = ":/Images/esrb_rating_" + QString::fromStdString(xex->GetEsrbRatingText()) + ".png";
    ui->imgEsrbRating->setPixmap(QPixmap(esrbRatingIconPath));

    QString pegiRatingIconPath = ":/Images/pegi_" + QString::fromStdString(xex->GetPegiRatingText()) + ".jpg";
    ui->imgPegiRating->setPixmap(QPixmap(pegiRatingIconPath));
}

XexDialog::~XexDialog()
{
    delete ui;
}

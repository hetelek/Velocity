#include "xexdialog.h"
#include "ui_xexdialog.h"

XexDialog::XexDialog(Xbox360Executable *xex, QWidget *parent) :
    QDialog(parent), ui(new Ui::XexDialog), xex(xex)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    setWindowTitle("XEX Dialog [" + QString::fromStdString(xex->GetOriginalPEImageName()) + "]");

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

    QString pegiRatingIconPath;
    if (xex->GetPegiRating() == PEGI_Unrated)
        pegiRatingIconPath = ":/Images/HiddenAchievement.png";
    else
        pegiRatingIconPath = ":/Images/pegi_" + QString::fromStdString(xex->GetPegiRatingText()) + ".jpg";
    ui->imgPegiRating->setPixmap(QPixmap(pegiRatingIconPath));

    QString oflcAuRating;
    if (xex->GetOflcAURating() == OFLCAU_UNRATED)
        pegiRatingIconPath = ":/Images/HiddenAchievement.png";
    else
        oflcAuRating = ":/Images/rating_oflc_au_" + QString::fromStdString(xex->GetOflcAURatingText()) + ".png";
    ui->imgOflcAU->setPixmap(QPixmap(oflcAuRating));

    QString oflcNzRating;
    if (xex->GetOflcNZRating() == OFLCNZ_UNRATED)
        oflcNzRating = ":/Images/HiddenAchievement.png";
    else
        oflcNzRating = ":/Images/rating_oflc_nz_" + QString::fromStdString(xex->GetOflcNZRatingText()) + ".png";
    ui->imgOflcNZ->setPixmap(QPixmap(oflcNzRating));


    // load the static libraries
    std::vector<XexStaticLibraryInfo> staticLibs = xex->GetStaticLibraries();
    for (size_t i = 0; i < staticLibs.size(); i++)
    {
        XexStaticLibraryInfo staticLib = staticLibs.at(i);

        QTreeWidgetItem *staticLibItem = new QTreeWidgetItem(ui->treStaticLibraries);
        staticLibItem->setText(0, QString::fromStdString(staticLib.name));
        staticLibItem->setText(1, QString::fromStdString(VersionToString(staticLib.version)));
    }

    // load resources
    std::vector<XexResourceFileEntry> resources = xex->GetResourceFileInfo();
    for (size_t i = 0; i < resources.size(); i++)
    {
        XexResourceFileEntry resourceEntry = resources.at(i);

        QTreeWidgetItem *resourceItem = new QTreeWidgetItem(ui->treResources);
        resourceItem->setText(0, QString::fromStdString(resourceEntry.name));
        resourceItem->setText(1, QString::fromStdString(ByteSizeToString(resourceEntry.size)));
    }

    // load executable information
    if (xex->GetImageBaseAddress() != 0)
        AddExecutableProperty("Image Base Address", xex->GetImageBaseAddress());
    if (xex->GetEntryPoint() != 0)
        AddExecutableProperty("EntryPoint", xex->GetEntryPoint());
    if (xex->GetOriginalBaseAddress() != 0)
        AddExecutableProperty("Original Base Address", xex->GetOriginalBaseAddress());
    if (xex->GetDefaultStackSize() != 0)
        AddExecutableProperty("Default Stack Size", xex->GetDefaultStackSize());
    if (xex->GetDefaultFileSystemCacheSize() != 0)
        AddExecutableProperty("Default File System Cache Size", xex->GetDefaultFileSystemCacheSize());
    if (xex->GetDefaultHeapSize() != 0)
        AddExecutableProperty("Default Heap Size", xex->GetDefaultHeapSize());
    if (xex->GetTitleWorkspaceSize() != 0)
        AddExecutableProperty("Title Workspace Size", xex->GetTitleWorkspaceSize());

    // load game regions
    ui->chkNorthAmerica->setChecked(xex->HasRegion(XexRegionNorthAmerica));
    ui->chkJapan->setChecked(xex->HasRegion(XexRegionJapan));
    ui->chkChina->setChecked(xex->HasRegion(XexRegionChina));
    ui->chkRestOfAsia->setChecked(xex->HasRegion(XexRegionRestOfAsia));
    ui->chkAustraliaNewZealand->setChecked(xex->HasRegion(XexRegionAustraliaNewZealand));
    ui->chkRestOfEurope->setChecked(xex->HasRegion(XexRegionRestOfEurope));
    ui->chkRestOfWorld->setChecked(xex->HasRegion(XexRegionRestOfWorld));

    // load the allowed media types
    DWORD allowedMediaTypes = xex->GetAllowedMediaTypes();
    ui->chkHardDisk->setChecked(!!(allowedMediaTypes & HardDisk));
    ui->chkDVDX2->setChecked(!!(allowedMediaTypes & DVDX2));
    ui->chkDVDCD->setChecked(!!(allowedMediaTypes & DVDCD));
    ui->chkDVD5->setChecked(!!(allowedMediaTypes & DVD5));
    ui->chkDVD9->setChecked(!!(allowedMediaTypes & DVD9));
    ui->chkSystemFlash->setChecked(!!(allowedMediaTypes & SystemFlash));
    ui->chkMemoryUnit->setChecked(!!(allowedMediaTypes & MemoryUnit));
    ui->chkMassStorageDevice->setChecked(!!(allowedMediaTypes & MassStorageDevice));
    ui->chkSMBFileSystem->setChecked(!!(allowedMediaTypes & SMBFileSystem));
    ui->chkDirectFromRAM->setChecked(!!(allowedMediaTypes & DirectFromRAM));
    ui->chkSecureOpticalDevice->setChecked(!!(allowedMediaTypes & SecureVirtualOpticalDevice));
    ui->chkWirelessNStorageDevice->setChecked(!!(allowedMediaTypes & WirelessNStorageDevice));
    ui->chkSystemExtendedPartition->setChecked(!!(allowedMediaTypes & SystemExtendedPartition));
    ui->chkSystemAuxillaryPartition->setChecked(!!(allowedMediaTypes & SystemAuxillaryPartition));
    ui->chkInsecurePackage->setChecked(!!(allowedMediaTypes & InsecurePackage));
    ui->chkSaveGamePackage->setChecked(!!(allowedMediaTypes & SaveGamePackage));
    ui->chkLocallySignedPackage->setChecked(!!(allowedMediaTypes & LocallySignedPackage));
    ui->chkLiveSignedPackage->setChecked(!!(allowedMediaTypes & LiveSignedPackage));
    ui->chkXboxPlatformPackage->setChecked(!!(allowedMediaTypes & XboxPlatformPackage));

    // load the image flags
    DWORD imageFlags = xex->GetImageFlags();
    ui->chkXgd2MediaOnly->setChecked(!!(imageFlags & XGD2MediaOnly));
    ui->chkCardeaKey->setChecked(!!(imageFlags & CardeaKey));
    ui->chkXeikaKey->setChecked(!!(imageFlags & XeikaKey));
    ui->chkTitleUserMode->setChecked(!!(imageFlags & TitleUserMode));
    ui->chkSystemUserMode->setChecked(!!(imageFlags & SystemUserMode));
    ui->chkOrange0->setChecked(!!(imageFlags & Orange0));
    ui->chkOrange1->setChecked(!!(imageFlags & Orange1));
    ui->chkOrange2->setChecked(!!(imageFlags & Orange2));
    ui->chkIptvSignupApplication->setChecked(!!(imageFlags & IPTVSignupApplication));
    ui->chkIptvTitleApplication->setChecked(!!(imageFlags & IPTVTitleApplication));
    ui->chkKeyvaultPrivilegesRequired->setChecked(!!(imageFlags & KeyvaultPrivilegesRequired));
    ui->chkOnlineActivationRequired->setChecked(!!(imageFlags & OnlineActivationRequired));
    ui->chkPageSize4KB->setChecked(!!(imageFlags & PageSize4KB));
    ui->chkNoGameRegion->setChecked(!!(imageFlags & NoGameRegion));
    ui->chkRevocationCheckOptional->setChecked(!!(imageFlags & RevocationCheckOptional));
    ui->chkRevocationCheckRequired->setChecked(!!(imageFlags & RevocationCheckRequired));
}

XexDialog::~XexDialog()
{
    delete xex;
    delete ui;
}

void XexDialog::AddExecutableProperty(QString name, DWORD value)
{
    AddExecutableProperty(name, "0x" + QString::fromStdString(Utils::ConvertToHexString(value)));
}

void XexDialog::AddExecutableProperty(QString name, QString value)
{
    QTreeWidgetItem *property = new QTreeWidgetItem(ui->treExecutableInfo);
    property->setText(0, name);
    property->setText(1, value);
}

void XexDialog::on_pushButton_clicked()
{
    QString suggestedBaseFilePath = QtHelpers::DesktopLocation() + "/" + QString::fromStdString(xex->GetOriginalPEImageName());
    QString outBaseFilePath = QFileDialog::getSaveFileName(this, "Save the base file", suggestedBaseFilePath);

    if (outBaseFilePath.isEmpty())
        return;

    try
    {
        xex->ExtractDecryptedData(outBaseFilePath.toStdString());
    }
    catch (std::string error)
    {
        QMessageBox::critical(this, "Error Extracting Base File", "An error occurred while extracting the base file.\n\n" +
                              QString::fromStdString(error));
    }
}

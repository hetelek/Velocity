#include "mainwindow.h"

MainWindow::MainWindow(QList<QUrl> arguments, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    settings = new QSettings("Exetelek", "Velocity");
    if (!settings->contains("PackageDropAction"))
        settings->setValue("PackageDropAction", 0);
    if (!settings->contains("ProfileDropAction"))
        settings->setValue("ProfileDropAction", 0);
    if (!settings->contains("PluginPath"))
        settings->setValue("PluginPath", QtHelpers::ExecutingDirectory() + "plugins");
    if (!settings->contains("AnonData"))
        settings->setValue("AnonData", true);

    LoadAllPlugins();
    setCentralWidget(ui->mdiArea);
    ui->mdiArea->setAcceptDrops(false);
    setAcceptDrops(true);

    QString fileNames[] = { "femaleAvatar.bin", "FFFE07D1.gpd", "KV_D.bin", "KV_R.bin" };

    // check for all of the startup files
    QString missingFiles = "";
    for (DWORD i = 0; i < 4; i++)
        if (!QFile::exists(QtHelpers::ExecutingDirectory() + "/" + fileNames[i]))
            missingFiles += fileNames[i] + ", ";

    if (missingFiles.size() != 0)
    {
        // remove the extra ", "
        missingFiles.chop(2);

        ui->statusBar->showMessage("The following file(s) weren't found: " + missingFiles, 10000);
    }
    else
        ui->statusBar->showMessage("Welcome to Velocity!", 10000);

    LoadFiles(arguments);

    GitHubCommitsDialog *dialog = new GitHubCommitsDialog(this);
    ui->mdiArea->addSubWindow(dialog);
    dialog->show();
}

void MainWindow::LoadPlugin(QString filename, bool addToMenu)
{
    QPluginLoader loader(filename);
    loader.setParent(ui->mdiArea);
    QObject *possiblePlugin = loader.instance();

    if (possiblePlugin)
    {
        IGameModder *game = qobject_cast<IGameModder*>(possiblePlugin);
        IGPDModder *gpd = qobject_cast<IGPDModder*>(possiblePlugin);

        if (game)
        {
            if (addToMenu)
            {
                QAction *action = new QAction(game->ToolName(), this);
                action->setData(QVariant(filename));
                connect(action, SIGNAL(triggered()), this, SLOT(on_actionGame_Modder_triggered()));
                ui->menuGame_Modders->addAction(action);
            }
            else
            {
                QWidget *widget = game->GetDialog();

                QString fileName = QFileDialog::getOpenFileName(this, tr("Open a Save Game"), QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), "All Files (*)");
                if (fileName.isNull())
                {
                    delete possiblePlugin;
                    return;
                }

                StfsPackage *package;
                try
                {
                    package = new StfsPackage(fileName.toStdString());
                }
                catch (string error)
                {
                    QMessageBox::critical(this, "Opening Error", "Could not open save game package.\n\n" + QString::fromStdString(error));
                    return;
                }
                catch (...)
                {
                    QMessageBox::critical(this, "Opening Error", "Could not open save game package for an unknown reason.");
                    return;
                }

                ui->mdiArea->addSubWindow(widget);
                widget->show();
                game->LoadPackage(package, this);
            }
        }
        else if (gpd)
        {
            if (addToMenu)
            {
                QAction *action = new QAction(gpd->ToolName(), this);
                action->setData(QVariant(filename));
                connect(action, SIGNAL(triggered()), this, SLOT(on_actionGame_Modder_triggered()));
                ui->menuProfile_Modders->addAction(action);
            }
            else
            {
                QWidget *widget = gpd->GetDialog();

                QString fileName = QFileDialog::getOpenFileName(this, tr("Open a Profile"), QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), "All Files (*)");
                if (fileName.isNull())
                {
                    delete possiblePlugin;
                    return;
                }

                try
                {
                    StfsPackage *package = new StfsPackage(fileName.toStdString());

                    QString tempPath = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");

                    Arguments *args = new Arguments;
                    args->package = package;
                    args->tempFilePath = tempPath;

                    package->ExtractFile(QString("%1").arg(gpd->TitleID(), 8, 16, QChar('0')).toUpper().toStdString() + ".gpd", tempPath.toStdString());

                    GameGPD *gameGPD = new GameGPD(tempPath.toStdString());
                    gpd->LoadGPD(gameGPD, (void*)args);

                    ui->mdiArea->addSubWindow(widget);
                    widget->show();

                    connect(widget, SIGNAL(InjectGPD()), this, SLOT(InjectGPD()));
                }
                catch (string error)
                {
                    QMessageBox::critical(this, "Opening Error", "Could not extract gpd.\n\n" + QString::fromStdString(error));
                    return;
                }
                catch (...)
                {
                    QMessageBox::critical(this, "Opening Error", "Could not extract gpd for an unknown reason.");
                    return;
                }
            }
        }
    }
}


void MainWindow::on_actionDonate_triggered()
{
    // donate page here
}

void MainWindow::on_actionView_Wiki_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/hetelek/Velocity/wiki"));
}

void MainWindow::InjectGPD()
{
    IGPDModder *gpd = qobject_cast<IGPDModder*>(sender());

    if (!gpd)
        throw "Some shit broke.";

    Arguments *args = (Arguments*)gpd->Arguments;
    try
    {
        args->package->ReplaceFile(args->tempFilePath.toStdString(), QString("%1").arg(gpd->TitleID(), 8, 16, QChar('0')).toUpper().toStdString() + ".gpd");
        args->package->Rehash();
        args->package->Resign(QtHelpers::GetKVPath(args->package->metaData->certificate.ownerConsoleType, this));
        args->package->Close();
        delete args->package;
    }
    catch (string error)
    {
        QMessageBox::critical(NULL, "Couldn't Repalce GPD", "The GPD could not be replaced.\n\n" + QString::fromStdString(error));
        try
        {
            delete args->package;
        }
        catch(...) { }
    }
}

void MainWindow::LoadAllPlugins()
{
    QDir path(settings->value("PluginPath").toString());

    foreach (QString filename, path.entryList(QDir::Files))
    {
        qDebug() << filename;
        LoadPlugin(path.absoluteFilePath(filename), true);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

void MainWindow::on_actionTheme_Creator_triggered()
{
    ThemeCreationWizard wiz(ui->statusBar, this);
    wiz.exec();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> filePaths = event->mimeData()->urls();

    // iterate through all of the files dropped
    LoadFiles(filePaths);
}

void MainWindow::LoadFiles(QList<QUrl> &filePaths)
{
    for (DWORD i = 0; i < filePaths.size(); i++)
    {
#ifdef __WIN32__
        std::string fileName = QString(filePaths.at(i).encodedPath()).mid(1).replace("%20", " ").toStdString();
#else
        std::string fileName = QString(filePaths.at(i).encodedPath()).replace("%20", " ").toStdString();
#endif

        // make sure the file exists
        if (!QFile::exists(QString::fromStdString(fileName)))
            continue;

        try
        {
            // read in the file magic
            FileIO io(fileName);
            DWORD fileMagic = io.readDword();
            io.close();

            switch (fileMagic)
            {
                case CON:
                case LIVE:
                case PIRS:
                {
                    StfsPackage *package = new StfsPackage(fileName);

                    if (package->metaData->contentType != Profile)
                    {
                        if (settings->value("PackageDropAction").toInt() == OpenInPackageViewer)
                        {
                            PackageViewer *viewer = new PackageViewer(ui->statusBar, package, this);
                            ui->mdiArea->addSubWindow(viewer);
                            viewer->show();

                            ui->statusBar->showMessage("STFS package loaded successfully.", 3000);
                        }
                        else
                        {
                            package->Rehash();
                            package->Resign(QtHelpers::GetKVPath(package->metaData->certificate.ownerConsoleType, this));

                            delete package;

                            ui->statusBar->showMessage("STFS package rehashed and resigned successfully.", 3000);
                        }
                    }
                    else
                    {
                        if (settings->value("ProfileDropAction").toInt() == OpenInPackageViewer)
                        {
                            PackageViewer *viewer = new PackageViewer(ui->statusBar, package, this);
                            ui->mdiArea->addSubWindow(viewer);
                            viewer->show();

                            ui->statusBar->showMessage("STFS package loaded successfully.", 3000);
                        }
                        else if (settings->value("ProfileDropAction").toInt() == RehashAndResign)
                        {
                            package->Rehash();
                            package->Resign(QtHelpers::GetKVPath(package->metaData->certificate.ownerConsoleType, this));

                            delete package;

                            ui->statusBar->showMessage("STFS package rehashed and resigned successfully.", 3000);
                        }
                        else
                        {
                            bool ok;
                            ProfileEditor *editor = new ProfileEditor(ui->statusBar, package, true, &ok, this);

                            if (ok)
                            {
                                ui->mdiArea->addSubWindow(editor);
                                editor->show();
                            }
                        }
                    }

                    break;
                }
                case 0x58444246:
                {
                    GPDBase *gpd = new GPDBase(fileName);
                    ui->statusBar->showMessage("GPD parsed successfully", 3000);

                    XdbfDialog *dialog = new XdbfDialog(ui->statusBar, gpd, NULL, this);
                    ui->mdiArea->addSubWindow(dialog);
                    dialog->show();

                    break;
                }
                case 0x53545242:
                {
                    AvatarAsset *asset = new AvatarAsset(fileName);

                    StrbDialog *dialog = new StrbDialog(asset, this);
                    ui->mdiArea->addSubWindow(dialog);
                    dialog->show();

                    ui->statusBar->showMessage("STRB file parsed successfully", 3000);
                    break;
                }
                default:
                    QMessageBox::warning(this, "Unknown File Format", "The following file is an unknown format. Velocity can only read STFS, XDBF, and STRB files.\n\n" + QString::fromStdString(fileName));
                    break;
            }
        }
        catch (string error)
        {
            QMessageBox::critical(this, "Error", "An error occurred while opening the file.\n\n" + QString::fromStdString(error));
        }
    }
}

void MainWindow::on_actionProfile_Editor_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open a Profile"), QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), "All Files (*)");

    if (fileName.isEmpty())
        return;

    try
    {
        StfsPackage *package = new StfsPackage(fileName.toStdString());

        bool ok;
        ProfileEditor *editor = new ProfileEditor(ui->statusBar, package, true, &ok, this);

        if (ok)
        {
            ui->mdiArea->addSubWindow(editor);
            editor->show();
        }
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Profile Error", "An error has occurred while opening the profile.\n\n" + QString::fromStdString(error));
    }
}


void MainWindow::on_actionAbout_triggered()
{
    About about(this);
    about.exec();
}
void MainWindow::on_actionPackage_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Package"), QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), "All Files (*)");

    if (fileName.isEmpty())
        return;

    try
    {
        StfsPackage *package = new StfsPackage(fileName.toStdString());

        PackageViewer *viewer = new PackageViewer(ui->statusBar, package, this);
        ui->mdiArea->addSubWindow(viewer);
        viewer->show();

        ui->statusBar->showMessage("Stfs package loaded successfully.", 3000);
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Package Error", "An error has occurred while opening the package.\n\n" + QString::fromStdString(error));
    }
}

void MainWindow::on_actionXDBF_File_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open XDBF File"), QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), "GPD File (*.gpd *.fit);;All Files (*)");

    if (fileName.isEmpty())
        return;

    try
    {
        GPDBase *gpd = new GPDBase(fileName.toStdString());
        ui->statusBar->showMessage("GPD parsed successfully", 3000);

        XdbfDialog *dialog = new XdbfDialog(ui->statusBar, gpd, NULL, this);
        ui->mdiArea->addSubWindow(dialog);
        dialog->show();
    }
    catch (string error)
    {
        QMessageBox::critical(this, "GPD Error", "An error has occurred while opening the GPD.\n\n" + QString::fromStdString(error));
    }
}

void MainWindow::on_actionSTRB_File_triggered()
{
    try
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open an Avatar Asset"), QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), "BIN File (*.bin);;All Files (*)");
        if (fileName.isEmpty())
            return;

        AvatarAsset *asset = new AvatarAsset(fileName.toStdString());

        StrbDialog *dialog = new StrbDialog(asset, this);
        ui->mdiArea->addSubWindow(dialog);
        dialog->show();
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Error", "An error occured while opening the STRB package.\n\n" + QString::fromStdString(error));
    }
}

void MainWindow::on_actionCreate_Package_triggered()
{
    QString packagePath;
    CreationWizard wizard(&packagePath, this);
    wizard.exec();

    if (packagePath == "error")
        return;

    try
    {
        StfsPackage *package = new StfsPackage(packagePath.toStdString());

        PackageViewer *viewer = new PackageViewer(ui->statusBar, package, this);
        ui->mdiArea->addSubWindow(viewer);
        viewer->show();

        ui->statusBar->showMessage("Stfs package created successfully.", 3000);
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Package Error", "An error has occurred while opening the package.\n\n" + QString::fromStdString(error));
    }
}

void MainWindow::on_actionTitle_ID_Finder_triggered()
{
    TitleIdFinderDialog *dialog = new TitleIdFinderDialog(ui->statusBar, this);
    ui->mdiArea->addSubWindow(dialog);
    dialog->show();
}

void MainWindow::on_actionProfile_Creator_triggered()
{
    if (!QFile::exists(QtHelpers::ExecutingDirectory() + "FFFE07D1.gpd"))
    {
        QMessageBox::critical(this, "File Not Found", "The file FFFE07D1.gpd was not found. This file must be in the same directory as this application.");
        return;
    }

    ProfileCreatorWizard wiz(ui->statusBar, this);
    wiz.exec();
}

void MainWindow::on_actionGame_Adder_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Package"), QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), "All Files (*)");
    if (fileName.isEmpty())
        return;

    StfsPackage *package = new StfsPackage(fileName.toStdString());

    GameAdderDialog dialog(package, this);
    dialog.exec();

    package->Close();
}

void MainWindow::on_actionGamer_Picture_Pack_Creator_triggered()
{
    GamerPicturePackDialog *dialog = new GamerPicturePackDialog(ui->statusBar, this);
    ui->mdiArea->addSubWindow(dialog);
    dialog->show();
}

void MainWindow::on_actionGame_Modder_triggered()
{
    if (sender())
    {
        QAction *menuAction = (QAction*)sender();
        LoadPlugin(menuAction->data().toString(), false);
    }
}

void MainWindow::on_actionPreferences_triggered()
{
    PreferencesDialog dialog(this);
    dialog.exec();
}

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
        settings->setValue("PluginPath", "./plugins");
    if (!settings->contains("AnonData"))
        settings->setValue("AnonData", true);

    LoadAllPlugins();
    setCentralWidget(ui->mdiArea);
    ui->mdiArea->setAcceptDrops(false);
    setAcceptDrops(true);

    QStringList fileNames;
    fileNames.append("femaleAvatar.bin");
    fileNames.append("FFFE07D1.gpd");
    fileNames.append("KV_D.bin");
    fileNames.append("KV_R.bin");
    fileNames.append("male default.png");
    fileNames.append("female default.png");

    // check for all of the startup files
    QString missingFiles = "";
    for (int i = 0; i < fileNames.size(); i++)
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

    GitHubCommitsDialog *dialog = new GitHubCommitsDialog(this);
    ui->mdiArea->addSubWindow(dialog);
    dialog->show();

    LoadFiles(arguments);
}

void MainWindow::LoadPlugin(QString filename, bool addToMenu, StfsPackage *package)
{
    // check if it's from the package viewer
    bool fromPackageViewer = package != NULL;

    // create a plugin loader/instance of plugin
    QPluginLoader loader(filename);
    QObject *possiblePlugin = loader.instance();

    // check if it's a possible plugin
    if (possiblePlugin)
    {
        // cast it as our available interfaces, to see if it meets criteria
        IGameModder *game = qobject_cast<IGameModder*>(possiblePlugin);
        IGPDModder *gpd = qobject_cast<IGPDModder*>(possiblePlugin);

        // if it's a game modder
        if (game)
        {
            // check if we are running it or adding it
            if (addToMenu)
            {
                // create the action
                QAction *action = new QAction(game->ToolName(), this);

                // set data
                action->setIcon(game->GetDialog()->windowIcon());
                action->setData(QVariant(filename));

                // connect it
                connect(action, SIGNAL(triggered()), this, SLOT(on_actionModder_triggered()));

                // add the action
                gameActions.push_back(action);
                ui->menuGame_Modders->addAction(action);
            }
            else
            {
                // get the dialog, and connect signals/slots
                QWidget *widget = game->GetDialog();
                connect(widget, SIGNAL(PluginFinished()), this, SLOT(PluginFinished()));

                if (package == NULL)
                {
                    QString fileName = QFileDialog::getOpenFileName(this, tr("Open a Save Game"), QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), "All Files (*)");
                    if (fileName.isNull())
                        return;

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
                }

                ui->mdiArea->addSubWindow(widget);
                widget->show();
                game->LoadPackage(package, this);
            }
        }
        else if (gpd)
        {
            // check if we are running it or adding it
            if (addToMenu)
            {
                // create the action
                QAction *action = new QAction(gpd->ToolName(), this);

                // set data
                action->setIcon(gpd->GetDialog()->windowIcon());
                action->setData(QVariant(filename));
                action->setProperty("titleid", QVariant((unsigned int)gpd->TitleID()));

                // connect it
                connect(action, SIGNAL(triggered()), this, SLOT(on_actionModder_triggered()));

                // add the action
                gpdActions.push_back(action);
                ui->menuProfile_Modders->addAction(action);

                qDebug() << loader.unload();
            }
            else
            {
                // get the dialog, and connect signals/slots
                QDialog *widget = gpd->GetDialog();
                connect(widget, SIGNAL(PluginFinished()), this, SLOT(PluginFinished()));

                try
                {
                    // if it's not from the package viewer, ask for a file
                    if (!fromPackageViewer)
                    {
                        QString fileName = QFileDialog::getOpenFileName(this, tr("Open a Profile"), QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), "All Files (*)");
                        if (fileName.isNull())
                            return;

                        package = new StfsPackage(fileName.toStdString());
                    }

                    // generate temporary path
                    QString tempPath = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");

                    // set arguments
                    Arguments *args = new Arguments;
                    args->package = package;
                    args->tempFilePath = tempPath;
                    args->fromPackageViewer = fromPackageViewer;

                    // extract the gpd
                    package->ExtractFile(QString("%1").arg(gpd->TitleID(), 8, 16, QChar('0')).toUpper().toStdString() + ".gpd", tempPath.toStdString());

                    // load the gpd in the modder
                    GameGPD *gameGPD = new GameGPD(tempPath.toStdString());
                    bool ok;
                    gpd->LoadGPD(gameGPD, &ok, (void*)args);

                    if (ok)
                    {
                        widget->exec();
                        widget->close();
                        qDebug() << loader.unload();
                    }

                }
                catch (string error)
                {
                    QMessageBox::critical(this, "Opening Error", "Could not extract gpd.\n\n" + QString::fromStdString(error));
                }
                catch (...)
                {
                    QMessageBox::critical(this, "Opening Error", "Could not extract gpd for an unknown reason.");
                }
            }
        }
    }
}


void MainWindow::on_actionDonate_triggered()
{
    QDesktopServices::openUrl(QUrl("http://goo.gl/0nBpD"));
}

void MainWindow::on_actionView_Wiki_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/hetelek/Velocity/wiki"));
}

void MainWindow::PluginFinished()
{
    IGPDModder *gpd = qobject_cast<IGPDModder*>(sender());
    IGameModder *game = qobject_cast<IGameModder*>(sender());

    Arguments *args;
    QMdiSubWindow *subWin = 0;

    // check if a gpd modder finished
    if (gpd)
    {
        try
        {
            // get the args
            args = (Arguments*)gpd->Arguments;

            // replace the unmodified with the modified
            args->package->ReplaceFile(args->tempFilePath.toStdString(), QString("%1").arg(gpd->TitleID(), 8, 16, QChar('0')).toUpper().toStdString() + ".gpd");

            // cast the parent as a mdi sub window
            subWin = qobject_cast<QMdiSubWindow*>(gpd->GetDialog()->parent());
        }
        catch (string error)
        {
            QMessageBox::critical(NULL, "Couldn't Repalce GPD", "The GPD could not be replaced.\n\n" + QString::fromStdString(error));
            try
            {
                if (!args->fromPackageViewer)
                    delete args->package;
            }
            catch(...) { }
            return;
        }
    }
    else if (game)
    {
        // get the args
        args = (Arguments*)game->Arguments;
    }
    else
        throw "Invalid plugin finished";

    // properly close the sub window if it is one
    if (subWin)
        subWin->close();

    // rehash/resign
    args->package->Rehash();
    args->package->Resign(QtHelpers::GetKVPath(args->package->metaData->certificate.ownerConsoleType, this));

    // dispose everything if it's not from the package viewer
    if (!args->fromPackageViewer)
    {
        args->package->Close();
        delete args->package;
    }
}

void MainWindow::LoadAllPlugins()
{
    QDir path(settings->value("PluginPath").toString());
    qDebug() << "Plugin Directory: " << path.absolutePath();

    foreach (QString filename, path.entryList(QDir::Files))
    {
        qDebug() << filename;
        LoadPlugin(path.absoluteFilePath(filename), true);
    }
}

MainWindow::~MainWindow()
{
    // close all of the open subviews
    QList<QMdiSubWindow*> subWindows = ui->mdiArea->subWindowList();
    for (DWORD i = 0; i < subWindows.length(); i++)
    {
        subWindows.at(i)->setParent(NULL);
        delete subWindows.at(i);
    }
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
    for (int i = 0; i < filePaths.size(); i++)
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
                            PackageViewer *viewer = new PackageViewer(ui->statusBar, package, gpdActions, this);
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
                            PackageViewer *viewer = new PackageViewer(ui->statusBar, package, gpdActions, this);
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

        bool *ok = new bool;
        ProfileEditor *editor = new ProfileEditor(ui->statusBar, package, true, ok, this);

        if (*ok)
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
        PackageViewer *viewer = new PackageViewer(ui->statusBar, package, gpdActions, this);
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

        PackageViewer *viewer = new PackageViewer(ui->statusBar, package, gpdActions, this);
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

void MainWindow::on_actionModder_triggered()
{
    if (sender())
    {
        QAction *menuAction = (QAction*)sender();

        // get package
        StfsPackage *package = NULL;
        if (menuAction->property("package").isValid())
             package = menuAction->property("package").value<StfsPackage*>();

        LoadPlugin(menuAction->data().toString(), false, package);
    }
}

void MainWindow::on_actionPreferences_triggered()
{
    PreferencesDialog dialog(this);
    dialog.exec();
}

void MainWindow::on_actionFATX_File_Path_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Package"), QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), "All Files (*)");

    if (fileName.isEmpty())
        return;

    try
    {
        StfsPackage *package = new StfsPackage(fileName.toStdString());
        FATXPathGenDialog *dialog = new FATXPathGenDialog(package, this);
        ui->mdiArea->addSubWindow(dialog);
        dialog->show();
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Package Error", "An error has occurred while opening the package.\n\n" + QString::fromStdString(error));
    }
}

#include "mainwindow.h"

MainWindow::MainWindow(QList<QUrl> arguments, QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow)
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
        settings->setValue("AnonData", false);

    setCentralWidget(ui->mdiArea);
    ui->mdiArea->setAcceptDrops(false);
    setAcceptDrops(true);

    ui->statusBar->showMessage("Welcome to Velocity!", 10000);

    LoadAllPlugins();

    GitHubCommitsDialog *dialog = new GitHubCommitsDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    QtHelpers::AddSubWindow(ui->mdiArea, dialog);
    dialog->show();

    pluginManager = new QNetworkAccessManager(this);
    connect(pluginManager, SIGNAL(finished(QNetworkReply*)), this,
            SLOT(pluginVersionReplyFinished(QNetworkReply*)));

    firstUpdateCheck = true;

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this,
            SLOT(versionReplyFinished(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl("http://velocity.expetelek.com/app.data")));

    LoadFiles(arguments);
}

void MainWindow::LoadPlugin(QString filename, bool addToMenu, StfsPackage *package)
{
    // check if it's from the package viewer
    bool fromPackageViewer = package != nullptr;

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
                action->setProperty("titleid", QVariant((unsigned int)game->TitleID()));

                // connect it
                connect(action, SIGNAL(triggered()), this, SLOT(on_actionModder_triggered()));

                // add the action
                gameActions.push_back(action);
                ui->menuGame_Modders->addAction(action);
                qDebug() << loader.unload();
            }
            else
            {
                pluginManager->setProperty("name", game->ToolName());
                pluginManager->setProperty("version", game->Version());
                pluginManager->get(QNetworkRequest(QUrl("http://velocity.expetelek.com/plugin.php?tid=" +
                        QString::number(game->TitleID(), 16) + "&type=0")));

                // get the dialog, and connect signals/slots
                QDialog *widget = (QDialog*)game->GetDialog();
                connect(widget, SIGNAL(PluginFinished()), this, SLOT(PluginFinished()));

                try
                {
                    if (!fromPackageViewer)
                    {
                        QString fileName = QFileDialog::getOpenFileName(this, tr("Open a Save Game"),
                                QtHelpers::DefaultLocation(), "All Files (*)");

                        if (fileName.isNull())
                            return;


                        package = new StfsPackage(fileName.toStdString());
                    }

                    if (package->metaData->contentType != SavedGame)
                    {
                        QMessageBox::critical(this, "Invalid Package", "The selected package is not a 'Saved Game' file.");
                        return;
                    }

                    Arguments *args = new Arguments;
                    args->package = package;
                    args->fromPackageViewer = fromPackageViewer;

                    bool ok;
                    game->LoadPackage(package, &ok, (void*)args);

                    if (ok)
                    {
                        widget->exec();
                        widget->close();
                        qDebug() << loader.unload();
                    }
                }
                catch (string error)
                {
                    QMessageBox::critical(this, "Opening Error",
                            "Could not open save game package.\n\n" + QString::fromStdString(error));
                    return;
                }
                catch (...)
                {
                    QMessageBox::critical(this, "Opening Error",
                            "Could not open save game package for an unknown reason.");
                    return;
                }
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
                pluginManager->setProperty("name", gpd->ToolName());
                pluginManager->setProperty("version", gpd->Version());
                pluginManager->get(QNetworkRequest(QUrl("http://velocity.expetelek.com/plugin.php?tid=" +
                        QString::number(gpd->TitleID(), 16) + "&type=0")));

                // get the dialog, and connect signals/slots
                QDialog *widget = (QDialog*)gpd->GetDialog();
                connect(widget, SIGNAL(PluginFinished()), this, SLOT(PluginFinished()));

                try
                {
                    // if it's not from the package viewer, ask for a file
                    if (!fromPackageViewer)
                    {
                        QString fileName = QFileDialog::getOpenFileName(this, tr("Open a Profile"),
                                QtHelpers::DefaultLocation(), "All Files (*)");

                        if (fileName.isNull())
                            return;

                        package = new StfsPackage(fileName.toStdString());
                    }

                    // generate temporary path
                    QString tempPath = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{",
                            "").replace("}", "").replace("-", "");

                    // set arguments
                    Arguments *args = new Arguments;
                    args->package = package;
                    args->tempFilePath = tempPath;
                    args->fromPackageViewer = fromPackageViewer;

                    // extract the gpd
                    package->ExtractFile(QString("%1").arg(gpd->TitleID(), 8, 16,
                            QChar('0')).toUpper().toStdString() + ".gpd", tempPath.toStdString());

                    // load the gpd in the modder
                    GameGpd *gameGpd = new GameGpd(tempPath.toStdString());
                    bool ok;
                    gpd->LoadGPD(gameGpd, &ok, (void*)args);

                    if (ok)
                    {
                        widget->exec();
                        widget->close();
                        qDebug() << loader.unload();
                    }

                }
                catch (string error)
                {
                    QMessageBox::critical(this, "Opening Error",
                            "Could not extract gpd.\n\n" + QString::fromStdString(error));
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
    QDesktopServices::openUrl(
        QUrl("https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=GW3CMHU8F9DT2"));
}

void MainWindow::on_actionDevice_Viewer_triggered()
{
    DeviceViewer *viewer = new DeviceViewer(ui->statusBar, gpdActions, gameActions, this);
    viewer->setAttribute(Qt::WA_DeleteOnClose);
    QtHelpers::AddSubWindow(ui->mdiArea, viewer);
    viewer->show();
    viewer->LoadDrives();
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
            args->package->ReplaceFile(args->tempFilePath.toStdString(), QString("%1").arg(gpd->TitleID(), 8,
                    16, QChar('0')).toUpper().toStdString() + ".gpd");

            // cast the parent as a mdi sub window
            subWin = qobject_cast<QMdiSubWindow*>(gpd->GetDialog()->parent());
        }
        catch (string error)
        {
            QMessageBox::critical(nullptr, "Couldn't Repalce Gpd",
                    "The Gpd could not be replaced.\n\n" + QString::fromStdString(error));
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
    args->package->Resign(QtHelpers::GetKVPath(args->package->metaData->certificate.ownerConsoleType,
            this));

    // dispose everything if it's not from the package viewer
    if (!args->fromPackageViewer)
    {
        args->package->Close();
        delete args->package;
    }
}

void MainWindow::LoadAllPlugins()
{
    ui->menuProfile_Modders->clear();
    ui->menuGame_Modders->clear();

    gpdActions.clear();
    gameActions.clear();

    QDir path(QtHelpers::ExecutingDirectory());
    QDir pluginPath(path.absoluteFilePath(settings->value("PluginPath").toString()));
    qDebug() << "Plugin Directory: " << pluginPath.absolutePath();

    foreach (QString filename, pluginPath.entryList(QDir::Files))
    {
        qDebug() << filename;
        LoadPlugin(pluginPath.absoluteFilePath(filename), true);
    }
}

MainWindow::~MainWindow()
{
    // close all of the open subviews
    QList<QMdiSubWindow*> subWindows = ui->mdiArea->subWindowList();
    for (int i = 0; i < subWindows.length(); i++)
    {
        subWindows.at(i)->setParent(nullptr);
        delete subWindows.at(i);
    }
    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
    {
        event->acceptProposedAction();
        if (settings->value("PackageDropAction").toInt() == OpenInPackageViewer)
            ui->statusBar->showMessage("Open file(s)");
        else
            ui->statusBar->showMessage("Rehash and resign file(s)");
    }
}

void MainWindow::on_actionTheme_Creator_triggered()
{
    ThemeCreationWizard wiz(ui->statusBar, this);
    wiz.exec();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    ui->statusBar->showMessage("");

    QList<QUrl> filePaths = event->mimeData()->urls();

    // iterate through all of the files dropped
    LoadFiles(filePaths);
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *)
{
    ui->statusBar->showMessage("");
}

void MainWindow::LoadFiles(QList<QUrl> &filePaths)
{
    for (int i = 0; i < filePaths.size(); i++)
    {
        std::string fileName = filePaths.at(i).toLocalFile().toStdString();

        // make sure the file exists
        if (!QFile::exists(QString::fromStdString(fileName)))
            continue;

        try
        {
            // read in the file magic
            FileIO io(fileName);
            DWORD fileMagic = io.ReadDword();
            io.Close();

            switch (fileMagic)
            {
                case CON:
                case LIVE:
                case PIRS:
                {
                    try
                    {
                        StfsPackage *package = new StfsPackage(fileName);

                        if (package->metaData->contentType != Profile)
                        {
                            if (settings->value("PackageDropAction").toInt() == OpenInPackageViewer)
                            {
                                PackageViewer *viewer = new PackageViewer(ui->statusBar, package, gpdActions, gameActions, this);
                                viewer->setAttribute(Qt::WA_DeleteOnClose);
                                QtHelpers::AddSubWindow(ui->mdiArea, viewer);
                                viewer->show();

                                ui->statusBar->showMessage("STFS package loaded successfully.", 3000);
                            }
                            else
                            {
                                package->Rehash();
                                if (fileMagic == CON)
                                {
                                    package->Resign(QtHelpers::GetKVPath(package->metaData->certificate.ownerConsoleType, this));
                                }

                                delete package;

                                ui->statusBar->showMessage("STFS package rehashed and resigned successfully.", 3000);
                            }
                        }
                        else
                        {
                            if (settings->value("ProfileDropAction").toInt() == OpenInPackageViewer)
                            {
                                PackageViewer *viewer = new PackageViewer(ui->statusBar, package, gpdActions, gameActions, this);
                                viewer->setAttribute(Qt::WA_DeleteOnClose);
                                QtHelpers::AddSubWindow(ui->mdiArea, viewer);
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
                                ProfileEditor *editor = new ProfileEditor(ui->statusBar, package, true, this);
                                editor->setAttribute(Qt::WA_DeleteOnClose);

                                if (editor->isOk())
                                {
                                    QtHelpers::AddSubWindow(ui->mdiArea, editor);
                                    editor->show();
                                }
                            }
                        }
                    }
                    catch(...)
                    {
                        SVOD *svod = new SVOD(fileName);
                        SvodDialog *dialog = new SvodDialog(svod, ui->statusBar, this);
                        QtHelpers::AddSubWindow(ui->mdiArea, dialog);
                        dialog->setAttribute(Qt::WA_DeleteOnClose);
                        dialog->exec();
                    }

                    break;
                }
                case 0x58444246:    // Xdbf
                {
                    GpdBase *gpd = new GpdBase(fileName);
                    ui->statusBar->showMessage("Gpd parsed successfully", 3000);

                    XdbfDialog *dialog = new XdbfDialog(ui->statusBar, gpd, nullptr, this);
                    dialog->setAttribute(Qt::WA_DeleteOnClose);
                    QtHelpers::AddSubWindow(ui->mdiArea, dialog);
                    dialog->show();

                    break;
                }
                case 0x53545242:    // STRB
                {
                    AvatarAsset *asset = new AvatarAsset(fileName);

                    StrbDialog *dialog = new StrbDialog(asset, this);
                    dialog->setAttribute(Qt::WA_DeleteOnClose);
                    QtHelpers::AddSubWindow(ui->mdiArea, dialog);
                    dialog->show();

                    ui->statusBar->showMessage("STRB file parsed successfully", 3000);
                    break;
                }

                case 0x59544752:    // Ytgr
                {
                    Ytgr *ytgr = new Ytgr(fileName);

                    YtgrDialog *dialog = new YtgrDialog(ytgr, ui->statusBar, this);
                    dialog->setAttribute(Qt::WA_DeleteOnClose);
                    QtHelpers::AddSubWindow(ui->mdiArea, dialog);
                    dialog->show();

                    break;
                }
                default:
                    QMessageBox::warning(this, "Unknown File Format",
                            "The following file is an unknown format. Velocity can only read STFS, SVOD, Xdbf, Ytgr, and STRB files.\n\n"
                            + QString::fromStdString(fileName));
                    break;
            }
        }
        catch (string error)
        {
            QMessageBox::critical(this, "Error",
                    "An error occurred while opening the file.\n\n" + QString::fromStdString(error));
        }
    }
}

void MainWindow::on_actionProfile_Editor_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open a Profile"),
            QtHelpers::DefaultLocation(), "All Files (*)");

    if (fileName.isEmpty())
        return;

    try
    {
        StfsPackage *package = new StfsPackage(fileName.toStdString());

        ProfileEditor *editor = new ProfileEditor(ui->statusBar, package, true, this);
        editor->setAttribute(Qt::WA_DeleteOnClose);

        if (editor->isOk())
        {
            QtHelpers::AddSubWindow(ui->mdiArea, editor);
            editor->show();
        }
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Profile Error",
                "An error has occurred while opening the profile.\n\n" + QString::fromStdString(error));
    }
}


void MainWindow::on_actionAbout_triggered()
{
    About about(this);
    about.exec();
}
void MainWindow::on_actionPackage_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Package"),
            QtHelpers::DefaultLocation(), "All Files (*)");

    if (fileName.isEmpty())
        return;

    try
    {
        StfsPackage *package = new StfsPackage(fileName.toStdString());
        PackageViewer *viewer = new PackageViewer(ui->statusBar, package, gpdActions, gameActions, this);
        viewer->setAttribute(Qt::WA_DeleteOnClose);
        QtHelpers::AddSubWindow(ui->mdiArea, viewer);
        viewer->show();

        ui->statusBar->showMessage("Stfs package loaded successfully.", 3000);
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Package Error",
                "An error has occurred while opening the package.\n\n" + QString::fromStdString(error));
    }
}

void MainWindow::on_actionXDBF_File_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Xdbf File"),
            QtHelpers::DefaultLocation(), "Gpd File (*.gpd *.fit);;All Files (*)");

    if (fileName.isEmpty())
        return;

    try
    {
        GpdBase *gpd = new GpdBase(fileName.toStdString());
        ui->statusBar->showMessage("Gpd parsed successfully", 3000);

        XdbfDialog *dialog = new XdbfDialog(ui->statusBar, gpd, nullptr, this);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        QtHelpers::AddSubWindow(ui->mdiArea, dialog);
        dialog->show();
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Gpd Error",
                "An error has occurred while opening the Gpd.\n\n" + QString::fromStdString(error));
    }
}

void MainWindow::on_actionSTRB_File_triggered()
{
    try
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open an Avatar Asset"),
                QtHelpers::DefaultLocation(), "BIN File (*.bin);;All Files (*)");

        if (fileName.isEmpty())
            return;

        AvatarAsset *asset = new AvatarAsset(fileName.toStdString());

        StrbDialog *dialog = new StrbDialog(asset, this);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        QtHelpers::AddSubWindow(ui->mdiArea, dialog);
        dialog->show();
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Error",
                "An error occured while opening the STRB package.\n\n" + QString::fromStdString(error));
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

        PackageViewer *viewer = new PackageViewer(ui->statusBar, package, gpdActions, gameActions, this);
        viewer->setAttribute(Qt::WA_DeleteOnClose);
        QtHelpers::AddSubWindow(ui->mdiArea, viewer);
        viewer->show();

        ui->statusBar->showMessage("Stfs package created successfully.", 3000);
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Package Error",
                "An error has occurred while opening the package.\n\n" + QString::fromStdString(error));
    }
}

void MainWindow::on_actionTitle_ID_Finder_triggered()
{
    TitleIdFinderDialog *dialog = new TitleIdFinderDialog(ui->statusBar, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    QtHelpers::AddSubWindow(ui->mdiArea, dialog);
    dialog->show();
}

void MainWindow::on_actionProfile_Creator_triggered()
{
    ProfileCreatorWizard wiz(ui->statusBar, this);
    wiz.exec();
}

void MainWindow::on_actionGame_Adder_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Package"),
            QtHelpers::DefaultLocation(), "All Files (*)");

    if (fileName.isEmpty())
        return;

    StfsPackage *package = new StfsPackage(fileName.toStdString());

    bool ok;
    GameAdderDialog dialog(package, this, false, &ok);
    if (ok)
        dialog.exec();

    package->Close();
    delete package;
}

void MainWindow::on_actionGamer_Picture_Pack_Creator_triggered()
{
    GamerPicturePackDialog *dialog = new GamerPicturePackDialog(ui->statusBar, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    QtHelpers::AddSubWindow(ui->mdiArea, dialog);
    dialog->show();
}

void MainWindow::on_actionModder_triggered()
{
    if (sender())
    {
        QAction *menuAction = (QAction*)sender();

        // get package
        StfsPackage *package = nullptr;
        if (menuAction->property("package").isValid() &&
                menuAction->property("fromPackageViewer").isValid())
        {
            menuAction->setProperty("fromPackageViewer", QVariant());
            package = menuAction->property("package").value<StfsPackage*>();
        }

        LoadPlugin(menuAction->data().toString(), false, package);
    }
}

void MainWindow::on_actionPreferences_triggered()
{
    PreferencesDialog dialog(this);
    dialog.exec();

    LoadAllPlugins();
}

void MainWindow::on_actionFATX_File_Path_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Package"),
            QtHelpers::DefaultLocation(), "All Files (*)");

    if (fileName.isEmpty())
        return;

    try
    {
        StfsPackage *package = new StfsPackage(fileName.toStdString());
        FATXPathGenDialog *dialog = new FATXPathGenDialog(package, this);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        QtHelpers::AddSubWindow(ui->mdiArea, dialog);
        dialog->show();
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Package Error",
                "An error has occurred while opening the package.\n\n" + QString::fromStdString(error));
    }
}

void MainWindow::versionReplyFinished(QNetworkReply *aReply)
{
    // Read the JSON response
    QByteArray jsonData = aReply->readAll();
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError)
        return;

    QJsonObject jsonObject = jsonDoc.object();
    QVariantMap result = jsonObject.toVariantMap(); // Convert to QVariantMap

    QString version = result.value("version").toString();
    QString downloadPage = result.value("dl_page").toString();

    // Check if a message is present
    bool hasMessage = result.contains("message");
    QString message = "<br><br>";
    if (hasMessage)
        message += result.value("message").toString();

    // Convert version strings to integers for comparison
    bool ok, ok1;
    int versionNum = version.replace('.', "").toInt(&ok);
    int currentVersionNum = QString(VERSION).replace('.', "").toInt(&ok1);

    if (!ok || !ok1)
        return;

    // Check if a newer version is available
    if (versionNum > currentVersionNum)
    {
        auto selection = QMessageBox::question(
            this,
            "Version " + version,
            "Version " + version + " of Velocity is available for download. Your current version is " +
                VERSION + ". Would you like to be brought to the download page?" + (hasMessage ? message : ""),
            QMessageBox::Yes,
            QMessageBox::No
            );

        if (selection == QMessageBox::Yes)
            QDesktopServices::openUrl(QUrl(downloadPage));
    }
    else if (!firstUpdateCheck)
    {
        QMessageBox::information(
            this,
            "Already Up-to-Date",
            "This version of Velocity is already the latest."
            );
    }

    firstUpdateCheck = false;
}

void MainWindow::pluginVersionReplyFinished(QNetworkReply *aReply)
{
    if (!sender()->property("name").isValid() || !sender()->property("version").isValid())
        return;

    QString name = sender()->property("name").toString();
    QString currVersion = sender()->property("version").toString();

    // Read all the data from the reply
    QByteArray jsonData = aReply->readAll();

    // Parse the JSON data
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError)
        return;

    QJsonObject jsonObject = jsonDoc.object();
    QVariantMap result = jsonObject.toVariantMap(); // Convert to QVariantMap

    // Check if "data" is present
    if (result.contains("data"))
        return;

    QString version = result.value("version").toString();
    QString downloadPage = result.value("dl_page").toString();

    // Check if the current version is different from the new version
    if (currVersion != version)
    {
        auto selection = QMessageBox::question(
            this,
            "Version " + version,
            "Version " + version + " of the tool, " + name +
                ", is available for download. Would you like to be brought to the download page?",
            QMessageBox::Yes,
            QMessageBox::No
            );

        if (selection == QMessageBox::Yes)
            QDesktopServices::openUrl(QUrl(downloadPage));
    }
}

void MainWindow::on_actionProfile_Cleaner_triggered()
{
    ProfileCleanerWizard wiz(this);
    wiz.exec();
}

void MainWindow::on_actionCheck_For_Updates_triggered()
{
    manager->get(QNetworkRequest(QUrl("http://velocity.expetelek.com/app.data")));
}

void MainWindow::on_actionSVOD_System_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open an SVOD root descriptor...",
            QtHelpers::DefaultLocation());

    if (filePath.isEmpty())
        return;

    try
    {
        ui->statusBar->showMessage("Loading SVOD system...");
        SVOD *svod = new SVOD(filePath.toStdString());
        SvodDialog *dialog = new SvodDialog(svod, ui->statusBar, this);
        QtHelpers::AddSubWindow(ui->mdiArea, dialog);
        dialog->show();
    }
    catch (string error)
    {
        ui->statusBar->showMessage("");
        QMessageBox::critical(this, "SVOD Error",
                "An error has occurred while opening the SVOD system.\n\n" + QString::fromStdString(error));
    }
}

void MainWindow::on_actionYTGR_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open a file with a Ytgr security header...",
            QtHelpers::DefaultLocation());

    if (filePath.isEmpty())
        return;

    try
    {
        Ytgr *ytgr = new Ytgr(filePath.toStdString());
        YtgrDialog *dialog = new YtgrDialog(ytgr, ui->statusBar, this);
        QtHelpers::AddSubWindow(ui->mdiArea, dialog);
        dialog->show();
    }
    catch (string error)
    {
        ui->statusBar->showMessage("");
        QMessageBox::critical(this, "Ytgr Error",
                "An error has occurred while parsing a Ytgr header.\n\n" + QString::fromStdString(error));
    }
}

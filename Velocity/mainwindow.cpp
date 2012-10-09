#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    LoadAllPlugins();
    setCentralWidget(ui->mdiArea);
    ui->mdiArea->setAcceptDrops(false);
    setAcceptDrops(true);
}

void MainWindow::LoadPlugin(QString filename, bool addToMenu)
{
    QPluginLoader loader(filename);
    QObject *possiblePlugin = loader.instance();

    if (possiblePlugin)
    {
        IGameModder *plugin = qobject_cast<IGameModder*>(possiblePlugin);

        if (plugin)
        {
            if (addToMenu)
            {
                QAction *action = new QAction(plugin->ToolName(), this);
                action->setData(QVariant(filename));
                connect(action, SIGNAL(triggered()), this, SLOT(on_actionGame_Modder_triggered()));
                ui->menuGame_Modders->addAction(action);
            }
            else
            {
                QWidget *widget = plugin->GetDialog();

                QString fileName = QFileDialog::getOpenFileName(this, tr("Open a Save Game"), QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), "All Files (*)");
                if (fileName.isNull())
                {
                    delete possiblePlugin;
                    return;
                }

                //ui->mdiArea->addSubWindow(widget);

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

                widget->show();
                plugin->LoadPackage(package, this);
            }
        }
    }
}

void MainWindow::LoadAllPlugins()
{
    QDir path (QDir::currentPath() + "/plugins");

    foreach (QString filename, path.entryList(QDir::Files))
        LoadPlugin(path.absolutePath() + "/" + filename, true);
}

MainWindow::~MainWindow()
{
    for (DWORD i = 0; i < openPackages.size(); i++)
        delete openPackages.at(i);
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
    for (DWORD i = 0; i < filePaths.size(); i++)
    {
        std::string fileName = QString(filePaths.at(i).encodedPath()).mid(1).replace("%20", " ").toStdString();

        // make sure the file exists
        if (!QFile::exists(QString::fromStdString(fileName)))
            continue;

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
                try
                {
                    StfsPackage *package = new StfsPackage(fileName);

                    PackageViewer *viewer = new PackageViewer(ui->statusBar, package, this);
                    ui->mdiArea->addSubWindow(viewer);
                    viewer->show();

                    ui->statusBar->showMessage("STFS package loaded successfully.", 3000);
                }
                catch (string error)
                {
                    QMessageBox::critical(this, "Package Error", "An error has occurred while opening the package.\n\n" + QString::fromStdString(error));
                }
                break;
            }
            case 0x58444246:
            {
                try
                {
                    GPDBase *gpd = new GPDBase(fileName);
                    ui->statusBar->showMessage("GPD parsed successfully", 3000);

                    XdbfDialog *dialog = new XdbfDialog(ui->statusBar, gpd, NULL, this);
                    ui->mdiArea->addSubWindow(dialog);
                    dialog->show();

                }
                catch (string error)
                {
                    QMessageBox::critical(this, "GPD Error", "An error has occurred while opening the GPD.\n\n" + QString::fromStdString(error));
                }
                break;
            }
            case 0x53545242:
            {
                try
                {
                    AvatarAsset *asset = new AvatarAsset(fileName);

                    StrbDialog *dialog = new StrbDialog(asset, this);
                    ui->mdiArea->addSubWindow(dialog);
                    dialog->show();

                    ui->statusBar->showMessage("STRB file parsed successfully", 3000);
                }
                catch (string error)
                {
                    QMessageBox::critical(this, "STRB Error", "An error occured while opening the STRB package.\n\n" + QString::fromStdString(error));
                }
                break;
            }
            default:
                QMessageBox::warning(this, "Unknown File Format", "The following file is an unknown format. Velocity can only read STFS, XDBF, and STRB files.\n\n" + QString::fromStdString(fileName));
                break;
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

        ProfileEditor *editor = new ProfileEditor(ui->statusBar, package, true, this);
        ui->mdiArea->addSubWindow(editor);
        editor->show();
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
    if (!QFile::exists("FFFE07D1.gpd"))
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
    GamerPicturePackDialog dialog(ui->statusBar);
    dialog.exec();
}

void MainWindow::on_actionGame_Modder_triggered()
{
    if (sender())
    {
        QAction *menuAction = (QAction*)sender();
        LoadPlugin(menuAction->data().toString(), false);
    }
}

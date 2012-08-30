#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    for (DWORD i = 0; i < openPackages.size(); i++)
        delete openPackages.at(i);
    delete ui;
}

void MainWindow::on_actionProfile_Manager_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Package"), QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), "All Files (*)");

    if (fileName.isEmpty())
        return;

    try
    {
        StfsPackage *package = new StfsPackage(fileName.toStdString());

        ProfileEditor *editor = new ProfileEditor(package, true, this);
        ui->mdiArea->addSubWindow(editor);
        editor->show();
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Package Error", "An error has occurred while opening the package.\n\n" + QString::fromStdString(error));
    }
}

void MainWindow::on_actionAbout_triggered()
{
    About *about = new About(this);
    about->show();
}
void MainWindow::on_actionPackage_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Package"), QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), "All Files (*)");

    if (fileName.isEmpty())
        return;

    try
    {
        StfsPackage *package = new StfsPackage(fileName.toStdString());

        PackageViewer *viewer = new PackageViewer(package, this);
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

        XdbfDialog *dialog = new XdbfDialog(gpd, NULL, this);
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
        QMessageBox::critical(this, "Error", "An error occured while openeing the STRB package.\n\n" + QString::fromStdString(error));
    }
}

void MainWindow::on_actionCreate_Package_triggered()
{
    CreationWizard wizard(this);
    wizard.exec();
}

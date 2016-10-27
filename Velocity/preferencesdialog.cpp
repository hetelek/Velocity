#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::PreferencesDialog)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    QtHelpers::GenAdjustWidgetAppearanceToOS(this);

#ifdef __WIN32__
    QSize size(477, 160);
    setFixedSize(size);
#elif __unix__
    QSize size(477, 182);
    setFixedSize(size);
#endif

    settings = new QSettings("Exetelek", "Velocity");

    ui->comboBox->setCurrentIndex(settings->value("PackageDropAction").toInt());
    ui->comboBox_2->setCurrentIndex(settings->value("ProfileDropAction").toInt());
    ui->lineEdit->setText(settings->value("PluginPath").toString());
    ui->checkBox->setChecked(settings->value("AnonData").toBool());
}

PreferencesDialog::~PreferencesDialog()
{
    delete settings;
    delete ui;
}

void PreferencesDialog::on_pushButton_clicked()
{
    settings->setValue("PackageDropAction", ui->comboBox->currentIndex());
    settings->setValue("ProfileDropAction", ui->comboBox_2->currentIndex());
    settings->setValue("PluginPath", ui->lineEdit->text());
    settings->setValue("AnonData", ui->checkBox->checkState());

    close();
}

void PreferencesDialog::on_pushButton_4_clicked()
{
    QString directory = QFileDialog::getExistingDirectory(this, "Choose the plugin path", QtHelpers::ExecutingDirectory());

    if (!directory.isNull())
        return;

    QDir dir(QtHelpers::ExecutingDirectory());
    QString relativePath = dir.relativeFilePath(directory);

    if (relativePath.indexOf("..") != relativePath.lastIndexOf("..") || relativePath.isEmpty())
        relativePath = directory;

    ui->lineEdit->setText(relativePath);
}

void PreferencesDialog::on_pushButton_2_clicked()
{
    close();
}

void PreferencesDialog::on_pushButton_3_clicked()
{
    ui->comboBox->setCurrentIndex(0);
    ui->comboBox_2->setCurrentIndex(0);
    ui->lineEdit->setText(QtHelpers::ExecutingDirectory() + "plugins");
}

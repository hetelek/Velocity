#include "renamedialog.h"
#include "ui_renamedialog.h"

RenameDialog::RenameDialog(QWidget *parent, std::string *newName) :
    QDialog(parent),
    ui(new Ui::RenameDialog),
    outName(newName)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    ui->leRename->setText(QString::fromStdString(*newName));
}

RenameDialog::~RenameDialog()
{
    delete ui;
}

void RenameDialog::on_pushButton_clicked()
{
    *outName = ui->leRename->text().toStdString();
    close();
}
void RenameDialog::on_pushButton_2_clicked()
{
    close();
}

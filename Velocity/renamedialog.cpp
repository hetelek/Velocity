#include "renamedialog.h"
#include "ui_renamedialog.h"

#include <QString>

RenameDialog::RenameDialog(QWidget *parent, QString *newName) :
    QDialog(parent),
    ui(new Ui::RenameDialog),
    outName(newName)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    ui->leRename->setText(*newName);
}

RenameDialog::~RenameDialog()
{
    delete ui;
}

void RenameDialog::on_pushButton_clicked()
{
    *outName = ui->leRename->text();
    close();
}
void RenameDialog::on_pushButton_2_clicked()
{
    close();
}

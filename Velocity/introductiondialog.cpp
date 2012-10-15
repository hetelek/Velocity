#include "introductiondialog.h"
#include "ui_introductiondialog.h"

IntroductionDialog::IntroductionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IntroductionDialog)
{
    ui->setupUi(this);
}

IntroductionDialog::~IntroductionDialog()
{
    delete ui;
}

#include "profilecreatorwizard.h"
#include "ui_profilecreatorwizard.h"

ProfileCreatorWizard::ProfileCreatorWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::ProfileCreatorWizard)
{
    ui->setupUi(this);
}

ProfileCreatorWizard::~ProfileCreatorWizard()
{
    delete ui;
}

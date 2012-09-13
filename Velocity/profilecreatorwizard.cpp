#include "profilecreatorwizard.h"
#include "ui_profilecreatorwizard.h"

ProfileCreatorWizard::ProfileCreatorWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::ProfileCreatorWizard)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
}

ProfileCreatorWizard::~ProfileCreatorWizard()
{
    delete ui;
}

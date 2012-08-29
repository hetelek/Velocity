#include "creationwizard.h"
#include "ui_creationwizard.h"

CreationWizard::CreationWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::CreationWizard)
{
    connect(this, SIGNAL(currentIdChanged(int)), SLOT(on_CreationWizard_currentIdChanged(int)));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    magic = CON;
    ui->setupUi(this);
    ui->label_4->setFont(QFont("Arial", 10));
}

CreationWizard::~CreationWizard()
{
    delete ui;
}

void CreationWizard::on_cbMagic_currentIndexChanged(int index)
{
    if (index == 0)
    {
        ui->label_4->setText("CON packages are console signed packages which means that they can be edited for use on a retail Xbox 360 console. These types of packages are typically used to store profiles, savegames and other offline content.");
        magic = CON;
    }
    else if (index == 1)
    {
        ui->label_4->setText("LIVE packages are strong signed meaning that only Microsoft can resign these packages, therefore they are very secure. These packages are used to store DLC such as avatar items and game add-ons.");
        magic = LIVE;
    }
    else
    {
        ui->label_4->setText("PIRS packages are strong signed meaning that only Microsoft can resign these packages, therefore they are very secure. These packages are used for both title updates and system updates.");
        magic = PIRS;
    }
}

void CreationWizard::on_CreationWizard_currentIdChanged(int id)
{
    switch (id)
    {
    case 2:
        ui->lwContentTypes->clear();
        // populate content type list

        if (magic == CON)
        {
            ui->lwContentTypes->addItem("Profile");
            ui->lwContentTypes->addItem("Saved Game");
            ui->lwContentTypes->addItem("Cache File");
            ui->lwContentTypes->addItem("Gamer Picture");
            ui->lwContentTypes->addItem("Installed Game");
            ui->lwContentTypes->addItem("License Store");
            ui->lwContentTypes->addItem("Publisher");
            ui->lwContentTypes->addItem("Theme");
            ui->lwContentTypes->addItem("Xbox Original Game");
            ui->lwContentTypes->addItem("XNA");
        }
        else if (magic == LIVE)
        {
            ui->lwContentTypes->addItem("App");
            ui->lwContentTypes->addItem("Arcade Game");
            ui->lwContentTypes->addItem("Avatar Item");
            ui->lwContentTypes->addItem("Game Demo");
            ui->lwContentTypes->addItem("Gamer Picture");
            ui->lwContentTypes->addItem("Game Trailer");
            ui->lwContentTypes->addItem("Installer");
            ui->lwContentTypes->addItem("Market Place Content");
        }
        else
        {
            ui->lwContentTypes->addItem("Theme");
        }

        break;
    }
}

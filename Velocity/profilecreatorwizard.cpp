#include "profilecreatorwizard.h"
#include "ui_profilecreatorwizard.h"

ProfileCreatorWizard::ProfileCreatorWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::ProfileCreatorWizard)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(onCurrentIdChanged(int)));
    connect(this, SIGNAL(finished(int)), this, SLOT(onFinished(int)));

    ui->lblSavePath->setText(QtHelpers::DesktopLocation() + "\\butthole.prf");
}

ProfileCreatorWizard::~ProfileCreatorWizard()
{
    delete ui;
}

void ProfileCreatorWizard::onCurrentIdChanged(int id)
{
    switch (id)
    {
        case 2:
            button(QWizard::NextButton)->setEnabled(ui->txtGamertag->text() != "");
            break;
    }
}

void ProfileCreatorWizard::onFinished(int status)
{
    if (status != 0)
        return;

    // create a new file
   /* FileIO io(ui->lblSavePath->text().toStdString(), true);

    // null out the first 0xD000 bytes
    BYTE tempBuffer[0x1000] = {0};
    for (DWORD i = 0; i < 0xD; i++)
        io.write(tempBuffer, 0x1000);

    // set up the metadata for the profile
    StfsMetaData metadata(&io, false, false);
    metadata.magic = CON;
    metadata.certificate.ownerConsoleType = (ui->cmbxType->currentIndex() == 0) ? Retail : DevKit; */
}

void ProfileCreatorWizard::on_txtGamertag_textChanged(const QString &arg1)
{
    button(QWizard::NextButton)->setEnabled(ui->txtGamertag->text() != "");
}

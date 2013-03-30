#include "ytgrdialog.h"
#include "ui_ytgrdialog.h"

YtgrDialog::YtgrDialog(Ytgr *ytgr, QStatusBar *statusBar, QWidget *parent) :
    QDialog(parent), ui(new Ui::YtgrDialog), ytgr(ytgr)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    QtHelpers::GenAdjustWidgetAppearanceToOS(this);

    // load the ytgr information into the gui
    ui->lblMagic->setText("0x" + QString::number(ytgr->magic, 16).toUpper() + " (Ytgr)");
    ui->lblXSignerMinimumVersion->setText(QString::number(ytgr->XSignerMinimumVersion.major) + "." +
                                          QString::number(ytgr->XSignerMinimumVersion.minor) + "." +
                                          QString::number(ytgr->XSignerMinimumVersion.build) + "." +
                                          QString::number(ytgr->XSignerMinimumVersion.revision));
    ui->lblAddedToServer->setText(QDateTime::fromTime_t(ytgr->dateAddedToServer).toString());
    ui->lblContentLength->setText("0x" + QString::number(ytgr->contentLength, 16).toUpper());
    ui->lblValid->setText((ytgr->valid) ? "Yes" : "No");
    ui->txtContentHash->setText(QtHelpers::ByteArrayToString(ytgr->contentHash, 0x14, true));
    ui->txtSignature->setPlainText(QtHelpers::ByteArrayToString(ytgr->rsaSignature, 0x100, true));

    statusBar->showMessage("Ytgr header parsed successfully", 3000);
}

YtgrDialog::~YtgrDialog()
{
    delete ytgr;
    delete ui;
}

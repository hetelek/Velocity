#include "githubcommitsdialog.h"
#include "ui_githubcommitsdialog.h"

GitHubCommitsDialog::GitHubCommitsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GitHubCommitsDialog)
{
    // set the fixed size based on the OS
#ifdef __WIN32__
    QSize appSize(274, 315);
#endif

    //setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    setMinimumSize(appSize);

    ui->setupUi(this);

    // create the objects needed
    manager = new QNetworkAccessManager(this);
    label = new QLabel(this);

    // set the scroll area's widget to the label, so the user can scroll through the label's text
    ui->scrollArea->setWidget(label);

    label->setWordWrap(true);

    // make a request to the API
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReply(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl("https://api.github.com/repos/hetelek/velocity/commits")));
}

GitHubCommitsDialog::~GitHubCommitsDialog()
{
    delete ui;
}

void GitHubCommitsDialog::onReply(QNetworkReply *reply)
{
    // if we can't connect to the server, then quit
    if (reply->bytesAvailable() == 0)
    {
        label->setText("Error connecting to GitHub...");
        return;
    }

    // parse the response
    QList<QVariant> commits = QtJson::Json::parse(QString(reply->readAll())).toList();

    // iterate through all of the commits
    foreach (QVariant commit, commits)
    {
        QVariantMap map = commit.toMap();

        // get all the data we need
        QString author = map["committer"].toMap()["login"].toString();
        QString message = map["commit"].toMap()["message"].toString();
        QString date = map["commit"].toMap()["author"].toMap()["date"].toString();

        QDateTime parsedTime = QDateTime::fromString(date, Qt::ISODate);

        // update the label with all the information
        label->setText(label->text() + "<b>" + author + "</b> - " + parsedTime.toString("MM/dd/yyyy") + "<br />" + message + "<br /><br />");
    }
}

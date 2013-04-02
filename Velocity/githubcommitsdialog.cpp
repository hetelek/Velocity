#include "githubcommitsdialog.h"
#include "ui_githubcommitsdialog.h"

GitHubCommitsDialog::GitHubCommitsDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::GitHubCommitsDialog), branchCount(0), retrievedCount(0)
{
    // set the fixed size based on the OS
#ifdef __WIN32__
    QSize appSize(274, 315);
#elif __unix | __APPLE__
    QSize appSize(345, 380);
#endif

    //setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    setMinimumSize(appSize);

    ui->setupUi(this);

    // create the objects needed
    commitsManager = new QNetworkAccessManager(this);
    branchesManager = new QNetworkAccessManager(this);
    label = new QLabel(this);

    // set the scroll area's widget to the label, so the user can scroll through the label's text
    ui->scrollArea->setWidget(label);

    label->setWordWrap(true);

    // make sure we can connect to the internet
    if (commitsManager->networkAccessible() == QNetworkAccessManager::NotAccessible)
    {
        label->setText("<center>Error connecting to GitHub...</center>");
        return;
    }

    // make a request to the API
    connect(branchesManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onBrachesReply(QNetworkReply*)));
    connect(commitsManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onCommitsReply(QNetworkReply*)));
    branchesManager->get(QNetworkRequest(QUrl("https://api.github.com/repos/hetelek/velocity/branches")));
}

GitHubCommitsDialog::~GitHubCommitsDialog()
{
    delete ui;
}

void GitHubCommitsDialog::onBrachesReply(QNetworkReply *reply)
{
    // parse the response
    bool ok;
    QList<QVariant> branches = QtJson::Json::parse(QString(reply->readAll()), ok).toList();

    if (!ok)
    {
        label->setText("<center>Error connecting to GitHub...</center>");
        return;
    }

    // iterate through all of the branches
    foreach (QVariant branch, branches)
    {
        QVariantMap map = branch.toMap();

        QString url = map["commit"].toMap()["url"].toString();
        url = url.mid(0, url.lastIndexOf("/")) + "?per_page=20&sha=" + url.mid(url.lastIndexOf("/") + 1);

        commitsManager->get(QNetworkRequest(QUrl(url)));

        branchCount++;
    }
}

void GitHubCommitsDialog::onCommitsReply(QNetworkReply *reply)
{
    // parse the response
    bool ok;
    QList<QVariant> commits = QtJson::Json::parse(QString(reply->readAll()), ok).toList();

    if (!ok)
    {
        label->setText("<center>Error connecting to GitHub...</center>");
        return;
    }

    // iterate through all of the commits
    foreach (QVariant commit, commits)
    {
        QVariantMap map = commit.toMap();
        Commit c;

        // get all the data we need
        c.author = map["committer"].toMap()["login"].toString();
        c.message = map["commit"].toMap()["message"].toString();
        QString date = map["commit"].toMap()["author"].toMap()["date"].toString();

        c.timestamp = QDateTime::fromString(date, Qt::ISODate);

        allCommits.push_back(c);
    }

    if (++retrievedCount == branchCount)
    {
        qSort(allCommits.begin(), allCommits.end(), commitCompare);

        int iterations = (allCommits.size() > 20) ? 20 : allCommits.size();
        for (int i = 0; i < iterations; i++)
        {
            // update the label with all the information
            label->setText(label->text() + "<b>" + allCommits.at(i).author + "</b> - " + allCommits.at(i).timestamp.toString("MM/dd/yyyy") + "<br />" + allCommits.at(i).message + "<br /><br />");
        }
    }
}

bool commitCompare(Commit a, Commit b)
{
    return a.timestamp > b.timestamp;
}

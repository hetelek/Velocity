#ifndef GITHUBCOMMITSDIALOG_H
#define GITHUBCOMMITSDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDateTime>
#include <QtAlgorithms>
#include "json.h"

namespace Ui {
class GitHubCommitsDialog;
}

struct Commit
{
    QString author;
    QString message;
    QDateTime timestamp;
};

class GitHubCommitsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit GitHubCommitsDialog(QWidget *parent = 0);
    ~GitHubCommitsDialog();
    
private slots:
    void onCommitsReply(QNetworkReply *reply);
    void onBrachesReply(QNetworkReply *reply);

private:
    Ui::GitHubCommitsDialog *ui;
    QNetworkAccessManager *commitsManager;
    QNetworkAccessManager *branchesManager;
    QLabel *label;
    QList<Commit> allCommits;

    int branchCount;
    int retrievedCount;
};

bool commitCompare(Commit a, Commit b);

#endif // GITHUBCOMMITSDIALOG_H

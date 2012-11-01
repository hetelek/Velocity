#ifndef GITHUBCOMMITSDIALOG_H
#define GITHUBCOMMITSDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDateTime>
#include "json.h"

namespace Ui {
class GitHubCommitsDialog;
}

class GitHubCommitsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit GitHubCommitsDialog(QWidget *parent = 0);
    ~GitHubCommitsDialog();
    
private slots:
    void onReply(QNetworkReply *reply);

private:
    Ui::GitHubCommitsDialog *ui;
    QNetworkAccessManager *manager;
    QLabel *label;
};

#endif // GITHUBCOMMITSDIALOG_H

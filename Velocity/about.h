#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include <QDesktopServices>
#include <QUrl>

namespace Ui {
class About;
}

class About : public QDialog
{
    Q_OBJECT
    
public:
    explicit About(QWidget *parent = 0);
    ~About();
    
private slots:
    void on_label_3_linkActivated(const QString &link);

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::About *ui;
};

#endif // ABOUT_H

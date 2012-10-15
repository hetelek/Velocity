#ifndef INTRODUCTIONDIALOG_H
#define INTRODUCTIONDIALOG_H

#include <QDialog>

namespace Ui {
class IntroductionDialog;
}

class IntroductionDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit IntroductionDialog(QWidget *parent = 0);
    ~IntroductionDialog();
    
private:
    Ui::IntroductionDialog *ui;
};

#endif // INTRODUCTIONDIALOG_H

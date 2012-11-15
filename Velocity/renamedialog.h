#ifndef RENAMEDIALOG_H
#define RENAMEDIALOG_H

#include <QDialog>

class QString;

namespace Ui {
class RenameDialog;
}

class RenameDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit RenameDialog(QWidget *parent = 0, QString *newName = 0);
    ~RenameDialog();
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::RenameDialog *ui;
    QString *outName;
};

#endif // RENAMEDIALOG_H

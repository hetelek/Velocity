#ifndef RENAMEDIALOG_H
#define RENAMEDIALOG_H

#include <QDialog>

namespace Ui {
class RenameDialog;
}

class RenameDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit RenameDialog(QWidget *parent = 0, std::string *newName = 0);
    ~RenameDialog();
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::RenameDialog *ui;
    std::string *outName;
};

#endif // RENAMEDIALOG_H

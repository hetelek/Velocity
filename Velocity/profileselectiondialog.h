#ifndef PROFILESELECTIONDIALOG_H
#define PROFILESELECTIONDIALOG_H

#include <QDialog>
#include <QTreeWidgetItem>

#include <vector>

#include "Fatx/XContentDeviceProfile.h"

namespace Ui {
class ProfileSelectionDialog;
}

class ProfileSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProfileSelectionDialog(std::vector<XContentDeviceProfile> *profiles, QString fileName, QWidget *parent = 0);
    ~ProfileSelectionDialog();

    XContentDeviceProfile GetSelectedProfile();
    bool HasSelectedProfile();

private slots:
    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

private:
    Ui::ProfileSelectionDialog *ui;
    std::vector<XContentDeviceProfile> *profiles;
    XContentDeviceProfile selectedProfile;
    bool hasSelectedProfile;
};

#endif // PROFILESELECTIONDIALOG_H

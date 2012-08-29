#ifndef CREATIONWIZARD_H
#define CREATIONWIZARD_H

// qt
#include <QWizard>
#include <QMessageBox>

// xbox 360
#include "Stfs/StfsPackage.h"

namespace Ui {
class CreationWizard;
}

class CreationWizard : public QWizard
{
    Q_OBJECT
    
public:
    explicit CreationWizard(QWidget *parent = 0);
    ~CreationWizard();
    
private slots:
    void on_cbMagic_currentIndexChanged(int index);

    void on_CreationWizard_currentIdChanged(int id);

private:
    Ui::CreationWizard *ui;
    Magic magic;
    ContentType type;
};

#endif // CREATIONWIZARD_H

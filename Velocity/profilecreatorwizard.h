#ifndef PROFILECREATORWIZARD_H
#define PROFILECREATORWIZARD_H

#include <QWizard>

namespace Ui {
class ProfileCreatorWizard;
}

class ProfileCreatorWizard : public QWizard
{
    Q_OBJECT
    
public:
    explicit ProfileCreatorWizard(QWidget *parent = 0);
    ~ProfileCreatorWizard();
    
private:
    Ui::ProfileCreatorWizard *ui;
};

#endif // PROFILECREATORWIZARD_H

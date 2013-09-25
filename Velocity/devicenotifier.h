#ifndef DEVICENOTIFIER_H
#define DEVICENOTIFIER_H

#include <QThread>
#include "Fatx/FatxDriveDetection.h"

class DeviceNotifier : public QThread
{
    Q_OBJECT
public:
    explicit DeviceNotifier(QObject *parent = 0);
    void run();
    
signals:
    
public slots:
    
};

#endif // DEVICENOTIFIER_H

#ifndef FATXDEVICENOTIFIER_H
#define FATXDEVICENOTIFIER_H

// qt
#include <QThread>
#include <QMutex>
#include <QMetaType>

// xbox
#include "Fatx/FatxDriveDetection.h"

#include <algorithm>

class FatxDeviceNotifier : public QThread
{
    Q_OBJECT
public:
    explicit FatxDeviceNotifier(QObject *parent = 0);

signals:
    void newDevicesDetected(QList<FatxDrive*> newDevices);

private:
    QMutex mutex;
    std::vector<FatxDrive *> cachedDrives;

    void run();
};

#endif // FATXDEVICENOTIFIER_H

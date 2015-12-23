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
    ~FatxDeviceNotifier();

    void SaveDevice(FatxDrive *drive);

    void StopLooking();

signals:
    void newDevicesDetected(QList<FatxDrive*> newDevices);

private:
    QMutex mutex;
    std::vector<FatxDrive *> cachedDrives;
    std::vector<FatxDrive *> saved;
    bool stop;

    void run();

    bool ContainsDrive(const std::vector<FatxDrive *> &drives, FatxDrive *drive);
};

#endif // FATXDEVICENOTIFIER_H

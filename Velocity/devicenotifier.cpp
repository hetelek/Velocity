#include "devicenotifier.h"

DeviceNotifier::DeviceNotifier(QObject *parent) :
    QThread(parent)
{
}

void DeviceNotifier::run()
{
    FatxDriveDetection::GetAllFatxDrives();
}

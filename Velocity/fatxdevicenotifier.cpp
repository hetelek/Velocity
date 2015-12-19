#include "fatxdevicenotifier.h"

FatxDeviceNotifier::FatxDeviceNotifier(QObject *parent) :
    QThread(parent)
{
    qRegisterMetaType<QList<FatxDrive *> >("QList<FatxDrive *>");
}

void FatxDeviceNotifier::run()
{
    while (true)
    {
        QMutexLocker lock(&mutex);

        // check for FATX devices
        QList<FatxDrive*> newDrives;
        std::vector<FatxDrive*> foundDrives = FatxDriveDetection::GetAllFatxDrives();
        for (size_t i = 0; i < foundDrives.size(); i++)
        {
            FatxDrive *currentDrive = foundDrives.at(i);

            // check to see if the devices was already detected
            bool found = false;
            for (size_t x = 0; x < cachedDrives.size(); x++)
            {
                if (*cachedDrives.at(i) == *currentDrive)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
                newDrives.push_back(currentDrive);
        }

        // free all the old devices
        for (size_t i = 0; i < cachedDrives.size(); i++)
            delete cachedDrives.at(i);

        cachedDrives = foundDrives;

        // if new ones were found then emit the signal
        if (newDrives.size() != 0)
            emit newDevicesDetected(newDrives);

        msleep(5000);
    }
}

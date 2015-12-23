#include "fatxdevicenotifier.h"

FatxDeviceNotifier::FatxDeviceNotifier(QObject *parent) :
    QThread(parent), stop(false)
{
    qRegisterMetaType<QList<FatxDrive *> >("QList<FatxDrive *>");
}

FatxDeviceNotifier::~FatxDeviceNotifier()
{
    stop = true;
    while (!isFinished())
    {
        quit();
        wait();
    }
}

void FatxDeviceNotifier::SaveDevice(FatxDrive *drive)
{
    saved.push_back(drive);
}

void FatxDeviceNotifier::StopLooking()
{
    stop = true;
}

void FatxDeviceNotifier::run()
{
    while (!stop)
    {
        QList<FatxDrive*> newDrives;
        {
            QMutexLocker lock(&mutex);

            // check for FATX devices
            std::vector<FatxDrive*> foundDrives = FatxDriveDetection::GetAllFatxDrives();
            for (size_t i = 0; i < foundDrives.size(); i++)
            {
                FatxDrive *currentDrive = foundDrives.at(i);

                // check to see if the devices was already detected
                bool cached = false, save = false;
                for (size_t x = 0; x < cachedDrives.size(); x++)
                {
                    if (*cachedDrives.at(x) == *currentDrive)
                    {
                        cached = true;
                        break;
                    }
                }
                for (size_t x = 0; x < saved.size(); x++)
                {
                    if (*saved.at(x) == *currentDrive)
                    {
                        save = true;
                        break;
                    }
                }

                if (!cached && !save)
                    newDrives.push_back(currentDrive);
            }

            // free all the old devices
            for (size_t i = 0; i < cachedDrives.size(); i++)
            {
                // make sure the drive isn't saved
                if (std::find(saved.begin(), saved.end(), cachedDrives.at(i)) == saved.end())
                    delete cachedDrives.at(i);
            }

            cachedDrives = foundDrives;

            // if new ones were found then emit the signal
            if (newDrives.size() != 0)
                emit newDevicesDetected(newDrives);

        }

        msleep(2000);
    }
}

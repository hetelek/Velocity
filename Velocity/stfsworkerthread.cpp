#include "stfsworkerthread.h"
#include "Stfs/StfsPackage.h"

StfsWorkerThread::StfsWorkerThread(StfsPackage *package, StfsJob job, FileEntry *packageFile, QString externalPath, QObject *parent, QString packageFilePath) :
    QThread(parent), package(package), job(job), packageFile(packageFile), externalPath(externalPath), packageFilePath(packageFilePath)
{

}

void StfsWorkerThread::run()
{
    switch (job)
    {
        case Extract:
            package->ExtractFile(packageFile, externalPath, progessUpdatedExternal, this);
            break;
        case Replace:
            package->ReplaceFile(externalPath, packageFilePath, progessUpdatedExternal, this);
            break;
        case Inject:
            FileEntry temp = package->InjectFile(externalPath, packageFilePath, progessUpdatedExternal, this);
            *packageFile = temp;
            break;
    }
}

void progessUpdatedExternal(void *thread, DWORD blocksExtracted, DWORD totalBlockCount)
{
    emit ((StfsWorkerThread*)thread)->progressUpdated(blocksExtracted, totalBlockCount);
}

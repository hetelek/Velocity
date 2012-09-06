#include "stfsworkerthread.h"
#include "Stfs/StfsPackage.h"

StfsWorkerThread::StfsWorkerThread(StfsPackage *package, StfsJob job, FileEntry *packageFile, QString externalPath, QObject *parent) :
    QThread(parent), package(package), job(job), packageFile(packageFile), externalPath(externalPath)
{

}

void StfsWorkerThread::run()
{
    switch (job)
    {
        case Extract:
            package->ExtractFile(packageFile, externalPath.toStdString(), progessUpdatedExternal, this);
            break;
    }
}

void progessUpdatedExternal(StfsWorkerThread *thread, DWORD blocksExtracted, DWORD totalBlockCount)
{
    emit thread->progressUpdated(blocksExtracted, totalBlockCount);
}

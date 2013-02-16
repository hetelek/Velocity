#ifndef STFSWORKERTHREAD_H
#define STFSWORKERTHREAD_H

// qt
#include <QThread>
#include <QtCore>

// xbox360
#include "Stfs/StfsPackage.h"

enum StfsJob
{
    Extract,
    Inject,
    Replace
};

class StfsWorkerThread;

void progessUpdatedExternal(void *thread, DWORD blocksExtracted, DWORD totalBlockCount);

class StfsWorkerThread : public QThread
{
    Q_OBJECT
public:
    explicit StfsWorkerThread(StfsPackage *package, StfsJob job, StfsFileEntry *packageFile, QString externalPath, QObject *parent = 0, QString packageFilePath = "");

    void run();
    
signals:
    void progressUpdated(DWORD, DWORD);
    
private:
    StfsPackage *package;
    StfsJob job;
    StfsFileEntry *packageFile;
    QString externalPath;
    QString packageFilePath;

    friend void progessUpdatedExternal(void *thread, DWORD blocksExtracted, DWORD totalBlockCount);
};

#endif // STFSWORKERTHREAD_H

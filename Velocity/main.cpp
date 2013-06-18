#include <QtGui/QApplication>
#include <QStringList>
#include <botan/botan.h>
#include "mainwindow.h"

#include <QDebug>
#include "Fatx/XContentDevice.h"
#include "Fatx/FatxDriveDetection.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.addLibraryPath(":/plugins/imageformats");

    QList<QUrl> args;
    for (int i = 1; i < argc; i++)
        args.append(QUrl("file:///" + QString::fromAscii(argv[i]).replace("\\", "/")));

    Botan::LibraryInitializer init;

    MainWindow w(args);
    w.show();
    
    return a.exec();
}

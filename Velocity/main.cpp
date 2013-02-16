#include <QtGui/QApplication>
#include <QStringList>
#include "mainwindow.h"
#include <botan/botan.h>

#include "FATX/FatxDrive.h"

int main(int argc, char *argv[])
{
    FatxDrive drive(L"\\\\.\\PHYSICALDRIVE1");
    Partition *part = drive.GetPartitions().at(0);

    qDebug() << "1";

    drive.GetChildFileEntries(&part->root);
    qDebug() << QString::number(part->root.cachedFiles.size());
    FatxFileEntry entry = part->root.cachedFiles.at(0);

    qDebug() << "2";

    drive.GetChildFileEntries(&entry);
    FatxFileEntry folder = entry.cachedFiles.at(0);

    qDebug() << "3";

    drive.GetChildFileEntries(&folder);

    qDebug() << "4";
    FatxIO io = drive.GetFatxIO(&folder.cachedFiles.at(0));
    qDebug() << "5";
    io.SaveFile("C:/Users/Stevie/Desktop/save.dat");
    qDebug() << "6";

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

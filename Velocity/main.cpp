#include <QtGui/QApplication>
#include <QStringList>
#include "mainwindow.h"
#include <botan/botan.h>

#include "FATX/FatxDrive.h"

int main(int argc, char *argv[])
{
    FatxDrive drive(L"\\\\.\\PHYSICALDRIVE1");
    Partition *part = drive.GetPartitions().at(0);

    drive.GetChildFileEntries(&part->root);
    FatxFileEntry entry = part->root.cachedFiles.at(0);

    drive.GetChildFileEntries(&entry);
    FatxFileEntry folder = entry.cachedFiles.at(0);

    drive.GetChildFileEntries(&folder);

    FatxIO io = drive.GetFatxIO(&folder.cachedFiles.at(0));

    io.SetPosition(0);
    FileIO newFile("C:/Users/Stevie/Desktop/extracted.dat", true);

    BYTE buffer[0x10000];
    DWORD size = io.GetFatxFileEntry().fileSize;
    while (size > 0)
    {
        DWORD amount = (size < 0x10000) ? size : 0x10000;
        io.ReadBytes(buffer, amount);
        newFile.write(buffer, amount);
        size -= amount;
    }

    newFile.close();

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

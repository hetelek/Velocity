#include <QtGui/QApplication>
#include <QStringList>
#include "mainwindow.h"
#include <botan/botan.h>

#include "FATX/FatxDrive.h"

int main(int argc, char *argv[])
{
    //FatxDrive drive(L"\\\\.\\PHYSICALDRIVE1");

    FileIO io("C:\\Users\\Adam\\Desktop\\dfg.new", true);

    BYTE nigga[0x50];
    for (int i = 0; i < 0x50; i++)
        nigga[i] = i;

    io.WriteBytes(nigga, 0x50);

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

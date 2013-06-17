#include <QtGui/QApplication>
#include <QStringList>
#include <botan/botan.h>
#include "mainwindow.h"

#include <QDebug>
#include "Fatx/XContentDevice.h"
#include "Fatx/FatxDriveDetection.h"

int main(int argc, char *argv[])
{
    try
    {
        XContentDevice device(FatxDriveDetection::GetAllFatxDrives().at(0));
        device.LoadDevice();

        for (int i = 0; i < device.profiles->size(); i++)
        {
            qDebug() << QString::fromStdWString(device.profiles->at(i).GetName());
            for (int x = 0; x < device.profiles->at(i).titles.size(); x++)
            {
                XContentDeviceTitle title = device.profiles->at(i).titles.at(x);
                qDebug() << "\t" << QString::fromStdWString(title.GetName()) << " - " << QString::number(title.GetTitleID(), 16);
                for (int y = 0; y < title.titleSaves.size(); y++)
                    qDebug() << "\t\t" << QString::fromStdWString(title.titleSaves.at(y).GetName());
            }
        }
    }
    catch (std::string error)
    {
        qDebug() << QString::fromStdString(error);
    }


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

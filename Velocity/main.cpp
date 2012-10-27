#include <QtGui/QApplication>
#include <QStringList>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QList<QUrl> args;
    for (int i = 1; i < argc; i++)
        args.append(QUrl("file:///" + QString::fromAscii(argv[i]).replace("\\", "/")));

    MainWindow w(args);
    w.show();
    
    return a.exec();
}

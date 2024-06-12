#if QT_VERSION >= 0x050000
#include <QtWidgets/QApplication>
#else
#include <QApplication>
#endif

#include <QStringList>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.addLibraryPath(":/plugins/imageformats");

    QList<QUrl> args;
    for (int i = 1; i < argc; i++)
        args.append(QUrl("file:///" + QString::fromLatin1(argv[i]).replace("\\", "/")));

    MainWindow w(args);
    w.show();

    return a.exec();
}

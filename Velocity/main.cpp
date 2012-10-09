#include <QtGui/QApplication>
#include <QDir>
#include <QCoreApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QDir::setCurrent(QCoreApplication::applicationFilePath());
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}

#if QT_VERSION >= 0x050000
#include <QtWidgets/QApplication>
#else
#include <QApplication>
#endif

#include <QStringList>
#include <QNetworkInformation>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.addLibraryPath(":/plugins/imageformats");

    // Quick Windows hack to enforce light mode until dark mode is properly implemented to aid in text visibility issues
    a.setStyle("Fusion");

    // Create a light palette
    QPalette lightPalette;
    lightPalette.setColor(QPalette::Window, QColor(255, 255, 255)); // Light background
    lightPalette.setColor(QPalette::WindowText, QColor(0, 0, 0)); // Dark text
    lightPalette.setColor(QPalette::Base, QColor(240, 240, 240)); // Light base
    lightPalette.setColor(QPalette::AlternateBase, QColor(230, 230, 230)); // Alternate base
    lightPalette.setColor(QPalette::ToolTipBase, QColor(255, 255, 255)); // Tooltip base
    lightPalette.setColor(QPalette::ToolTipText, QColor(0, 0, 0)); // Tooltip text
    lightPalette.setColor(QPalette::Text, QColor(0, 0, 0)); // Text color
    lightPalette.setColor(QPalette::Button, QColor(200, 200, 200)); // Button background
    lightPalette.setColor(QPalette::ButtonText, QColor(0, 0, 0)); // Button text

    // Set the palette for the application
    a.setPalette(lightPalette);

    QList<QUrl> args;
    for (int i = 1; i < argc; i++)
        args.append(QUrl("file:///" + QString::fromLatin1(argv[i]).replace("\\", "/")));

    QNetworkInformation::loadDefaultBackend();

    MainWindow w(args);
    w.show();

    return a.exec();
}

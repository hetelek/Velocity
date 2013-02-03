#ifndef THEMECREATIONWIZARD_H
#define THEMECREATIONWIZARD_H

// qt
#include <QWizard>
#include <QFileDialog>
#include <QMessageBox>
#include <QBuffer>
#include <QTextStream>
#include <QFile>
#include <QUuid>
#include <QStatusBar>
#include "qthelpers.h"

// xbox 360
#include "Stfs/StfsPackage.h"

// std
#include <iostream>

const QColor colors[] =
{
    QColor(215, 215, 215),
    QColor(149, 149, 149),
    QColor(54, 54, 54),
    QColor(249, 173, 129),
    QColor(255, 247, 154),
    QColor(196, 223, 200),
    QColor(123, 205, 200),
    QColor(126, 167, 216),
    QColor(188, 141, 191),
    QColor(199, 178, 153),
    QColor(83, 71, 65),
    QColor(210, 101, 49),
    QColor(130, 175, 70),
    QColor(186, 68, 68),
    QColor(16, 103, 161),
    QColor(128, 46, 126)
};

const QString colorNames[] =
{
    QString("Light Grey"),
    QString("Grey"),
    QString("Dark Grey"),
    QString("Salmon"),
    QString("Pale Yellow"),
    QString("Pale Green"),
    QString("Turquoise"),
    QString("Pale Blue"),
    QString("Pale Purple"),
    QString("Tan"),
    QString("Brown"),
    QString("Orange"),
    QString("Green"),
    QString("Red"),
    QString("Blue"),
    QString("Purple")
};

namespace Ui {
class ThemeCreationWizard;
}

class ThemeCreationWizard : public QWizard
{
    Q_OBJECT
    
public:
    explicit ThemeCreationWizard(QStatusBar *statusBar, QWidget *parent = 0);
    ~ThemeCreationWizard();

private slots:
    void on_pushButton_clicked();

    void onCurrentIdChanged(int index);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_txtName_textChanged(const QString &arg1);

    void onFinished(int status);

    void on_pushButton_6_clicked();

    void on_radioButton_clicked(bool checked);

    void on_radioButton_2_clicked(bool checked);

private:
    Ui::ThemeCreationWizard *ui;
    QImage wallpaper1, wallpaper2, wallpaper3, wallpaper4;
    QStatusBar *statusBar;
    QList<QPixmap*> allocatedImages;

    BYTE imagesLoaded;
    ConsoleType consoleType;

    void openWallpaper(QLabel *imageViewer, QImage *saveImage);

    void injectImage(StfsPackage *theme, QImage *image, QString fileName);
};

#endif // THEMECREATIONWIZARD_H

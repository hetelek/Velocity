#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// qt
#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

// forms
#include "profileeditor.h"
#include "about.h"
#include "packageviewer.h"
#include "xdbfdialog.h"
#include "strbdialog.h"
#include "profileeditor.h"

// other
#include "ui_mainwindow.h"
#include "Stfs/StfsPackage.h"
#include "GPD/GPDBase.h"
#include <iostream>
#include <vector>

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionProfile_Manager_triggered();

    void on_actionAbout_triggered();

    void on_actionPackage_triggered();

    void on_actionXDBF_File_triggered();

    void on_actionSTRB_File_triggered();

private:
    Ui::MainWindow *ui;
    vector<StfsPackage*> openPackages;
};

#endif // MAINWINDOW_H

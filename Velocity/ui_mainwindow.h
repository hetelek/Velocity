/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Thu Aug 30 18:18:49 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMdiArea>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionCreate_Package;
    QAction *actionProfile_Manager;
    QAction *actionTheme_Creator;
    QAction *actionAbout;
    QAction *actionPackage;
    QAction *actionXDBF_File;
    QAction *actionSTRB_File;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QMdiArea *mdiArea;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuOpen;
    QMenu *menuProfile;
    QMenu *menuHelp;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1013, 623);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/Leaner24.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        actionCreate_Package = new QAction(MainWindow);
        actionCreate_Package->setObjectName(QString::fromUtf8("actionCreate_Package"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/CreateFile35.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionCreate_Package->setIcon(icon1);
        actionProfile_Manager = new QAction(MainWindow);
        actionProfile_Manager->setObjectName(QString::fromUtf8("actionProfile_Manager"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Images/ProfileEditorIcon35.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionProfile_Manager->setIcon(icon2);
        actionTheme_Creator = new QAction(MainWindow);
        actionTheme_Creator->setObjectName(QString::fromUtf8("actionTheme_Creator"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/Images/ThemeCreator35.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionTheme_Creator->setIcon(icon3);
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionPackage = new QAction(MainWindow);
        actionPackage->setObjectName(QString::fromUtf8("actionPackage"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/Images/package.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPackage->setIcon(icon4);
        actionXDBF_File = new QAction(MainWindow);
        actionXDBF_File->setObjectName(QString::fromUtf8("actionXDBF_File"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/Images/gpd.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionXDBF_File->setIcon(icon5);
        actionSTRB_File = new QAction(MainWindow);
        actionSTRB_File->setObjectName(QString::fromUtf8("actionSTRB_File"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/Images/asset.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSTRB_File->setIcon(icon6);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        mdiArea = new QMdiArea(centralWidget);
        mdiArea->setObjectName(QString::fromUtf8("mdiArea"));
        mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        gridLayout->addWidget(mdiArea, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1013, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuOpen = new QMenu(menuFile);
        menuOpen->setObjectName(QString::fromUtf8("menuOpen"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/Images/OpenIcon35.png"), QSize(), QIcon::Normal, QIcon::Off);
        menuOpen->setIcon(icon7);
        menuProfile = new QMenu(menuBar);
        menuProfile->setObjectName(QString::fromUtf8("menuProfile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuProfile->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(menuOpen->menuAction());
        menuFile->addAction(actionCreate_Package);
        menuOpen->addAction(actionPackage);
        menuOpen->addAction(actionXDBF_File);
        menuOpen->addAction(actionSTRB_File);
        menuProfile->addAction(actionProfile_Manager);
        menuProfile->addAction(actionTheme_Creator);
        menuHelp->addAction(actionAbout);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Velocity - Xbox 360 File Manager", 0, QApplication::UnicodeUTF8));
        actionCreate_Package->setText(QApplication::translate("MainWindow", "Create Package", 0, QApplication::UnicodeUTF8));
        actionProfile_Manager->setText(QApplication::translate("MainWindow", "Profile Editor", 0, QApplication::UnicodeUTF8));
        actionTheme_Creator->setText(QApplication::translate("MainWindow", "Theme Creator", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("MainWindow", "About", 0, QApplication::UnicodeUTF8));
        actionPackage->setText(QApplication::translate("MainWindow", "Package", 0, QApplication::UnicodeUTF8));
        actionXDBF_File->setText(QApplication::translate("MainWindow", "GPD", 0, QApplication::UnicodeUTF8));
        actionSTRB_File->setText(QApplication::translate("MainWindow", "Avatar Asset", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
        menuOpen->setTitle(QApplication::translate("MainWindow", "Open", 0, QApplication::UnicodeUTF8));
        menuProfile->setTitle(QApplication::translate("MainWindow", "Tools", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        statusBar->setStatusTip(QString());
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_WHATSTHIS
        statusBar->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

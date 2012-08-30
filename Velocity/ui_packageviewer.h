/********************************************************************************
** Form generated from reading UI file 'packageviewer.ui'
**
** Created: Thu Aug 30 18:18:49 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PACKAGEVIEWER_H
#define UI_PACKAGEVIEWER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_PackageViewer
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout_3;
    QGridLayout *gridLayout;
    QLabel *lblMagic;
    QLabel *lblTitleName;
    QLabel *lblType;
    QLabel *lblDisplayName;
    QLabel *imgTile;
    QGridLayout *gridLayout_2;
    QLabel *label_5;
    QLineEdit *txtProfileID;
    QLabel *label_6;
    QLineEdit *txtDeviceID;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnProfileEditor;
    QPushButton *btnViewAll;
    QPushButton *btnFix;
    QTreeWidget *treeWidget;

    void setupUi(QDialog *PackageViewer)
    {
        if (PackageViewer->objectName().isEmpty())
            PackageViewer->setObjectName(QString::fromUtf8("PackageViewer"));
        PackageViewer->resize(514, 498);
        PackageViewer->setMinimumSize(QSize(514, 498));
        verticalLayout = new QVBoxLayout(PackageViewer);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setVerticalSpacing(10);
        lblMagic = new QLabel(PackageViewer);
        lblMagic->setObjectName(QString::fromUtf8("lblMagic"));
        lblMagic->setWordWrap(false);

        gridLayout->addWidget(lblMagic, 0, 0, 1, 1);

        lblTitleName = new QLabel(PackageViewer);
        lblTitleName->setObjectName(QString::fromUtf8("lblTitleName"));

        gridLayout->addWidget(lblTitleName, 0, 1, 1, 1);

        lblType = new QLabel(PackageViewer);
        lblType->setObjectName(QString::fromUtf8("lblType"));

        gridLayout->addWidget(lblType, 1, 0, 1, 1);

        lblDisplayName = new QLabel(PackageViewer);
        lblDisplayName->setObjectName(QString::fromUtf8("lblDisplayName"));

        gridLayout->addWidget(lblDisplayName, 1, 1, 1, 1);


        gridLayout_3->addLayout(gridLayout, 0, 0, 1, 1);

        imgTile = new QLabel(PackageViewer);
        imgTile->setObjectName(QString::fromUtf8("imgTile"));
        imgTile->setMinimumSize(QSize(64, 64));
        imgTile->setMaximumSize(QSize(64, 64));
        imgTile->setFrameShape(QFrame::StyledPanel);
        imgTile->setFrameShadow(QFrame::Plain);
        imgTile->setMargin(0);

        gridLayout_3->addWidget(imgTile, 0, 1, 2, 1);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setVerticalSpacing(10);
        gridLayout_2->setContentsMargins(-1, 4, -1, -1);
        label_5 = new QLabel(PackageViewer);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout_2->addWidget(label_5, 0, 0, 1, 1);

        txtProfileID = new QLineEdit(PackageViewer);
        txtProfileID->setObjectName(QString::fromUtf8("txtProfileID"));
        txtProfileID->setMaxLength(16);

        gridLayout_2->addWidget(txtProfileID, 0, 1, 1, 1);

        label_6 = new QLabel(PackageViewer);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout_2->addWidget(label_6, 1, 0, 1, 1);

        txtDeviceID = new QLineEdit(PackageViewer);
        txtDeviceID->setObjectName(QString::fromUtf8("txtDeviceID"));
        txtDeviceID->setMaxLength(40);

        gridLayout_2->addWidget(txtDeviceID, 1, 1, 1, 1);


        gridLayout_3->addLayout(gridLayout_2, 1, 0, 1, 1);


        verticalLayout->addLayout(gridLayout_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        btnProfileEditor = new QPushButton(PackageViewer);
        btnProfileEditor->setObjectName(QString::fromUtf8("btnProfileEditor"));
        btnProfileEditor->setEnabled(false);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/ProfileEditorIcon35.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnProfileEditor->setIcon(icon);

        horizontalLayout->addWidget(btnProfileEditor);

        btnViewAll = new QPushButton(PackageViewer);
        btnViewAll->setObjectName(QString::fromUtf8("btnViewAll"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/Eye16.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnViewAll->setIcon(icon1);

        horizontalLayout->addWidget(btnViewAll);

        btnFix = new QPushButton(PackageViewer);
        btnFix->setObjectName(QString::fromUtf8("btnFix"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Images/Fix24.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnFix->setIcon(icon2);

        horizontalLayout->addWidget(btnFix);


        verticalLayout->addLayout(horizontalLayout);

        treeWidget = new QTreeWidget(PackageViewer);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        treeWidget->setAlternatingRowColors(true);
        treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
        treeWidget->setIconSize(QSize(24, 24));
        treeWidget->setSortingEnabled(false);
        treeWidget->setAnimated(true);
        treeWidget->header()->setDefaultSectionSize(120);

        verticalLayout->addWidget(treeWidget);

        label_5->raise();
        label_6->raise();
        txtProfileID->raise();
        txtDeviceID->raise();
        lblMagic->raise();

        retranslateUi(PackageViewer);

        QMetaObject::connectSlotsByName(PackageViewer);
    } // setupUi

    void retranslateUi(QDialog *PackageViewer)
    {
        PackageViewer->setWindowTitle(QApplication::translate("PackageViewer", "Package Viewer", 0, QApplication::UnicodeUTF8));
        lblMagic->setText(QApplication::translate("PackageViewer", "Magic:", 0, QApplication::UnicodeUTF8));
        lblTitleName->setText(QApplication::translate("PackageViewer", "Title ID:", 0, QApplication::UnicodeUTF8));
        lblType->setText(QApplication::translate("PackageViewer", "Package Type:", 0, QApplication::UnicodeUTF8));
        lblDisplayName->setText(QApplication::translate("PackageViewer", "Display Name:", 0, QApplication::UnicodeUTF8));
        imgTile->setText(QString());
        label_5->setText(QApplication::translate("PackageViewer", "ProfileID:", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("PackageViewer", "DeviceID:", 0, QApplication::UnicodeUTF8));
        btnProfileEditor->setText(QApplication::translate("PackageViewer", "Open in Profile Editor", 0, QApplication::UnicodeUTF8));
        btnViewAll->setText(QApplication::translate("PackageViewer", "View All Metadata", 0, QApplication::UnicodeUTF8));
        btnFix->setText(QApplication::translate("PackageViewer", "Rehash and Resign", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(3, QApplication::translate("PackageViewer", "Starting Block", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(2, QApplication::translate("PackageViewer", "Starting Address", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(1, QApplication::translate("PackageViewer", "File Size", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(0, QApplication::translate("PackageViewer", "File Name", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PackageViewer: public Ui_PackageViewer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PACKAGEVIEWER_H

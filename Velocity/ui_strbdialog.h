/********************************************************************************
** Form generated from reading UI file 'strbdialog.ui'
**
** Created: Thu Aug 30 18:18:50 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STRBDIALOG_H
#define UI_STRBDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_StrbDialog
{
public:
    QVBoxLayout *verticalLayout_3;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QGridLayout *gridLayout;
    QLabel *lblGender;
    QLabel *lblSubCat;
    QLabel *lblBinType;
    QLabel *lblSkeletonV;
    QLabel *lblDetails;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout_2;
    QLabel *lblFrameCnt;
    QLabel *lblDuration;
    QLabel *lblFPS;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_2;
    QTreeWidget *treeWidget;

    void setupUi(QDialog *StrbDialog)
    {
        if (StrbDialog->objectName().isEmpty())
            StrbDialog->setObjectName(QString::fromUtf8("StrbDialog"));
        StrbDialog->resize(393, 380);
        StrbDialog->setMinimumSize(QSize(385, 380));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/Leaner24.png"), QSize(), QIcon::Normal, QIcon::Off);
        StrbDialog->setWindowIcon(icon);
        verticalLayout_3 = new QVBoxLayout(StrbDialog);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        groupBox = new QGroupBox(StrbDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        lblGender = new QLabel(groupBox);
        lblGender->setObjectName(QString::fromUtf8("lblGender"));

        gridLayout->addWidget(lblGender, 0, 0, 1, 1);

        lblSubCat = new QLabel(groupBox);
        lblSubCat->setObjectName(QString::fromUtf8("lblSubCat"));

        gridLayout->addWidget(lblSubCat, 0, 1, 1, 1);

        lblBinType = new QLabel(groupBox);
        lblBinType->setObjectName(QString::fromUtf8("lblBinType"));

        gridLayout->addWidget(lblBinType, 1, 0, 1, 1);

        lblSkeletonV = new QLabel(groupBox);
        lblSkeletonV->setObjectName(QString::fromUtf8("lblSkeletonV"));

        gridLayout->addWidget(lblSkeletonV, 1, 1, 1, 1);

        lblDetails = new QLabel(groupBox);
        lblDetails->setObjectName(QString::fromUtf8("lblDetails"));

        gridLayout->addWidget(lblDetails, 2, 0, 1, 1);


        horizontalLayout->addLayout(gridLayout);


        verticalLayout_3->addWidget(groupBox);

        groupBox_2 = new QGroupBox(StrbDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        verticalLayout = new QVBoxLayout(groupBox_2);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        lblFrameCnt = new QLabel(groupBox_2);
        lblFrameCnt->setObjectName(QString::fromUtf8("lblFrameCnt"));

        gridLayout_2->addWidget(lblFrameCnt, 0, 0, 1, 1);

        lblDuration = new QLabel(groupBox_2);
        lblDuration->setObjectName(QString::fromUtf8("lblDuration"));

        gridLayout_2->addWidget(lblDuration, 0, 1, 1, 1);

        lblFPS = new QLabel(groupBox_2);
        lblFPS->setObjectName(QString::fromUtf8("lblFPS"));

        gridLayout_2->addWidget(lblFPS, 1, 0, 1, 1);


        verticalLayout->addLayout(gridLayout_2);


        verticalLayout_3->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(StrbDialog);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        verticalLayout_2 = new QVBoxLayout(groupBox_3);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        treeWidget = new QTreeWidget(groupBox_3);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        treeWidget->header()->setDefaultSectionSize(65);

        verticalLayout_2->addWidget(treeWidget);


        verticalLayout_3->addWidget(groupBox_3);


        retranslateUi(StrbDialog);

        QMetaObject::connectSlotsByName(StrbDialog);
    } // setupUi

    void retranslateUi(QDialog *StrbDialog)
    {
        StrbDialog->setWindowTitle(QApplication::translate("StrbDialog", "Avatar Asset Dialog", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("StrbDialog", "Metadata", 0, QApplication::UnicodeUTF8));
        lblGender->setText(QApplication::translate("StrbDialog", "Avatar Gender:", 0, QApplication::UnicodeUTF8));
        lblSubCat->setText(QApplication::translate("StrbDialog", "Sub-Category", 0, QApplication::UnicodeUTF8));
        lblBinType->setText(QApplication::translate("StrbDialog", "Binary Asset Type:", 0, QApplication::UnicodeUTF8));
        lblSkeletonV->setText(QApplication::translate("StrbDialog", "Skeleton Version:", 0, QApplication::UnicodeUTF8));
        lblDetails->setText(QApplication::translate("StrbDialog", "Asset Type Details:", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("StrbDialog", "Animation", 0, QApplication::UnicodeUTF8));
        lblFrameCnt->setText(QApplication::translate("StrbDialog", "Frame Count:", 0, QApplication::UnicodeUTF8));
        lblDuration->setText(QApplication::translate("StrbDialog", "Duration:", 0, QApplication::UnicodeUTF8));
        lblFPS->setText(QApplication::translate("StrbDialog", "Frames Per Second:", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("StrbDialog", "Blocks", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(2, QApplication::translate("StrbDialog", "Size", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(1, QApplication::translate("StrbDialog", "Address", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(0, QApplication::translate("StrbDialog", "Type", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class StrbDialog: public Ui_StrbDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STRBDIALOG_H

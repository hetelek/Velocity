/********************************************************************************
** Form generated from reading UI file 'transferflagsdialog.ui'
**
** Created: Thu Aug 30 18:18:49 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRANSFERFLAGSDIALOG_H
#define UI_TRANSFERFLAGSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_TransferFlagsDialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *comboBox;
    QPushButton *pushButton;
    QTreeWidget *treeWidget;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_3;
    QPushButton *pushButton_2;

    void setupUi(QDialog *TransferFlagsDialog)
    {
        if (TransferFlagsDialog->objectName().isEmpty())
            TransferFlagsDialog->setObjectName(QString::fromUtf8("TransferFlagsDialog"));
        TransferFlagsDialog->resize(362, 269);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/Leaner24.png"), QSize(), QIcon::Normal, QIcon::Off);
        TransferFlagsDialog->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(TransferFlagsDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(TransferFlagsDialog);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        comboBox = new QComboBox(TransferFlagsDialog);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        horizontalLayout->addWidget(comboBox);

        pushButton = new QPushButton(TransferFlagsDialog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setMaximumSize(QSize(41, 16777215));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/add.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton->setIcon(icon1);

        horizontalLayout->addWidget(pushButton);


        verticalLayout->addLayout(horizontalLayout);

        treeWidget = new QTreeWidget(TransferFlagsDialog);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        treeWidget->header()->setDefaultSectionSize(130);

        verticalLayout->addWidget(treeWidget);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        pushButton_3 = new QPushButton(TransferFlagsDialog);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Images/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_3->setIcon(icon2);

        horizontalLayout_2->addWidget(pushButton_3);

        pushButton_2 = new QPushButton(TransferFlagsDialog);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/Images/cancel.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_2->setIcon(icon3);

        horizontalLayout_2->addWidget(pushButton_2);


        verticalLayout->addLayout(horizontalLayout_2);


        retranslateUi(TransferFlagsDialog);

        QMetaObject::connectSlotsByName(TransferFlagsDialog);
    } // setupUi

    void retranslateUi(QDialog *TransferFlagsDialog)
    {
        TransferFlagsDialog->setWindowTitle(QApplication::translate("TransferFlagsDialog", "Transfer Flags Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("TransferFlagsDialog", "Flags:", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QString());
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(1, QApplication::translate("TransferFlagsDialog", "Value", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(0, QApplication::translate("TransferFlagsDialog", "Name", 0, QApplication::UnicodeUTF8));
        pushButton_3->setText(QApplication::translate("TransferFlagsDialog", "Save Changes", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("TransferFlagsDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TransferFlagsDialog: public Ui_TransferFlagsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRANSFERFLAGSDIALOG_H

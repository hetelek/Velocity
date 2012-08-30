/********************************************************************************
** Form generated from reading UI file 'renamedialog.ui'
**
** Created: Thu Aug 30 18:18:50 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RENAMEDIALOG_H
#define UI_RENAMEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_RenameDialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *leRename;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QDialog *RenameDialog)
    {
        if (RenameDialog->objectName().isEmpty())
            RenameDialog->setObjectName(QString::fromUtf8("RenameDialog"));
        RenameDialog->resize(269, 72);
        verticalLayout = new QVBoxLayout(RenameDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(RenameDialog);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        leRename = new QLineEdit(RenameDialog);
        leRename->setObjectName(QString::fromUtf8("leRename"));
        leRename->setMaxLength(40);
        leRename->setFrame(true);

        horizontalLayout->addWidget(leRename);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        pushButton = new QPushButton(RenameDialog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton->setIcon(icon);

        horizontalLayout_2->addWidget(pushButton);

        pushButton_2 = new QPushButton(RenameDialog);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/cancel.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_2->setIcon(icon1);

        horizontalLayout_2->addWidget(pushButton_2);


        verticalLayout->addLayout(horizontalLayout_2);


        retranslateUi(RenameDialog);

        QMetaObject::connectSlotsByName(RenameDialog);
    } // setupUi

    void retranslateUi(QDialog *RenameDialog)
    {
        RenameDialog->setWindowTitle(QApplication::translate("RenameDialog", "Rename Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("RenameDialog", "New Name:", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("RenameDialog", "Save", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("RenameDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RenameDialog: public Ui_RenameDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RENAMEDIALOG_H

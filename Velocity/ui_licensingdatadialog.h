/********************************************************************************
** Form generated from reading UI file 'licensingdatadialog.ui'
**
** Created: Thu Aug 30 18:18:49 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LICENSINGDATADIALOG_H
#define UI_LICENSINGDATADIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_LicensingDataDialog
{
public:
    QVBoxLayout *verticalLayout;
    QTableWidget *tableWidget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QDialog *LicensingDataDialog)
    {
        if (LicensingDataDialog->objectName().isEmpty())
            LicensingDataDialog->setObjectName(QString::fromUtf8("LicensingDataDialog"));
        LicensingDataDialog->resize(392, 258);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/Leaner24.png"), QSize(), QIcon::Normal, QIcon::Off);
        LicensingDataDialog->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(LicensingDataDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tableWidget = new QTableWidget(LicensingDataDialog);
        if (tableWidget->columnCount() < 3)
            tableWidget->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->horizontalHeader()->setDefaultSectionSize(100);

        verticalLayout->addWidget(tableWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton = new QPushButton(LicensingDataDialog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton->setIcon(icon1);

        horizontalLayout->addWidget(pushButton);

        pushButton_2 = new QPushButton(LicensingDataDialog);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Images/cancel.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_2->setIcon(icon2);

        horizontalLayout->addWidget(pushButton_2);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(LicensingDataDialog);

        QMetaObject::connectSlotsByName(LicensingDataDialog);
    } // setupUi

    void retranslateUi(QDialog *LicensingDataDialog)
    {
        LicensingDataDialog->setWindowTitle(QApplication::translate("LicensingDataDialog", "License Data Table", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("LicensingDataDialog", "License ID", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("LicensingDataDialog", "Bits", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("LicensingDataDialog", "Flags", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("LicensingDataDialog", "Save Changes", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("LicensingDataDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class LicensingDataDialog: public Ui_LicensingDataDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LICENSINGDATADIALOG_H

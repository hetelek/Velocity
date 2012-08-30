/********************************************************************************
** Form generated from reading UI file 'certificatedialog.ui'
**
** Created: Thu Aug 30 18:18:49 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CERTIFICATEDIALOG_H
#define UI_CERTIFICATEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CertificateDialog
{
public:
    QVBoxLayout *verticalLayout;
    QTableWidget *tableWidget;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout;
    QPlainTextEdit *txtPubModulus;
    QWidget *tab_2;
    QGridLayout *gridLayout_2;
    QPlainTextEdit *txtCertSig;
    QWidget *tab_3;
    QGridLayout *gridLayout_3;
    QPlainTextEdit *txtSig;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QDialog *CertificateDialog)
    {
        if (CertificateDialog->objectName().isEmpty())
            CertificateDialog->setObjectName(QString::fromUtf8("CertificateDialog"));
        CertificateDialog->resize(505, 459);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/Leaner24.png"), QSize(), QIcon::Normal, QIcon::Off);
        CertificateDialog->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(CertificateDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tableWidget = new QTableWidget(CertificateDialog);
        if (tableWidget->columnCount() < 1)
            tableWidget->setColumnCount(1);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        if (tableWidget->rowCount() < 6)
            tableWidget->setRowCount(6);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(0, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(1, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(2, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(3, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(4, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(5, __qtablewidgetitem6);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->horizontalHeader()->setDefaultSectionSize(300);

        verticalLayout->addWidget(tableWidget);

        tabWidget = new QTabWidget(CertificateDialog);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        gridLayout = new QGridLayout(tab);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        txtPubModulus = new QPlainTextEdit(tab);
        txtPubModulus->setObjectName(QString::fromUtf8("txtPubModulus"));
        txtPubModulus->setReadOnly(true);

        gridLayout->addWidget(txtPubModulus, 0, 0, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        gridLayout_2 = new QGridLayout(tab_2);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        txtCertSig = new QPlainTextEdit(tab_2);
        txtCertSig->setObjectName(QString::fromUtf8("txtCertSig"));
        txtCertSig->setReadOnly(true);

        gridLayout_2->addWidget(txtCertSig, 0, 0, 1, 1);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        gridLayout_3 = new QGridLayout(tab_3);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        txtSig = new QPlainTextEdit(tab_3);
        txtSig->setObjectName(QString::fromUtf8("txtSig"));
        txtSig->setReadOnly(true);

        gridLayout_3->addWidget(txtSig, 0, 0, 1, 1);

        tabWidget->addTab(tab_3, QString());

        verticalLayout->addWidget(tabWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton = new QPushButton(CertificateDialog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton->setIcon(icon1);

        horizontalLayout->addWidget(pushButton);

        pushButton_2 = new QPushButton(CertificateDialog);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Images/cancel.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_2->setIcon(icon2);

        horizontalLayout->addWidget(pushButton_2);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(CertificateDialog);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(CertificateDialog);
    } // setupUi

    void retranslateUi(QDialog *CertificateDialog)
    {
        CertificateDialog->setWindowTitle(QApplication::translate("CertificateDialog", "Certificate", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("CertificateDialog", "Value", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->verticalHeaderItem(0);
        ___qtablewidgetitem1->setText(QApplication::translate("CertificateDialog", "Public Key Certificate Size", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget->verticalHeaderItem(1);
        ___qtablewidgetitem2->setText(QApplication::translate("CertificateDialog", "Console ID", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem3 = tableWidget->verticalHeaderItem(2);
        ___qtablewidgetitem3->setText(QApplication::translate("CertificateDialog", "Console Part Number", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem4 = tableWidget->verticalHeaderItem(3);
        ___qtablewidgetitem4->setText(QApplication::translate("CertificateDialog", "Owner Console Type", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem5 = tableWidget->verticalHeaderItem(4);
        ___qtablewidgetitem5->setText(QApplication::translate("CertificateDialog", "Date of Generation", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem6 = tableWidget->verticalHeaderItem(5);
        ___qtablewidgetitem6->setText(QApplication::translate("CertificateDialog", "Public Exponent", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("CertificateDialog", "Public Modulus", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("CertificateDialog", "Certificate Signature", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("CertificateDialog", "Signature", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("CertificateDialog", "Save Changes", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("CertificateDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CertificateDialog: public Ui_CertificateDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CERTIFICATEDIALOG_H

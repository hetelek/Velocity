/********************************************************************************
** Form generated from reading UI file 'xdbfdialog.ui'
**
** Created: Thu Aug 30 18:18:50 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_XDBFDIALOG_H
#define UI_XDBFDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_XdbfDialog
{
public:
    QVBoxLayout *verticalLayout;
    QTreeWidget *treeWidget;

    void setupUi(QDialog *XdbfDialog)
    {
        if (XdbfDialog->objectName().isEmpty())
            XdbfDialog->setObjectName(QString::fromUtf8("XdbfDialog"));
        XdbfDialog->resize(632, 439);
        XdbfDialog->setMinimumSize(QSize(632, 439));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/Leaner24.png"), QSize(), QIcon::Normal, QIcon::Off);
        XdbfDialog->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(XdbfDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        treeWidget = new QTreeWidget(XdbfDialog);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        treeWidget->setAlternatingRowColors(true);
        treeWidget->header()->setDefaultSectionSize(115);

        verticalLayout->addWidget(treeWidget);


        retranslateUi(XdbfDialog);

        QMetaObject::connectSlotsByName(XdbfDialog);
    } // setupUi

    void retranslateUi(QDialog *XdbfDialog)
    {
        XdbfDialog->setWindowTitle(QApplication::translate("XdbfDialog", "XDBF Viewer", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(3, QApplication::translate("XdbfDialog", "Type", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(2, QApplication::translate("XdbfDialog", "Size", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(1, QApplication::translate("XdbfDialog", "Address", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(0, QApplication::translate("XdbfDialog", "ID", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class XdbfDialog: public Ui_XdbfDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_XDBFDIALOG_H

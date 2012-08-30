/********************************************************************************
** Form generated from reading UI file 'creationwizard.ui'
**
** Created: Thu Aug 30 18:18:50 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CREATIONWIZARD_H
#define UI_CREATIONWIZARD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWizard>
#include <QtGui/QWizardPage>

QT_BEGIN_NAMESPACE

class Ui_CreationWizard
{
public:
    QWizardPage *wizardPage1;
    QGridLayout *gridLayout;
    QLabel *label;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QLabel *label_3;
    QSpacerItem *verticalSpacer;
    QSpacerItem *verticalSpacer_2;
    QWizardPage *wizardPage2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_2;
    QComboBox *cbMagic;
    QSpacerItem *verticalSpacer_3;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_5;
    QLabel *label_4;
    QSpacerItem *verticalSpacer_4;
    QWizardPage *wizardPage;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_9;
    QListWidget *lwContentTypes;
    QWizardPage *wizardPage_2;

    void setupUi(QWizard *CreationWizard)
    {
        if (CreationWizard->objectName().isEmpty())
            CreationWizard->setObjectName(QString::fromUtf8("CreationWizard"));
        CreationWizard->resize(559, 328);
        CreationWizard->setWizardStyle(QWizard::AeroStyle);
        CreationWizard->setTitleFormat(Qt::RichText);
        CreationWizard->setSubTitleFormat(Qt::RichText);
        wizardPage1 = new QWizardPage();
        wizardPage1->setObjectName(QString::fromUtf8("wizardPage1"));
        gridLayout = new QGridLayout(wizardPage1);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(wizardPage1);
        label->setObjectName(QString::fromUtf8("label"));
        label->setWordWrap(true);

        gridLayout->addWidget(label, 0, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_2 = new QLabel(wizardPage1);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setMaximumSize(QSize(128, 128));
        label_2->setPixmap(QPixmap(QString::fromUtf8(":/Images/package512.png")));
        label_2->setScaledContents(false);

        horizontalLayout->addWidget(label_2);

        label_3 = new QLabel(wizardPage1);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setWordWrap(true);

        horizontalLayout->addWidget(label_3);


        gridLayout->addLayout(horizontalLayout, 2, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 1, 0, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_2, 3, 0, 1, 1);

        CreationWizard->addPage(wizardPage1);
        wizardPage2 = new QWizardPage();
        wizardPage2->setObjectName(QString::fromUtf8("wizardPage2"));
        verticalLayout = new QVBoxLayout(wizardPage2);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        cbMagic = new QComboBox(wizardPage2);
        cbMagic->setObjectName(QString::fromUtf8("cbMagic"));
        cbMagic->setMinimumSize(QSize(100, 30));

        horizontalLayout_3->addWidget(cbMagic);


        verticalLayout->addLayout(horizontalLayout_3);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_5 = new QLabel(wizardPage2);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setMinimumSize(QSize(128, 128));
        label_5->setMaximumSize(QSize(128, 128));
        label_5->setFrameShape(QFrame::NoFrame);
        label_5->setPixmap(QPixmap(QString::fromUtf8(":/Images/package512.png")));
        label_5->setScaledContents(true);

        horizontalLayout_4->addWidget(label_5);

        label_4 = new QLabel(wizardPage2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setWordWrap(true);

        horizontalLayout_4->addWidget(label_4);


        verticalLayout->addLayout(horizontalLayout_4);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_4);

        CreationWizard->addPage(wizardPage2);
        wizardPage = new QWizardPage();
        wizardPage->setObjectName(QString::fromUtf8("wizardPage"));
        horizontalLayout_5 = new QHBoxLayout(wizardPage);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_9 = new QLabel(wizardPage);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setPixmap(QPixmap(QString::fromUtf8(":/Images/package512.png")));

        horizontalLayout_5->addWidget(label_9);

        lwContentTypes = new QListWidget(wizardPage);
        lwContentTypes->setObjectName(QString::fromUtf8("lwContentTypes"));

        horizontalLayout_5->addWidget(lwContentTypes);

        CreationWizard->addPage(wizardPage);
        wizardPage_2 = new QWizardPage();
        wizardPage_2->setObjectName(QString::fromUtf8("wizardPage_2"));
        CreationWizard->addPage(wizardPage_2);

        retranslateUi(CreationWizard);

        QMetaObject::connectSlotsByName(CreationWizard);
    } // setupUi

    void retranslateUi(QWizard *CreationWizard)
    {
        CreationWizard->setWindowTitle(QApplication::translate("CreationWizard", "Package Creation Wizard", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("CreationWizard", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:18pt; font-weight:600;\">Package Creation Wizard</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_2->setText(QString());
        label_3->setText(QApplication::translate("CreationWizard", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'arial'; font-size:10pt; color:#222222; background-color:#ffffff;\">    Welcome to Velocity's package creation wizard! Here you will be guided through a series of pages where you will select various options which your package will depend on. The file format of the package will be STFS, which is used on the Xbox360.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'arial'; font-size:10pt; color:#222222; background-co"
                        "lor:#ffffff;\">    The </span><span style=\" font-family:'arial'; font-size:10pt; font-style:italic; color:#222222; background-color:#ffffff;\">Secure Transacted File System</span><span style=\" font-family:'arial'; font-size:10pt; color:#222222; background-color:#ffffff;\"> (STFS) is used to store save game files, marketplace items, gamer pictures, or any other accessible content. STFS provides two important benefits to the Xbox 360 title developer: content security and content reliability. Continue to the next page to begin.</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        wizardPage2->setTitle(QApplication::translate("CreationWizard", "Magic", 0, QApplication::UnicodeUTF8));
        wizardPage2->setSubTitle(QApplication::translate("CreationWizard", "The type of STFS package.", 0, QApplication::UnicodeUTF8));
        cbMagic->clear();
        cbMagic->insertItems(0, QStringList()
         << QApplication::translate("CreationWizard", "CON", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("CreationWizard", "LIVE", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("CreationWizard", "PIRS", 0, QApplication::UnicodeUTF8)
        );
        label_5->setText(QString());
        label_4->setText(QApplication::translate("CreationWizard", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:arial; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\"  font-family:arial;font-size:10pt;\">CON packages are console signed packages which means that they can be edited for use on a retail Xbox360 console. These types of packages are typically used to store profiles, savegames and other offline content.</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        wizardPage->setTitle(QApplication::translate("CreationWizard", "Content Type", 0, QApplication::UnicodeUTF8));
        wizardPage->setSubTitle(QApplication::translate("CreationWizard", "The type of content stored inside the package.", 0, QApplication::UnicodeUTF8));
        label_9->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class CreationWizard: public Ui_CreationWizard {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREATIONWIZARD_H

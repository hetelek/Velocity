/********************************************************************************
** Form generated from reading UI file 'achievementcreationwizard.ui'
**
** Created: Thu Aug 30 18:18:50 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ACHIEVEMENTCREATIONWIZARD_H
#define UI_ACHIEVEMENTCREATIONWIZARD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWizard>
#include <QtGui/QWizardPage>

QT_BEGIN_NAMESPACE

class Ui_AchievementCreationWizard
{
public:
    QWizardPage *wizardPage1;
    QVBoxLayout *verticalLayout;
    QLabel *label_2;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLabel *label_3;
    QSpacerItem *verticalSpacer_2;
    QWizardPage *wizardPage2;
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_6;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_4;
    QLineEdit *lineEdit;
    QSpacerItem *verticalSpacer_3;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_5;
    QPlainTextEdit *plainTextEdit;
    QSpacerItem *verticalSpacer_4;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_7;
    QPlainTextEdit *plainTextEdit_2;
    QWizardPage *wizardPage;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_8;
    QVBoxLayout *verticalLayout_7;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *verticalLayout_6;
    QLabel *label_9;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_10;
    QSpacerItem *verticalSpacer_5;
    QSpacerItem *verticalSpacer_6;
    QWizardPage *wizardPage_2;
    QVBoxLayout *verticalLayout_9;
    QSpacerItem *verticalSpacer_7;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_11;
    QVBoxLayout *verticalLayout_8;
    QHBoxLayout *horizontalLayout_7;
    QSpacerItem *horizontalSpacer_3;
    QSpinBox *spinBox;
    QLabel *label_12;
    QSpacerItem *verticalSpacer_8;
    QWizardPage *wizardPage_4;
    QVBoxLayout *verticalLayout_11;
    QSpacerItem *verticalSpacer_9;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_13;
    QVBoxLayout *verticalLayout_10;
    QHBoxLayout *horizontalLayout_9;
    QSpacerItem *horizontalSpacer_4;
    QComboBox *comboBox;
    QLabel *lblAchievementType;
    QSpacerItem *verticalSpacer_10;
    QWizardPage *wizardPage_3;
    QWizardPage *wizardPage_5;

    void setupUi(QWizard *AchievementCreationWizard)
    {
        if (AchievementCreationWizard->objectName().isEmpty())
            AchievementCreationWizard->setObjectName(QString::fromUtf8("AchievementCreationWizard"));
        AchievementCreationWizard->resize(546, 375);
        AchievementCreationWizard->setWizardStyle(QWizard::AeroStyle);
        AchievementCreationWizard->setTitleFormat(Qt::RichText);
        AchievementCreationWizard->setSubTitleFormat(Qt::RichText);
        wizardPage1 = new QWizardPage();
        wizardPage1->setObjectName(QString::fromUtf8("wizardPage1"));
        verticalLayout = new QVBoxLayout(wizardPage1);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_2 = new QLabel(wizardPage1);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setWordWrap(true);

        verticalLayout->addWidget(label_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(wizardPage1);
        label->setObjectName(QString::fromUtf8("label"));
        label->setMinimumSize(QSize(175, 175));
        label->setMaximumSize(QSize(175, 175));
        label->setPixmap(QPixmap(QString::fromUtf8(":/Images/trophy.png")));
        label->setScaledContents(true);

        horizontalLayout->addWidget(label);

        label_3 = new QLabel(wizardPage1);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setWordWrap(true);

        horizontalLayout->addWidget(label_3);


        verticalLayout->addLayout(horizontalLayout);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        AchievementCreationWizard->addPage(wizardPage1);
        wizardPage2 = new QWizardPage();
        wizardPage2->setObjectName(QString::fromUtf8("wizardPage2"));
        verticalLayout_5 = new QVBoxLayout(wizardPage2);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_6 = new QLabel(wizardPage2);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setMinimumSize(QSize(175, 175));
        label_6->setMaximumSize(QSize(175, 175));
        label_6->setPixmap(QPixmap(QString::fromUtf8(":/Images/trophy.png")));
        label_6->setScaledContents(true);

        horizontalLayout_4->addWidget(label_6);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_4 = new QLabel(wizardPage2);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_3->addWidget(label_4);

        lineEdit = new QLineEdit(wizardPage2);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setMaxLength(32);

        horizontalLayout_3->addWidget(lineEdit);


        verticalLayout_4->addLayout(horizontalLayout_3);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        label_5 = new QLabel(wizardPage2);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        verticalLayout_2->addWidget(label_5);

        plainTextEdit = new QPlainTextEdit(wizardPage2);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));

        verticalLayout_2->addWidget(plainTextEdit);


        horizontalLayout_2->addLayout(verticalLayout_2);


        verticalLayout_4->addLayout(horizontalLayout_2);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_4);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        label_7 = new QLabel(wizardPage2);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        verticalLayout_3->addWidget(label_7);

        plainTextEdit_2 = new QPlainTextEdit(wizardPage2);
        plainTextEdit_2->setObjectName(QString::fromUtf8("plainTextEdit_2"));

        verticalLayout_3->addWidget(plainTextEdit_2);


        verticalLayout_4->addLayout(verticalLayout_3);


        horizontalLayout_4->addLayout(verticalLayout_4);


        verticalLayout_5->addLayout(horizontalLayout_4);

        AchievementCreationWizard->addPage(wizardPage2);
        wizardPage = new QWizardPage();
        wizardPage->setObjectName(QString::fromUtf8("wizardPage"));
        gridLayout = new QGridLayout(wizardPage);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label_8 = new QLabel(wizardPage);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setMinimumSize(QSize(175, 175));
        label_8->setMaximumSize(QSize(175, 175));
        label_8->setPixmap(QPixmap(QString::fromUtf8(":/Images/trophy.png")));
        label_8->setScaledContents(true);

        horizontalLayout_6->addWidget(label_8);

        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        label_9 = new QLabel(wizardPage);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setMinimumSize(QSize(64, 64));
        label_9->setMaximumSize(QSize(64, 64));
        label_9->setPixmap(QPixmap(QString::fromUtf8(":/Images/HiddenAchievement.png")));
        label_9->setScaledContents(true);

        verticalLayout_6->addWidget(label_9);

        pushButton = new QPushButton(wizardPage);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setMinimumSize(QSize(64, 0));
        pushButton->setMaximumSize(QSize(64, 16777215));

        verticalLayout_6->addWidget(pushButton);


        horizontalLayout_5->addLayout(verticalLayout_6);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_2);


        verticalLayout_7->addLayout(horizontalLayout_5);

        label_10 = new QLabel(wizardPage);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setWordWrap(true);

        verticalLayout_7->addWidget(label_10);


        horizontalLayout_6->addLayout(verticalLayout_7);


        gridLayout->addLayout(horizontalLayout_6, 1, 0, 1, 1);

        verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_5, 0, 0, 1, 1);

        verticalSpacer_6 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_6, 2, 0, 1, 1);

        AchievementCreationWizard->addPage(wizardPage);
        wizardPage_2 = new QWizardPage();
        wizardPage_2->setObjectName(QString::fromUtf8("wizardPage_2"));
        verticalLayout_9 = new QVBoxLayout(wizardPage_2);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        verticalSpacer_7 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_9->addItem(verticalSpacer_7);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        label_11 = new QLabel(wizardPage_2);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setMinimumSize(QSize(175, 175));
        label_11->setMaximumSize(QSize(175, 175));
        label_11->setPixmap(QPixmap(QString::fromUtf8(":/Images/trophy.png")));
        label_11->setScaledContents(true);

        horizontalLayout_8->addWidget(label_11);

        verticalLayout_8 = new QVBoxLayout();
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_3);

        spinBox = new QSpinBox(wizardPage_2);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setMinimumSize(QSize(0, 30));
        QFont font;
        font.setPointSize(10);
        spinBox->setFont(font);
        spinBox->setMinimum(-214748364);
        spinBox->setMaximum(2147483647);

        horizontalLayout_7->addWidget(spinBox);


        verticalLayout_8->addLayout(horizontalLayout_7);

        label_12 = new QLabel(wizardPage_2);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setWordWrap(true);

        verticalLayout_8->addWidget(label_12);


        horizontalLayout_8->addLayout(verticalLayout_8);


        verticalLayout_9->addLayout(horizontalLayout_8);

        verticalSpacer_8 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_9->addItem(verticalSpacer_8);

        AchievementCreationWizard->addPage(wizardPage_2);
        wizardPage_4 = new QWizardPage();
        wizardPage_4->setObjectName(QString::fromUtf8("wizardPage_4"));
        verticalLayout_11 = new QVBoxLayout(wizardPage_4);
        verticalLayout_11->setObjectName(QString::fromUtf8("verticalLayout_11"));
        verticalSpacer_9 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_11->addItem(verticalSpacer_9);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        label_13 = new QLabel(wizardPage_4);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setMinimumSize(QSize(175, 175));
        label_13->setMaximumSize(QSize(175, 175));
        label_13->setPixmap(QPixmap(QString::fromUtf8(":/Images/trophy.png")));
        label_13->setScaledContents(true);

        horizontalLayout_10->addWidget(label_13);

        verticalLayout_10 = new QVBoxLayout();
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_4);

        comboBox = new QComboBox(wizardPage_4);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setMinimumSize(QSize(0, 30));
        comboBox->setFont(font);

        horizontalLayout_9->addWidget(comboBox);


        verticalLayout_10->addLayout(horizontalLayout_9);

        lblAchievementType = new QLabel(wizardPage_4);
        lblAchievementType->setObjectName(QString::fromUtf8("lblAchievementType"));
        lblAchievementType->setWordWrap(true);

        verticalLayout_10->addWidget(lblAchievementType);


        horizontalLayout_10->addLayout(verticalLayout_10);


        verticalLayout_11->addLayout(horizontalLayout_10);

        verticalSpacer_10 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_11->addItem(verticalSpacer_10);

        AchievementCreationWizard->addPage(wizardPage_4);
        wizardPage_3 = new QWizardPage();
        wizardPage_3->setObjectName(QString::fromUtf8("wizardPage_3"));
        AchievementCreationWizard->addPage(wizardPage_3);
        wizardPage_5 = new QWizardPage();
        wizardPage_5->setObjectName(QString::fromUtf8("wizardPage_5"));
        AchievementCreationWizard->addPage(wizardPage_5);

        retranslateUi(AchievementCreationWizard);

        QMetaObject::connectSlotsByName(AchievementCreationWizard);
    } // setupUi

    void retranslateUi(QWizard *AchievementCreationWizard)
    {
        AchievementCreationWizard->setWindowTitle(QApplication::translate("AchievementCreationWizard", "Wizard", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("AchievementCreationWizard", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:18pt; font-weight:600;\">Achievement Creation Wizard</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label->setText(QString());
        label_3->setText(QApplication::translate("AchievementCreationWizard", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Welcome to the achievement creation wizard! This wizard will allow you to create an achievement for the game _____. The achievement you create will only be obtainable through modding, and since it isn't an official achievement, your friends on XboxLIVE will not be able to see it.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\"></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0"
                        "px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Click next to begin.</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        wizardPage2->setTitle(QApplication::translate("AchievementCreationWizard", "Name and Description", 0, QApplication::UnicodeUTF8));
        wizardPage2->setSubTitle(QApplication::translate("AchievementCreationWizard", "Choose the text that you want the gamer to see", 0, QApplication::UnicodeUTF8));
        label_6->setText(QString());
        label_4->setText(QApplication::translate("AchievementCreationWizard", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Name:</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("AchievementCreationWizard", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Locked Description:</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("AchievementCreationWizard", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Unlocked Description:</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        wizardPage->setTitle(QApplication::translate("AchievementCreationWizard", "Thumbnail", 0, QApplication::UnicodeUTF8));
        wizardPage->setSubTitle(QApplication::translate("AchievementCreationWizard", "Choose the image that will represent the achievement", 0, QApplication::UnicodeUTF8));
        label_8->setText(QString());
        label_9->setText(QString());
        pushButton->setText(QApplication::translate("AchievementCreationWizard", "...", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("AchievementCreationWizard", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">The thumbnail chosen must be 64x64 pixels and it must be a PNG image. The thumbnail for the achievement will be available to the gamer both before and after it has been unlocked.</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        wizardPage_2->setTitle(QApplication::translate("AchievementCreationWizard", "Gamerscore", 0, QApplication::UnicodeUTF8));
        wizardPage_2->setSubTitle(QApplication::translate("AchievementCreationWizard", "Choose the amount of gamerscore to be awared to the gamer upon unlocking it", 0, QApplication::UnicodeUTF8));
        label_11->setText(QString());
        spinBox->setSuffix(QApplication::translate("AchievementCreationWizard", "G", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("AchievementCreationWizard", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">No matter how much gamerscore is awarded for unlocking this achievement, none if it will go towards the amount seen by your friends. </span></p></body></html>", 0, QApplication::UnicodeUTF8));
        wizardPage_4->setTitle(QApplication::translate("AchievementCreationWizard", "Achievement Type", 0, QApplication::UnicodeUTF8));
        wizardPage_4->setSubTitle(QApplication::translate("AchievementCreationWizard", "A category the achievement is put in based on how it is obtainable", 0, QApplication::UnicodeUTF8));
        label_13->setText(QString());
        comboBox->clear();
        comboBox->insertItems(0, QStringList()
         << QApplication::translate("AchievementCreationWizard", "Completion", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("AchievementCreationWizard", "Leveling", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("AchievementCreationWizard", "Unlock", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("AchievementCreationWizard", "Event", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("AchievementCreationWizard", "Tournament", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("AchievementCreationWizard", "Checkpoint", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("AchievementCreationWizard", "Other", 0, QApplication::UnicodeUTF8)
        );
        lblAchievementType->setText(QApplication::translate("AchievementCreationWizard", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">The player has completed a game. For example, finishing the single-player campaign or finishing Dynasty Mode.</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class AchievementCreationWizard: public Ui_AchievementCreationWizard {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ACHIEVEMENTCREATIONWIZARD_H

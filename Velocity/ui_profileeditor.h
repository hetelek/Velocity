/********************************************************************************
** Form generated from reading UI file 'profileeditor.ui'
**
** Created: Thu Aug 30 18:18:49 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROFILEEDITOR_H
#define UI_PROFILEEDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDateTimeEdit>
#include <QtGui/QDialog>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSplitter>
#include <QtGui/QTabWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ProfileEditor
{
public:
    QGridLayout *gridLayout_9;
    QTabWidget *tabWidget;
    QWidget *tabGeneralSettings;
    QGridLayout *gridLayout_5;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_3;
    QGridLayout *gridLayout;
    QLabel *imgGamerpicture;
    QLineEdit *txtGamertag;
    QLabel *lblGamerscore;
    QSpacerItem *horizontalSpacer;
    QLabel *lblLanguage;
    QLabel *lblLanguage_2;
    QLabel *lblParentalControls;
    QLabel *label_21;
    QSpacerItem *horizontalSpacer_10;
    QVBoxLayout *verticalLayout_4;
    QComboBox *comboBox_4;
    QComboBox *comboBox_5;
    QLabel *label_22;
    QLabel *label_23;
    QCheckBox *chxLIVE;
    QCheckBox *chxRecovering;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_4;
    QVBoxLayout *verticalLayout_3;
    QGridLayout *gridLayout_2;
    QLabel *label_12;
    QLineEdit *txtName;
    QLabel *label_13;
    QLineEdit *txtMotto;
    QLabel *label_14;
    QLineEdit *txtLocation;
    QLabel *label_15;
    QComboBox *cmbxGamerzone;
    QLabel *label_17;
    QComboBox *cmbxRegion;
    QLabel *label_18;
    QComboBox *cmbxTenure;
    QLabel *label_16;
    QDoubleSpinBox *spnRep;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_19;
    QPlainTextEdit *txtBio;
    QLabel *imgAvatar;
    QWidget *tabAchievements;
    QGridLayout *gridLayout_10;
    QSplitter *splitter_3;
    QTreeWidget *gamesList;
    QSplitter *splitter;
    QTreeWidget *achievementsList;
    QTabWidget *tabWidget_2;
    QWidget *tab_5;
    QGridLayout *gridLayout_11;
    QLabel *lblGameName;
    QLabel *imgBoxArt;
    QVBoxLayout *verticalLayout_14;
    QPushButton *btnUnlockAllAchvs;
    QPushButton *btnCreateAch;
    QPushButton *btnExtractGPD;
    QLabel *lblGameTitleID;
    QLabel *lblGameLastPlayed;
    QLabel *lblGameAchvs;
    QLabel *lblGameGamerscore;
    QWidget *tab_6;
    QHBoxLayout *horizontalLayout_16;
    QVBoxLayout *verticalLayout_9;
    QLabel *lblAchName;
    QLabel *lblAchLockDesc;
    QLabel *lblAchUnlDesc;
    QLabel *lblAchGamescore;
    QVBoxLayout *verticalLayout_12;
    QLabel *lblAchType;
    QLabel *lblAchSecret;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_29;
    QDateTimeEdit *dteAchTimestamp;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_30;
    QComboBox *cmbxAchState;
    QLabel *imgAch;
    QWidget *tabAvatarAwards;
    QGridLayout *gridLayout_12;
    QSplitter *splitter_4;
    QTreeWidget *aaGamelist;
    QSplitter *splitter_2;
    QTreeWidget *avatarAwardsList;
    QTabWidget *tabWidget_3;
    QWidget *tab;
    QGridLayout *gridLayout_8;
    QLabel *lblAwGameName;
    QLabel *imgAwBoxArt;
    QVBoxLayout *verticalLayout_13;
    QPushButton *btnUnlockAllAwards;
    QPushButton *btnCreateAward;
    QPushButton *btnExtractGPD_2;
    QLabel *lblAwGameTitleID;
    QLabel *lblAwGameLastPlayed;
    QLabel *lblAwGameAwards;
    QWidget *tab_2;
    QHBoxLayout *horizontalLayout_15;
    QVBoxLayout *verticalLayout_10;
    QLabel *lblAwName;
    QLabel *lblAwLockDesc;
    QLabel *lblAwUnlDesc;
    QLabel *lblAwType;
    QVBoxLayout *verticalLayout_11;
    QLabel *lblAwGender;
    QLabel *lblAwSecret;
    QHBoxLayout *horizontalLayout_13;
    QLabel *lblAwLockDesc_6;
    QDateTimeEdit *dteAwTimestamp;
    QHBoxLayout *horizontalLayout_12;
    QLabel *lblAwLockDesc_5;
    QComboBox *cmbxAwState;
    QLabel *imgAw;

    void setupUi(QDialog *ProfileEditor)
    {
        if (ProfileEditor->objectName().isEmpty())
            ProfileEditor->setObjectName(QString::fromUtf8("ProfileEditor"));
        ProfileEditor->resize(920, 408);
        ProfileEditor->setMinimumSize(QSize(920, 408));
        gridLayout_9 = new QGridLayout(ProfileEditor);
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        tabWidget = new QTabWidget(ProfileEditor);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setMinimumSize(QSize(0, 0));
        tabGeneralSettings = new QWidget();
        tabGeneralSettings->setObjectName(QString::fromUtf8("tabGeneralSettings"));
        gridLayout_5 = new QGridLayout(tabGeneralSettings);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        groupBox_3 = new QGroupBox(tabGeneralSettings);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        gridLayout_3 = new QGridLayout(groupBox_3);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        imgGamerpicture = new QLabel(groupBox_3);
        imgGamerpicture->setObjectName(QString::fromUtf8("imgGamerpicture"));
        imgGamerpicture->setMinimumSize(QSize(64, 64));
        imgGamerpicture->setFrameShape(QFrame::Box);

        gridLayout->addWidget(imgGamerpicture, 0, 0, 2, 1);

        txtGamertag = new QLineEdit(groupBox_3);
        txtGamertag->setObjectName(QString::fromUtf8("txtGamertag"));
        txtGamertag->setMaxLength(15);

        gridLayout->addWidget(txtGamertag, 0, 1, 1, 1);

        lblGamerscore = new QLabel(groupBox_3);
        lblGamerscore->setObjectName(QString::fromUtf8("lblGamerscore"));

        gridLayout->addWidget(lblGamerscore, 1, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 2, 1, 1);


        gridLayout_3->addLayout(gridLayout, 0, 0, 1, 3);

        lblLanguage = new QLabel(groupBox_3);
        lblLanguage->setObjectName(QString::fromUtf8("lblLanguage"));

        gridLayout_3->addWidget(lblLanguage, 1, 0, 1, 1);

        lblLanguage_2 = new QLabel(groupBox_3);
        lblLanguage_2->setObjectName(QString::fromUtf8("lblLanguage_2"));

        gridLayout_3->addWidget(lblLanguage_2, 2, 0, 1, 1);

        lblParentalControls = new QLabel(groupBox_3);
        lblParentalControls->setObjectName(QString::fromUtf8("lblParentalControls"));

        gridLayout_3->addWidget(lblParentalControls, 3, 0, 1, 2);

        label_21 = new QLabel(groupBox_3);
        label_21->setObjectName(QString::fromUtf8("label_21"));

        gridLayout_3->addWidget(label_21, 4, 0, 1, 1);

        horizontalSpacer_10 = new QSpacerItem(121, 28, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_10, 4, 1, 1, 1);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        comboBox_4 = new QComboBox(groupBox_3);
        comboBox_4->setObjectName(QString::fromUtf8("comboBox_4"));

        verticalLayout_4->addWidget(comboBox_4);

        comboBox_5 = new QComboBox(groupBox_3);
        comboBox_5->setObjectName(QString::fromUtf8("comboBox_5"));

        verticalLayout_4->addWidget(comboBox_5);


        gridLayout_3->addLayout(verticalLayout_4, 4, 2, 2, 1);

        label_22 = new QLabel(groupBox_3);
        label_22->setObjectName(QString::fromUtf8("label_22"));

        gridLayout_3->addWidget(label_22, 5, 0, 1, 1);

        label_23 = new QLabel(groupBox_3);
        label_23->setObjectName(QString::fromUtf8("label_23"));

        gridLayout_3->addWidget(label_23, 6, 0, 1, 1);

        chxLIVE = new QCheckBox(groupBox_3);
        chxLIVE->setObjectName(QString::fromUtf8("chxLIVE"));

        gridLayout_3->addWidget(chxLIVE, 7, 0, 1, 1);

        chxRecovering = new QCheckBox(groupBox_3);
        chxRecovering->setObjectName(QString::fromUtf8("chxRecovering"));

        gridLayout_3->addWidget(chxRecovering, 7, 1, 1, 1);


        gridLayout_5->addWidget(groupBox_3, 0, 0, 1, 1);

        groupBox_2 = new QGroupBox(tabGeneralSettings);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout_4 = new QGridLayout(groupBox_2);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label_12 = new QLabel(groupBox_2);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        gridLayout_2->addWidget(label_12, 0, 0, 1, 1);

        txtName = new QLineEdit(groupBox_2);
        txtName->setObjectName(QString::fromUtf8("txtName"));
        txtName->setMaxLength(129);

        gridLayout_2->addWidget(txtName, 0, 1, 1, 3);

        label_13 = new QLabel(groupBox_2);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        gridLayout_2->addWidget(label_13, 1, 0, 1, 1);

        txtMotto = new QLineEdit(groupBox_2);
        txtMotto->setObjectName(QString::fromUtf8("txtMotto"));
        txtMotto->setMaxLength(21);

        gridLayout_2->addWidget(txtMotto, 1, 1, 1, 3);

        label_14 = new QLabel(groupBox_2);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        gridLayout_2->addWidget(label_14, 2, 0, 1, 1);

        txtLocation = new QLineEdit(groupBox_2);
        txtLocation->setObjectName(QString::fromUtf8("txtLocation"));
        txtLocation->setMaxLength(40);

        gridLayout_2->addWidget(txtLocation, 2, 1, 1, 3);

        label_15 = new QLabel(groupBox_2);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        gridLayout_2->addWidget(label_15, 3, 0, 1, 2);

        cmbxGamerzone = new QComboBox(groupBox_2);
        cmbxGamerzone->setObjectName(QString::fromUtf8("cmbxGamerzone"));

        gridLayout_2->addWidget(cmbxGamerzone, 3, 2, 1, 2);

        label_17 = new QLabel(groupBox_2);
        label_17->setObjectName(QString::fromUtf8("label_17"));

        gridLayout_2->addWidget(label_17, 4, 0, 1, 2);

        cmbxRegion = new QComboBox(groupBox_2);
        cmbxRegion->setObjectName(QString::fromUtf8("cmbxRegion"));

        gridLayout_2->addWidget(cmbxRegion, 4, 2, 1, 2);

        label_18 = new QLabel(groupBox_2);
        label_18->setObjectName(QString::fromUtf8("label_18"));

        gridLayout_2->addWidget(label_18, 5, 0, 1, 2);

        cmbxTenure = new QComboBox(groupBox_2);
        cmbxTenure->setObjectName(QString::fromUtf8("cmbxTenure"));

        gridLayout_2->addWidget(cmbxTenure, 5, 2, 1, 2);

        label_16 = new QLabel(groupBox_2);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        gridLayout_2->addWidget(label_16, 6, 0, 1, 3);

        spnRep = new QDoubleSpinBox(groupBox_2);
        spnRep->setObjectName(QString::fromUtf8("spnRep"));
        spnRep->setMaximum(100);

        gridLayout_2->addWidget(spnRep, 6, 3, 1, 1);


        verticalLayout_3->addLayout(gridLayout_2);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        label_19 = new QLabel(groupBox_2);
        label_19->setObjectName(QString::fromUtf8("label_19"));

        verticalLayout_2->addWidget(label_19);

        txtBio = new QPlainTextEdit(groupBox_2);
        txtBio->setObjectName(QString::fromUtf8("txtBio"));

        verticalLayout_2->addWidget(txtBio);


        verticalLayout_3->addLayout(verticalLayout_2);


        gridLayout_4->addLayout(verticalLayout_3, 0, 0, 1, 1);


        gridLayout_5->addWidget(groupBox_2, 0, 1, 1, 1);

        imgAvatar = new QLabel(tabGeneralSettings);
        imgAvatar->setObjectName(QString::fromUtf8("imgAvatar"));
        imgAvatar->setMinimumSize(QSize(150, 300));
        imgAvatar->setMaximumSize(QSize(150, 300));
        imgAvatar->setFrameShape(QFrame::NoFrame);

        gridLayout_5->addWidget(imgAvatar, 0, 2, 1, 1);

        tabWidget->addTab(tabGeneralSettings, QString());
        tabAchievements = new QWidget();
        tabAchievements->setObjectName(QString::fromUtf8("tabAchievements"));
        gridLayout_10 = new QGridLayout(tabAchievements);
        gridLayout_10->setObjectName(QString::fromUtf8("gridLayout_10"));
        splitter_3 = new QSplitter(tabAchievements);
        splitter_3->setObjectName(QString::fromUtf8("splitter_3"));
        splitter_3->setOrientation(Qt::Horizontal);
        gamesList = new QTreeWidget(splitter_3);
        gamesList->setObjectName(QString::fromUtf8("gamesList"));
        gamesList->setIconSize(QSize(32, 32));
        splitter_3->addWidget(gamesList);
        splitter = new QSplitter(splitter_3);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Vertical);
        achievementsList = new QTreeWidget(splitter);
        achievementsList->setObjectName(QString::fromUtf8("achievementsList"));
        achievementsList->setMinimumSize(QSize(621, 191));
        achievementsList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        achievementsList->setAlternatingRowColors(true);
        achievementsList->setWordWrap(false);
        splitter->addWidget(achievementsList);
        achievementsList->header()->setDefaultSectionSize(105);
        tabWidget_2 = new QTabWidget(splitter);
        tabWidget_2->setObjectName(QString::fromUtf8("tabWidget_2"));
        tabWidget_2->setTabPosition(QTabWidget::South);
        tab_5 = new QWidget();
        tab_5->setObjectName(QString::fromUtf8("tab_5"));
        gridLayout_11 = new QGridLayout(tab_5);
        gridLayout_11->setObjectName(QString::fromUtf8("gridLayout_11"));
        lblGameName = new QLabel(tab_5);
        lblGameName->setObjectName(QString::fromUtf8("lblGameName"));

        gridLayout_11->addWidget(lblGameName, 0, 0, 1, 1);

        imgBoxArt = new QLabel(tab_5);
        imgBoxArt->setObjectName(QString::fromUtf8("imgBoxArt"));
        imgBoxArt->setMinimumSize(QSize(85, 120));
        imgBoxArt->setMaximumSize(QSize(85, 120));
        imgBoxArt->setFrameShape(QFrame::NoFrame);

        gridLayout_11->addWidget(imgBoxArt, 0, 1, 5, 1);

        verticalLayout_14 = new QVBoxLayout();
        verticalLayout_14->setObjectName(QString::fromUtf8("verticalLayout_14"));
        btnUnlockAllAchvs = new QPushButton(tab_5);
        btnUnlockAllAchvs->setObjectName(QString::fromUtf8("btnUnlockAllAchvs"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/unlock.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnUnlockAllAchvs->setIcon(icon);
        btnUnlockAllAchvs->setIconSize(QSize(20, 20));

        verticalLayout_14->addWidget(btnUnlockAllAchvs);

        btnCreateAch = new QPushButton(tab_5);
        btnCreateAch->setObjectName(QString::fromUtf8("btnCreateAch"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/add.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnCreateAch->setIcon(icon1);
        btnCreateAch->setIconSize(QSize(20, 20));

        verticalLayout_14->addWidget(btnCreateAch);

        btnExtractGPD = new QPushButton(tab_5);
        btnExtractGPD->setObjectName(QString::fromUtf8("btnExtractGPD"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Images/extract.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnExtractGPD->setIcon(icon2);
        btnExtractGPD->setIconSize(QSize(20, 20));

        verticalLayout_14->addWidget(btnExtractGPD);


        gridLayout_11->addLayout(verticalLayout_14, 0, 2, 5, 1);

        lblGameTitleID = new QLabel(tab_5);
        lblGameTitleID->setObjectName(QString::fromUtf8("lblGameTitleID"));

        gridLayout_11->addWidget(lblGameTitleID, 1, 0, 1, 1);

        lblGameLastPlayed = new QLabel(tab_5);
        lblGameLastPlayed->setObjectName(QString::fromUtf8("lblGameLastPlayed"));

        gridLayout_11->addWidget(lblGameLastPlayed, 2, 0, 1, 1);

        lblGameAchvs = new QLabel(tab_5);
        lblGameAchvs->setObjectName(QString::fromUtf8("lblGameAchvs"));

        gridLayout_11->addWidget(lblGameAchvs, 3, 0, 1, 1);

        lblGameGamerscore = new QLabel(tab_5);
        lblGameGamerscore->setObjectName(QString::fromUtf8("lblGameGamerscore"));

        gridLayout_11->addWidget(lblGameGamerscore, 4, 0, 1, 1);

        tabWidget_2->addTab(tab_5, QString());
        tab_6 = new QWidget();
        tab_6->setObjectName(QString::fromUtf8("tab_6"));
        horizontalLayout_16 = new QHBoxLayout(tab_6);
        horizontalLayout_16->setObjectName(QString::fromUtf8("horizontalLayout_16"));
        verticalLayout_9 = new QVBoxLayout();
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        lblAchName = new QLabel(tab_6);
        lblAchName->setObjectName(QString::fromUtf8("lblAchName"));
        lblAchName->setWordWrap(true);

        verticalLayout_9->addWidget(lblAchName);

        lblAchLockDesc = new QLabel(tab_6);
        lblAchLockDesc->setObjectName(QString::fromUtf8("lblAchLockDesc"));
        lblAchLockDesc->setWordWrap(true);

        verticalLayout_9->addWidget(lblAchLockDesc);

        lblAchUnlDesc = new QLabel(tab_6);
        lblAchUnlDesc->setObjectName(QString::fromUtf8("lblAchUnlDesc"));
        lblAchUnlDesc->setWordWrap(true);

        verticalLayout_9->addWidget(lblAchUnlDesc);

        lblAchGamescore = new QLabel(tab_6);
        lblAchGamescore->setObjectName(QString::fromUtf8("lblAchGamescore"));
        lblAchGamescore->setWordWrap(true);

        verticalLayout_9->addWidget(lblAchGamescore);


        horizontalLayout_16->addLayout(verticalLayout_9);

        verticalLayout_12 = new QVBoxLayout();
        verticalLayout_12->setObjectName(QString::fromUtf8("verticalLayout_12"));
        lblAchType = new QLabel(tab_6);
        lblAchType->setObjectName(QString::fromUtf8("lblAchType"));
        lblAchType->setWordWrap(true);

        verticalLayout_12->addWidget(lblAchType);

        lblAchSecret = new QLabel(tab_6);
        lblAchSecret->setObjectName(QString::fromUtf8("lblAchSecret"));
        lblAchSecret->setWordWrap(true);

        verticalLayout_12->addWidget(lblAchSecret);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        label_29 = new QLabel(tab_6);
        label_29->setObjectName(QString::fromUtf8("label_29"));

        horizontalLayout_11->addWidget(label_29);

        dteAchTimestamp = new QDateTimeEdit(tab_6);
        dteAchTimestamp->setObjectName(QString::fromUtf8("dteAchTimestamp"));

        horizontalLayout_11->addWidget(dteAchTimestamp);


        verticalLayout_12->addLayout(horizontalLayout_11);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        label_30 = new QLabel(tab_6);
        label_30->setObjectName(QString::fromUtf8("label_30"));

        horizontalLayout_10->addWidget(label_30);

        cmbxAchState = new QComboBox(tab_6);
        cmbxAchState->setObjectName(QString::fromUtf8("cmbxAchState"));

        horizontalLayout_10->addWidget(cmbxAchState);


        verticalLayout_12->addLayout(horizontalLayout_10);


        horizontalLayout_16->addLayout(verticalLayout_12);

        imgAch = new QLabel(tab_6);
        imgAch->setObjectName(QString::fromUtf8("imgAch"));
        imgAch->setMinimumSize(QSize(64, 64));
        imgAch->setMaximumSize(QSize(64, 64));
        imgAch->setFrameShape(QFrame::NoFrame);

        horizontalLayout_16->addWidget(imgAch);

        tabWidget_2->addTab(tab_6, QString());
        splitter->addWidget(tabWidget_2);
        splitter_3->addWidget(splitter);

        gridLayout_10->addWidget(splitter_3, 0, 0, 1, 1);

        tabWidget->addTab(tabAchievements, QString());
        tabAvatarAwards = new QWidget();
        tabAvatarAwards->setObjectName(QString::fromUtf8("tabAvatarAwards"));
        gridLayout_12 = new QGridLayout(tabAvatarAwards);
        gridLayout_12->setObjectName(QString::fromUtf8("gridLayout_12"));
        splitter_4 = new QSplitter(tabAvatarAwards);
        splitter_4->setObjectName(QString::fromUtf8("splitter_4"));
        splitter_4->setOrientation(Qt::Horizontal);
        aaGamelist = new QTreeWidget(splitter_4);
        aaGamelist->setObjectName(QString::fromUtf8("aaGamelist"));
        aaGamelist->setMaximumSize(QSize(16777215, 16777215));
        aaGamelist->setIconSize(QSize(32, 32));
        splitter_4->addWidget(aaGamelist);
        splitter_2 = new QSplitter(splitter_4);
        splitter_2->setObjectName(QString::fromUtf8("splitter_2"));
        splitter_2->setOrientation(Qt::Vertical);
        avatarAwardsList = new QTreeWidget(splitter_2);
        avatarAwardsList->setObjectName(QString::fromUtf8("avatarAwardsList"));
        avatarAwardsList->setMinimumSize(QSize(621, 191));
        avatarAwardsList->setAlternatingRowColors(true);
        avatarAwardsList->setIconSize(QSize(32, 32));
        splitter_2->addWidget(avatarAwardsList);
        avatarAwardsList->header()->setDefaultSectionSize(105);
        tabWidget_3 = new QTabWidget(splitter_2);
        tabWidget_3->setObjectName(QString::fromUtf8("tabWidget_3"));
        tabWidget_3->setTabPosition(QTabWidget::South);
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        gridLayout_8 = new QGridLayout(tab);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        lblAwGameName = new QLabel(tab);
        lblAwGameName->setObjectName(QString::fromUtf8("lblAwGameName"));

        gridLayout_8->addWidget(lblAwGameName, 0, 0, 1, 1);

        imgAwBoxArt = new QLabel(tab);
        imgAwBoxArt->setObjectName(QString::fromUtf8("imgAwBoxArt"));
        imgAwBoxArt->setMinimumSize(QSize(85, 120));
        imgAwBoxArt->setMaximumSize(QSize(85, 120));
        imgAwBoxArt->setFrameShape(QFrame::NoFrame);

        gridLayout_8->addWidget(imgAwBoxArt, 0, 1, 4, 1);

        verticalLayout_13 = new QVBoxLayout();
        verticalLayout_13->setObjectName(QString::fromUtf8("verticalLayout_13"));
        btnUnlockAllAwards = new QPushButton(tab);
        btnUnlockAllAwards->setObjectName(QString::fromUtf8("btnUnlockAllAwards"));
        btnUnlockAllAwards->setIcon(icon);
        btnUnlockAllAwards->setIconSize(QSize(20, 20));

        verticalLayout_13->addWidget(btnUnlockAllAwards);

        btnCreateAward = new QPushButton(tab);
        btnCreateAward->setObjectName(QString::fromUtf8("btnCreateAward"));
        btnCreateAward->setIcon(icon1);
        btnCreateAward->setIconSize(QSize(20, 20));

        verticalLayout_13->addWidget(btnCreateAward);

        btnExtractGPD_2 = new QPushButton(tab);
        btnExtractGPD_2->setObjectName(QString::fromUtf8("btnExtractGPD_2"));
        btnExtractGPD_2->setIcon(icon2);
        btnExtractGPD_2->setIconSize(QSize(20, 20));

        verticalLayout_13->addWidget(btnExtractGPD_2);


        gridLayout_8->addLayout(verticalLayout_13, 0, 2, 4, 1);

        lblAwGameTitleID = new QLabel(tab);
        lblAwGameTitleID->setObjectName(QString::fromUtf8("lblAwGameTitleID"));

        gridLayout_8->addWidget(lblAwGameTitleID, 1, 0, 1, 1);

        lblAwGameLastPlayed = new QLabel(tab);
        lblAwGameLastPlayed->setObjectName(QString::fromUtf8("lblAwGameLastPlayed"));

        gridLayout_8->addWidget(lblAwGameLastPlayed, 2, 0, 1, 1);

        lblAwGameAwards = new QLabel(tab);
        lblAwGameAwards->setObjectName(QString::fromUtf8("lblAwGameAwards"));

        gridLayout_8->addWidget(lblAwGameAwards, 3, 0, 1, 1);

        tabWidget_3->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        horizontalLayout_15 = new QHBoxLayout(tab_2);
        horizontalLayout_15->setObjectName(QString::fromUtf8("horizontalLayout_15"));
        verticalLayout_10 = new QVBoxLayout();
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        lblAwName = new QLabel(tab_2);
        lblAwName->setObjectName(QString::fromUtf8("lblAwName"));

        verticalLayout_10->addWidget(lblAwName);

        lblAwLockDesc = new QLabel(tab_2);
        lblAwLockDesc->setObjectName(QString::fromUtf8("lblAwLockDesc"));
        lblAwLockDesc->setWordWrap(true);

        verticalLayout_10->addWidget(lblAwLockDesc);

        lblAwUnlDesc = new QLabel(tab_2);
        lblAwUnlDesc->setObjectName(QString::fromUtf8("lblAwUnlDesc"));
        lblAwUnlDesc->setWordWrap(true);

        verticalLayout_10->addWidget(lblAwUnlDesc);

        lblAwType = new QLabel(tab_2);
        lblAwType->setObjectName(QString::fromUtf8("lblAwType"));

        verticalLayout_10->addWidget(lblAwType);


        horizontalLayout_15->addLayout(verticalLayout_10);

        verticalLayout_11 = new QVBoxLayout();
        verticalLayout_11->setObjectName(QString::fromUtf8("verticalLayout_11"));
        lblAwGender = new QLabel(tab_2);
        lblAwGender->setObjectName(QString::fromUtf8("lblAwGender"));

        verticalLayout_11->addWidget(lblAwGender);

        lblAwSecret = new QLabel(tab_2);
        lblAwSecret->setObjectName(QString::fromUtf8("lblAwSecret"));

        verticalLayout_11->addWidget(lblAwSecret);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));
        lblAwLockDesc_6 = new QLabel(tab_2);
        lblAwLockDesc_6->setObjectName(QString::fromUtf8("lblAwLockDesc_6"));

        horizontalLayout_13->addWidget(lblAwLockDesc_6);

        dteAwTimestamp = new QDateTimeEdit(tab_2);
        dteAwTimestamp->setObjectName(QString::fromUtf8("dteAwTimestamp"));

        horizontalLayout_13->addWidget(dteAwTimestamp);


        verticalLayout_11->addLayout(horizontalLayout_13);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        lblAwLockDesc_5 = new QLabel(tab_2);
        lblAwLockDesc_5->setObjectName(QString::fromUtf8("lblAwLockDesc_5"));

        horizontalLayout_12->addWidget(lblAwLockDesc_5);

        cmbxAwState = new QComboBox(tab_2);
        cmbxAwState->setObjectName(QString::fromUtf8("cmbxAwState"));

        horizontalLayout_12->addWidget(cmbxAwState);


        verticalLayout_11->addLayout(horizontalLayout_12);


        horizontalLayout_15->addLayout(verticalLayout_11);

        imgAw = new QLabel(tab_2);
        imgAw->setObjectName(QString::fromUtf8("imgAw"));
        imgAw->setMinimumSize(QSize(128, 128));
        imgAw->setMaximumSize(QSize(128, 128));
        imgAw->setFrameShape(QFrame::NoFrame);

        horizontalLayout_15->addWidget(imgAw);

        tabWidget_3->addTab(tab_2, QString());
        splitter_2->addWidget(tabWidget_3);
        splitter_4->addWidget(splitter_2);

        gridLayout_12->addWidget(splitter_4, 0, 0, 1, 1);

        tabWidget->addTab(tabAvatarAwards, QString());

        gridLayout_9->addWidget(tabWidget, 0, 0, 1, 1);


        retranslateUi(ProfileEditor);

        tabWidget->setCurrentIndex(0);
        tabWidget_2->setCurrentIndex(1);
        tabWidget_3->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(ProfileEditor);
    } // setupUi

    void retranslateUi(QDialog *ProfileEditor)
    {
        ProfileEditor->setWindowTitle(QApplication::translate("ProfileEditor", "Profile Editor", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("ProfileEditor", "Account Information", 0, QApplication::UnicodeUTF8));
        lblGamerscore->setText(QApplication::translate("ProfileEditor", "gamerscore", 0, QApplication::UnicodeUTF8));
        lblLanguage->setText(QApplication::translate("ProfileEditor", "Language:", 0, QApplication::UnicodeUTF8));
        lblLanguage_2->setText(QApplication::translate("ProfileEditor", "Subscription Teir:", 0, QApplication::UnicodeUTF8));
        lblParentalControls->setText(QApplication::translate("ProfileEditor", "Parental Controls Enabled:", 0, QApplication::UnicodeUTF8));
        label_21->setText(QApplication::translate("ProfileEditor", "Type:", 0, QApplication::UnicodeUTF8));
        comboBox_4->clear();
        comboBox_4->insertItems(0, QStringList()
         << QApplication::translate("ProfileEditor", "Retail", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ProfileEditor", "Developer", 0, QApplication::UnicodeUTF8)
        );
        comboBox_5->clear();
        comboBox_5->insertItems(0, QStringList()
         << QApplication::translate("ProfileEditor", "XboxLIVE (PROD)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ProfileEditor", "PartnerNet (PART)", 0, QApplication::UnicodeUTF8)
        );
        label_22->setText(QApplication::translate("ProfileEditor", "Network:", 0, QApplication::UnicodeUTF8));
        label_23->setText(QApplication::translate("ProfileEditor", "XUID:", 0, QApplication::UnicodeUTF8));
        chxLIVE->setText(QApplication::translate("ProfileEditor", "LIVE Enabled", 0, QApplication::UnicodeUTF8));
        chxRecovering->setText(QApplication::translate("ProfileEditor", "Recovering", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("ProfileEditor", "Profile Settings", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("ProfileEditor", "Name", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("ProfileEditor", "Motto:", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("ProfileEditor", "Location:", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("ProfileEditor", "Gamerzone:", 0, QApplication::UnicodeUTF8));
        cmbxGamerzone->clear();
        cmbxGamerzone->insertItems(0, QStringList()
         << QApplication::translate("ProfileEditor", "None", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ProfileEditor", "Recreational", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ProfileEditor", "Pro", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ProfileEditor", "Family", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ProfileEditor", "Underground", 0, QApplication::UnicodeUTF8)
        );
        label_17->setText(QApplication::translate("ProfileEditor", "Region:", 0, QApplication::UnicodeUTF8));
        label_18->setText(QApplication::translate("ProfileEditor", "Years on Live:", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("ProfileEditor", "Reputation:", 0, QApplication::UnicodeUTF8));
        label_19->setText(QApplication::translate("ProfileEditor", "Bio:", 0, QApplication::UnicodeUTF8));
        imgAvatar->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(tabGeneralSettings), QApplication::translate("ProfileEditor", "General Settings", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = gamesList->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("ProfileEditor", "Games", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem1 = achievementsList->headerItem();
        ___qtreewidgetitem1->setText(3, QApplication::translate("ProfileEditor", "Gamerscore", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem1->setText(2, QApplication::translate("ProfileEditor", "State", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem1->setText(1, QApplication::translate("ProfileEditor", "Description", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem1->setText(0, QApplication::translate("ProfileEditor", "Name", 0, QApplication::UnicodeUTF8));
        lblGameName->setText(QApplication::translate("ProfileEditor", "Name: ", 0, QApplication::UnicodeUTF8));
        imgBoxArt->setText(QString());
        btnUnlockAllAchvs->setText(QApplication::translate("ProfileEditor", "Unlock All Achievements", 0, QApplication::UnicodeUTF8));
        btnCreateAch->setText(QApplication::translate("ProfileEditor", "Create Achievement", 0, QApplication::UnicodeUTF8));
        btnExtractGPD->setText(QApplication::translate("ProfileEditor", "Extract GPD", 0, QApplication::UnicodeUTF8));
        lblGameTitleID->setText(QApplication::translate("ProfileEditor", "TitleID:", 0, QApplication::UnicodeUTF8));
        lblGameLastPlayed->setText(QApplication::translate("ProfileEditor", "Last Played:", 0, QApplication::UnicodeUTF8));
        lblGameAchvs->setText(QApplication::translate("ProfileEditor", "Achievements:", 0, QApplication::UnicodeUTF8));
        lblGameGamerscore->setText(QApplication::translate("ProfileEditor", "Gamerscore:", 0, QApplication::UnicodeUTF8));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_5), QApplication::translate("ProfileEditor", "Game", 0, QApplication::UnicodeUTF8));
        lblAchName->setText(QApplication::translate("ProfileEditor", "Name:", 0, QApplication::UnicodeUTF8));
        lblAchLockDesc->setText(QApplication::translate("ProfileEditor", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Locked Description:</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        lblAchUnlDesc->setText(QApplication::translate("ProfileEditor", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Unlocked Description:</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        lblAchGamescore->setText(QApplication::translate("ProfileEditor", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Gamerscore:</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        lblAchType->setText(QApplication::translate("ProfileEditor", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Type:</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        lblAchSecret->setText(QApplication::translate("ProfileEditor", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Secret:</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_29->setText(QApplication::translate("ProfileEditor", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Timestamp:</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_30->setText(QApplication::translate("ProfileEditor", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">State:</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        cmbxAchState->clear();
        cmbxAchState->insertItems(0, QStringList()
         << QApplication::translate("ProfileEditor", "Locked", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ProfileEditor", "Unlocked Offline", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ProfileEditor", "Unlocked Online", 0, QApplication::UnicodeUTF8)
        );
        imgAch->setText(QApplication::translate("ProfileEditor", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_6), QApplication::translate("ProfileEditor", "Achievement", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabAchievements), QApplication::translate("ProfileEditor", "Achievements", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem2 = aaGamelist->headerItem();
        ___qtreewidgetitem2->setText(0, QApplication::translate("ProfileEditor", "Games", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem3 = avatarAwardsList->headerItem();
        ___qtreewidgetitem3->setText(3, QApplication::translate("ProfileEditor", "State", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem3->setText(2, QApplication::translate("ProfileEditor", "Gender", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem3->setText(1, QApplication::translate("ProfileEditor", "Descritpion", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem3->setText(0, QApplication::translate("ProfileEditor", "Name", 0, QApplication::UnicodeUTF8));
        lblAwGameName->setText(QApplication::translate("ProfileEditor", "Name:", 0, QApplication::UnicodeUTF8));
        imgAwBoxArt->setText(QString());
        btnUnlockAllAwards->setText(QApplication::translate("ProfileEditor", "Unlock All Awards", 0, QApplication::UnicodeUTF8));
        btnCreateAward->setText(QApplication::translate("ProfileEditor", "Create Award", 0, QApplication::UnicodeUTF8));
        btnExtractGPD_2->setText(QApplication::translate("ProfileEditor", "Extract GPD", 0, QApplication::UnicodeUTF8));
        lblAwGameTitleID->setText(QApplication::translate("ProfileEditor", "TitleID:", 0, QApplication::UnicodeUTF8));
        lblAwGameLastPlayed->setText(QApplication::translate("ProfileEditor", "Last Played:", 0, QApplication::UnicodeUTF8));
        lblAwGameAwards->setText(QApplication::translate("ProfileEditor", "Awards:", 0, QApplication::UnicodeUTF8));
        tabWidget_3->setTabText(tabWidget_3->indexOf(tab), QApplication::translate("ProfileEditor", "Game", 0, QApplication::UnicodeUTF8));
        lblAwName->setText(QApplication::translate("ProfileEditor", "Name:", 0, QApplication::UnicodeUTF8));
        lblAwLockDesc->setText(QApplication::translate("ProfileEditor", "Locked Description:", 0, QApplication::UnicodeUTF8));
        lblAwUnlDesc->setText(QApplication::translate("ProfileEditor", "Unlocked Description:", 0, QApplication::UnicodeUTF8));
        lblAwType->setText(QApplication::translate("ProfileEditor", "Type:", 0, QApplication::UnicodeUTF8));
        lblAwGender->setText(QApplication::translate("ProfileEditor", "Gender:", 0, QApplication::UnicodeUTF8));
        lblAwSecret->setText(QApplication::translate("ProfileEditor", "Secret:", 0, QApplication::UnicodeUTF8));
        lblAwLockDesc_6->setText(QApplication::translate("ProfileEditor", "Timestamp:", 0, QApplication::UnicodeUTF8));
        lblAwLockDesc_5->setText(QApplication::translate("ProfileEditor", "State:", 0, QApplication::UnicodeUTF8));
        cmbxAwState->clear();
        cmbxAwState->insertItems(0, QStringList()
         << QApplication::translate("ProfileEditor", "Locked", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ProfileEditor", "Unlocked Offline", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ProfileEditor", "Unlocked Online", 0, QApplication::UnicodeUTF8)
        );
        imgAw->setText(QString());
        tabWidget_3->setTabText(tabWidget_3->indexOf(tab_2), QApplication::translate("ProfileEditor", "Award", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabAvatarAwards), QApplication::translate("ProfileEditor", "Avatar Awards", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ProfileEditor: public Ui_ProfileEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROFILEEDITOR_H

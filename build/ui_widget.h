/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QGridLayout *gridLayout_3;
    QTextBrowser *connectText;
    QGridLayout *controlLayout;
    QPushButton *powerButton;
    QPushButton *elseButton;
    QPushButton *gpsButton;
    QPushButton *energyButton;
    QSpinBox *nodeIDBox;
    QPushButton *configButton;
    QPushButton *rebootButton;
    QSpacerItem *horizontalSpacer;
    QLabel *label;
    QPushButton *geoButton;
    QLabel *nodeIDText;
    QPushButton *energyratioButton;
    QPushButton *pulseButton;
    QPushButton *startButton;
    QPushButton *energythresButton;
    QSpacerItem *horizontalSpacer_2;
    QGraphicsView *trackView;
    QGridLayout *gridLayout_2;
    QVBoxLayout *node0Layout;
    QLabel *node0Label;
    QPushButton *node0Button;
    QVBoxLayout *node1Layout;
    QLabel *node1Label;
    QPushButton *node1Button;
    QVBoxLayout *node2Layout;
    QLabel *node2Label;
    QPushButton *node2Button;
    QVBoxLayout *node3Layout;
    QLabel *node3Label;
    QPushButton *node3Button;
    QVBoxLayout *node4Layout;
    QLabel *node4Label;
    QPushButton *node4Button;
    QVBoxLayout *node5Layout;
    QLabel *node5Label;
    QPushButton *node5Button;
    QTextBrowser *trackText;
    QTextBrowser *logText;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(2290, 1165);
        gridLayout_3 = new QGridLayout(Widget);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setHorizontalSpacing(30);
        gridLayout_3->setVerticalSpacing(20);
        connectText = new QTextBrowser(Widget);
        connectText->setObjectName(QString::fromUtf8("connectText"));
        connectText->setMaximumSize(QSize(740, 125));

        gridLayout_3->addWidget(connectText, 2, 2, 1, 1);

        controlLayout = new QGridLayout();
        controlLayout->setObjectName(QString::fromUtf8("controlLayout"));
        controlLayout->setHorizontalSpacing(30);
        controlLayout->setVerticalSpacing(15);
        powerButton = new QPushButton(Widget);
        powerButton->setObjectName(QString::fromUtf8("powerButton"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(powerButton->sizePolicy().hasHeightForWidth());
        powerButton->setSizePolicy(sizePolicy);
        powerButton->setMaximumSize(QSize(16777215, 40));
        QFont font;
        font.setFamily(QString::fromUtf8("Times New Roman"));
        font.setPointSize(14);
        powerButton->setFont(font);

        controlLayout->addWidget(powerButton, 1, 6, 1, 1);

        elseButton = new QPushButton(Widget);
        elseButton->setObjectName(QString::fromUtf8("elseButton"));
        sizePolicy.setHeightForWidth(elseButton->sizePolicy().hasHeightForWidth());
        elseButton->setSizePolicy(sizePolicy);
        elseButton->setMaximumSize(QSize(60, 40));
        elseButton->setFont(font);

        controlLayout->addWidget(elseButton, 1, 15, 1, 1);

        gpsButton = new QPushButton(Widget);
        gpsButton->setObjectName(QString::fromUtf8("gpsButton"));
        sizePolicy.setHeightForWidth(gpsButton->sizePolicy().hasHeightForWidth());
        gpsButton->setSizePolicy(sizePolicy);
        gpsButton->setMaximumSize(QSize(16777215, 40));
        gpsButton->setFont(font);

        controlLayout->addWidget(gpsButton, 1, 7, 1, 1);

        energyButton = new QPushButton(Widget);
        energyButton->setObjectName(QString::fromUtf8("energyButton"));
        sizePolicy.setHeightForWidth(energyButton->sizePolicy().hasHeightForWidth());
        energyButton->setSizePolicy(sizePolicy);
        energyButton->setMaximumSize(QSize(16777215, 40));
        energyButton->setFont(font);

        controlLayout->addWidget(energyButton, 1, 9, 1, 1);

        nodeIDBox = new QSpinBox(Widget);
        nodeIDBox->setObjectName(QString::fromUtf8("nodeIDBox"));
        sizePolicy.setHeightForWidth(nodeIDBox->sizePolicy().hasHeightForWidth());
        nodeIDBox->setSizePolicy(sizePolicy);
        nodeIDBox->setMinimumSize(QSize(50, 40));
        nodeIDBox->setMaximumSize(QSize(50, 40));
        nodeIDBox->setFont(font);
        nodeIDBox->setAlignment(Qt::AlignCenter);
        nodeIDBox->setMaximum(5);
        nodeIDBox->setSingleStep(1);
        nodeIDBox->setStepType(QAbstractSpinBox::DefaultStepType);

        controlLayout->addWidget(nodeIDBox, 1, 2, 1, 1);

        configButton = new QPushButton(Widget);
        configButton->setObjectName(QString::fromUtf8("configButton"));
        sizePolicy.setHeightForWidth(configButton->sizePolicy().hasHeightForWidth());
        configButton->setSizePolicy(sizePolicy);
        configButton->setMaximumSize(QSize(16777215, 40));
        configButton->setFont(font);

        controlLayout->addWidget(configButton, 1, 8, 1, 1);

        rebootButton = new QPushButton(Widget);
        rebootButton->setObjectName(QString::fromUtf8("rebootButton"));
        sizePolicy.setHeightForWidth(rebootButton->sizePolicy().hasHeightForWidth());
        rebootButton->setSizePolicy(sizePolicy);
        rebootButton->setMaximumSize(QSize(60, 40));
        rebootButton->setFont(font);

        controlLayout->addWidget(rebootButton, 1, 14, 1, 1);

        horizontalSpacer = new QSpacerItem(70, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        controlLayout->addItem(horizontalSpacer, 1, 4, 1, 1);

        label = new QLabel(Widget);
        label->setObjectName(QString::fromUtf8("label"));
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        label->setMaximumSize(QSize(16777215, 80));
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        controlLayout->addWidget(label, 0, 2, 1, 13);

        geoButton = new QPushButton(Widget);
        geoButton->setObjectName(QString::fromUtf8("geoButton"));
        sizePolicy.setHeightForWidth(geoButton->sizePolicy().hasHeightForWidth());
        geoButton->setSizePolicy(sizePolicy);
        geoButton->setMaximumSize(QSize(16777215, 40));
        geoButton->setFont(font);

        controlLayout->addWidget(geoButton, 1, 12, 1, 1);

        nodeIDText = new QLabel(Widget);
        nodeIDText->setObjectName(QString::fromUtf8("nodeIDText"));
        sizePolicy.setHeightForWidth(nodeIDText->sizePolicy().hasHeightForWidth());
        nodeIDText->setSizePolicy(sizePolicy);
        nodeIDText->setMinimumSize(QSize(70, 40));
        nodeIDText->setMaximumSize(QSize(70, 40));
        nodeIDText->setFont(font);
        nodeIDText->setAlignment(Qt::AlignCenter);

        controlLayout->addWidget(nodeIDText, 1, 1, 1, 1);

        energyratioButton = new QPushButton(Widget);
        energyratioButton->setObjectName(QString::fromUtf8("energyratioButton"));
        sizePolicy.setHeightForWidth(energyratioButton->sizePolicy().hasHeightForWidth());
        energyratioButton->setSizePolicy(sizePolicy);
        energyratioButton->setMaximumSize(QSize(16777215, 40));
        energyratioButton->setFont(font);

        controlLayout->addWidget(energyratioButton, 1, 11, 1, 1);

        pulseButton = new QPushButton(Widget);
        pulseButton->setObjectName(QString::fromUtf8("pulseButton"));
        sizePolicy.setHeightForWidth(pulseButton->sizePolicy().hasHeightForWidth());
        pulseButton->setSizePolicy(sizePolicy);
        pulseButton->setMaximumSize(QSize(16777215, 40));
        pulseButton->setFont(font);

        controlLayout->addWidget(pulseButton, 1, 13, 1, 1);

        startButton = new QPushButton(Widget);
        startButton->setObjectName(QString::fromUtf8("startButton"));
        sizePolicy.setHeightForWidth(startButton->sizePolicy().hasHeightForWidth());
        startButton->setSizePolicy(sizePolicy);
        startButton->setMaximumSize(QSize(16777215, 40));
        startButton->setFont(font);

        controlLayout->addWidget(startButton, 1, 5, 1, 1);

        energythresButton = new QPushButton(Widget);
        energythresButton->setObjectName(QString::fromUtf8("energythresButton"));
        sizePolicy.setHeightForWidth(energythresButton->sizePolicy().hasHeightForWidth());
        energythresButton->setSizePolicy(sizePolicy);
        energythresButton->setMaximumSize(QSize(16777215, 40));
        energythresButton->setFont(font);

        controlLayout->addWidget(energythresButton, 1, 10, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(70, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        controlLayout->addItem(horizontalSpacer_2, 1, 0, 1, 1);


        gridLayout_3->addLayout(controlLayout, 0, 0, 1, 4);

        trackView = new QGraphicsView(Widget);
        trackView->setObjectName(QString::fromUtf8("trackView"));
        trackView->setMaximumSize(QSize(2500, 1800));

        gridLayout_3->addWidget(trackView, 1, 1, 1, 3);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setSizeConstraint(QLayout::SetMaximumSize);
        gridLayout_2->setHorizontalSpacing(30);
        node0Layout = new QVBoxLayout();
        node0Layout->setObjectName(QString::fromUtf8("node0Layout"));
        node0Label = new QLabel(Widget);
        node0Label->setObjectName(QString::fromUtf8("node0Label"));
        sizePolicy.setHeightForWidth(node0Label->sizePolicy().hasHeightForWidth());
        node0Label->setSizePolicy(sizePolicy);
        node0Label->setMaximumSize(QSize(16777215, 45));
        node0Label->setFont(font);
        node0Label->setAlignment(Qt::AlignCenter);

        node0Layout->addWidget(node0Label);

        node0Button = new QPushButton(Widget);
        node0Button->setObjectName(QString::fromUtf8("node0Button"));
        sizePolicy.setHeightForWidth(node0Button->sizePolicy().hasHeightForWidth());
        node0Button->setSizePolicy(sizePolicy);
        node0Button->setMaximumSize(QSize(16777215, 60));

        node0Layout->addWidget(node0Button);

        node0Layout->setStretch(0, 1);
        node0Layout->setStretch(1, 1);

        gridLayout_2->addLayout(node0Layout, 0, 0, 1, 1);

        node1Layout = new QVBoxLayout();
        node1Layout->setObjectName(QString::fromUtf8("node1Layout"));
        node1Label = new QLabel(Widget);
        node1Label->setObjectName(QString::fromUtf8("node1Label"));
        sizePolicy.setHeightForWidth(node1Label->sizePolicy().hasHeightForWidth());
        node1Label->setSizePolicy(sizePolicy);
        node1Label->setMaximumSize(QSize(16777215, 45));
        node1Label->setFont(font);
        node1Label->setAlignment(Qt::AlignCenter);

        node1Layout->addWidget(node1Label);

        node1Button = new QPushButton(Widget);
        node1Button->setObjectName(QString::fromUtf8("node1Button"));
        sizePolicy.setHeightForWidth(node1Button->sizePolicy().hasHeightForWidth());
        node1Button->setSizePolicy(sizePolicy);
        node1Button->setMaximumSize(QSize(16777215, 60));

        node1Layout->addWidget(node1Button);

        node1Layout->setStretch(0, 1);
        node1Layout->setStretch(1, 1);

        gridLayout_2->addLayout(node1Layout, 0, 1, 1, 1);

        node2Layout = new QVBoxLayout();
        node2Layout->setObjectName(QString::fromUtf8("node2Layout"));
        node2Label = new QLabel(Widget);
        node2Label->setObjectName(QString::fromUtf8("node2Label"));
        sizePolicy.setHeightForWidth(node2Label->sizePolicy().hasHeightForWidth());
        node2Label->setSizePolicy(sizePolicy);
        node2Label->setMaximumSize(QSize(16777215, 45));
        node2Label->setFont(font);
        node2Label->setAlignment(Qt::AlignCenter);

        node2Layout->addWidget(node2Label);

        node2Button = new QPushButton(Widget);
        node2Button->setObjectName(QString::fromUtf8("node2Button"));
        sizePolicy.setHeightForWidth(node2Button->sizePolicy().hasHeightForWidth());
        node2Button->setSizePolicy(sizePolicy);
        node2Button->setMaximumSize(QSize(16777215, 60));

        node2Layout->addWidget(node2Button);

        node2Layout->setStretch(0, 1);
        node2Layout->setStretch(1, 1);

        gridLayout_2->addLayout(node2Layout, 0, 2, 1, 1);

        node3Layout = new QVBoxLayout();
        node3Layout->setObjectName(QString::fromUtf8("node3Layout"));
        node3Label = new QLabel(Widget);
        node3Label->setObjectName(QString::fromUtf8("node3Label"));
        sizePolicy.setHeightForWidth(node3Label->sizePolicy().hasHeightForWidth());
        node3Label->setSizePolicy(sizePolicy);
        node3Label->setMaximumSize(QSize(16777215, 45));
        node3Label->setFont(font);
        node3Label->setAlignment(Qt::AlignCenter);

        node3Layout->addWidget(node3Label);

        node3Button = new QPushButton(Widget);
        node3Button->setObjectName(QString::fromUtf8("node3Button"));
        sizePolicy.setHeightForWidth(node3Button->sizePolicy().hasHeightForWidth());
        node3Button->setSizePolicy(sizePolicy);
        node3Button->setMaximumSize(QSize(16777215, 60));

        node3Layout->addWidget(node3Button);

        node3Layout->setStretch(0, 1);
        node3Layout->setStretch(1, 1);

        gridLayout_2->addLayout(node3Layout, 0, 3, 1, 1);

        node4Layout = new QVBoxLayout();
        node4Layout->setObjectName(QString::fromUtf8("node4Layout"));
        node4Label = new QLabel(Widget);
        node4Label->setObjectName(QString::fromUtf8("node4Label"));
        sizePolicy.setHeightForWidth(node4Label->sizePolicy().hasHeightForWidth());
        node4Label->setSizePolicy(sizePolicy);
        node4Label->setMaximumSize(QSize(16777215, 45));
        node4Label->setFont(font);
        node4Label->setAlignment(Qt::AlignCenter);

        node4Layout->addWidget(node4Label);

        node4Button = new QPushButton(Widget);
        node4Button->setObjectName(QString::fromUtf8("node4Button"));
        sizePolicy.setHeightForWidth(node4Button->sizePolicy().hasHeightForWidth());
        node4Button->setSizePolicy(sizePolicy);
        node4Button->setMaximumSize(QSize(16777215, 60));

        node4Layout->addWidget(node4Button);

        node4Layout->setStretch(0, 1);
        node4Layout->setStretch(1, 1);

        gridLayout_2->addLayout(node4Layout, 0, 4, 1, 1);

        node5Layout = new QVBoxLayout();
        node5Layout->setObjectName(QString::fromUtf8("node5Layout"));
        node5Label = new QLabel(Widget);
        node5Label->setObjectName(QString::fromUtf8("node5Label"));
        sizePolicy.setHeightForWidth(node5Label->sizePolicy().hasHeightForWidth());
        node5Label->setSizePolicy(sizePolicy);
        node5Label->setMaximumSize(QSize(16777215, 45));
        node5Label->setFont(font);
        node5Label->setAlignment(Qt::AlignCenter);

        node5Layout->addWidget(node5Label);

        node5Button = new QPushButton(Widget);
        node5Button->setObjectName(QString::fromUtf8("node5Button"));
        sizePolicy.setHeightForWidth(node5Button->sizePolicy().hasHeightForWidth());
        node5Button->setSizePolicy(sizePolicy);
        node5Button->setMaximumSize(QSize(16777215, 60));

        node5Layout->addWidget(node5Button);

        node5Layout->setStretch(0, 1);
        node5Layout->setStretch(1, 1);

        gridLayout_2->addLayout(node5Layout, 0, 5, 1, 1);


        gridLayout_3->addLayout(gridLayout_2, 2, 0, 1, 2);

        trackText = new QTextBrowser(Widget);
        trackText->setObjectName(QString::fromUtf8("trackText"));
        trackText->setMaximumSize(QSize(740, 125));

        gridLayout_3->addWidget(trackText, 2, 3, 1, 1);

        logText = new QTextBrowser(Widget);
        logText->setObjectName(QString::fromUtf8("logText"));
        logText->setMinimumSize(QSize(400, 900));
        logText->setMaximumSize(QSize(400, 900));

        gridLayout_3->addWidget(logText, 1, 0, 1, 1);


        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        powerButton->setText(QCoreApplication::translate("Widget", "\346\237\245\350\257\242\347\224\265\351\207\217", nullptr));
        elseButton->setText(QCoreApplication::translate("Widget", "\345\205\266\344\273\226", nullptr));
        gpsButton->setText(QCoreApplication::translate("Widget", "\346\237\245\350\257\242GPS", nullptr));
        energyButton->setText(QCoreApplication::translate("Widget", "\350\216\267\345\217\226\347\233\256\346\240\207\351\242\221\347\202\271\350\203\275\351\207\217", nullptr));
        configButton->setText(QCoreApplication::translate("Widget", "\346\237\245\350\257\242\351\205\215\347\275\256", nullptr));
        rebootButton->setText(QCoreApplication::translate("Widget", "\351\207\215\345\220\257", nullptr));
        label->setText(QCoreApplication::translate("Widget", "\346\216\247\345\210\266\346\240\217", nullptr));
        geoButton->setText(QCoreApplication::translate("Widget", "\350\216\267\345\217\226\347\273\217\347\272\254\345\272\246", nullptr));
        nodeIDText->setText(QCoreApplication::translate("Widget", "\350\212\202\347\202\271ID", nullptr));
        energyratioButton->setText(QCoreApplication::translate("Widget", "\350\256\276\347\275\256\346\257\224\345\200\274\351\230\210\345\200\274", nullptr));
        pulseButton->setText(QCoreApplication::translate("Widget", "\350\216\267\345\217\226\350\204\211\345\206\262\344\277\241\345\217\267", nullptr));
        startButton->setText(QCoreApplication::translate("Widget", "\345\274\200\345\247\213\345\267\245\344\275\234", nullptr));
        energythresButton->setText(QCoreApplication::translate("Widget", "\350\256\276\347\275\256\350\203\275\351\207\217\351\230\210\345\200\274", nullptr));
        node0Label->setText(QCoreApplication::translate("Widget", "\350\212\202\347\202\2710", nullptr));
        node0Button->setText(QCoreApplication::translate("Widget", "\350\277\236\346\216\245", nullptr));
        node1Label->setText(QCoreApplication::translate("Widget", "\350\212\202\347\202\2711", nullptr));
        node1Button->setText(QCoreApplication::translate("Widget", "\350\277\236\346\216\245", nullptr));
        node2Label->setText(QCoreApplication::translate("Widget", "\350\212\202\347\202\2712", nullptr));
        node2Button->setText(QCoreApplication::translate("Widget", "\350\277\236\346\216\245", nullptr));
        node3Label->setText(QCoreApplication::translate("Widget", "\350\212\202\347\202\2713", nullptr));
        node3Button->setText(QCoreApplication::translate("Widget", "\350\277\236\346\216\245", nullptr));
        node4Label->setText(QCoreApplication::translate("Widget", "\350\212\202\347\202\2714", nullptr));
        node4Button->setText(QCoreApplication::translate("Widget", "\350\277\236\346\216\245", nullptr));
        node5Label->setText(QCoreApplication::translate("Widget", "\350\212\202\347\202\2715", nullptr));
        node5Button->setText(QCoreApplication::translate("Widget", "\350\277\236\346\216\245", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H

/********************************************************************************
** Form generated from reading UI file 'motor.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef MOTOR_UI_H
#define MOTOR_UI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDial>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QLabel *label;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QDial *dial;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QStringLiteral("Dialog"));
        Dialog->resize(644, 437);
        label = new QLabel(Dialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 70, 211, 71));
        QFont font;
        font.setFamily(QStringLiteral("Agency FB"));
        font.setPointSize(24);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        pushButton = new QPushButton(Dialog);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(10, 170, 181, 81));
        QFont font1;
        font1.setFamily(QStringLiteral("Agency FB"));
        font1.setPointSize(24);
        pushButton->setFont(font1);
        pushButton_2 = new QPushButton(Dialog);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(10, 260, 181, 81));
        QFont font2;
        font2.setFamily(QStringLiteral("Agency FB"));
        font2.setPointSize(28);
        pushButton_2->setFont(font2);
        dial = new QDial(Dialog);
        dial->setObjectName(QStringLiteral("dial"));
        dial->setGeometry(QRect(260, 60, 331, 321));
        dial->setMinimum(0);
        dial->setMaximum(360);
        dial->setSingleStep(3);
        dial->setPageStep(10);
        dial->setValue(180);
        dial->setSliderPosition(180);
        dial->setOrientation(Qt::Horizontal);
        dial->setInvertedAppearance(false);
        dial->setInvertedControls(false);
        dial->setWrapping(true);
        dial->setNotchTarget(4);
        dial->setNotchesVisible(true);

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", Q_NULLPTR));
        label->setText(QApplication::translate("Dialog", "\346\265\213\351\243\216\346\255\245\350\277\233\347\224\265\346\234\272", Q_NULLPTR));
        pushButton->setText(QApplication::translate("Dialog", "\345\233\233\346\226\271\344\275\215\346\211\253\346\217\217", Q_NULLPTR));
        pushButton_2->setText(QApplication::translate("Dialog", "\346\211\276\345\214\227\345\275\222\344\275\215", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // MOTOR_UI_H

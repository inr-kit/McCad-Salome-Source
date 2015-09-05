/********************************************************************************
** Form generated from reading UI file 'ImportGEOMObjDialog.ui'
**
** Created: Mon Dec 2 13:28:36 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMPORTGEOMOBJDIALOG_H
#define UI_IMPORTGEOMOBJDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_MCCADGUI_ImportGEOMObjDialog
{
public:
    QLabel *label;
    QLabel *label_2;
    QLabel *lb_geom_name;
    QLabel *lb_mccad_name;
    QLabel *label_5;
    QPushButton *Btn_ok;
    QPushButton *Btn_cancel;

    void setupUi(QDialog *MCCADGUI_ImportGEOMObjDialog)
    {
        if (MCCADGUI_ImportGEOMObjDialog->objectName().isEmpty())
            MCCADGUI_ImportGEOMObjDialog->setObjectName(QString::fromUtf8("MCCADGUI_ImportGEOMObjDialog"));
        MCCADGUI_ImportGEOMObjDialog->resize(349, 184);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MCCADGUI_ImportGEOMObjDialog->sizePolicy().hasHeightForWidth());
        MCCADGUI_ImportGEOMObjDialog->setSizePolicy(sizePolicy);
        MCCADGUI_ImportGEOMObjDialog->setMinimumSize(QSize(0, 0));
        label = new QLabel(MCCADGUI_ImportGEOMObjDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(40, 30, 291, 41));
        QFont font;
        font.setPointSize(11);
        label->setFont(font);
        label_2 = new QLabel(MCCADGUI_ImportGEOMObjDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(40, 80, 62, 17));
        lb_geom_name = new QLabel(MCCADGUI_ImportGEOMObjDialog);
        lb_geom_name->setObjectName(QString::fromUtf8("lb_geom_name"));
        lb_geom_name->setGeometry(QRect(120, 80, 62, 17));
        QFont font1;
        font1.setItalic(true);
        lb_geom_name->setFont(font1);
        lb_mccad_name = new QLabel(MCCADGUI_ImportGEOMObjDialog);
        lb_mccad_name->setObjectName(QString::fromUtf8("lb_mccad_name"));
        lb_mccad_name->setGeometry(QRect(120, 100, 111, 17));
        lb_mccad_name->setFont(font1);
        label_5 = new QLabel(MCCADGUI_ImportGEOMObjDialog);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(40, 100, 62, 17));
        Btn_ok = new QPushButton(MCCADGUI_ImportGEOMObjDialog);
        Btn_ok->setObjectName(QString::fromUtf8("Btn_ok"));
        Btn_ok->setGeometry(QRect(40, 130, 93, 27));
        Btn_cancel = new QPushButton(MCCADGUI_ImportGEOMObjDialog);
        Btn_cancel->setObjectName(QString::fromUtf8("Btn_cancel"));
        Btn_cancel->setGeometry(QRect(180, 130, 93, 27));

        retranslateUi(MCCADGUI_ImportGEOMObjDialog);

        QMetaObject::connectSlotsByName(MCCADGUI_ImportGEOMObjDialog);
    } // setupUi

    void retranslateUi(QDialog *MCCADGUI_ImportGEOMObjDialog)
    {
        MCCADGUI_ImportGEOMObjDialog->setWindowTitle(QApplication::translate("MCCADGUI_ImportGEOMObjDialog", "Averaging Mesh", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MCCADGUI_ImportGEOMObjDialog", "Please select a group or a part to assign\n"
"the geometry:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MCCADGUI_ImportGEOMObjDialog", "From:", 0, QApplication::UnicodeUTF8));
        lb_geom_name->setText(QApplication::translate("MCCADGUI_ImportGEOMObjDialog", "No name", 0, QApplication::UnicodeUTF8));
        lb_mccad_name->setText(QApplication::translate("MCCADGUI_ImportGEOMObjDialog", "No selected", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MCCADGUI_ImportGEOMObjDialog", "To:", 0, QApplication::UnicodeUTF8));
        Btn_ok->setText(QApplication::translate("MCCADGUI_ImportGEOMObjDialog", "OK", 0, QApplication::UnicodeUTF8));
        Btn_cancel->setText(QApplication::translate("MCCADGUI_ImportGEOMObjDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MCCADGUI_ImportGEOMObjDialog: public Ui_MCCADGUI_ImportGEOMObjDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMPORTGEOMOBJDIALOG_H

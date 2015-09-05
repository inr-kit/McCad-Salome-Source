/********************************************************************************
** Form generated from reading UI file 'ImportObjDialog.ui'
**
** Created: Tue Apr 28 15:52:26 2015
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMPORTOBJDIALOG_H
#define UI_IMPORTOBJDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_MCCADGUI_ImportObjDialog
{
public:
    QLabel *label;
    QLabel *label_2;
    QLabel *lb_geom_name;
    QLabel *lb_mccad_name;
    QLabel *label_5;
    QPushButton *Btn_ok;
    QPushButton *Btn_cancel;

    void setupUi(QDialog *MCCADGUI_ImportObjDialog)
    {
        if (MCCADGUI_ImportObjDialog->objectName().isEmpty())
            MCCADGUI_ImportObjDialog->setObjectName(QString::fromUtf8("MCCADGUI_ImportObjDialog"));
        MCCADGUI_ImportObjDialog->resize(349, 184);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MCCADGUI_ImportObjDialog->sizePolicy().hasHeightForWidth());
        MCCADGUI_ImportObjDialog->setSizePolicy(sizePolicy);
        MCCADGUI_ImportObjDialog->setMinimumSize(QSize(0, 0));
        label = new QLabel(MCCADGUI_ImportObjDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(40, 30, 291, 41));
        QFont font;
        font.setPointSize(11);
        label->setFont(font);
        label_2 = new QLabel(MCCADGUI_ImportObjDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(40, 80, 62, 17));
        lb_geom_name = new QLabel(MCCADGUI_ImportObjDialog);
        lb_geom_name->setObjectName(QString::fromUtf8("lb_geom_name"));
        lb_geom_name->setGeometry(QRect(120, 80, 62, 17));
        QFont font1;
        font1.setItalic(true);
        lb_geom_name->setFont(font1);
        lb_mccad_name = new QLabel(MCCADGUI_ImportObjDialog);
        lb_mccad_name->setObjectName(QString::fromUtf8("lb_mccad_name"));
        lb_mccad_name->setGeometry(QRect(120, 100, 111, 17));
        lb_mccad_name->setFont(font1);
        label_5 = new QLabel(MCCADGUI_ImportObjDialog);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(40, 100, 62, 17));
        Btn_ok = new QPushButton(MCCADGUI_ImportObjDialog);
        Btn_ok->setObjectName(QString::fromUtf8("Btn_ok"));
        Btn_ok->setGeometry(QRect(40, 130, 93, 27));
        Btn_cancel = new QPushButton(MCCADGUI_ImportObjDialog);
        Btn_cancel->setObjectName(QString::fromUtf8("Btn_cancel"));
        Btn_cancel->setGeometry(QRect(180, 130, 93, 27));

        retranslateUi(MCCADGUI_ImportObjDialog);

        QMetaObject::connectSlotsByName(MCCADGUI_ImportObjDialog);
    } // setupUi

    void retranslateUi(QDialog *MCCADGUI_ImportObjDialog)
    {
        MCCADGUI_ImportObjDialog->setWindowTitle(QApplication::translate("MCCADGUI_ImportObjDialog", "Import objects", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MCCADGUI_ImportObjDialog", "Please select a group or a part to assign\n"
"the geometry/mesh:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MCCADGUI_ImportObjDialog", "From:", 0, QApplication::UnicodeUTF8));
        lb_geom_name->setText(QApplication::translate("MCCADGUI_ImportObjDialog", "No name", 0, QApplication::UnicodeUTF8));
        lb_mccad_name->setText(QApplication::translate("MCCADGUI_ImportObjDialog", "No selected", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MCCADGUI_ImportObjDialog", "To:", 0, QApplication::UnicodeUTF8));
        Btn_ok->setText(QApplication::translate("MCCADGUI_ImportObjDialog", "OK", 0, QApplication::UnicodeUTF8));
        Btn_cancel->setText(QApplication::translate("MCCADGUI_ImportObjDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MCCADGUI_ImportObjDialog: public Ui_MCCADGUI_ImportObjDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMPORTOBJDIALOG_H

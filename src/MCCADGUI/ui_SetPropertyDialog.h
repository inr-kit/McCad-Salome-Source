/********************************************************************************
** Form generated from reading UI file 'SetPropertyDialog.ui'
**
** Created: Tue Apr 28 15:52:26 2015
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETPROPERTYDIALOG_H
#define UI_SETPROPERTYDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_MCCADGUI_SetPropertyDialog
{
public:
    QGroupBox *gb_Material;
    QLabel *label;
    QComboBox *cmb_mat_id;
    QCheckBox *chk_recolor;
    QPushButton *Btn_edit_mat;
    QGroupBox *gb_Imp;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLineEdit *le_imp_n;
    QLineEdit *le_imp_p;
    QLineEdit *le_imp_e;
    QPushButton *Btn_apply;
    QPushButton *Btn_close;
    QGroupBox *gb_Remark;
    QTextEdit *tb_remark;
    QGroupBox *gb_Additive;
    QTextEdit *tb_add_card;
    QGroupBox *gb_Graphic;
    QPushButton *Btn_color;
    QLabel *label_7;

    void setupUi(QDialog *MCCADGUI_SetPropertyDialog)
    {
        if (MCCADGUI_SetPropertyDialog->objectName().isEmpty())
            MCCADGUI_SetPropertyDialog->setObjectName(QString::fromUtf8("MCCADGUI_SetPropertyDialog"));
        MCCADGUI_SetPropertyDialog->resize(250, 642);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MCCADGUI_SetPropertyDialog->sizePolicy().hasHeightForWidth());
        MCCADGUI_SetPropertyDialog->setSizePolicy(sizePolicy);
        MCCADGUI_SetPropertyDialog->setMinimumSize(QSize(220, 10));
        MCCADGUI_SetPropertyDialog->setBaseSize(QSize(0, 0));
        gb_Material = new QGroupBox(MCCADGUI_SetPropertyDialog);
        gb_Material->setObjectName(QString::fromUtf8("gb_Material"));
        gb_Material->setEnabled(true);
        gb_Material->setGeometry(QRect(20, 20, 180, 131));
        gb_Material->setCheckable(true);
        gb_Material->setChecked(false);
        label = new QLabel(gb_Material);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 35, 41, 21));
        QFont font;
        font.setPointSize(11);
        label->setFont(font);
        cmb_mat_id = new QComboBox(gb_Material);
        cmb_mat_id->setObjectName(QString::fromUtf8("cmb_mat_id"));
        cmb_mat_id->setGeometry(QRect(40, 30, 130, 27));
        chk_recolor = new QCheckBox(gb_Material);
        chk_recolor->setObjectName(QString::fromUtf8("chk_recolor"));
        chk_recolor->setGeometry(QRect(40, 60, 141, 22));
        Btn_edit_mat = new QPushButton(gb_Material);
        Btn_edit_mat->setObjectName(QString::fromUtf8("Btn_edit_mat"));
        Btn_edit_mat->setGeometry(QRect(40, 90, 130, 27));
        gb_Imp = new QGroupBox(MCCADGUI_SetPropertyDialog);
        gb_Imp->setObjectName(QString::fromUtf8("gb_Imp"));
        gb_Imp->setGeometry(QRect(20, 160, 180, 131));
        gb_Imp->setCheckable(true);
        gb_Imp->setChecked(false);
        label_2 = new QLabel(gb_Imp);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 30, 41, 21));
        label_2->setFont(font);
        label_3 = new QLabel(gb_Imp);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 60, 41, 31));
        label_3->setFont(font);
        label_4 = new QLabel(gb_Imp);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 90, 41, 21));
        label_4->setFont(font);
        le_imp_n = new QLineEdit(gb_Imp);
        le_imp_n->setObjectName(QString::fromUtf8("le_imp_n"));
        le_imp_n->setGeometry(QRect(40, 30, 130, 24));
        le_imp_p = new QLineEdit(gb_Imp);
        le_imp_p->setObjectName(QString::fromUtf8("le_imp_p"));
        le_imp_p->setGeometry(QRect(40, 60, 130, 24));
        le_imp_e = new QLineEdit(gb_Imp);
        le_imp_e->setObjectName(QString::fromUtf8("le_imp_e"));
        le_imp_e->setGeometry(QRect(40, 90, 130, 24));
        Btn_apply = new QPushButton(MCCADGUI_SetPropertyDialog);
        Btn_apply->setObjectName(QString::fromUtf8("Btn_apply"));
        Btn_apply->setGeometry(QRect(30, 590, 80, 27));
        Btn_close = new QPushButton(MCCADGUI_SetPropertyDialog);
        Btn_close->setObjectName(QString::fromUtf8("Btn_close"));
        Btn_close->setGeometry(QRect(130, 590, 80, 27));
        gb_Remark = new QGroupBox(MCCADGUI_SetPropertyDialog);
        gb_Remark->setObjectName(QString::fromUtf8("gb_Remark"));
        gb_Remark->setGeometry(QRect(20, 360, 201, 111));
        gb_Remark->setCheckable(true);
        gb_Remark->setChecked(false);
        tb_remark = new QTextEdit(gb_Remark);
        tb_remark->setObjectName(QString::fromUtf8("tb_remark"));
        tb_remark->setGeometry(QRect(10, 30, 181, 71));
        gb_Additive = new QGroupBox(MCCADGUI_SetPropertyDialog);
        gb_Additive->setObjectName(QString::fromUtf8("gb_Additive"));
        gb_Additive->setGeometry(QRect(20, 480, 201, 101));
        gb_Additive->setCheckable(true);
        gb_Additive->setChecked(false);
        tb_add_card = new QTextEdit(gb_Additive);
        tb_add_card->setObjectName(QString::fromUtf8("tb_add_card"));
        tb_add_card->setGeometry(QRect(10, 20, 181, 71));
        gb_Graphic = new QGroupBox(MCCADGUI_SetPropertyDialog);
        gb_Graphic->setObjectName(QString::fromUtf8("gb_Graphic"));
        gb_Graphic->setGeometry(QRect(20, 290, 181, 61));
        gb_Graphic->setCheckable(true);
        gb_Graphic->setChecked(false);
        Btn_color = new QPushButton(gb_Graphic);
        Btn_color->setObjectName(QString::fromUtf8("Btn_color"));
        Btn_color->setGeometry(QRect(70, 30, 81, 27));
        Btn_color->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"background:rgb(255, 170, 0)\n"
"}"));
        label_7 = new QLabel(gb_Graphic);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(20, 30, 61, 31));
        label_7->setFont(font);

        retranslateUi(MCCADGUI_SetPropertyDialog);

        QMetaObject::connectSlotsByName(MCCADGUI_SetPropertyDialog);
    } // setupUi

    void retranslateUi(QDialog *MCCADGUI_SetPropertyDialog)
    {
        MCCADGUI_SetPropertyDialog->setWindowTitle(QApplication::translate("MCCADGUI_SetPropertyDialog", "Properties", 0, QApplication::UnicodeUTF8));
        gb_Material->setTitle(QApplication::translate("MCCADGUI_SetPropertyDialog", "Material", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MCCADGUI_SetPropertyDialog", "ID:", 0, QApplication::UnicodeUTF8));
        chk_recolor->setText(QApplication::translate("MCCADGUI_SetPropertyDialog", "Replace Color", 0, QApplication::UnicodeUTF8));
        Btn_edit_mat->setText(QApplication::translate("MCCADGUI_SetPropertyDialog", "Edit Material", 0, QApplication::UnicodeUTF8));
        gb_Imp->setTitle(QApplication::translate("MCCADGUI_SetPropertyDialog", "Importance", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MCCADGUI_SetPropertyDialog", "N:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MCCADGUI_SetPropertyDialog", "P:", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MCCADGUI_SetPropertyDialog", "E:", 0, QApplication::UnicodeUTF8));
        Btn_apply->setText(QApplication::translate("MCCADGUI_SetPropertyDialog", "Apply", 0, QApplication::UnicodeUTF8));
        Btn_close->setText(QApplication::translate("MCCADGUI_SetPropertyDialog", "Close", 0, QApplication::UnicodeUTF8));
        gb_Remark->setTitle(QApplication::translate("MCCADGUI_SetPropertyDialog", "Remark", 0, QApplication::UnicodeUTF8));
        gb_Additive->setTitle(QApplication::translate("MCCADGUI_SetPropertyDialog", "Additive Cards:", 0, QApplication::UnicodeUTF8));
        gb_Graphic->setTitle(QApplication::translate("MCCADGUI_SetPropertyDialog", "Graphic", 0, QApplication::UnicodeUTF8));
        Btn_color->setText(QString());
        label_7->setText(QApplication::translate("MCCADGUI_SetPropertyDialog", "Color:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MCCADGUI_SetPropertyDialog: public Ui_MCCADGUI_SetPropertyDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETPROPERTYDIALOG_H

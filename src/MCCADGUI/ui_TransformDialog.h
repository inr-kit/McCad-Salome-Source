/********************************************************************************
** Form generated from reading UI file 'TransformDialog.ui'
**
** Created: Tue Apr 28 15:52:26 2015
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRANSFORMDIALOG_H
#define UI_TRANSFORMDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_MCCADGUI_TransformDialog
{
public:
    QGroupBox *gb_tsl;
    QLabel *label_2;
    QLineEdit *le_tsl_x;
    QLineEdit *le_tsl_y;
    QLineEdit *le_tsl_z;
    QPushButton *Btn_apply;
    QPushButton *Btn_close;
    QGroupBox *gb_scl;
    QLabel *label_10;
    QLineEdit *le_scl_ft;
    QGroupBox *gb_rt;
    QLabel *label_7;
    QLineEdit *le_rt_org_z;
    QLineEdit *le_rt_org_y;
    QLineEdit *le_rt_org_x;
    QLineEdit *le_rt_dir_y;
    QLineEdit *le_rt_dir_z;
    QLineEdit *le_rt_dir_x;
    QLabel *label_8;
    QLabel *label_9;
    QLineEdit *le_rt_ang;

    void setupUi(QDialog *MCCADGUI_TransformDialog)
    {
        if (MCCADGUI_TransformDialog->objectName().isEmpty())
            MCCADGUI_TransformDialog->setObjectName(QString::fromUtf8("MCCADGUI_TransformDialog"));
        MCCADGUI_TransformDialog->resize(416, 368);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MCCADGUI_TransformDialog->sizePolicy().hasHeightForWidth());
        MCCADGUI_TransformDialog->setSizePolicy(sizePolicy);
        MCCADGUI_TransformDialog->setMinimumSize(QSize(220, 10));
        MCCADGUI_TransformDialog->setBaseSize(QSize(0, 0));
        gb_tsl = new QGroupBox(MCCADGUI_TransformDialog);
        gb_tsl->setObjectName(QString::fromUtf8("gb_tsl"));
        gb_tsl->setGeometry(QRect(30, 30, 361, 61));
        gb_tsl->setCheckable(true);
        gb_tsl->setChecked(false);
        label_2 = new QLabel(gb_tsl);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(30, 30, 51, 21));
        QFont font;
        font.setPointSize(11);
        label_2->setFont(font);
        le_tsl_x = new QLineEdit(gb_tsl);
        le_tsl_x->setObjectName(QString::fromUtf8("le_tsl_x"));
        le_tsl_x->setGeometry(QRect(100, 30, 61, 24));
        le_tsl_y = new QLineEdit(gb_tsl);
        le_tsl_y->setObjectName(QString::fromUtf8("le_tsl_y"));
        le_tsl_y->setGeometry(QRect(180, 30, 61, 24));
        le_tsl_z = new QLineEdit(gb_tsl);
        le_tsl_z->setObjectName(QString::fromUtf8("le_tsl_z"));
        le_tsl_z->setGeometry(QRect(260, 30, 61, 24));
        Btn_apply = new QPushButton(MCCADGUI_TransformDialog);
        Btn_apply->setObjectName(QString::fromUtf8("Btn_apply"));
        Btn_apply->setGeometry(QRect(200, 320, 80, 27));
        Btn_close = new QPushButton(MCCADGUI_TransformDialog);
        Btn_close->setObjectName(QString::fromUtf8("Btn_close"));
        Btn_close->setGeometry(QRect(300, 320, 80, 27));
        gb_scl = new QGroupBox(MCCADGUI_TransformDialog);
        gb_scl->setObjectName(QString::fromUtf8("gb_scl"));
        gb_scl->setGeometry(QRect(30, 250, 361, 61));
        gb_scl->setCheckable(true);
        gb_scl->setChecked(false);
        label_10 = new QLabel(gb_scl);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(30, 30, 111, 31));
        label_10->setFont(font);
        le_scl_ft = new QLineEdit(gb_scl);
        le_scl_ft->setObjectName(QString::fromUtf8("le_scl_ft"));
        le_scl_ft->setGeometry(QRect(140, 30, 81, 24));
        gb_rt = new QGroupBox(MCCADGUI_TransformDialog);
        gb_rt->setObjectName(QString::fromUtf8("gb_rt"));
        gb_rt->setGeometry(QRect(30, 100, 361, 141));
        gb_rt->setCheckable(true);
        gb_rt->setChecked(false);
        label_7 = new QLabel(gb_rt);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(30, 30, 61, 31));
        label_7->setFont(font);
        le_rt_org_z = new QLineEdit(gb_rt);
        le_rt_org_z->setObjectName(QString::fromUtf8("le_rt_org_z"));
        le_rt_org_z->setGeometry(QRect(260, 30, 61, 24));
        le_rt_org_y = new QLineEdit(gb_rt);
        le_rt_org_y->setObjectName(QString::fromUtf8("le_rt_org_y"));
        le_rt_org_y->setGeometry(QRect(180, 30, 61, 24));
        le_rt_org_x = new QLineEdit(gb_rt);
        le_rt_org_x->setObjectName(QString::fromUtf8("le_rt_org_x"));
        le_rt_org_x->setGeometry(QRect(100, 30, 61, 24));
        le_rt_dir_y = new QLineEdit(gb_rt);
        le_rt_dir_y->setObjectName(QString::fromUtf8("le_rt_dir_y"));
        le_rt_dir_y->setGeometry(QRect(180, 70, 61, 24));
        le_rt_dir_z = new QLineEdit(gb_rt);
        le_rt_dir_z->setObjectName(QString::fromUtf8("le_rt_dir_z"));
        le_rt_dir_z->setGeometry(QRect(260, 70, 61, 24));
        le_rt_dir_x = new QLineEdit(gb_rt);
        le_rt_dir_x->setObjectName(QString::fromUtf8("le_rt_dir_x"));
        le_rt_dir_x->setGeometry(QRect(100, 70, 61, 24));
        label_8 = new QLabel(gb_rt);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(30, 70, 71, 31));
        label_8->setFont(font);
        label_9 = new QLabel(gb_rt);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(30, 110, 111, 31));
        label_9->setFont(font);
        le_rt_ang = new QLineEdit(gb_rt);
        le_rt_ang->setObjectName(QString::fromUtf8("le_rt_ang"));
        le_rt_ang->setGeometry(QRect(140, 110, 81, 24));

        retranslateUi(MCCADGUI_TransformDialog);

        QMetaObject::connectSlotsByName(MCCADGUI_TransformDialog);
    } // setupUi

    void retranslateUi(QDialog *MCCADGUI_TransformDialog)
    {
        MCCADGUI_TransformDialog->setWindowTitle(QApplication::translate("MCCADGUI_TransformDialog", "Transformation", 0, QApplication::UnicodeUTF8));
        gb_tsl->setTitle(QApplication::translate("MCCADGUI_TransformDialog", "Translation", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MCCADGUI_TransformDialog", "Vector", 0, QApplication::UnicodeUTF8));
        Btn_apply->setText(QApplication::translate("MCCADGUI_TransformDialog", "Apply", 0, QApplication::UnicodeUTF8));
        Btn_close->setText(QApplication::translate("MCCADGUI_TransformDialog", "Close", 0, QApplication::UnicodeUTF8));
        gb_scl->setTitle(QApplication::translate("MCCADGUI_TransformDialog", "Scaling", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("MCCADGUI_TransformDialog", "Factor", 0, QApplication::UnicodeUTF8));
        gb_rt->setTitle(QApplication::translate("MCCADGUI_TransformDialog", "Rotation", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MCCADGUI_TransformDialog", "Origin", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("MCCADGUI_TransformDialog", "Direction", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("MCCADGUI_TransformDialog", "Angle(Degree)", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MCCADGUI_TransformDialog: public Ui_MCCADGUI_TransformDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRANSFORMDIALOG_H

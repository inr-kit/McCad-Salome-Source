/********************************************************************************
** Form generated from reading UI file 'GenMeshDialog.ui'
**
** Created: Wed Feb 6 10:26:05 2013
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GENMESHDIALOG_H
#define UI_GENMESHDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_GenMeshDialog
{
public:
    QLineEdit *Gen_le_Y;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *Gen_le_X;
    QLineEdit *Gen_le_Z;
    QLabel *label_4;
    QLineEdit *Gen_le_name;

    void setupUi(QDialog *GenMeshDialog)
    {
        if (GenMeshDialog->objectName().isEmpty())
            GenMeshDialog->setObjectName(QString::fromUtf8("GenMeshDialog"));
        GenMeshDialog->resize(400, 230);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(GenMeshDialog->sizePolicy().hasHeightForWidth());
        GenMeshDialog->setSizePolicy(sizePolicy);
        GenMeshDialog->setMinimumSize(QSize(400, 230));
        Gen_le_Y = new QLineEdit(GenMeshDialog);
        Gen_le_Y->setObjectName(QString::fromUtf8("Gen_le_Y"));
        Gen_le_Y->setGeometry(QRect(110, 100, 261, 27));
        label = new QLabel(GenMeshDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 70, 81, 17));
        label_2 = new QLabel(GenMeshDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 110, 81, 17));
        label_3 = new QLabel(GenMeshDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 150, 81, 17));
        Gen_le_X = new QLineEdit(GenMeshDialog);
        Gen_le_X->setObjectName(QString::fromUtf8("Gen_le_X"));
        Gen_le_X->setGeometry(QRect(110, 60, 261, 27));
        Gen_le_Z = new QLineEdit(GenMeshDialog);
        Gen_le_Z->setObjectName(QString::fromUtf8("Gen_le_Z"));
        Gen_le_Z->setGeometry(QRect(110, 140, 261, 27));
        label_4 = new QLabel(GenMeshDialog);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(20, 30, 91, 17));
        Gen_le_name = new QLineEdit(GenMeshDialog);
        Gen_le_name->setObjectName(QString::fromUtf8("Gen_le_name"));
        Gen_le_name->setGeometry(QRect(110, 20, 113, 27));

        retranslateUi(GenMeshDialog);

        QMetaObject::connectSlotsByName(GenMeshDialog);
    } // setupUi

    void retranslateUi(QDialog *GenMeshDialog)
    {
        GenMeshDialog->setWindowTitle(QApplication::translate("GenMeshDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("GenMeshDialog", "X intervals:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("GenMeshDialog", "Y intervals:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("GenMeshDialog", "Z intervals:", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("GenMeshDialog", "Mesh Name:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class GenMeshDialog: public Ui_GenMeshDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GENMESHDIALOG_H

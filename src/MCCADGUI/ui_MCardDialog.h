/********************************************************************************
** Form generated from reading UI file 'MCardDialog.ui'
**
** Created: Tue Apr 28 15:52:26 2015
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MCARDDIALOG_H
#define UI_MCARDDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>
#include <QtGui/QTreeWidget>

QT_BEGIN_NAMESPACE

class Ui_MCCADGUI_MCardDialog
{
public:
    QTreeWidget *mMatTree;
    QLineEdit *IDEdit;
    QLineEdit *densityEdit;
    QLineEdit *nameEdit;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QTextEdit *mCardText;
    QPushButton *btnDelMat;
    QPushButton *loadXMLButton;
    QPushButton *saveXMLButton;
    QPushButton *applyButton;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QLabel *label_4;
    QPushButton *colorButton;
    QLabel *label_5;

    void setupUi(QDialog *MCCADGUI_MCardDialog)
    {
        if (MCCADGUI_MCardDialog->objectName().isEmpty())
            MCCADGUI_MCardDialog->setObjectName(QString::fromUtf8("MCCADGUI_MCardDialog"));
        MCCADGUI_MCardDialog->resize(895, 624);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MCCADGUI_MCardDialog->sizePolicy().hasHeightForWidth());
        MCCADGUI_MCardDialog->setSizePolicy(sizePolicy);
        MCCADGUI_MCardDialog->setMinimumSize(QSize(895, 624));
        MCCADGUI_MCardDialog->setBaseSize(QSize(0, 0));
        mMatTree = new QTreeWidget(MCCADGUI_MCardDialog);
        mMatTree->setObjectName(QString::fromUtf8("mMatTree"));
        mMatTree->setGeometry(QRect(10, 40, 180, 510));
        mMatTree->setHeaderHidden(false);
        mMatTree->setColumnCount(1);
        mMatTree->header()->setVisible(true);
        mMatTree->header()->setCascadingSectionResizes(false);
        mMatTree->header()->setProperty("showSortIndicator", QVariant(false));
        IDEdit = new QLineEdit(MCCADGUI_MCardDialog);
        IDEdit->setObjectName(QString::fromUtf8("IDEdit"));
        IDEdit->setGeometry(QRect(240, 40, 71, 27));
        densityEdit = new QLineEdit(MCCADGUI_MCardDialog);
        densityEdit->setObjectName(QString::fromUtf8("densityEdit"));
        densityEdit->setGeometry(QRect(390, 40, 91, 27));
        nameEdit = new QLineEdit(MCCADGUI_MCardDialog);
        nameEdit->setObjectName(QString::fromUtf8("nameEdit"));
        nameEdit->setGeometry(QRect(560, 40, 161, 27));
        label = new QLabel(MCCADGUI_MCardDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(210, 40, 31, 31));
        QFont font;
        font.setPointSize(10);
        label->setFont(font);
        label_2 = new QLabel(MCCADGUI_MCardDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(320, 40, 81, 31));
        label_2->setFont(font);
        label_3 = new QLabel(MCCADGUI_MCardDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(500, 40, 61, 31));
        label_3->setFont(font);
        mCardText = new QTextEdit(MCCADGUI_MCardDialog);
        mCardText->setObjectName(QString::fromUtf8("mCardText"));
        mCardText->setGeometry(QRect(200, 75, 671, 475));
        btnDelMat = new QPushButton(MCCADGUI_MCardDialog);
        btnDelMat->setObjectName(QString::fromUtf8("btnDelMat"));
        btnDelMat->setGeometry(QRect(520, 580, 81, 27));
        loadXMLButton = new QPushButton(MCCADGUI_MCardDialog);
        loadXMLButton->setObjectName(QString::fromUtf8("loadXMLButton"));
        loadXMLButton->setGeometry(QRect(10, 580, 93, 27));
        saveXMLButton = new QPushButton(MCCADGUI_MCardDialog);
        saveXMLButton->setObjectName(QString::fromUtf8("saveXMLButton"));
        saveXMLButton->setGeometry(QRect(120, 580, 93, 27));
        applyButton = new QPushButton(MCCADGUI_MCardDialog);
        applyButton->setObjectName(QString::fromUtf8("applyButton"));
        applyButton->setGeometry(QRect(610, 580, 81, 27));
        okButton = new QPushButton(MCCADGUI_MCardDialog);
        okButton->setObjectName(QString::fromUtf8("okButton"));
        okButton->setGeometry(QRect(700, 580, 81, 27));
        cancelButton = new QPushButton(MCCADGUI_MCardDialog);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
        cancelButton->setGeometry(QRect(790, 580, 81, 27));
        label_4 = new QLabel(MCCADGUI_MCardDialog);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(730, 40, 61, 31));
        label_4->setFont(font);
        colorButton = new QPushButton(MCCADGUI_MCardDialog);
        colorButton->setObjectName(QString::fromUtf8("colorButton"));
        colorButton->setGeometry(QRect(780, 40, 51, 27));
        colorButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"background:rgb(255, 255, 0)\n"
"}"));
        label_5 = new QLabel(MCCADGUI_MCardDialog);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(200, 560, 361, 16));
        label_5->setFont(font);

        retranslateUi(MCCADGUI_MCardDialog);

        QMetaObject::connectSlotsByName(MCCADGUI_MCardDialog);
    } // setupUi

    void retranslateUi(QDialog *MCCADGUI_MCardDialog)
    {
        MCCADGUI_MCardDialog->setWindowTitle(QApplication::translate("MCCADGUI_MCardDialog", "Material management", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = mMatTree->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("MCCADGUI_MCardDialog", "Material", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MCCADGUI_MCardDialog", "ID:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MCCADGUI_MCardDialog", "Density:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MCCADGUI_MCardDialog", "Name:", 0, QApplication::UnicodeUTF8));
        btnDelMat->setText(QApplication::translate("MCCADGUI_MCardDialog", "Delete", 0, QApplication::UnicodeUTF8));
        loadXMLButton->setText(QApplication::translate("MCCADGUI_MCardDialog", "Import", 0, QApplication::UnicodeUTF8));
        saveXMLButton->setText(QApplication::translate("MCCADGUI_MCardDialog", "Export", 0, QApplication::UnicodeUTF8));
        applyButton->setText(QApplication::translate("MCCADGUI_MCardDialog", "Apply", 0, QApplication::UnicodeUTF8));
        okButton->setText(QApplication::translate("MCCADGUI_MCardDialog", "OK", 0, QApplication::UnicodeUTF8));
        cancelButton->setText(QApplication::translate("MCCADGUI_MCardDialog", "Cancel", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MCCADGUI_MCardDialog", "Color:", 0, QApplication::UnicodeUTF8));
        colorButton->setText(QString());
        label_5->setText(QApplication::translate("MCCADGUI_MCardDialog", "*Fill the above box to add or edit material properties.", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MCCADGUI_MCardDialog: public Ui_MCCADGUI_MCardDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MCARDDIALOG_H

/********************************************************************************
** Form generated from reading UI file 'McCad_LoadVisualizationList.ui'
**
** Created: Wed Jun 29 09:36:40 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MCCAD_LOADVISUALIZATIONLIST_H
#define UI_MCCAD_LOADVISUALIZATIONLIST_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_McCad_LoadVisualizationList
{
public:
    QDialogButtonBox *buttonBox;
    QPushButton *loadButton;
    QGroupBox *valuesBox;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *minimumEdit;
    QLineEdit *maximumEdit;
    QLineEdit *nbColorsEdit;
    QLabel *label_3;

    void setupUi(QDialog *McCad_LoadVisualizationList)
    {
        if (McCad_LoadVisualizationList->objectName().isEmpty())
            McCad_LoadVisualizationList->setObjectName(QString::fromUtf8("McCad_LoadVisualizationList"));
        McCad_LoadVisualizationList->resize(451, 106);
        buttonBox = new QDialogButtonBox(McCad_LoadVisualizationList);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(100, 70, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        loadButton = new QPushButton(McCad_LoadVisualizationList);
        loadButton->setObjectName(QString::fromUtf8("loadButton"));
        loadButton->setEnabled(true);
        loadButton->setGeometry(QRect(10, 20, 75, 24));
        loadButton->setDefault(false);
        loadButton->setFlat(false);
        valuesBox = new QGroupBox(McCad_LoadVisualizationList);
        valuesBox->setObjectName(QString::fromUtf8("valuesBox"));
        valuesBox->setGeometry(QRect(100, 10, 341, 51));
        label = new QLabel(valuesBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 20, 56, 14));
        label_2 = new QLabel(valuesBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(180, 20, 56, 14));
        minimumEdit = new QLineEdit(valuesBox);
        minimumEdit->setObjectName(QString::fromUtf8("minimumEdit"));
        minimumEdit->setGeometry(QRect(70, 20, 91, 20));
        minimumEdit->setReadOnly(false);
        maximumEdit = new QLineEdit(valuesBox);
        maximumEdit->setObjectName(QString::fromUtf8("maximumEdit"));
        maximumEdit->setGeometry(QRect(240, 20, 91, 20));
        maximumEdit->setText(QString::fromUtf8(""));
        maximumEdit->setReadOnly(false);
        nbColorsEdit = new QLineEdit(McCad_LoadVisualizationList);
        nbColorsEdit->setObjectName(QString::fromUtf8("nbColorsEdit"));
        nbColorsEdit->setGeometry(QRect(222, 70, 41, 20));
        label_3 = new QLabel(McCad_LoadVisualizationList);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(110, 70, 111, 20));

        retranslateUi(McCad_LoadVisualizationList);
        QObject::connect(buttonBox, SIGNAL(accepted()), McCad_LoadVisualizationList, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), McCad_LoadVisualizationList, SLOT(reject()));

        QMetaObject::connectSlotsByName(McCad_LoadVisualizationList);
    } // setupUi

    void retranslateUi(QDialog *McCad_LoadVisualizationList)
    {
        McCad_LoadVisualizationList->setWindowTitle(QApplication::translate("McCad_LoadVisualizationList", "Dialog", 0, QApplication::UnicodeUTF8));
        loadButton->setText(QApplication::translate("McCad_LoadVisualizationList", "Load List", 0, QApplication::UnicodeUTF8));
        valuesBox->setTitle(QApplication::translate("McCad_LoadVisualizationList", "Value Range", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("McCad_LoadVisualizationList", "Minimum", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("McCad_LoadVisualizationList", "Maximum", 0, QApplication::UnicodeUTF8));
        minimumEdit->setText(QString());
        //minimumEdit->setPlaceholderText(QApplication::translate("McCad_LoadVisualizationList", "0", 0, QApplication::UnicodeUTF8));
       // maximumEdit->setPlaceholderText(QApplication::translate("McCad_LoadVisualizationList", "0", 0, QApplication::UnicodeUTF8));
        //nbColorsEdit->setPlaceholderText(QApplication::translate("McCad_LoadVisualizationList", "12", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("McCad_LoadVisualizationList", "Number Of Colors", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class McCad_LoadVisualizationList: public Ui_McCad_LoadVisualizationList {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MCCAD_LOADVISUALIZATIONLIST_H

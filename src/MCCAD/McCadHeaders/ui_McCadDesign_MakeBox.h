#ifndef MAKEBOX1_H
#define MAKEBOX1_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

class Ui_MakeBoxDialog
{
public:
    QWidget *layoutWidget;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QGroupBox *groupBox;
    QLineEdit *Ori_X;
    QLineEdit *Ori_Y;
    QLineEdit *Ori_Z;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QGroupBox *groupBox_2;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_4;
    QLineEdit *Dim_X;
    QLineEdit *Dim_Y;
    QLineEdit *Dim_Z;

    void setupUi(QDialog *MakeBoxDialog)
    {
    MakeBoxDialog->setObjectName(QString::fromUtf8("MakeBoxDialog"));
    MakeBoxDialog->resize(QSize(362, 180).expandedTo(MakeBoxDialog->minimumSizeHint()));
    layoutWidget = new QWidget(MakeBoxDialog);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    layoutWidget->setGeometry(QRect(10, 140, 341, 33));
    hboxLayout = new QHBoxLayout(layoutWidget);
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    groupBox_2 = new QGroupBox(MakeBoxDialog);
   groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
   groupBox_2->setGeometry(QRect(190, 10, 161, 121));
   label_5 = new QLabel(groupBox_2);
   label_5->setObjectName(QString::fromUtf8("label_5"));
   label_5->setGeometry(QRect(20, 90, 21, 17));
   label_6 = new QLabel(groupBox_2);
   label_6->setObjectName(QString::fromUtf8("label_6"));
   label_6->setGeometry(QRect(20, 60, 21, 17));
   label_4 = new QLabel(groupBox_2);
   label_4->setObjectName(QString::fromUtf8("label_4"));
   label_4->setGeometry(QRect(20, 30, 21, 17));
   Dim_X = new QLineEdit(groupBox_2);
   Dim_X->setObjectName(QString::fromUtf8("Dim_X"));
   Dim_X->setGeometry(QRect(40, 30, 113, 20));
   Dim_X->setAlignment(Qt::AlignRight);
   Dim_Y = new QLineEdit(groupBox_2);
   Dim_Y->setObjectName(QString::fromUtf8("Dim_Y"));
   Dim_Y->setGeometry(QRect(40, 60, 113, 20));
   Dim_Y->setAlignment(Qt::AlignRight);
   Dim_Z = new QLineEdit(groupBox_2);
   Dim_Z->setObjectName(QString::fromUtf8("Dim_Z"));
   Dim_Z->setGeometry(QRect(40, 90, 113, 20));
   Dim_Z->setAlignment(Qt::AlignRight);


    hboxLayout->addItem(spacerItem);

    okButton = new QPushButton(layoutWidget);
    okButton->setObjectName(QString::fromUtf8("okButton"));

    hboxLayout->addWidget(okButton);

    cancelButton = new QPushButton(layoutWidget);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

    hboxLayout->addWidget(cancelButton);

    groupBox = new QGroupBox(MakeBoxDialog);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    groupBox->setGeometry(QRect(10, 10, 161, 121));
    Ori_X = new QLineEdit(groupBox);
    Ori_X->setObjectName(QString::fromUtf8("Ori_X"));
    Ori_X->setGeometry(QRect(40, 30, 113, 20));
    Ori_X->setAlignment(Qt::AlignRight);
    Ori_Y = new QLineEdit(groupBox);
    Ori_Y->setObjectName(QString::fromUtf8("Ori_Y"));
    Ori_Y->setGeometry(QRect(40, 60, 113, 20));
    Ori_Y->setAlignment(Qt::AlignRight);
    Ori_Z = new QLineEdit(groupBox);
    Ori_Z->setObjectName(QString::fromUtf8("Ori_Z"));
    Ori_Z->setGeometry(QRect(40, 90, 113, 20));
    Ori_Z->setAlignment(Qt::AlignRight);
    label = new QLabel(groupBox);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(20, 30, 21, 17));
    label_2 = new QLabel(groupBox);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setGeometry(QRect(20, 60, 21, 17));
    label_3 = new QLabel(groupBox);
    label_3->setObjectName(QString::fromUtf8("label_3"));
    label_3->setGeometry(QRect(20, 90, 21, 17));
    retranslateUi(MakeBoxDialog);
    QObject::connect(okButton, SIGNAL(clicked()), MakeBoxDialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), MakeBoxDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(MakeBoxDialog);
    } // setupUi

    void retranslateUi(QDialog *MakeBoxDialog)
    {
    MakeBoxDialog->setWindowTitle(QApplication::translate("MakeBoxDialog", "Make Box", 0, QApplication::UnicodeUTF8));
    groupBox_2->setTitle(QApplication::translate("MakeBoxDialog", "Dimensions", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("MakeBoxDialog", "X", 0, QApplication::UnicodeUTF8));
    label_6->setText(QApplication::translate("MakeBoxDialog", "Y", 0, QApplication::UnicodeUTF8));
    label_5->setText(QApplication::translate("MakeBoxDialog", "Z", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("MakeBoxDialog", "OK", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("MakeBoxDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("MakeBoxDialog", "Origin", 0, QApplication::UnicodeUTF8));
    Ori_X->setText(QApplication::translate("MakeBoxDialog", "0", 0, QApplication::UnicodeUTF8));
    Ori_Y->setText(QApplication::translate("MakeBoxDialog", "0", 0, QApplication::UnicodeUTF8));
    Ori_Z->setText(QApplication::translate("MakeBoxDialog", "0", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("MakeBoxDialog", "X", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("MakeBoxDialog", "Y", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("MakeBoxDialog", "Z", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(MakeBoxDialog);
    } // retranslateUi

};

namespace Ui {
    class MakeBoxDialog: public Ui_MakeBoxDialog {};
} // namespace Ui

#endif // MAKEBOX1_H

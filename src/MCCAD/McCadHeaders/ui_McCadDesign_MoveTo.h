#ifndef MOVETO_H
#define MOVETO_H

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

class Ui_moveToDialog
{
public:
    QGroupBox *groupBox_2;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLineEdit *disX;
    QLineEdit *disY;
    QLineEdit *disZ;
    QWidget *layoutWidget;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QGroupBox *groupBox;
    QLabel *label_3;
    QLabel *label_2;
    QLabel *label;
    QLineEdit *coX;
    QLineEdit *coY;
    QLineEdit *coZ;

    void setupUi(QDialog *moveToDialog)
    {
    moveToDialog->setObjectName(QString::fromUtf8("moveToDialog"));
    moveToDialog->resize(QSize(379, 170).expandedTo(moveToDialog->minimumSizeHint()));
    groupBox_2 = new QGroupBox(moveToDialog);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    groupBox_2->setGeometry(QRect(200, 10, 171, 111));
    label_4 = new QLabel(groupBox_2);
    label_4->setObjectName(QString::fromUtf8("label_4"));
    label_4->setGeometry(QRect(20, 70, 16, 17));
    label_5 = new QLabel(groupBox_2);
    label_5->setObjectName(QString::fromUtf8("label_5"));
    label_5->setGeometry(QRect(20, 50, 16, 17));
    label_6 = new QLabel(groupBox_2);
    label_6->setObjectName(QString::fromUtf8("label_6"));
    label_6->setGeometry(QRect(20, 30, 16, 17));
    disX = new QLineEdit(groupBox_2);
    disX->setObjectName(QString::fromUtf8("disX"));
    disX->setGeometry(QRect(40, 30, 113, 20));
    disX->setAlignment(Qt::AlignRight);
    disY = new QLineEdit(groupBox_2);
    disY->setObjectName(QString::fromUtf8("disY"));
    disY->setGeometry(QRect(40, 50, 113, 20));
    disY->setAlignment(Qt::AlignRight);
    disZ = new QLineEdit(groupBox_2);
    disZ->setObjectName(QString::fromUtf8("disZ"));
    disZ->setGeometry(QRect(40, 70, 113, 20));
    disZ->setAlignment(Qt::AlignRight);
    layoutWidget = new QWidget(moveToDialog);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    layoutWidget->setGeometry(QRect(10, 130, 361, 33));
    hboxLayout = new QHBoxLayout(layoutWidget);
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem);

    okButton = new QPushButton(layoutWidget);
    okButton->setObjectName(QString::fromUtf8("okButton"));

    hboxLayout->addWidget(okButton);

    cancelButton = new QPushButton(layoutWidget);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

    hboxLayout->addWidget(cancelButton);

    groupBox = new QGroupBox(moveToDialog);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    groupBox->setGeometry(QRect(10, 10, 171, 111));
    label_3 = new QLabel(groupBox);
    label_3->setObjectName(QString::fromUtf8("label_3"));
    label_3->setGeometry(QRect(20, 70, 16, 17));
    label_2 = new QLabel(groupBox);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setGeometry(QRect(20, 50, 16, 17));
    label = new QLabel(groupBox);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(20, 30, 16, 17));
    coX = new QLineEdit(groupBox);
    coX->setObjectName(QString::fromUtf8("coX"));
    coX->setGeometry(QRect(40, 30, 113, 20));
    coX->setAlignment(Qt::AlignRight);
    coY = new QLineEdit(groupBox);
    coY->setObjectName(QString::fromUtf8("coY"));
    coY->setGeometry(QRect(40, 50, 113, 20));
    coY->setAlignment(Qt::AlignRight);
    coZ = new QLineEdit(groupBox);
    coZ->setObjectName(QString::fromUtf8("coZ"));
    coZ->setGeometry(QRect(40, 70, 113, 20));
    coZ->setAlignment(Qt::AlignRight);
    retranslateUi(moveToDialog);
    QObject::connect(okButton, SIGNAL(clicked()), moveToDialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), moveToDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(moveToDialog);
    } // setupUi

    void retranslateUi(QDialog *moveToDialog)
    {
    moveToDialog->setWindowTitle(QApplication::translate("moveToDialog", "Move ...", 0, QApplication::UnicodeUTF8));
    groupBox_2->setTitle(QApplication::translate("moveToDialog", "a distance", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("moveToDialog", "Z", 0, QApplication::UnicodeUTF8));
    label_5->setText(QApplication::translate("moveToDialog", "Y", 0, QApplication::UnicodeUTF8));
    label_6->setText(QApplication::translate("moveToDialog", "X", 0, QApplication::UnicodeUTF8));
    disX->setText(QApplication::translate("moveToDialog", "0", 0, QApplication::UnicodeUTF8));
    disY->setText(QApplication::translate("moveToDialog", "0", 0, QApplication::UnicodeUTF8));
    disZ->setText(QApplication::translate("moveToDialog", "0", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("moveToDialog", "OK", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("moveToDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("moveToDialog", "to coordinates", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("moveToDialog", "Z", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("moveToDialog", "Y", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("moveToDialog", "X", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(moveToDialog);
    } // retranslateUi

};

namespace Ui {
    class moveToDialog: public Ui_moveToDialog {};
} // namespace Ui

#endif // MOVETO_H

#ifndef MAKESPHERE_H
#define MAKESPHERE_H

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

class Ui_SphereDialog
{
public:
    QWidget *layoutWidget;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QGroupBox *groupBox_2;
    QLabel *label_4;
    QLineEdit *radius;
    QGroupBox *groupBox;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *oriX;
    QLineEdit *oriY;
    QLineEdit *oriZ;

    void setupUi(QDialog *SphereDialog)
    {
    SphereDialog->setObjectName(QString::fromUtf8("SphereDialog"));
    SphereDialog->resize(QSize(193, 218).expandedTo(SphereDialog->minimumSizeHint()));
    layoutWidget = new QWidget(SphereDialog);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    layoutWidget->setGeometry(QRect(0, 180, 188, 33));
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

    groupBox_2 = new QGroupBox(SphereDialog);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    groupBox_2->setGeometry(QRect(10, 110, 171, 61));
    label_4 = new QLabel(groupBox_2);
    label_4->setObjectName(QString::fromUtf8("label_4"));
    label_4->setGeometry(QRect(10, 30, 21, 17));

    groupBox = new QGroupBox(SphereDialog);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    groupBox->setGeometry(QRect(10, 10, 171, 101));
    label = new QLabel(groupBox);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(10, 30, 21, 17));
    label_2 = new QLabel(groupBox);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setGeometry(QRect(10, 50, 21, 17));
    label_3 = new QLabel(groupBox);
    label_3->setObjectName(QString::fromUtf8("label_3"));
    label_3->setGeometry(QRect(10, 70, 21, 17));
    oriX = new QLineEdit(groupBox);
    oriX->setObjectName(QString::fromUtf8("oriX"));
    oriX->setGeometry(QRect(30, 30, 131, 20));
    oriX->setAlignment(Qt::AlignRight);
    oriY = new QLineEdit(groupBox);
    oriY->setObjectName(QString::fromUtf8("oriY"));
    oriY->setGeometry(QRect(30, 50, 131, 20));
    oriY->setAlignment(Qt::AlignRight);
    oriZ = new QLineEdit(groupBox);
    oriZ->setObjectName(QString::fromUtf8("oriZ"));
    oriZ->setGeometry(QRect(30, 70, 131, 20));
    oriZ->setAlignment(Qt::AlignRight);
    radius = new QLineEdit(groupBox_2);
	radius->setObjectName(QString::fromUtf8("radius"));
	radius->setGeometry(QRect(30, 30, 131, 20));
	radius->setAlignment(Qt::AlignRight);
    retranslateUi(SphereDialog);
    QObject::connect(okButton, SIGNAL(clicked()), SphereDialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), SphereDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(SphereDialog);
    } // setupUi

    void retranslateUi(QDialog *SphereDialog)
    {
    SphereDialog->setWindowTitle(QApplication::translate("SphereDialog", "Make Sphere", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("SphereDialog", "OK", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("SphereDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    oriX->setText(QApplication::translate("ConeDialog", "0", 0, QApplication::UnicodeUTF8));
	oriY->setText(QApplication::translate("ConeDialog", "0", 0, QApplication::UnicodeUTF8));
	oriZ->setText(QApplication::translate("ConeDialog", "0", 0, QApplication::UnicodeUTF8));
    groupBox_2->setTitle(QApplication::translate("SphereDialog", "Radius", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("SphereDialog", "R", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("SphereDialog", "Origin", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("SphereDialog", "X", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("SphereDialog", "Y", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("SphereDialog", "Z", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(SphereDialog);
    } // retranslateUi

};

namespace Ui {
    class SphereDialog: public Ui_SphereDialog {};
} // namespace Ui

#endif // MAKESPHERE_H

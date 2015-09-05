#ifndef SCALE_H
#define SCALE_H

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

class Ui_scaleDialog
{
public:
    QWidget *layoutWidget;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QLabel *label;
    QGroupBox *groupBox;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLineEdit *sf_Y;
    QLineEdit *sf_Z;
    QLineEdit *sf_X;
    QLineEdit *sf_All;

    void setupUi(QDialog *scaleDialog)
    {
    scaleDialog->setObjectName(QString::fromUtf8("scaleDialog"));
    scaleDialog->resize(QSize(197, 200).expandedTo(scaleDialog->minimumSizeHint()));
    layoutWidget = new QWidget(scaleDialog);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    layoutWidget->setGeometry(QRect(0, 160, 188, 33));
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

    label = new QLabel(scaleDialog);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(20, 20, 101, 17));
    groupBox = new QGroupBox(scaleDialog);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    groupBox->setGeometry(QRect(20, 50, 161, 101));
    label_2 = new QLabel(groupBox);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setGeometry(QRect(40, 30, 21, 17));
    label_3 = new QLabel(groupBox);
    label_3->setObjectName(QString::fromUtf8("label_3"));
    label_3->setGeometry(QRect(40, 50, 21, 17));
    label_4 = new QLabel(groupBox);
    label_4->setObjectName(QString::fromUtf8("label_4"));
    label_4->setGeometry(QRect(40, 70, 21, 17));
    sf_All = new QLineEdit(scaleDialog);
	sf_All->setObjectName(QString::fromUtf8("sf_All"));
	sf_All->setGeometry(QRect(120, 20, 51, 20));
	sf_All->setAlignment(Qt::AlignRight);
    sf_X = new QLineEdit(groupBox);
    sf_X->setObjectName(QString::fromUtf8("sf_X"));
    sf_X->setGeometry(QRect(70, 30, 51, 20));
    sf_X->setAlignment(Qt::AlignRight);
    sf_Y = new QLineEdit(groupBox);
	sf_Y->setObjectName(QString::fromUtf8("sf_Y"));
	sf_Y->setGeometry(QRect(70, 50, 51, 20));
	sf_Y->setAlignment(Qt::AlignRight);
	sf_Z = new QLineEdit(groupBox);
	sf_Z->setObjectName(QString::fromUtf8("sf_Z"));
	sf_Z->setGeometry(QRect(70, 70, 51, 20));
	sf_Z->setAlignment(Qt::AlignRight);
    retranslateUi(scaleDialog);
    QObject::connect(okButton, SIGNAL(clicked()), scaleDialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), scaleDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(scaleDialog);
    } // setupUi

    void retranslateUi(QDialog *scaleDialog)
    {
    scaleDialog->setWindowTitle(QApplication::translate("scaleDialog", "Scaling", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("scaleDialog", "OK", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("scaleDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    sf_All->setText(QApplication::translate("scaleDialog", "1.0", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("scaleDialog", "Scale Factor", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("scaleDialog", "Axis Dependent Scaling", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("scaleDialog", "X", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("scaleDialog", "Y", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("scaleDialog", "Z", 0, QApplication::UnicodeUTF8));
    sf_X->setText(QApplication::translate("scaleDialog", "", 0, QApplication::UnicodeUTF8));
    sf_Y->setText(QApplication::translate("scaleDialog", "", 0, QApplication::UnicodeUTF8));
    sf_Z->setText(QApplication::translate("scaleDialog", "", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(scaleDialog);
    } // retranslateUi

};

namespace Ui {
    class scaleDialog: public Ui_scaleDialog {};
} // namespace Ui

#endif // SCALE_H

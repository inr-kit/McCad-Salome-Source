#ifndef ROTATE_H
#define ROTATE_H

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
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

class Ui_rotateDialog
{
public:
    QGroupBox *groupBox;
    QLabel *label_3;
    QLabel *label_2;
    QLabel *label;
    QLineEdit *axX;
    QLineEdit *axY;
    QLineEdit *axZ;
    QWidget *layoutWidget;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QGroupBox *groupBox_2;
    QLabel *label_4;
    QLineEdit *angle;
    QRadioButton *radButton;
    QRadioButton *degButton;

    void setupUi(QDialog *rotateDialog)
    {
    rotateDialog->setObjectName(QString::fromUtf8("rotateDialog"));
    rotateDialog->resize(QSize(191, 256).expandedTo(rotateDialog->minimumSizeHint()));
    groupBox = new QGroupBox(rotateDialog);
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
    axX = new QLineEdit(groupBox);
    axX->setObjectName(QString::fromUtf8("axX"));
    axX->setGeometry(QRect(40, 30, 113, 20));
    axX->setAlignment(Qt::AlignRight);
    axY = new QLineEdit(groupBox);
    axY->setObjectName(QString::fromUtf8("axY"));
    axY->setGeometry(QRect(40, 50, 113, 20));
    axY->setAlignment(Qt::AlignRight);
    axZ = new QLineEdit(groupBox);
    axZ->setObjectName(QString::fromUtf8("axZ"));
    axZ->setGeometry(QRect(40, 70, 113, 20));
    axZ->setAlignment(Qt::AlignRight);
    layoutWidget = new QWidget(rotateDialog);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    layoutWidget->setGeometry(QRect(0, 220, 188, 33));
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

    groupBox_2 = new QGroupBox(rotateDialog);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    groupBox_2->setGeometry(QRect(10, 120, 171, 91));
    label_4 = new QLabel(groupBox_2);
    label_4->setObjectName(QString::fromUtf8("label_4"));
    label_4->setGeometry(QRect(20, 30, 16, 17));
    angle = new QLineEdit(groupBox_2);
    angle->setObjectName(QString::fromUtf8("angle"));
    angle->setGeometry(QRect(40, 30, 113, 20));
    angle->setAlignment(Qt::AlignRight);
    radButton = new QRadioButton(groupBox_2);
    radButton->setObjectName(QString::fromUtf8("radButton"));
    radButton->setGeometry(QRect(10, 60, 51, 23));
    radButton->setLayoutDirection(Qt::RightToLeft);
    degButton = new QRadioButton(groupBox_2);
    degButton->setObjectName(QString::fromUtf8("degButton"));
    degButton->setGeometry(QRect(100, 60, 51, 23));
    degButton->setLayoutDirection(Qt::RightToLeft);
    degButton->setChecked(true);
    retranslateUi(rotateDialog);
    QObject::connect(okButton, SIGNAL(clicked()), rotateDialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), rotateDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(rotateDialog);
    } // setupUi

    void retranslateUi(QDialog *rotateDialog)
    {
    rotateDialog->setWindowTitle(QApplication::translate("rotateDialog", "Rotate", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("rotateDialog", "Axis", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("rotateDialog", "Z", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("rotateDialog", "Y", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("rotateDialog", "X", 0, QApplication::UnicodeUTF8));
    axX->setText(QApplication::translate("rotateDialog", "0", 0, QApplication::UnicodeUTF8));
    axY->setText(QApplication::translate("rotateDialog", "0", 0, QApplication::UnicodeUTF8));
    axZ->setText(QApplication::translate("rotateDialog", "1", 0, QApplication::UnicodeUTF8));
    angle->setText(QApplication::translate("rotateDialog", "0", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("rotateDialog", "OK", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("rotateDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    groupBox_2->setTitle(QApplication::translate("rotateDialog", "Angle", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("rotateDialog", "a", 0, QApplication::UnicodeUTF8));
    radButton->setText(QApplication::translate("rotateDialog", "rad", 0, QApplication::UnicodeUTF8));
    degButton->setText(QApplication::translate("rotateDialog", "deg", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(rotateDialog);
    } // retranslateUi

};

namespace Ui {
    class rotateDialog: public Ui_rotateDialog {};
} // namespace Ui

#endif // ROTATE_H

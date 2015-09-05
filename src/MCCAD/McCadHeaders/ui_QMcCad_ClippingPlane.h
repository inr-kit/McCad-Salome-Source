#ifndef CLIPPINGPLANE_H
#define CLIPPINGPLANE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

class Ui_ui_ClippingPlaneDialog
{
public:
    QLabel *label;
    QGroupBox *groupBox_2;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *locX;
    QLineEdit *locY;
    QLineEdit *locZ;
    QLineEdit *dirY;
    QLineEdit *dirZ;
    QLineEdit *dirX;
    QGroupBox *groupBox;
    QLabel *label_4;
    QLabel *label_6;
    QLabel *label_5;
    QLabel *label_7;
    QLineEdit *A;
    QLineEdit *C;
    QLineEdit *B;
    QLineEdit *D;
    QGroupBox *groupBox_3;
    QSlider *plnSlider;
    QWidget *layoutWidget;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QCheckBox *checkBox;

    void setupUi(QDialog *ui_ClippingPlaneDialog)
    {
    ui_ClippingPlaneDialog->setObjectName(QString::fromUtf8("ui_ClippingPlaneDialog"));
    ui_ClippingPlaneDialog->resize(QSize(229, 410).expandedTo(ui_ClippingPlaneDialog->minimumSizeHint()));
    label = new QLabel(ui_ClippingPlaneDialog);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(10, 10, 171, 17));
    groupBox_2 = new QGroupBox(ui_ClippingPlaneDialog);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    groupBox_2->setGeometry(QRect(10, 40, 211, 131));
    label_2 = new QLabel(groupBox_2);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setGeometry(QRect(10, 20, 57, 17));
    label_3 = new QLabel(groupBox_2);
    label_3->setObjectName(QString::fromUtf8("label_3"));
    label_3->setGeometry(QRect(130, 20, 57, 17));
    locX = new QLineEdit(groupBox_2);
    locX->setObjectName(QString::fromUtf8("locX"));
    locX->setGeometry(QRect(10, 40, 71, 25));
    locX->setAlignment(Qt::AlignRight);
    locY = new QLineEdit(groupBox_2);
    locY->setObjectName(QString::fromUtf8("locY"));
    locY->setGeometry(QRect(10, 70, 71, 25));
    locY->setAlignment(Qt::AlignRight);
    locZ = new QLineEdit(groupBox_2);
    locZ->setObjectName(QString::fromUtf8("locZ"));
    locZ->setGeometry(QRect(10, 100, 71, 25));
    locZ->setAlignment(Qt::AlignRight);
    dirX = new QLineEdit(groupBox_2);
	dirX->setObjectName(QString::fromUtf8("dirX"));
	dirX->setGeometry(QRect(120, 40, 71, 25));
	dirX->setAlignment(Qt::AlignRight);
    dirY = new QLineEdit(groupBox_2);
    dirY->setObjectName(QString::fromUtf8("dirY"));
    dirY->setGeometry(QRect(120, 70, 71, 25));
    dirY->setAlignment(Qt::AlignRight);
    dirZ = new QLineEdit(groupBox_2);
    dirZ->setObjectName(QString::fromUtf8("dirZ"));
    dirZ->setGeometry(QRect(120, 100, 71, 25));
    dirZ->setAlignment(Qt::AlignRight);
    groupBox = new QGroupBox(ui_ClippingPlaneDialog);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    groupBox->setGeometry(QRect(10, 180, 211, 80));
    label_4 = new QLabel(groupBox);
    label_4->setObjectName(QString::fromUtf8("label_4"));
    label_4->setGeometry(QRect(10, 20, 21, 17));
    label_6 = new QLabel(groupBox);
    label_6->setObjectName(QString::fromUtf8("label_6"));
    label_6->setGeometry(QRect(130, 20, 21, 17));
    label_5 = new QLabel(groupBox);
    label_5->setObjectName(QString::fromUtf8("label_5"));
    label_5->setGeometry(QRect(10, 50, 21, 17));
    label_7 = new QLabel(groupBox);
    label_7->setObjectName(QString::fromUtf8("label_7"));
    label_7->setGeometry(QRect(130, 50, 21, 17));
    A = new QLineEdit(groupBox);
    A->setObjectName(QString::fromUtf8("A"));
    A->setGeometry(QRect(40, 20, 51, 25));
    A->setAlignment(Qt::AlignRight);
    C = new QLineEdit(groupBox);
    C->setObjectName(QString::fromUtf8("C"));
    C->setGeometry(QRect(40, 50, 51, 25));
    C->setAlignment(Qt::AlignRight);
    B = new QLineEdit(groupBox);
    B->setObjectName(QString::fromUtf8("B"));
    B->setGeometry(QRect(150, 20, 51, 25));
    B->setAlignment(Qt::AlignRight);
    D = new QLineEdit(groupBox);
    D->setObjectName(QString::fromUtf8("D"));
    D->setGeometry(QRect(150, 50, 51, 25));
    D->setAlignment(Qt::AlignRight);
    groupBox_3 = new QGroupBox(ui_ClippingPlaneDialog);
    groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
    groupBox_3->setGeometry(QRect(10, 270, 211, 51));
    plnSlider = new QSlider(groupBox_3);
    plnSlider->setObjectName(QString::fromUtf8("plnSlider"));
    plnSlider->setGeometry(QRect(20, 20, 171, 20));
    plnSlider->setMinimum(-100000);
    plnSlider->setMaximum(100000);
    plnSlider->setOrientation(Qt::Horizontal);
    layoutWidget = new QWidget(ui_ClippingPlaneDialog);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    layoutWidget->setGeometry(QRect(10, 370, 211, 33));
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

    checkBox = new QCheckBox(ui_ClippingPlaneDialog);
    checkBox->setObjectName(QString::fromUtf8("checkBox"));
    checkBox->setGeometry(QRect(0, 340, 161, 22));
    checkBox->setLayoutDirection(Qt::RightToLeft);
    checkBox->setChecked(false);
    retranslateUi(ui_ClippingPlaneDialog);
    QObject::connect(okButton, SIGNAL(clicked()), ui_ClippingPlaneDialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), ui_ClippingPlaneDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(ui_ClippingPlaneDialog);
    } // setupUi

    void retranslateUi(QDialog *ui_ClippingPlaneDialog)
    {
    ui_ClippingPlaneDialog->setWindowTitle(QApplication::translate("ui_ClippingPlaneDialog", "Clipping Plane Dialog", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("ui_ClippingPlaneDialog", "Define clipping plane by ...", 0, QApplication::UnicodeUTF8));
    groupBox_2->setTitle(QApplication::translate("ui_ClippingPlaneDialog", "... location and plane normal", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("ui_ClippingPlaneDialog", "Location", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("ui_ClippingPlaneDialog", "Normal", 0, QApplication::UnicodeUTF8));
    locX->setText(QApplication::translate("ui_ClippingPlaneDialog", "X", 0, QApplication::UnicodeUTF8));
    locY->setText(QApplication::translate("ui_ClippingPlaneDialog", "Y", 0, QApplication::UnicodeUTF8));
    locZ->setText(QApplication::translate("ui_ClippingPlaneDialog", "Z", 0, QApplication::UnicodeUTF8));
    dirX->setText(QApplication::translate("ui_ClippingPlaneDialog", "X", 0, QApplication::UnicodeUTF8));
    dirY->setText(QApplication::translate("ui_ClippingPlaneDialog", "Y", 0, QApplication::UnicodeUTF8));
    dirZ->setText(QApplication::translate("ui_ClippingPlaneDialog", "Z", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("ui_ClippingPlaneDialog", "... plane coefficients", "A*x + B*y + C*z - D = 0", QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("ui_ClippingPlaneDialog", "A", 0, QApplication::UnicodeUTF8));
    label_6->setText(QApplication::translate("ui_ClippingPlaneDialog", "B", 0, QApplication::UnicodeUTF8));
    label_5->setText(QApplication::translate("ui_ClippingPlaneDialog", "C", 0, QApplication::UnicodeUTF8));
    label_7->setText(QApplication::translate("ui_ClippingPlaneDialog", "D", 0, QApplication::UnicodeUTF8));
    A->setText(QApplication::translate("ui_ClippingPlaneDialog", "", 0, QApplication::UnicodeUTF8));
    C->setText(QApplication::translate("ui_ClippingPlaneDialog", "", 0, QApplication::UnicodeUTF8));
    B->setText(QApplication::translate("ui_ClippingPlaneDialog", "", 0, QApplication::UnicodeUTF8));
    D->setText(QApplication::translate("ui_ClippingPlaneDialog", "", 0, QApplication::UnicodeUTF8));
    groupBox_3->setTitle(QApplication::translate("ui_ClippingPlaneDialog", "Slide plane along axis", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("ui_ClippingPlaneDialog", "OK", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("ui_ClippingPlaneDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    checkBox->setText(QApplication::translate("ui_ClippingPlaneDialog", "Activate clipping plane", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(ui_ClippingPlaneDialog);
    } // retranslateUi

};

namespace Ui {
    class ui_ClippingPlaneDialog: public Ui_ui_ClippingPlaneDialog {};
} // namespace Ui

#endif // CLIPPINGPLANE_H

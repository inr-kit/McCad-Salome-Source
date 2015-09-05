#ifndef COPYTO_H
#define COPYTO_H

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

class Ui_copyToDialog
{
public:
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

    void setupUi(QDialog *copyToDialog)
    {
    copyToDialog->setObjectName(QString::fromUtf8("copyToDialog"));
    copyToDialog->resize(QSize(205, 166).expandedTo(copyToDialog->minimumSizeHint()));
    layoutWidget = new QWidget(copyToDialog);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    layoutWidget->setGeometry(QRect(10, 130, 188, 33));
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

    groupBox = new QGroupBox(copyToDialog);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    groupBox->setGeometry(QRect(20, 10, 171, 111));
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
    retranslateUi(copyToDialog);
    QObject::connect(okButton, SIGNAL(clicked()), copyToDialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), copyToDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(copyToDialog);
    } // setupUi

    void retranslateUi(QDialog *copyToDialog)
    {
    copyToDialog->setWindowTitle(QApplication::translate("copyToDialog", "Copy to...", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("copyToDialog", "OK", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("copyToDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("copyToDialog", "Coordinates", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("copyToDialog", "Z", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("copyToDialog", "Y", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("copyToDialog", "X", 0, QApplication::UnicodeUTF8));
    coX->setText(QApplication::translate("copyToDialog", "0", 0, QApplication::UnicodeUTF8));
    coY->setText(QApplication::translate("copyToDialog", "0", 0, QApplication::UnicodeUTF8));
    coZ->setText(QApplication::translate("copyToDialog", "0", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(copyToDialog);
    } // retranslateUi

};

namespace Ui {
    class copyToDialog: public Ui_copyToDialog {};
} // namespace Ui

#endif // COPYTO_H

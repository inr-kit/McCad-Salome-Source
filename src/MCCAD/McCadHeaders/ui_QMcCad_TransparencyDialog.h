#ifndef QMCCAD_TRANSPARENCYDIALOG_H
#define QMCCAD_TRANSPARENCYDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLCDNumber>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>

class Ui_TransDialog
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *hboxLayout;
    QSlider *horizontalSlider;
    QLCDNumber *lcdNumber;
    QHBoxLayout *hboxLayout1;
    QSpacerItem *spacerItem;
    QPushButton *okButton;

    void setupUi(QDialog *TransDialog)
    {
    TransDialog->setObjectName(QString::fromUtf8("TransDialog"));
    TransDialog->resize(QSize(301, 94).expandedTo(TransDialog->minimumSizeHint()));
    gridLayout = new QGridLayout(TransDialog);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(9);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    horizontalSlider = new QSlider(TransDialog);
    horizontalSlider->setObjectName(QString::fromUtf8("horizontalSlider"));
    horizontalSlider->setMaximum(100);
    horizontalSlider->setSingleStep(5);
    horizontalSlider->setOrientation(Qt::Horizontal);

    hboxLayout->addWidget(horizontalSlider);

    lcdNumber = new QLCDNumber(TransDialog);
    lcdNumber->setObjectName(QString::fromUtf8("lcdNumber"));
    lcdNumber->setSegmentStyle(QLCDNumber::Flat);

    hboxLayout->addWidget(lcdNumber);


    gridLayout->addLayout(hboxLayout, 0, 0, 1, 1);

    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setSpacing(6);
    hboxLayout1->setMargin(0);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout1->addItem(spacerItem);

    okButton = new QPushButton(TransDialog);
    okButton->setObjectName(QString::fromUtf8("okButton"));

    hboxLayout1->addWidget(okButton);


    gridLayout->addLayout(hboxLayout1, 1, 0, 1, 1);

    retranslateUi(TransDialog);
    QObject::connect(okButton, SIGNAL(clicked()), TransDialog, SLOT(accept()));
    QObject::connect(horizontalSlider, SIGNAL(valueChanged(int)), lcdNumber, SLOT(display(int)));
    QObject::connect(horizontalSlider, SIGNAL(sliderReleased()), TransDialog, SLOT(accept()));

    QMetaObject::connectSlotsByName(TransDialog);
    } // setupUi

    void retranslateUi(QDialog *TransDialog)
    {
    TransDialog->setWindowTitle(QApplication::translate("TransDialog", "Set Transparency", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("TransDialog", "OK", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(TransDialog);
    } // retranslateUi

};

namespace Ui {
    class TransDialog: public Ui_TransDialog {};
} // namespace Ui

#endif // QMCCAD_TRANSPARENCYDIALOG_H

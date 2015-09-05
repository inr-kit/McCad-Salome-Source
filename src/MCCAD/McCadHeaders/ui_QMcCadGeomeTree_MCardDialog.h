#ifndef QMCCADGEOMETREE_MCARDDIALOG1_H
#define QMCCADGEOMETREE_MCARDDIALOG1_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextEdit>
#include <QtGui/QWidget>
#include <QtGui/QTreeWidget>

class Ui_MCardDialog
{
public:
    QWidget *layoutWidget;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *okButton;
    QPushButton *cancelButton;
    //QPushButton *saveXMLButton;
    //QPushButton *loadXMLButton;

    QPushButton *btnAddMat;
    QPushButton *btnDelMat;
    QPushButton *btnMdfMat;
    QLabel *label_1;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLineEdit *IDEdit;
    QLineEdit *densityEdit;
    QLineEdit *nameEdit;
    QTextEdit *mCardText;
    QTreeWidget *mMatTree;

    void setupUi(QDialog *MCardDialog)
    {
    MCardDialog->setObjectName(QString::fromUtf8("MCardDialog"));
    MCardDialog->resize(QSize(800, 600).expandedTo(MCardDialog->minimumSizeHint()));
    layoutWidget = new QWidget(MCardDialog);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    layoutWidget->setGeometry(QRect(10, 550, 770, 50));
    hboxLayout = new QHBoxLayout(layoutWidget);
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    spacerItem = new QSpacerItem(200, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    btnAddMat = new QPushButton(layoutWidget);
    btnAddMat->setObjectName(QString::fromUtf8("AddButton"));
    hboxLayout->addWidget(btnAddMat);
    //btnMdfMat = new QPushButton(layoutWidget);
    //btnMdfMat->setObjectName(QString::fromUtf8("ModifyButton"));
    //hboxLayout->addWidget(btnMdfMat);
    btnDelMat = new QPushButton(layoutWidget);
    btnDelMat->setObjectName(QString::fromUtf8("DelButton"));
    hboxLayout->addWidget(btnDelMat);

    hboxLayout->addItem(spacerItem);
    //saveXMLButton = new QPushButton(layoutWidget);
    //saveXMLButton->setObjectName(QString::fromUtf8("saveXMLButton"));
    //hboxLayout->addWidget(saveXMLButton);
    //loadXMLButton = new QPushButton(layoutWidget);
    //loadXMLButton->setObjectName(QString::fromUtf8("loadXMLButton"));
    //hboxLayout->addWidget(loadXMLButton);
    okButton = new QPushButton(layoutWidget);
    okButton->setObjectName(QString::fromUtf8("okButton"));
    hboxLayout->addWidget(okButton);
    cancelButton = new QPushButton(layoutWidget);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
    hboxLayout->addWidget(cancelButton);

    label_1 = new QLabel(MCardDialog);
    label_1->setObjectName(QString::fromUtf8("label_1"));
    label_1->setGeometry(QRect(10, 10, 780, 25));

    label_2 = new QLabel(MCardDialog);
    label_2->setObjectName(QString::fromUtf8("label_3"));
    label_2->setGeometry(QRect(200, 40, 20, 25));
    label_3 = new QLabel(MCardDialog);
    label_3->setObjectName(QString::fromUtf8("label_3"));
    label_3->setGeometry(QRect(310, 40, 50, 25));
    label_4 = new QLabel(MCardDialog);
    label_4->setObjectName(QString::fromUtf8("label_4"));
    label_4->setGeometry(QRect(470, 40, 50, 25));

    QIntValidator *vInt = new QIntValidator(1,1000,MCardDialog);
    IDEdit = new QLineEdit(MCardDialog);
    IDEdit->setObjectName(QString::fromUtf8("IDEdit"));
    IDEdit->setGeometry(QRect(230, 40, 60, 25));
    IDEdit->setLayoutDirection(Qt::LeftToRight);
    IDEdit->setValidator(vInt);

    QDoubleValidator *vDbl = new QDoubleValidator(-10000.0,10000.0,4,MCardDialog);
    densityEdit = new QLineEdit(MCardDialog);
    densityEdit->setObjectName(QString::fromUtf8("densityEdit"));
    densityEdit->setGeometry(QRect(370, 40, 80, 25));
    densityEdit->setLayoutDirection(Qt::LeftToRight);
    densityEdit->setValidator(vDbl);

    nameEdit = new QLineEdit(MCardDialog);
    nameEdit->setObjectName(QString::fromUtf8("nameEdit"));
    nameEdit->setGeometry(QRect(530, 40, 200, 25));
    nameEdit->setLayoutDirection(Qt::LeftToRight);

    mCardText = new QTextEdit(MCardDialog);
    mCardText->setObjectName(QString::fromUtf8("mCardText"));
    mCardText->setGeometry(QRect(200, 75, 580, 475));

    mMatTree = new QTreeWidget(MCardDialog);
    mMatTree->setGeometry(QRect(10, 40, 180, 510));
    mMatTree->setObjectName(QString::fromUtf8("mMatTree"));
    mMatTree->setHeaderLabel(QString("Material"));

  /*  QTreeWidgetItem *pItem1 = new QTreeWidgetItem(mMatTree,QStringList(QString("Mat 1")));
    QTreeWidgetItem *pItem2 = new QTreeWidgetItem(mMatTree,QStringList(QString("Mat 2")));
    QTreeWidgetItem *pItem3 = new QTreeWidgetItem(mMatTree,QStringList(QString("Mat 3")));
    QTreeWidgetItem *pItem4 = new QTreeWidgetItem(mMatTree,QStringList(QString("Mat 4")));
    QTreeWidgetItem *pItem5 = new QTreeWidgetItem(mMatTree,QStringList(QString("Mat 5")));
    QTreeWidgetItem *pItem6 = new QTreeWidgetItem(mMatTree,QStringList(QString("Mat 6")));
    QTreeWidgetItem *pItem7 = new QTreeWidgetItem(mMatTree,QStringList(QString("Mat 7")));*/

    retranslateUi(MCardDialog);
    QObject::connect(okButton, SIGNAL(clicked()), MCardDialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), MCardDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(MCardDialog);
    } // setupUi

    void retranslateUi(QDialog *MCardDialog)
    {
    MCardDialog->setWindowTitle(QApplication::translate("MCardDialog", "Material Group", 0, QApplication::UnicodeUTF8));

    //saveXMLButton->setText(QApplication::translate("MCardDialog", "Save Material", 0, QApplication::UnicodeUTF8));
    //loadXMLButton->setText(QApplication::translate("MCardDialog", "Load Material", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("MCardDialog", "OK", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("MCardDialog", "Cancel", 0, QApplication::UnicodeUTF8));

    btnAddMat->setText(QApplication::translate("MCardDialog", "Add / Modify", 0, QApplication::UnicodeUTF8));
    //btnMdfMat->setText(QApplication::translate("MCardDialog", "Modify", 0, QApplication::UnicodeUTF8));
    btnDelMat->setText(QApplication::translate("MCardDialog", "Delete", 0, QApplication::UnicodeUTF8));

    label_1->setText(QApplication::translate("MCardDialog", "Material Card", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("MCardDialog", "ID:", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("MCardDialog", "Density:", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("MCardDialog", "Name:", 0, QApplication::UnicodeUTF8));
    //nameEdit->setText(QApplication::translate("MCardDialog", "", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(MCardDialog);
    } // retranslateUi

};

namespace Ui {
    class MCardDialog: public Ui_MCardDialog {};

} // namespace Ui

#endif // QMCCADGEOMETREE_MCARDDIALOG1_H

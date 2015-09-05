#ifndef QMCCADGEOMETREE_ADDTOMGDIALOG_H
#define QMCCADGEOMETREE_ADDTOMGDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>

class Ui_addToMGDialog
{
public:
    QPushButton *addButton;
    QListWidget *listWidget;
    QLabel *label;

    void setupUi(QDialog *addToMGDialog)
    {
    addToMGDialog->setObjectName(QString::fromUtf8("addToMGDialog"));
    addToMGDialog->resize(QSize(279, 287).expandedTo(addToMGDialog->minimumSizeHint()));
    addToMGDialog->setContextMenuPolicy(Qt::NoContextMenu);
    addButton = new QPushButton(addToMGDialog);
    addButton->setObjectName(QString::fromUtf8("addButton"));
    addButton->setGeometry(QRect(100, 250, 87, 29));
    listWidget = new QListWidget(addToMGDialog);
    listWidget->setObjectName(QString::fromUtf8("listWidget"));
    listWidget->setGeometry(QRect(10, 30, 261, 211));
    label = new QLabel(addToMGDialog);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(10, 10, 101, 17));
    retranslateUi(addToMGDialog);
    QObject::connect(addButton, SIGNAL(clicked()), addToMGDialog, SLOT(accept()));
    QMetaObject::connectSlotsByName(addToMGDialog);
    } // setupUi

    void retranslateUi(QDialog *addToMGDialog)
    {
    addToMGDialog->setWindowTitle(QApplication::translate("addToMGDialog", "Add To Material Group", 0, QApplication::UnicodeUTF8));
    addButton->setText(QApplication::translate("addToMGDialog", "Add", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("addToMGDialog", "Material Groups", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(addToMGDialog);
    } // retranslateUi

};

namespace Ui {
    class addToMGDialog: public Ui_addToMGDialog {};
} // namespace Ui

#endif // QMCCADGEOMETREE_ADDTOMGDIALOG_H

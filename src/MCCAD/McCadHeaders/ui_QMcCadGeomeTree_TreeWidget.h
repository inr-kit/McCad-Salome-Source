#ifndef QMCCADGEOMETREE_TREEWIDGET_GRID_H
#define QMCCADGEOMETREE_TREEWIDGET_GRID_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QTreeWidget>
#include <QtGui/QWidget>

class Ui_McTreeView
{
public:
    QGridLayout *gridLayout;
    QTreeWidget *myTreeWidget;
//    QTreeWidget *matTW;

    void setupUi(QWidget *McTreeView)
    {
    McTreeView->setObjectName(QString::fromUtf8("McTreeView"));
    McTreeView->resize(QSize(236, 762).expandedTo(McTreeView->minimumSizeHint()));
    McTreeView->setWindowIcon(QIcon());
    gridLayout = new QGridLayout(McTreeView);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    myTreeWidget = new QTreeWidget(McTreeView);
    myTreeWidget->setObjectName(QString::fromUtf8("myTreeWidget"));
    myTreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    myTreeWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
    myTreeWidget->setColumnCount(1);
    myTreeWidget->headerItem()->setText(0, QApplication::translate("Dialog", "Assemblies", 0, QApplication::UnicodeUTF8));
/*	matTW = new QTreeWidget(McTreeView);
    matTW->setObjectName(QString::fromUtf8("myTreeWidget"));
    matTW->setSelectionMode(QAbstractItemView::ExtendedSelection);
    matTW->setSelectionBehavior(QAbstractItemView::SelectItems);
    matTW->setColumnCount(1);
    matTW->headerItem()->setText(0, QApplication::translate("Dialog", "Materials", 0, QApplication::UnicodeUTF8));
*/

    gridLayout->addWidget(myTreeWidget, 0, 0, 1, 1);
//    gridLayout->addWidget(matTW, 1,0,1,1);
//    gridLayout->setRowStretch(0,2);
//    gridLayout->setRowStretch(1,1);
    //gridLayout->setRowStretch(1,0);

    retranslateUi(McTreeView);

    QMetaObject::connectSlotsByName(McTreeView);
    } // setupUi

    void retranslateUi(QWidget *McTreeView)
    {
    McTreeView->setWindowTitle(QApplication::translate("McTreeView", "GeomeTree", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(McTreeView);
    } // retranslateUi

};

namespace Ui {
    class McTreeView: public Ui_McTreeView {};
} // namespace Ui

#endif // QMCCADGEOMETREE_TREEWIDGET_GRID_H

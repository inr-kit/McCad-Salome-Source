#ifndef QMCCADGEOMETREE_TREEVIEW_H
#define QMCCADGEOMETREE_TREEVIEW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QTreeView>
#include <QtGui/QWidget>
#include <QtGui/QGridLayout>

class Ui_McTreeView
{
public:
    QTreeView *myTreeView;
    QGridLayout *gridLayout;
    void setupUi(QWidget *McTreeView)
    {
    McTreeView->setObjectName(QString::fromUtf8("McTreeView"));
    McTreeView->resize(QSize(1024, 350).expandedTo(McTreeView->minimumSizeHint()));
    McTreeView->setWindowIcon(QIcon());

    gridLayout = new QGridLayout(McTreeView);
	gridLayout->setSpacing(6);
	gridLayout->setMargin(0);
	gridLayout->setObjectName(QString::fromUtf8("gridLayout"));

    myTreeView = new QTreeView(McTreeView);
    myTreeView->setObjectName(QString::fromUtf8("myTreeView"));
    //myTreeView->setGeometry(QRect(0, 0, 300, 800));
    myTreeView->setMinimumSize(150,350);
    myTreeView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
    myTreeView->setAcceptDrops(true);
    myTreeView->setFrameShape(QFrame::StyledPanel);
    myTreeView->setFrameShadow(QFrame::Sunken);
    myTreeView->setLineWidth(1);
    myTreeView->setProperty("showDropIndicator", QVariant(true));
    myTreeView->setDragEnabled(true);
    myTreeView->setSelectionMode(QAbstractItemView::MultiSelection);
    myTreeView->setSelectionBehavior(QAbstractItemView::SelectItems);

    gridLayout->addWidget(myTreeView, 0, 0, 1, 1);

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

#endif // QMCCADGEOMETREE_TREEVIEW_H

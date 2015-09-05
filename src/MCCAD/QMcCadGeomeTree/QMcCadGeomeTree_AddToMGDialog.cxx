/*
 * QMcCadGeomeTree_AddToMGDialog.cxx
 *
 *  Created on: Mar 4, 2009
 *      Author: grosse
 */

#include <QMcCadGeomeTree_AddToMGDialog.h>
#include <QList>

QMcCadGeomeTree_AddToMGDialog::QMcCadGeomeTree_AddToMGDialog(QWidget* parent) : QDialog(parent)
{
	myAddDialog.setupUi(this);
	myListWidget = myAddDialog.listWidget;
}

void QMcCadGeomeTree_AddToMGDialog::PopulateList(QTreeWidgetItem* theTWI)
{
	myCurrentTWI = theTWI;

	for(int i=0; i < myCurrentTWI->childCount(); i++)
	{
		QTreeWidgetItem* curTWI = theTWI->child(i);
		myListWidget->addItem(curTWI->text(0));
		connect(myListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(accept()));
	}
}


QTreeWidgetItem* QMcCadGeomeTree_AddToMGDialog::GetSelected()
{
	QTreeWidgetItem* curTWI = NULL;

	for(int i=0; i < myCurrentTWI->childCount(); i++)
	{
		curTWI = myCurrentTWI->child(i);

		QList<QListWidgetItem*> selectedItems = myListWidget->selectedItems();

		if(selectedItems.count() < 1 )
			return NULL;

		QListWidgetItem* selected = selectedItems.at(0);

		if(curTWI->text(0) == selected->text())
			return curTWI;
	}

	return curTWI;
}

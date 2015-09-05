#ifndef _QMCCADGEOMETREE_ADDTOMGDIALOG_H
#define _QMCCADGEOMETREE_ADDTOMGDIALOG_H

#include <ui_QMcCadGeomeTree_AddToMGDialog.h>

#include <QMap>
#include <QWidget>
#include <QListWidget>
#include <QTreeWidgetItem>

class QMcCadGeomeTree_AddToMGDialog: public QDialog
{
	Q_OBJECT

public:

	QMcCadGeomeTree_AddToMGDialog(QWidget* parent=NULL);

	void PopulateList(QTreeWidgetItem* theTWI);
	QTreeWidgetItem* GetSelected();


private:
	Ui::addToMGDialog myAddDialog;
	QListWidget* myListWidget;
	QTreeWidgetItem* myCurrentTWI;

};

#endif // _QMCCADGEOMETREE_ADDTOMGDIALOG_H

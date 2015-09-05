/*
 * QMcCadGeomeTree_TreeView.hxx
 *
 *  Created on: Jan 21, 2009
 *      Author: grosse
 */

#ifndef QMCCADGEOMETREE_TREEWIDGET_HXX_
#define QMCCADGEOMETREE_TREEWIDGET_HXX_

#include "ui_QMcCadGeomeTree_TreeWidget.h"
#include <TDocStd_Document.hxx>
#include <QMcCad_Application.h>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDF_ChildIterator.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <TDataStd_Name.hxx>
#include <TCollection_AsciiString.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <QMcCad_Editor.h>
#include <AIS_ListOfInteractive.hxx>
#include <McCadMDReader_Reader.hxx>

#include <QMap>
#include <QWidget>
#include <QList>
#include <memory>

#include "../McCadDesign/McCadMatManageDlg.hxx"
//class AIS_ListOfInteractive;

using namespace std;
class McCadMatManageDlg;

class QMcCadGeomeTree_TreeWidget : public QWidget
{
	Q_OBJECT

public:
	QMcCadGeomeTree_TreeWidget(QWidget* theParent = NULL); //ctor
	void AddEditor( int theID, QString theName = "unnamed ");
	void RemoveEditor(int theID);
	void LoadDocument(int theID);
    void UpdateDocument(int theID, QList<TCollection_AsciiString> label_list);
	void UpdateEditorName(int theID);
	Standard_Boolean GetNameByIO(const Handle(AIS_InteractiveObject)& theIO, TCollection_AsciiString& theName);
	//void RedisplaySelected();
	void GetSelected(AIS_ListOfInteractive& ioList);
	void AddMaterialGroup(QString& qName, QString& densName, QString& theMCard);
    void UpdateTreeWidget(int theID, int iBegin, int &iEnd, TCollection_AsciiString MatGroupName);



public slots:
	void SlotSelectionChanged();
	void SlotVisuSelectionChanged();
	void SlotItemNameChanged(QTreeWidgetItem* item, int column);
	void SlotAddMaterialGroup();
	void SlotAddToMaterialGroup();
	void SlotEditMCard(QTreeWidgetItem* theTWI);
    void SlotEditMaterialCards();//Lei 14/10/2013
    void SlotOnClickRButton(const QPoint &point);

    void SlotSaveMaterials();
    void SlotLoadMaterials();

private:
	//functions
	TCollection_AsciiString GetItemLabelEntry(QTreeWidgetItem* theItem);
	QTreeWidgetItem* MakeNewTreeWidgetItem(const TCollection_AsciiString& theName, QTreeWidgetItem* theParent);
	Standard_Boolean IsRegistered(TCollection_AsciiString& theID);
	void LockSignalSelectionChanged();
	void UnlockSignalSelectionChanged();
	void Mcout(QString theMessage);
	QMcCad_Editor* GetEditor(int id=-1);
	TCollection_AsciiString QS2AS(QString& qStr);
	void LoadMCards();
	void LoadMaterialLink(TDF_Label& theLab);
	void AddToMaterialGroup(QTreeWidgetItem* selected, QList<QTreeWidgetItem*> itemList);
	QTreeWidgetItem* GetMaterialTWI();
	void RecursiveSelection(QTreeWidgetItem* theTWI, const QString& theName, QList<QTreeWidgetItem*>& theList);

	//fields
	Ui::McTreeView theTreeWidget;
	QTreeWidget* myTreeWidget;
	QTreeWidget* myMaterialTreeWidget;
	//QMap<int, QTreeWidgetItem*> myEditorItemMap;
	QMap<TCollection_AsciiString, QTreeWidgetItem*> myIDLabelItemMap; //maps the extended label id to a qtreewidgetitem
	bool mySignalSelectionChangedLocked;
	bool myReadDocument;
	bool myAddMaterialMembershipIsLocked;
	QString myDoubleClickedItemName;
	bool myItemDoubleClicked;
	bool myLock;

    QString m_strSelectedItem;
    auto_ptr<McCadMatManageDlg> pMcDialog;
    /**< Create a map to store the item on the tree corresponding to each editor*/
    QMap<TCollection_AsciiString, QTreeWidgetItem*> m_EditorItemMap;
};


#endif /* QMCCADGEOMETREE_TREEWIDGET_HXX_ */

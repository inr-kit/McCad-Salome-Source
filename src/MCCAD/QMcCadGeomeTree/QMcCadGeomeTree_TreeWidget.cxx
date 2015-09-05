/*
 * QMcCadGeomeTree_TreeView.cxx
 *
 *  Created on: Jan 21, 2009
 *      Author: grosse
 *
 *  Purpose : QMcCadGeomeTree_TreeWidget inherits QTreeWidget. It displays the shape names and additional information, like
 *  grouping, material and density numbers, surface and cell numbers after decomposition.
 *
 *	One of the basic fields of QMcCadGeomeTree_TreeWidget is the QMap which maps an extended label to an QTreeWidgetItem* .
 *	Extended Labels are TCollection_AsciiStrings which store the information about the editor they belong to and the label
 *	they correspond to in the Editors TDocStd_Document. The root label of Editor five e.g. would look like this:
 *	5_0:1:1:1:1. All digits in front of the '_' denote the Editor, all digits after the '_' corespond directly to the labels
 *	entry.
 */

#include <QMcCadGeomeTree_TreeWidget.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <ui_QMcCadGeomeTree_MCardDialog.h>
#include <TDataStd_AsciiString.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_TreeNode.hxx>
#include <QMcCadGeomeTree_AddToMGDialog.h>
#include <McCadXCAF_TDocShapeLabelIterator.hxx>
#include <McCadGUI_ProgressDialogPtr.hxx>
#include <Message_ProgressIndicator.hxx>
#include <TNaming_NamedShape.hxx>

#include <XCAFDoc_ColorTool.hxx>
#include <qmenu.h>

TCollection_AsciiString SHAPEROOT("_0:1:1:1:1"); //Shapes Label in xml document : OCC Standard!
TCollection_AsciiString MCARDROOT("0:1:1:1:6");


/**
 * Creates an QMcCadGeomeTree_TreeWidget
 */
QMcCadGeomeTree_TreeWidget::QMcCadGeomeTree_TreeWidget(QWidget* theParent) : QWidget(theParent)
{
	theTreeWidget.setupUi(this);
	myTreeWidget = theTreeWidget.myTreeWidget;
    //myTreeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //myTreeWidget->setRootIsDecorated(false);
    //myTreeWidget->setAlternatingRowColors(true);

    myTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);


	myTreeWidget->setSortingEnabled(false);
    //connect(myTreeWidget, SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(SlotOnClickRButton(const QPoint)));

    connect(myTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(SlotSelectionChanged()));
    connect(myTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(SlotItemNameChanged(QTreeWidgetItem*, int)));
	mySignalSelectionChangedLocked = false;

    //myAddMaterialMembershipIsLocked = false;
    //connect(myTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(SlotEditMCard(QTreeWidgetItem*)));

    connect(myTreeWidget, SIGNAL(customContextMenuRequested(QPoint)),this, SLOT(SlotOnClickRButton(const QPoint)));
    myItemDoubleClicked = false;
	myLock = false;

    pMcDialog.reset(new McCadMatManageDlg());
}

void QMcCadGeomeTree_TreeWidget::SlotOnClickRButton(const QPoint &point)
{
    QMenu *rButtonMenu = new QMenu(myTreeWidget);
    QModelIndex indexSelect = myTreeWidget->indexAt(point);
    QModelIndex indexParent = indexSelect.parent();

    QString QStrParentItem, QStrSelectItem;
    QStrParentItem = indexParent.data().toString();
    QStrSelectItem = indexSelect.data().toString();

    m_strSelectedItem = QStrSelectItem;
    if (QStrParentItem == "Geometry") // Lei xiugai
    {
        QAction *showAction = new QAction("&Edit Material",this);
        rButtonMenu->addAction(showAction);
        connect(showAction,SIGNAL(triggered()),this,SLOT(SlotEditMaterialCards()));
        rButtonMenu->addSeparator();
        QAction *showActionSave = new QAction("&Save Materials",this);
        QAction *showActionLoad = new QAction("&Load Materials",this);
        rButtonMenu->addAction(showActionSave);
        rButtonMenu->addAction(showActionLoad);
        connect(showActionSave,SIGNAL(triggered()),this,SLOT(SlotSaveMaterials()));
        connect(showActionLoad,SIGNAL(triggered()),this,SLOT(SlotLoadMaterials()));

        rButtonMenu->exec(QCursor::pos());
    }
}


/**
 * Register a new QMcCad_Editor. In this function a newly created Editor with the id theID is added to the treewidget
 */
void QMcCadGeomeTree_TreeWidget::AddEditor(int theID, QString theName)
{
	QTreeWidgetItem* newItem = new QTreeWidgetItem(myTreeWidget);
	newItem->setText(0,theName);
	newItem->setFlags(Qt::ItemIsSelectable);
	newItem->setTextColor(0,Qt::black);
	newItem->setFlags(Qt::ItemIsEnabled);

	QFont bigFont;
	bigFont.setBold(true);

	QIcon editorIcon(":images/document.png");

	newItem->setFont(0,bigFont);
	newItem->setIcon(0,editorIcon);
	myTreeWidget->setItemExpanded(newItem, true);
	newItem->setBackgroundColor(0,Qt::darkCyan);

	/*QTreeWidgetItem* mCards = new QTreeWidgetItem(newItem);
	mCards->setText(0,"Material Cards");
	mCards->setFont(0,bigFont);
	mCards->setTextColor(0,Qt::gray);*/
    /*QTreeWidgetItem* mGroups = new QTreeWidgetItem(newItem);
	mGroups->setText(0,"Material Groups");
	mGroups->setFont(0,bigFont);
	mGroups->setTextColor(0,Qt::gray);
    mGroups->setFlags(Qt::ItemIsEnabled);
	QTreeWidgetItem* voids = new QTreeWidgetItem(mGroups);
	voids->setText(0,"Void");
	voids->setTextColor(0,Qt::blue);
    voids->setFlags(Qt::ItemIsEnabled);*/
	QTreeWidgetItem* geometry = new QTreeWidgetItem(newItem);
	geometry->setText(0,"Geometry");
	geometry->setFont(0,bigFont);
	geometry->setFlags(Qt::ItemIsEnabled);
	myTreeWidget->setItemExpanded(geometry, true);

	TCollection_AsciiString id(theID);
	id += SHAPEROOT;
    m_EditorItemMap[id] = geometry;
	myReadDocument = false;
}


/**
 * Removes a registered Editor
 */
void QMcCadGeomeTree_TreeWidget::RemoveEditor(int theID)
{
    GetEditor(theID)->DeleteTreeItem();
    TCollection_AsciiString id(theID);
	id += SHAPEROOT;

    QTreeWidgetItem * pItemPrt = NULL;
    QTreeWidgetItem * pItemCld = NULL;
    pItemPrt = (m_EditorItemMap[id])->parent();
    pItemCld = (m_EditorItemMap[id]);
    delete pItemCld;
    delete pItemPrt;
    pItemPrt = NULL;
    pItemCld = NULL;

    QMap<TCollection_AsciiString, QTreeWidgetItem*>::const_iterator iter;
    m_EditorItemMap.erase(m_EditorItemMap.find(id));

}



/**
 * Load/Reload the TDocStd_Document from the QMcCad_Editor with the id theID. And show its content in the treewidget.
 */



/** ********************************************************************
* @brief
* @param
* @return
*
* @date 21/11/2013
* @author  Lei Lu
* @modify  Lei Lu
***********************************************************************/

void QMcCadGeomeTree_TreeWidget::LoadDocument(int theID)
{
	TCollection_AsciiString id = TCollection_AsciiString(theID) + SHAPEROOT;
    QTreeWidgetItem* shapeRoot = m_EditorItemMap[id];
	QTreeWidgetItem* currentRoot;

	//reload the document
	QMcCad_Editor* theEditor = GetEditor(theID);
	Handle(TDocStd_Document) theTDoc = theEditor->GetDocument()->GetTDoc();
	Handle(XCAFDoc_ShapeTool) sTool = XCAFDoc_DocumentTool::ShapeTool(theTDoc->Main());
	TCollection_AsciiString prefix(theID);
	prefix += "_";

	TDF_LabelSequence freeShapes;
	sTool->GetFreeShapes(freeShapes);

	Standard_GUID theNameGUID = TDataStd_Name::GetID();
	Handle(TDataStd_Name) tmpName;
	TCollection_AsciiString theName;
	TCollection_AsciiString labelName;

	//Load MCards
    //myReadDocument = true;
    //    LoadMCards();  // lei 11/10/2013
	myReadDocument = false;

    McCadXCAF_TDocShapeLabelIterator slIter(theTDoc); //iterate through all shapes in theTDoc

    QTreeWidgetItem* newRootItem = NULL;
    for(;slIter.More(); slIter.Next())
	{
		TDF_Label curL = slIter.Current();
		TDF_Tool::Entry(curL, labelName);
		if(curL.FindAttribute(theNameGUID, tmpName))
            theName = tmpName->Get();
        else
            theName = labelName;
        id = prefix + labelName;

		//cout << labelName.ToCString() << endl;
        if(sTool->IsTopLevel(curL)) // free shapes
		{
			currentRoot = shapeRoot;           
		}
        else
		{
			TDF_Label father = curL.Father();
			TCollection_AsciiString extLab;
			TDF_Tool::Entry(father, extLab);
			extLab.Prepend(prefix);
            currentRoot = myIDLabelItemMap[extLab];
        }

        TCollection_AsciiString strItemName("Solid ");
        strItemName += theName;
        QTreeWidgetItem* newItem = MakeNewTreeWidgetItem(strItemName, newRootItem);
        theEditor->AddTreeItem(id,newItem);
	}
}

// Added by Lei 25.10.2013
void QMcCadGeomeTree_TreeWidget::UpdateTreeWidget(int theID, int iBegin, int &iEnd, TCollection_AsciiString MatGroupName)
{
    TCollection_AsciiString id = TCollection_AsciiString(theID) + SHAPEROOT;
    QTreeWidgetItem* shapeRoot = m_EditorItemMap[id];
    QTreeWidgetItem* currentRoot;

    //reload the document
    QMcCad_Editor* theEditor = GetEditor(theID);
    Handle(TDocStd_Document) theTDoc = theEditor->GetDocument()->GetTDoc();
    Handle(XCAFDoc_ShapeTool) sTool = XCAFDoc_DocumentTool::ShapeTool(theTDoc->Main());
    TCollection_AsciiString prefix(theID);
    prefix += "_";

    TDF_LabelSequence freeShapes;
    sTool->GetFreeShapes(freeShapes);

    //Standard_GUID theNameGUID = TDataStd_Name::GetID();
    Handle(TDataStd_Name) tmpName;
    //TCollection_AsciiString theName;
    TCollection_AsciiString labelName;
    myReadDocument = false;

    McCadXCAF_TDocShapeLabelIterator slIter(theTDoc); //iterate through all shapes in theTDoc

    QTreeWidgetItem* newGroupItem = MakeNewTreeWidgetItem(MatGroupName, shapeRoot);
    QFont itemFont;
    itemFont.setBold(true);
    newGroupItem->setFont(0,itemFont);

    iEnd = slIter.NumberOfShapes();
    int index_solid = 0;
    for(int i = iBegin+1; i <= iEnd; i++)
    {
        TDF_Label curL = slIter.GetAt(i);
        TDF_Tool::Entry(curL, labelName);
        id = prefix + labelName;

        TDataStd_Name::Set(curL, MatGroupName);// for color and tree item, Lei Lu 20/11/2013
        if(sTool->IsTopLevel(curL)) // free shapes
        {
            currentRoot = shapeRoot;
        }
        else
        {
            TDF_Label father = curL.Father();
            TCollection_AsciiString extLab;
            TDF_Tool::Entry(father, extLab);
            extLab.Prepend(prefix);
            currentRoot = myIDLabelItemMap[extLab];
        }

        TCollection_AsciiString strItemName("Solid ");
        strItemName += TCollection_AsciiString(++index_solid);
        QTreeWidgetItem* newItem = MakeNewTreeWidgetItem(strItemName, newGroupItem);
        theEditor->AddTreeItem(id,newItem);
    }
}



/**
 * 	Updates the documet structure (deleted shapes will be removed, etc.)
 */
void QMcCadGeomeTree_TreeWidget::UpdateDocument(int theID, QList<TCollection_AsciiString> label_list)
{
	myLock=true;

    QMcCad_Editor *pEdit = GetEditor(theID);
    for(int i = 0; i < label_list.size(); i++)
    {
        pEdit->DeleteSelectedItem(label_list.at(i));
    }

	myLock=false;
   // LoadDocument(theID);
}


/**
 *
 */
//void QMcCadGeomeTree_TreeWidget::RedisplaySelected()
void QMcCadGeomeTree_TreeWidget::GetSelected(AIS_ListOfInteractive& ioList)
{
	QList<QTreeWidgetItem*> itemList;
	itemList = myTreeWidget->selectedItems();

	TCollection_AsciiString theExtendedEntry;
	Handle(AIS_InteractiveContext) theIC;

	for(int i=0; i<itemList.size(); i++)
	{
        theExtendedEntry = GetItemLabelEntry(itemList.value(i));

		if(theExtendedEntry.Length() < 1)
			continue;

		TCollection_AsciiString theNum = theExtendedEntry;
		theExtendedEntry = theNum.Split(theExtendedEntry.Search("_"));
		theNum.RemoveAll('_');

		if(!theNum.IsIntegerValue())
			Mcout("Not an integer value!!!");

		if(theExtendedEntry.IsEqual("0:1:1:1:1"))//skip root
			continue;

		Standard_Integer theID = theNum.IntegerValue();

		QMcCad_Editor* curEd = GetEditor(theID);
		theIC = curEd->GetDocument()->GetContext();

		AIS_ListOfInteractive colList;
		theIC->ObjectsInCollector(colList);

		Handle(TDF_Data) theData = curEd->GetDocument()->GetTDoc()->GetData();
		//Handle(TPrsStd_AISPresentation) aPres;
		TDF_Label curLab;
		TDF_Tool::Label(theData, theExtendedEntry, curLab);

		Handle(TNaming_NamedShape) nShp;

		if(! curLab.FindAttribute(TNaming_NamedShape::GetID(), nShp))
			continue;

		TopoDS_Shape refShp = nShp->Get(); // workaround ...

		for(AIS_ListIteratorOfListOfInteractive it(colList); it.More(); it.Next())
		{
			Handle(AIS_Shape) aisShp = Handle(AIS_Shape)::DownCast(it.Value());
			TopoDS_Shape curShp = aisShp->Shape();

			if(curShp.IsEqual(refShp))
				ioList.Append(it.Value());
		}
	}
}



/**
 *	Changes the Name of the Editor Node (QTreeWidgetItem) in myTreeWidget
 */
void QMcCadGeomeTree_TreeWidget::UpdateEditorName(int theID)
{
	TCollection_AsciiString id = TCollection_AsciiString(theID)+ SHAPEROOT;
	QTreeWidgetItem* theQTWI = myIDLabelItemMap[id];

	QMcCad_Editor* ed = GetEditor();
	TCollection_AsciiString theName = ed->GetDocument()->GetDocName();
	if(theName.Search("/")>0)
		theName.Remove(1,theName.SearchFromEnd("/"));
	if(theName.Search(".")>0)
		theName.Remove(theName.SearchFromEnd("."), theName.Length()-theName.SearchFromEnd(".")+1);

	QString qName(theName.ToCString());

	theQTWI->parent()->setText(0,qName);
}


/**
 * Retrieves name of selected InteractiveObject
 */
Standard_Boolean QMcCadGeomeTree_TreeWidget::GetNameByIO(const Handle(AIS_InteractiveObject)& theIO, TCollection_AsciiString& theName)
{
	if(!theIO->HasOwner()) //no TPrsStd_AISPresentation, no link to the document, no name
		return Standard_False;

	QMcCad_Editor* ed = GetEditor();
	TCollection_AsciiString edID(ed->ID());
	edID += "_";

	Handle(TPrsStd_AISPresentation) prs = Handle(TPrsStd_AISPresentation::DownCast(theIO->GetOwner()));

	TDF_Label theLab = prs->Label();
	TCollection_AsciiString labEntry;
	TDF_Tool::Entry(theLab, labEntry);

	labEntry.Prepend(edID);

	QTreeWidgetItem* curQTWI = myIDLabelItemMap[labEntry];
	QString qName =	curQTWI->text(0);
	theName = QS2AS(qName);

	return Standard_True;
}


/**
 *  Add a new Material Group, i.e., Name of Material, density and the MCard entry
 */
void QMcCadGeomeTree_TreeWidget::AddMaterialGroup(QString& qName, QString& densName, QString& theMCard)
{
    TCollection_AsciiString asciiMCard = QS2AS(theMCard);
	TCollection_AsciiString mName = QS2AS(qName);
	TCollection_AsciiString densString = QS2AS(densName);

	Standard_Real density(0.0);
	if(mName.Length()<1)
        mName= "Material Group";

	if(densString.IsRealValue())
		density = densString.RealValue();
	else
		Mcout("Density not a Real Value. Set to void!");

	Standard_Integer edID = GetEditor()->ID();
	TCollection_AsciiString extLab(edID);
	extLab+=SHAPEROOT;

	QTreeWidgetItem* geomTWI = myIDLabelItemMap[extLab];
	QTreeWidgetItem* matTWI = geomTWI->parent()->child(0);

	for(int i=0; i < matTWI->childCount(); i++)
	{
		if(qName == matTWI->child(i)->text(0))
			return;
	}

    //QTreeWidgetItem* newMCard = MakeNewTreeWidgetItem(mName, matTWI);
    //newMCard->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

	if(myReadDocument == true)
		return;

	//Set new MCard
	Handle(TDF_Data) theData = GetEditor()->GetDocument()->GetTDoc()->GetData();
	TDF_Label mLab;
	TCollection_AsciiString newLab = MCARDROOT;
	TDF_Tool::Label(theData, newLab, mLab, Standard_True);

	Standard_Integer newTag = mLab.NbChildren();
	newTag++;
	newLab += TCollection_AsciiString(":");
	newLab += TCollection_AsciiString(newTag);
	TDF_Tool::Label(theData, newLab, mLab, Standard_True);

	//add MCard to TDocument and register label in myIDLabelItemMap
	Handle(TDataStd_Name) tMName;
	tMName->Set(mLab,mName);

	TDataStd_AsciiString tMCard;
	tMCard.Set(mLab, asciiMCard);

	TDataStd_Real theDensity;
	theDensity.Set(mLab, density);
}

/*
 * ////////////////////////////////////////////////////////////////////////////
 * SLOTS
 */


/**
 * Report to QMcCad_Editor that the Selection has been changed. The shown items corespond to visualized shapes in the v3d_viewer
 * This slot is the one way of adjustment between the v3d_viewer and the qtreewidget. QMcCad_Editor/View should have a slot to adapt
 * the content of the QTreeWidget. That way there is only one valid selection of shapes.
 *
 * Modified by Lei lu at 22.11.2013
*/

void QMcCadGeomeTree_TreeWidget::SlotSelectionChanged()
{
    if(mySignalSelectionChangedLocked || myLock)
    {
        return;
    }

    QList<QTreeWidgetItem*> itemList = myTreeWidget->selectedItems(); // Get the selected items
    if(itemList.size()<1)
    {
        return;
    }

    Handle(AIS_InteractiveContext) theIC;
    TCollection_AsciiString strEntry;           // Get the entry of selected solid
	Standard_Boolean isFirst = Standard_True;	

	if(myItemDoubleClicked == true)
	{
		myItemDoubleClicked = false;
		return;
    }

    QMcCad_Editor *pEdit = GetEditor();         // Get the actived editor
    for(int i = 0; i < itemList.size(); i++)
    {
        QTreeWidgetItem* curTWI = itemList.value(i);
        strEntry = pEdit->GetSolidFromItem(curTWI);
        theIC = pEdit->GetDocument()->GetContext();

        if(strEntry.IsEmpty())
        {
            if(itemList.size()==1)
            {
               theIC->ClearSelected(Standard_True);
            }
            continue;
        }

        strEntry = strEntry.Split(strEntry.Search("_"));

		// Fill list with Interactive Objects in Viewer (including Collector)
		AIS_ListOfInteractive ioList;
		AIS_ListOfInteractive tmpList;

		theIC->ObjectsInCollector(tmpList);
		theIC->DisplayedObjects(ioList, 0);
		ioList.Append(tmpList);

        Handle(TDF_Data) theData = pEdit->GetDocument()->GetTDoc()->GetData();

		TDF_Label curLab;
        TDF_Tool::Label(theData, strEntry, curLab);

		Handle(TNaming_NamedShape) nShp;		
        if(!curLab.FindAttribute(TNaming_NamedShape::GetID(), nShp))
        {
			continue;
        }

        TopoDS_Shape refShp = nShp->Get();

		if(isFirst)
		{
			isFirst = Standard_False;
			theIC->ClearSelected(Standard_False);
		}

        for(AIS_ListIteratorOfListOfInteractive it(ioList); it.More(); it.Next())
        {
            if(theIC->IsSelected(it.Value()))
            {
                continue;
            }

            Handle(AIS_Shape) aisShp = Handle(AIS_Shape)::DownCast(it.Value());
            TopoDS_Shape curShp = aisShp->Shape();

            if(curShp.IsEqual(refShp))
            {
                theIC->AddOrRemoveSelected(it.Value(), 0);
            }
        }
	}
    theIC->UpdateCurrentViewer();
}


/**
 * Make selection consistent with the visual selection in V3d_View
 */
void QMcCadGeomeTree_TreeWidget::SlotVisuSelectionChanged()
{
	LockSignalSelectionChanged(); //make sure no update-recursion begins

	QMcCad_Editor* ed = GetEditor();
	Handle(AIS_InteractiveContext) theIC = ed->GetDocument()->GetContext();

//    int i = theIC->NbCurrents();
//    QMessageBox msg;
//    msg.setText(QString::number(i));
//    msg.exec();

	TCollection_AsciiString edID(ed->ID());
	edID += "_";

	Handle(TDocStd_Document) theTDoc = ed->GetDocument()->GetTDoc();
	Handle(XCAFDoc_ShapeTool) _sTool = XCAFDoc_DocumentTool::ShapeTool(theTDoc->Main());

	//deselect originaly selected items
	QList<QTreeWidgetItem*> initSelected = myTreeWidget->selectedItems();
	QList<QTreeWidgetItem*>::const_iterator iter;

	for(iter = initSelected.constBegin(); iter != initSelected.constEnd(); ++iter)
            myTreeWidget->setItemSelected(*iter,false);

	//select visual selected
	for(theIC->InitCurrent(); theIC->MoreCurrent(); theIC->NextCurrent())
	{
		Handle(AIS_InteractiveObject) curIO = theIC->Current();
		//Handle(TPrsStd_AISPresentation) curPres = Handle(TPrsStd_AISPresentation::DownCast(curIO->GetOwner()));

		Handle(AIS_Shape) aisShp = Handle(AIS_Shape)::DownCast(curIO);
		TopoDS_Shape theShp = aisShp->Shape();
		TDF_Label theLab = _sTool->FindShape(theShp,1);

		//= curPres->Label();
		TCollection_AsciiString labEntry;
		TDF_Tool::Entry(theLab, labEntry);

		labEntry.Prepend(edID);

        QTreeWidgetItem* curQTWI = ed->GetTreeItem(labEntry);
        //QTreeWidgetItem* curQTWI = myIDLabelItemMap[labEntry];
		myTreeWidget->setItemSelected(curQTWI, true);
	}

	UnlockSignalSelectionChanged();
}


/**
 *  Forwards name changing from tree widget item to TDocument
 */
void QMcCadGeomeTree_TreeWidget::SlotItemNameChanged(QTreeWidgetItem * item, int column)
{
    TCollection_AsciiString extLab = GetItemLabelEntry(item);
	if(extLab.Length() < 1 || extLab.Search('_') < 1)
		return;

	QString qName = item->text(0);

	if(qName.isNull())
		return;

	TCollection_AsciiString newName = QS2AS(qName);
	TCollection_AsciiString theNum = extLab;
	//cout << theNum.ToCString() << endl;
	extLab = theNum.Split(extLab.Search("_"));
	theNum.RemoveAll('_');

	if(!theNum.IsIntegerValue())
		Mcout("Not an integer value!!!");

	Standard_Integer edID = theNum.IntegerValue();
	QMcCad_Editor* ed = GetEditor(edID);

	Handle(TDataStd_Name) curName;
	Handle(TDF_Data) theData = ed->GetDocument()->GetTDoc()->GetData();
	TDF_Label theLab;
	TDF_Tool::Label(theData, extLab, theLab);

	if(theLab.FindAttribute(TDataStd_Name::GetID(),curName))
		theLab.ForgetAttribute(curName);

	TCollection_ExtendedString extName(newName.ToCString());
	curName->Set(extName);
	theLab.AddAttribute(curName);

	//update name in material section
	///////////////////////////////////////////
/*	QString oldName = myDoubleClickedItemName;
	oldName += " ";
	qName += " ";

	TCollection_AsciiString shpRoot = TCollection_AsciiString(edID) + SHAPEROOT;
    QTreeWidgetItem* matTWI = m_EditorItemMap[shpRoot]->parent()->child(0);

	int num = matTWI->childCount();

	for(int i = 0; i < num; i++)
	{
		QTreeWidgetItem* curChild = matTWI->child(i);
		for(int j = 0; j < curChild->childCount(); j++)
		{
			if(curChild->child(j)->text(0) == oldName)
			{
				curChild->child(j)->setText(0,qName);
				return;
			}
		}
    }*/
}

/**
 *	Add a new MCard to current Editor, see AddMaterialGroup() above
 */
void QMcCadGeomeTree_TreeWidget::SlotAddMaterialGroup()
{
    Ui::MCardDialog mcDialog;
	QDialog *theDialog = new QDialog;
	mcDialog.setupUi(theDialog);
    theDialog->exec();

    McCadMatManageDlg MatDlg(QMcCad_Application::GetAppMainWin());    
    MatDlg.exec();

    QString theMCard = mcDialog.mCardText->toPlainText();
    QString qName = mcDialog.nameEdit->text();
    QString densName = mcDialog.densityEdit->text();

    AddMaterialGroup(qName, densName, theMCard);
}


/**
 * 	Add selected geometries to material group
 */
void QMcCadGeomeTree_TreeWidget::SlotAddToMaterialGroup()
{
	QMcCadGeomeTree_AddToMGDialog theDialog(this);

	QTreeWidgetItem* matTWI = GetMaterialTWI();

	theDialog.PopulateList(matTWI);
	theDialog.exec();

	// Selected Material
	QTreeWidgetItem* selected = theDialog.GetSelected();
	if(selected == NULL)
		return;

	QList<QTreeWidgetItem*> itemList;
	itemList = myTreeWidget->selectedItems();

	// MCard-Label & Shape_Label
	AddToMaterialGroup(selected, itemList);
}



/**
 *  Load the material information, edit the detail composition. Lei 14/10/2013
 */
void QMcCadGeomeTree_TreeWidget::SlotEditMaterialCards()
{
    /*Ui::MCardDialog mcDialog;
    QDialog *theDialog = new QDialog;
    mcDialog.setupUi(theDialog);*/
   // QTreeWidgetItem* itemCur = this->
    if (pMcDialog.get() == NULL)
    {
       //McCadMatManageDlg *pDlg = new McCadMatManageDlg(QMcCad_Application::GetAppMainWin());
        pMcDialog.reset(new McCadMatManageDlg());
    }

    pMcDialog->SetCurrentEditor(QMcCad_Application::GetAppMainWin()->GetEditor());
    pMcDialog->SetEditGroup(m_strSelectedItem);       // Set which materia group being edited.
    pMcDialog->LoadMaterials(m_strSelectedItem);         // Load the material ID list.
    pMcDialog->exec();

    //QTreeWidgetItem* simItem = new QTreeWidgetItem();
    //simItem->setText(0,"TEST");
    //mcDialog.mMatTree->addTopLevelItem(simItem);

    //theDialog->exec();

    //QString theMCard = mcDialog.mCardText->toPlainText();
    //QString qName = mcDialog.nameEdit->text();
    //QString densName = mcDialog.densityEdit->text();

    //AddMaterialGroup(qName, densName, theMCard);
}

/** ******************************************************
 *  Save the materials into XML file. Lei Lu  18/11/2013
 *********************************************************/
void QMcCadGeomeTree_TreeWidget::SlotSaveMaterials()
{
    if (pMcDialog.get() == NULL)
    {
        pMcDialog.reset(new McCadMatManageDlg());
    }
    pMcDialog->SetCurrentEditor(QMcCad_Application::GetAppMainWin()->GetEditor());
    pMcDialog->SetEditGroup(m_strSelectedItem);       // Set which materia group being edited.
    QString strFileName;
    pMcDialog->SaveXMLFile(strFileName);

    QString strMsg = QString("Material file: \"%1\" is saved").arg(strFileName);
    Mcout(strMsg.toUtf8().constData());
}

/** ******************************************************
 *  Load the materials into XML file. Lei Lu  18/11/2013
 *********************************************************/
void QMcCadGeomeTree_TreeWidget::SlotLoadMaterials()
{
    if (pMcDialog.get() == NULL)
    {
        pMcDialog.reset(new McCadMatManageDlg());
    }
    pMcDialog->SetCurrentEditor(QMcCad_Application::GetAppMainWin()->GetEditor());
    pMcDialog->SetEditGroup(m_strSelectedItem);       // Set which materia group being edited.
    QString strFileName;
    pMcDialog->LoadXMLFile(strFileName);
    pMcDialog->LoadMaterials(m_strSelectedItem);         // Load the material ID list.
    pMcDialog->exec();

    QString strMsg = QString("Material file: \"%1\" is loaded").arg(strFileName);
    Mcout(strMsg.toUtf8().constData());
}


/**
 * Edit the MCard entries
 */
void QMcCadGeomeTree_TreeWidget::SlotEditMCard(QTreeWidgetItem* theTWI)
{
    myDoubleClickedItemName = theTWI->text(0);
	myItemDoubleClicked = true;

	Ui::MCardDialog mcDialog;
	QDialog *theDialog = new QDialog;
	mcDialog.setupUi(theDialog);

	QString qName = theTWI->text(0);
	TCollection_AsciiString mCardName = QS2AS(qName);

	//check children of MCARDROOT for mCardName
	Handle(TDF_Data) theData = GetEditor()->GetDocument()->GetTDoc()->GetData();

	TDF_Label mLab;
	TCollection_AsciiString newLab = MCARDROOT;
	TDF_Tool::Label(theData, newLab, mLab, Standard_True);

	TCollection_AsciiString anEntry;
	TDF_Tool::Entry(mLab, anEntry);

	for( TDF_ChildIterator it( mLab, Standard_False ); it.More(); it.Next() )
	{
		TDF_Label curLab = it.Value();
		Handle(TDataStd_Name) curName;

		if(!curLab.FindAttribute(TDataStd_Name::GetID(), curName))
			continue;

		TCollection_AsciiString extName(curName->Get());

		if(extName.IsEqual(mCardName))
		{
			Handle(TDataStd_AsciiString) theMCard;
			Handle(TDataStd_Real) theDensity;

			TCollection_AsciiString densStr;

			if(!curLab.FindAttribute(TDataStd_AsciiString::GetID(), theMCard))
				break;
			if(!curLab.FindAttribute(TDataStd_Real::GetID(),theDensity))
				densStr = "0.0";
			else
				densStr = TCollection_AsciiString(theDensity->Get());


			mcDialog.mCardText->setPlainText(theMCard->Get().ToCString());
			mcDialog.densityEdit->setText(densStr.ToCString());
			mcDialog.nameEdit->setText(qName);
			theDialog->exec();

			QString newQName = mcDialog.nameEdit->text();
			TCollection_AsciiString newName = QS2AS(newQName);

			QString qMCard = mcDialog.mCardText->toPlainText();
			TCollection_AsciiString asciiMCard = QS2AS(qMCard);

			QString qDens = mcDialog.densityEdit->text();
			TCollection_AsciiString aDens = QS2AS(qDens);

			if(!aDens.IsRealValue())
				aDens="0.0";

			curLab.ForgetAttribute(TDataStd_Name::GetID());
			curName->Set(curLab, newName);
			theTWI->setText(0,newQName);

			curLab.ForgetAttribute(TDataStd_AsciiString::GetID());
			theMCard->Set(curLab, asciiMCard);

			curLab.ForgetAttribute(TDataStd_Real::GetID());
			theDensity->Set(curLab, aDens.RealValue());

			break;
		}
	}
}

/**
 * # ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 *  Private Functions
 */
/**
 *	prohibits the update of visual selection due to change of Tree-selection
 */
void QMcCadGeomeTree_TreeWidget::LockSignalSelectionChanged()
{
	mySignalSelectionChangedLocked = true;
}


/**
 * allows the update of visual selection due to change of Tree-selection
 */
void QMcCadGeomeTree_TreeWidget::UnlockSignalSelectionChanged()
{
	mySignalSelectionChangedLocked = false;
}


/**
 *  Load MCards from TDoc
 */
void QMcCadGeomeTree_TreeWidget::LoadMCards()
{
	Handle(TDF_Data) theData = GetEditor()->GetDocument()->GetTDoc()->GetData();
	TDF_Label mLab;
	TCollection_AsciiString newLab = MCARDROOT;
	TDF_Tool::Label(theData, newLab, mLab, Standard_True);

	for( TDF_ChildIterator it( mLab, Standard_False ); it.More(); it.Next() )
	{
		TDF_Label curLab = it.Value();

		QString qName;
		QString qDens;
		QString qMCard;
		Handle(TDataStd_Name) theName;
		Handle(TDataStd_Real) theDens;
		Handle(TDataStd_AsciiString) theMCard;

		if(curLab.FindAttribute(TDataStd_Name::GetID(), theName))
		{
			TCollection_AsciiString aName(theName->Get());
			qName = aName.ToCString();
		}

		if(curLab.FindAttribute(TDataStd_Real::GetID(), theDens))
		{
			TCollection_AsciiString aDens(theDens->Get());
			qDens = aDens.ToCString();
		}

		if(curLab.FindAttribute(TDataStd_AsciiString::GetID(), theMCard))
			qMCard = theMCard->Get().ToCString();

		AddMaterialGroup(qName, qDens, qMCard);
	}
}


/**
 * Returns the extended label entry of a given QTreeWidgetItem*.
 */
TCollection_AsciiString QMcCadGeomeTree_TreeWidget::GetItemLabelEntry(QTreeWidgetItem* theItem)
{
	//find the editor the item belongs to
    //TCollection_AsciiString theID;
//    QMcCad_Editor *pEdit = GetEditor();
//    TCollection_AsciiString Label = pEdit->GetSolidFromItem(theItem);

//    QMessageBox msg;
//    msg.setText(Label.ToCString());
//    msg.exec();

//    return Label;


    //Lei Lu 21.11.2013
    QMap<TCollection_AsciiString, QTreeWidgetItem*>::const_iterator iter;
    TCollection_AsciiString aLabelEntry;
    for(iter = m_EditorItemMap.constBegin(); iter != m_EditorItemMap.constEnd(); ++iter)
    {
        if(iter.value() == theItem)
        {
            aLabelEntry = iter.key();
            return aLabelEntry;
        }
    }

    return aLabelEntry;
}


/**
 * Create new TreeWidgetItem with the name theName as child of theParent.
 */
QTreeWidgetItem* QMcCadGeomeTree_TreeWidget::MakeNewTreeWidgetItem(const TCollection_AsciiString& theName, QTreeWidgetItem* theParent)
{
	QString qName(theName.ToCString());
	QTreeWidgetItem* newItem = new QTreeWidgetItem(theParent);
	newItem->setText(0,qName);
	newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
	newItem->setTextColor(0,Qt::black);

	return newItem;
}


/**
 * Checks if an extended label is already mapped to a QTreeWidgetItem*
 */
Standard_Boolean QMcCadGeomeTree_TreeWidget::IsRegistered(TCollection_AsciiString& theID)
{    
    if (myIDLabelItemMap.contains(theID))
    {
        return Standard_True;
    }
    else
    {
        return Standard_False;
    }

    //for(iter = myIDLabelItemMap.constBegin(); iter != myIDLabelItemMap.constEnd(); ++iter)
    //	if(iter.key() == theID)
    //		return Standard_True;

    //return Standard_False;
}


/**
 * Calls QMcCad_Application::Mcout(). Prints information to the logwindow
 */
void QMcCadGeomeTree_TreeWidget::Mcout(QString theMessage)
{
	theMessage.prepend("QMcCadGeomeTree : ");
        QMcCad_Application::GetAppMainWin()->Mcout(theMessage.toUtf8().constData());
}


/**
 *  Returns Editor
 */
QMcCad_Editor* QMcCadGeomeTree_TreeWidget::GetEditor(int id)
{
	if(id == -1)
		return QMcCad_Application::GetAppMainWin()->GetEditor();
	else
		return QMcCad_Application::GetAppMainWin()->GetEditor(id);
}


/**
 *  Convert Qstring to TCollection_AsciiString
 */
TCollection_AsciiString QMcCadGeomeTree_TreeWidget::QS2AS(QString& qStr)
{
	char aStr[qStr.length()];
	QByteArray ba = qStr.toAscii();
	strcpy(aStr, ba.data());
	TCollection_AsciiString asciiStr(aStr);

	return asciiStr;
}


/**
 *  Set Link between registered shapes and material
 */
void QMcCadGeomeTree_TreeWidget::LoadMaterialLink(TDF_Label& theLab)
{
	myAddMaterialMembershipIsLocked = true;

	bool isVoid = true;

        QTreeWidgetItem* theMatTWI = NULL;
	QTreeWidgetItem* materialRootTWI = GetMaterialTWI();
	QList<QTreeWidgetItem*> listOfShapeTWI;
	Handle(TDataStd_TreeNode) aTreeNode;

	//find shape TWI
	TCollection_AsciiString prefix(GetEditor()->ID());
	prefix+="_";
	TCollection_AsciiString anEntry;
	TDF_Tool::Entry(theLab, anEntry);
	anEntry.Prepend(prefix);
	listOfShapeTWI.append(myIDLabelItemMap[anEntry]);

	if(theLab.FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), aTreeNode))
	{
		if(aTreeNode->HasFather())
		{
			TDF_Label matLab = aTreeNode->Father()->Label();
			Handle(TDataStd_Name) mName;

			//find material TWI
			if(matLab.FindAttribute(TDataStd_Name::GetID(),mName))
			{
				isVoid = false;
				TCollection_AsciiString aName(mName->Get());
				QString qName = aName.ToCString();

				for(int i=0; i < materialRootTWI->childCount(); i++)
				{
					if(materialRootTWI->child(i)->text(0) == qName)
						theMatTWI = materialRootTWI->child(i);
				}
			}
		}
	}

	if(isVoid)
		theMatTWI = materialRootTWI->child(0);

	if(theMatTWI!=NULL)
		AddToMaterialGroup(theMatTWI, listOfShapeTWI);

	myAddMaterialMembershipIsLocked = false;
}


/**
 *  Add List Of Selected Shapes to the selected Material Group
 */
void QMcCadGeomeTree_TreeWidget::AddToMaterialGroup(QTreeWidgetItem* selected, QList<QTreeWidgetItem*> itemList)
{
	QTreeWidgetItem* matTWI = GetMaterialTWI();

	Handle(TDF_Data) theData = GetEditor()->GetDocument()->GetTDoc()->GetData();
	TDF_Label mRootLab, mLab, sLab;
	TCollection_AsciiString newLab = MCARDROOT;
	TDF_Tool::Label(theData, newLab, mRootLab, Standard_True);

	for(int i=0; i<itemList.count(); i++)
	{
		// delete from other material groups and add to new one
		QString simName = itemList.at(i)->text(0);
		simName += " ";

		for(int j=0; j < matTWI->childCount(); j++) // material groups
		{
			QTreeWidgetItem* curMatGrp = matTWI->child(j);
			for(int k=0; k < curMatGrp->childCount(); k++)
			{
				QTreeWidgetItem* curMat = curMatGrp->child(k);
				if(curMat->text(0) == simName)
					delete curMat;
			}
		}

		QTreeWidgetItem* simItem = new QTreeWidgetItem(selected);
		simItem->setText(0,simName);
		simItem->setFlags(Qt::ItemIsEnabled); //not selectable

		//set material group membership to shapes in TDoc
		if(!myAddMaterialMembershipIsLocked)
		{
			TCollection_AsciiString extSLab = GetItemLabelEntry(itemList.at(i));
			extSLab.Remove(1,extSLab.Search("_"));
			TDF_Tool::Label(theData, extSLab, sLab, Standard_True);

			for(TDF_ChildIterator it(mRootLab, Standard_False); it.More(); it.Next())
			{
				Handle(TDataStd_Name) tName;
				if(it.Value().FindAttribute(TDataStd_Name::GetID(),tName))
				{
					TCollection_ExtendedString extName = tName->Get();
					TCollection_AsciiString aName(extName);
					QString qName = aName.ToCString();

					if(qName == selected->text(0))
						mLab = it.Value();
				}
			}

			if(!mLab.IsNull() && !sLab.IsNull()) // link shape label with MCard label
			{
				Handle(TDataStd_TreeNode) refNode, mainNode;
				mainNode = TDataStd_TreeNode::Set ( mLab, TDataStd_TreeNode::GetDefaultTreeID());
				refNode  = TDataStd_TreeNode::Set ( sLab, TDataStd_TreeNode::GetDefaultTreeID());
				refNode->Remove(); // abv: fix against bug in TreeNode::Append()
				mainNode->Prepend(refNode);
			}
		}
	}
}

/**
 * Recursively selects items with a specific name starting from theTWI
 */

void  QMcCadGeomeTree_TreeWidget::RecursiveSelection(QTreeWidgetItem* theTWI, const QString& theName, QList<QTreeWidgetItem*>& theList)
{
	if(theTWI->childCount() > 0)
	{
		for(int i=0; i<theTWI->childCount(); i++)
		{
			if(theTWI->child(i)->text(0) == theName)
			{
				// print rank in list of selected
				//cout << i+1 << endl;
				theList.append(theTWI->child(i));
			}
				//myTreeWidget->setItemSelected(theTWI->child(i),true);

			RecursiveSelection(theTWI->child(i), theName, theList);
		}
	}
}


/**
 *  Returns Material Root QTreeWidgetItem of current Editor
 */
QTreeWidgetItem* QMcCadGeomeTree_TreeWidget::GetMaterialTWI()
{
    Standard_Integer edID = GetEditor()->ID();
	TCollection_AsciiString extLab(edID);
	extLab+=SHAPEROOT;

	QTreeWidgetItem* geomTWI = myIDLabelItemMap[extLab];
	return geomTWI->parent()->child(0);
}




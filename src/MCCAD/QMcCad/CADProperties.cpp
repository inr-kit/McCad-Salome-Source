#include "CADProperties.h"
#include <QMcCad_Editor.h>

QString ToQString(const TCollection_ExtendedString& theName)
{
		TCollection_AsciiString aMessage(theName);
		char aStr[aMessage.Length()];
		char* ba = aMessage.ToCString();
		strcpy(aStr, ba);
		QString aqMessage(aStr);
		return aqMessage;
}


CADProperties::CADProperties (QWidget* theFather) : QWidget(theFather)
{
	uiProps.setupUi(this);

	myTransSlider = uiProps.transparencySlider;
	myTransLCD = uiProps.transparencyLCDNumber;
	myColorButton = uiProps.buttonColor;
	//myMaterialButton = uiProps.buttonMaterial;
	myMaterialCombo = uiProps.comboMaterial;
	myViewSolidButton = uiProps.buttonSolid;
	myViewWireFrameButton = uiProps.buttonWireFrame;
	myBackgroundColorButton = uiProps.toolBackgroundColor;

	FillMaterialBox();

	myTreeWidget = uiProps.treeView;
	myTreeWidget->setColumnCount(1);
	myTreeWidget->headerItem()->setText(0, QApplication::translate("Dialog", "Assemblies/Shapes", 0, QApplication::UnicodeUTF8));
	//myTreeWidget->setSelectionMode(QAbstractItemView::ContiguousSelection | QAbstractItemView::ExtendedSelection);


	//********** SIGNALS/SLOTS ************
	connect(myTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(SlotItemSelectionChanged()));
	connect(myMaterialCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(SlotMaterialSelected(QString)));
}

/*
 * Slots
 */
void CADProperties::SlotItemSelectionChanged()
{
	//retrieve labels and change selection in AISView
	//cout << "Selection changed\n";
	QMcCad_Editor* theEditor = QMcCad_Application::GetAppMainWin()->GetEditor();
	QList<QTreeWidgetItem* > selectedItems = myTreeWidget->selectedItems();

	for(int i=0; i<selectedItems.size(); i++)
	{
		TDF_Label curLab = myQTWItemLabelMap[selectedItems.at(i)]; // get label of selected item
		TCollection_AsciiString theLabString;
		TDF_Tool::Entry(curLab, theLabString);
		QMcCad_Application::GetAppMainWin()->Mcout(theLabString.ToCString());

		//check who is father:
		TDF_Label theFather = curLab.Father();
		TDF_Tool::Entry(theFather, theLabString);
		QMcCad_Application::GetAppMainWin()->Mcout(theLabString.ToCString());
	}
}

void CADProperties::SlotMaterialSelected(QString theMaterialName)
{
	std::string stdString = theMaterialName.toStdString();
	TCollection_AsciiString theAString(stdString.c_str());

	Graphic3d_NameOfMaterial theNOM = Graphic3d_NOM_DEFAULT;

	if(theAString.IsEqual("Aluminium"))
		theNOM = Graphic3d_NOM_ALUMINIUM;
	if(theAString.IsEqual("Brass"))
		theNOM = Graphic3d_NOM_BRASS;
	if(theAString.IsEqual("Bronze"))
		theNOM = Graphic3d_NOM_BRONZE;
	if(theAString.IsEqual("Chrome"))
		theNOM = Graphic3d_NOM_CHROME;
	if(theAString.IsEqual("Copper"))
		theNOM = Graphic3d_NOM_COPPER;
	if(theAString.IsEqual("Gold"))
		theNOM = Graphic3d_NOM_GOLD;
	if(theAString.IsEqual("Jade"))
		theNOM = Graphic3d_NOM_JADE;
	if(theAString.IsEqual("Metalized"))
		theNOM = Graphic3d_NOM_METALIZED;
	if(theAString.IsEqual("Neon_Gnc"))
		theNOM = Graphic3d_NOM_NEON_GNC;
	if(theAString.IsEqual("Neon_Phc"))
		theNOM = Graphic3d_NOM_NEON_PHC;
	if(theAString.IsEqual("Pewter"))
		theNOM = Graphic3d_NOM_PEWTER;
	if(theAString.IsEqual("Plaster"))
		theNOM = Graphic3d_NOM_PLASTER;
	if(theAString.IsEqual("Plastic"))
		theNOM = Graphic3d_NOM_PLASTIC;
	if(theAString.IsEqual("Satin"))
		theNOM = Graphic3d_NOM_SATIN;
	if(theAString.IsEqual("Silver"))
		theNOM = Graphic3d_NOM_SILVER;
	if(theAString.IsEqual("Shiny_Plastic"))
		theNOM = Graphic3d_NOM_SHINY_PLASTIC;
	if(theAString.IsEqual("Steel"))
		theNOM = Graphic3d_NOM_STEEL;
	if(theAString.IsEqual("Stone"))
		theNOM = Graphic3d_NOM_STONE;

	emit MaterialSelected(theNOM);
}

/*
 * Tree Widget Stuff
 */
QTreeWidget* CADProperties::GetTreeWidget()
{
	return myTreeWidget;
}

void CADProperties::NewEditorAdded(const int theEditorID, const QString& theName)
{
	QTreeWidgetItem* newItem = new QTreeWidgetItem(myTreeWidget);
	newItem->setText(0,theName);
	newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
	newItem->setTextColor(0, Qt::black);
	myEditorTreeMap[theEditorID] = newItem;
}

void CADProperties::SetModelName(const int theEditorID, const QString& theName)
{
	(myEditorTreeMap[theEditorID])->setText(0, theName);
}

void CADProperties::EditorClosed(const int theEditorID)
{
	delete myEditorTreeMap[theEditorID];
}


void CADProperties::AddAssembliesToTree(int theEditorID, const Handle(TDocStd_Document) theTDoc )
{
	Handle(XCAFDoc_ShapeTool) theAssembly = XCAFDoc_DocumentTool::ShapeTool(theTDoc->Main());
	TDF_LabelSequence theAssemblyLabelSeq;
	theAssembly->GetFreeShapes(theAssemblyLabelSeq);

	for(int j=1; j<=theAssemblyLabelSeq.Length(); j++)
	{
		TCollection_AsciiString theLabString;

		TDF_Label theLab = theAssemblyLabelSeq.Value(j);
		TDF_Tool::Entry(theLab, theLabString);

		//test if label is already bound
		if(myLabelList.contains(theLab))
			continue;

		//register label
		myLabelList.append(theLab);


		Handle(TDataStd_Name) theName = new TDataStd_Name;

		Standard_GUID theNameGUID = TDataStd_Name::GetID();
		Standard_Boolean hasName = theLab.FindAttribute(theNameGUID, theName);
		Standard_Boolean hasChildren = theLab.HasChild();

		//QTreeWidgetItem* assemblyPtr;
		//QTreeWidgetItem* partPtr;

		if(hasName)
		{
			//cout << theLabString.ToCString() << "  :  " << theName->Get() << endl;
			Standard_Integer numColon = CountNumberOfColons(theLabString);

			QTreeWidgetItem* assemblyPtr = new QTreeWidgetItem(myEditorTreeMap[theEditorID]);
			assemblyPtr->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
			assemblyPtr->setText(0,ToQString(theName->Get()));
			assemblyPtr->setTextColor(0, Qt::black);

			//bind label
			myQTWItemLabelMap[assemblyPtr] = theLab;

			if(hasChildren)
			{
				TDF_ChildIterator it;

				int cnt(0);
				for(it.Initialize(theLab); it.More(); it.Next())
				{
					cnt++;
					QTreeWidgetItem* partPtr = new QTreeWidgetItem(assemblyPtr);
					partPtr->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

					if(it.Value().FindAttribute(theNameGUID, theName)) //has Name
						partPtr->setText(0,ToQString(theName->Get()));
					else
					{
						QString defName("part ");
						QString theNum;
						theNum.setNum(cnt);
						defName.append(theNum);
						partPtr->setText(0,defName);
					}

					partPtr->setTextColor(0, Qt::black);
					myQTWItemLabelMap[partPtr] = it.Value();
					myLabelList.append(it.Value());
					myLabelList.append(theAssemblyLabelSeq.Value(j+cnt));
				}
			}
		}
		else
		{
			QTreeWidgetItem* assemblyPtr = new QTreeWidgetItem(myEditorTreeMap[theEditorID]);
			assemblyPtr->setText(0,"assembly");
			assemblyPtr->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
			//bind label
			myQTWItemLabelMap[assemblyPtr] = theLab;

			if(hasChildren)
			{
				TDF_ChildIterator it;

				int cnt(0);
				for(it.Initialize(theLab); it.More(); it.Next())
				{
					cnt++;
					QTreeWidgetItem* partPtr = new QTreeWidgetItem(assemblyPtr);

					if(it.Value().FindAttribute(theNameGUID, theName)) //has Name
						partPtr->setText(0,ToQString(theName->Get()));
					else
					{
						QString defName("part ");
						defName.append(QString(cnt));
						partPtr->setText(0,defName);
					}
					partPtr->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
					partPtr->setTextColor(0, Qt::black);
					myQTWItemLabelMap[partPtr] = it.Value();
					myLabelList.append(it.Value());
					myLabelList.append(theAssemblyLabelSeq.Value(j+cnt));
				}
			}
		}
	}
}


Standard_Integer CADProperties::CountNumberOfColons(const TCollection_AsciiString theLabString)
{
	Standard_Integer numb(0);
	Standard_Integer colPos(0);

	TCollection_AsciiString tmpStr = theLabString;
	colPos = tmpStr.Search(":");

	while(colPos > 0)
	{
		tmpStr.Remove(1, colPos);
		colPos = tmpStr.Search(":");
		numb++;
	}

	return numb;
}


/*
 *
 */

QSlider* CADProperties::GetSlider()
{
	return myTransSlider;
}

QToolButton* CADProperties::GetColorButton()
{
	return myColorButton;
}

QComboBox* CADProperties::GetMaterialCombo()
{
	return myMaterialCombo;
}

QToolButton* CADProperties::GetViewSolidButton()
{
	return myViewSolidButton;
}

QToolButton* CADProperties::GetViewWireFrameButton()
{
	return myViewWireFrameButton;
}

QToolButton* CADProperties::GetBackgroundColorButton()
{
	return myBackgroundColorButton;
}

void CADProperties::ResetSlider()
{
	myTransSlider->setValue(0);
	myTransLCD->display(0);
}

void CADProperties::FillMaterialBox()
{
	myMaterialCombo->addItem("Shape Material");
	myMaterialCombo->addItem("Aluminium");
	myMaterialCombo->addItem("Brass");
	myMaterialCombo->addItem("Bronze");
	myMaterialCombo->addItem("Chrome");
	myMaterialCombo->addItem("Copper");
	myMaterialCombo->addItem("Gold");
	myMaterialCombo->addItem("Jade");
	myMaterialCombo->addItem("Metalized");
	myMaterialCombo->addItem("Neon_Gnc");
	myMaterialCombo->addItem("Neon_Phc");
	myMaterialCombo->addItem("Pewter");
	myMaterialCombo->addItem("Plaster");
	myMaterialCombo->addItem("Plastic");
	myMaterialCombo->addItem("Satin");
	myMaterialCombo->addItem("Silver");
	myMaterialCombo->addItem("Shiny_Plastic");
	myMaterialCombo->addItem("Steel");
	myMaterialCombo->addItem("Stone");
}

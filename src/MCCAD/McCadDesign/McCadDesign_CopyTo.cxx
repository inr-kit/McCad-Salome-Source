/*
 * McCadDesign_MakeCone.cxx
 *
 *  Created on: Apr 21, 2009
 *      Author: grosse
 */

#include <McCadDesign_CopyTo.hxx>
#include <TDataStd_Name.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <QMcCadGeomeTree_TreeWidget.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDataStd_Name.hxx>
#include <McCadDesign_MoveTo.hxx>
#include <TNaming_CopyShape.hxx>
#include <TColStd_IndexedDataMapOfTransientTransient.hxx>


McCadDesign_CopyTo::McCadDesign_CopyTo(QWidget* theParent) : QDialog(theParent), McCadDesign_Tool()
{
	theCopyToDialog.setupUi(this);

	connect(theCopyToDialog.okButton, SIGNAL(clicked()), this, SLOT(CreateCopy()));
}


void McCadDesign_CopyTo::CreateCopy()
{
	myOriX = GetRealValue(theCopyToDialog.coX->text());
	myOriY = GetRealValue(theCopyToDialog.coY->text());
	myOriZ = GetRealValue(theCopyToDialog.coZ->text());

	TCollection_AsciiString namePrefix("Copy of ");

	AIS_ListOfInteractive listOfSelected;
	Handle(AIS_InteractiveContext) ic = Editor()->GetDocument()->GetContext();
	for(ic->InitCurrent();ic->MoreCurrent(); ic->NextCurrent())
		listOfSelected.Append(ic->Current());

	//QMcCad_Application::GetAppMainWin()->GetTreeWidget()->GetSelected(listOfSelected);
	AIS_ListIteratorOfListOfInteractive it(listOfSelected);

	for(; it.More(); it.Next())
	{
		Handle(AIS_InteractiveObject) curIO = it.Value();
		Handle(TPrsStd_AISPresentation) curPres = Handle(TPrsStd_AISPresentation::DownCast(curIO->GetOwner()));

		TDF_Label theLab = curPres->Label();
		Handle(TNaming_NamedShape) nmdShp;
		if(!theLab.FindAttribute(TNaming_NamedShape::GetID(), nmdShp))
			continue;

		Handle(TDataStd_Name) theName;
		theLab.FindAttribute(TDataStd_Name::GetID(),theName);
		TCollection_AsciiString asciiName(namePrefix);
		asciiName+=TCollection_AsciiString(theName->Get());

		TopoDS_Shape theShape = nmdShp->Get();//BRepBuilderAPI_Copy(nmdShp->Get()).Shape();
		TopoDS_Shape newShape;
		TColStd_IndexedDataMapOfTransientTransient aMap;
		TNaming_CopyShape::CopyTool(theShape, aMap, newShape);
		/*TopLoc_Location newLoc = newShape.Location();
		gp_Trsf theTrsf = newLoc.Transformation();

		theTrsf.SetValues(theTrsf.Value(1,1), theTrsf.Value(1,2), theTrsf.Value(1,3), myOriX,
						  theTrsf.Value(2,1), theTrsf.Value(2,2), theTrsf.Value(2,3), myOriY,
						  theTrsf.Value(3,1), theTrsf.Value(3,2), theTrsf.Value(3,3), myOriZ,
						  1e-7, 1e-7);

		newLoc = TopLoc_Location(theTrsf);
		newShape.Location(newLoc);*/

		Handle(TDocStd_Document) theTDoc = Editor()->GetDocument()->GetTDoc();
		Handle(XCAFDoc_ShapeTool) sTool = XCAFDoc_DocumentTool::ShapeTool(theTDoc->Main());

		TDF_Label newLab = sTool->AddShape(newShape, 0,0);
		TDataStd_Name::Set(newLab, asciiName);

		McCadDesign_MoveTo mover(NULL);
		mover.PerformMove(newShape, gp_Pnt(myOriX, myOriY, myOriZ));
		sTool->SetShape(newLab, newShape);

	}

	Editor()->UpdateView();
	QMcCad_Application::GetAppMainWin()->GetTreeWidget()->LoadDocument(Editor()->ID());

}

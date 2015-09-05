/*
 * McCadDesign_MakeCone.cxx
 *
 *  Created on: Apr 21, 2009
 *      Author: grosse
 */

#include <McCadDesign_MoveTo.hxx>
#include <TDataStd_Name.hxx>
#include <QMcCadGeomeTree_TreeWidget.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDataStd_Name.hxx>
#include <TopLoc_Datum3D.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <TDataStd_BooleanList.hxx>

McCadDesign_MoveTo::McCadDesign_MoveTo(QWidget* theParent) : QDialog(theParent), McCadDesign_Tool()
{
	theMoveToDialog.setupUi(this);

	connect(theMoveToDialog.okButton, SIGNAL(clicked()), this, SLOT(CreateMove()));
	connect(theMoveToDialog.coX, SIGNAL(textEdited(QString)), this, SLOT(ConfirmMoveToPnt(QString)));
	connect(theMoveToDialog.coY, SIGNAL(textEdited(QString)), this, SLOT(ConfirmMoveToPnt(QString)));
	connect(theMoveToDialog.coZ, SIGNAL(textEdited(QString)), this, SLOT(ConfirmMoveToPnt(QString)));
	myOriX = 0;
	myOriY = 0;
	myOriZ = 0;
	myDistX = 0;
	myDistY = 0;
	myDistZ = 0;
	myMoveToPnt = Standard_False;
	myExternCall = Standard_True;
}


void McCadDesign_MoveTo::CreateMove()
{
	myExternCall = Standard_False;
	myOriX = GetRealValue(theMoveToDialog.coX->text());
	myOriY = GetRealValue(theMoveToDialog.coY->text());
	myOriZ = GetRealValue(theMoveToDialog.coZ->text());
	myDistX = GetRealValue(theMoveToDialog.disX->text());
	myDistY = GetRealValue(theMoveToDialog.disY->text());
	myDistZ = GetRealValue(theMoveToDialog.disZ->text());

	AIS_ListOfInteractive listOfSelected;
	Handle(AIS_InteractiveContext) ic = Editor()->GetDocument()->GetContext();
	for(ic->InitCurrent();ic->MoreCurrent(); ic->NextCurrent())
		listOfSelected.Append(ic->Current());

	AIS_ListIteratorOfListOfInteractive it(listOfSelected);

	Handle(TDocStd_Document) theTDoc = Editor()->GetDocument()->GetTDoc();
	Handle(XCAFDoc_ShapeTool) sTool = XCAFDoc_DocumentTool::ShapeTool(theTDoc->Main());

	for(; it.More(); it.Next())
	{
		Handle(AIS_InteractiveObject) curIO = it.Value();
		/*Handle(TPrsStd_AISPresentation) curPres = Handle(TPrsStd_AISPresentation::DownCast(curIO->GetOwner()));

		TDF_Label theLab = curPres->Label();
		Handle(TNaming_NamedShape) nmdShp;
		if(!theLab.FindAttribute(TNaming_NamedShape::GetID(), nmdShp))
			continue;

		TopoDS_Shape movedShape = nmdShp->Get();

		ic->Clear(curPres->GetAIS(), 0);*/

		Handle(AIS_Shape) aisShp = Handle(AIS_Shape)::DownCast(curIO);
		TopoDS_Shape theShp = aisShp->Shape();
		TDF_Label theLab = sTool->FindShape(theShp, 1);

		PerformMove(theShp, gp_Pnt(myOriX, myOriY, myOriZ));

		sTool->SetShape(theLab, theShp);
		//remove registered flag in order 2 update view
		theLab.ForgetAttribute(TDataStd_BooleanList::GetID());
		ic->Remove(curIO, 0);

		//theLab.ForgetAttribute(TPrsStd_AISPresentation::GetID());
	}

	Editor()->UpdateView();
}

void McCadDesign_MoveTo::ConfirmMoveToPnt(const QString& dummyStr )
{
	myMoveToPnt = Standard_True;
}

void McCadDesign_MoveTo::PerformMove(TopoDS_Shape& theShp, gp_Pnt locPnt)
{
	TopLoc_Location oldLoc = theShp.Location();
	gp_Trsf oldTrsf = oldLoc.Transformation();
	gp_XYZ transl = oldTrsf.TranslationPart();

	if(!myMoveToPnt && !myExternCall)
	{
		locPnt.SetX(transl.X() + myDistX);
		locPnt.SetY(transl.Y() + myDistY);
		locPnt.SetZ(transl.Z() + myDistZ);
	}

	// handle rotation of shape
	gp_Vec translation(locPnt.X(), locPnt.Y(), locPnt.Z());
	gp_XYZ rotAxis;
	Standard_Real rotAngle;
	oldTrsf.GetRotation(rotAxis, rotAngle);
	gp_Dir tmpDir(rotAxis);
	gp_Ax1 tmpAxis(locPnt, tmpDir);
	gp_Trsf newTrsf;

	if(Abs(rotAxis.X()) > 1e-7 || Abs(rotAxis.Y()) > 1e-7 || Abs(rotAxis.Z()) > 1e-7)
		newTrsf.SetRotation(tmpAxis, rotAngle);

	newTrsf.SetTranslationPart(translation);
	//theTrsf.SetTranslationPart(translation);
	TopLoc_Location newLoc(newTrsf);
	theShp.Location(newLoc);
}


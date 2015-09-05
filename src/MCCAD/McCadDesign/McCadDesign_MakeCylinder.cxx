/*
 * McCadDesign_MakeCylinder.cxx
 *
 *  Created on: Apr 21, 2009
 *      Author: grosse
 */

#include <McCadDesign_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <TDataStd_Name.hxx>
#include <QMcCadGeomeTree_TreeWidget.hxx>
#include <McCadDesign_MoveTo.hxx>


McCadDesign_MakeCylinder::McCadDesign_MakeCylinder(QWidget* theParent) : QDialog(theParent), McCadDesign_Tool()
{
	theCylinderDialog.setupUi(this);

	connect(theCylinderDialog.okButton, SIGNAL(clicked()), this, SLOT(CreateCylinder()));
}


void McCadDesign_MakeCylinder::CreateCylinder()
{
	myOriX = GetRealValue(theCylinderDialog.oriX->text());
	myOriY = GetRealValue(theCylinderDialog.oriY->text());
	myOriZ = GetRealValue(theCylinderDialog.oriZ->text());

	myDirX = GetRealValue(theCylinderDialog.dirX->text());
	myDirY = GetRealValue(theCylinderDialog.dirY->text());
	myDirZ = GetRealValue(theCylinderDialog.dirZ->text());

	myRadius = GetRealValue(theCylinderDialog.radius->text());
	myHeight = GetRealValue(theCylinderDialog.height->text());

	if(myRadius <= 0 || myHeight == 0)
	{
		Editor()->Mcout("NULL dimension in cylinder build!!!");
		return;
	}

	gp_Pnt ori(0, 0, 0);
	gp_Dir dir(myDirX, myDirY, myDirZ);
	gp_Ax2 theAx(ori, dir);

	TopoDS_Shape newCylinder = BRepPrimAPI_MakeCylinder(theAx, myRadius, myHeight).Solid();

	Handle(TDocStd_Document) theTDoc = Editor()->GetDocument()->GetTDoc();
	Handle(XCAFDoc_ShapeTool) sTool = XCAFDoc_DocumentTool::ShapeTool(theTDoc->Main());

	TDF_Label newLab = sTool->AddShape(newCylinder, 0,0);
	TCollection_AsciiString theName("new Cylinder");
	TDataStd_Name::Set(newLab, theName);

	if(myOriX != 0 || myOriY != 0 || myOriZ != 0)
	{
		McCadDesign_MoveTo mover(NULL);
		mover.PerformMove(newCylinder, gp_Pnt(myOriX, myOriY, myOriZ));
		sTool->SetShape(newLab, newCylinder);
	}

	Editor()->UpdateView();
	QMcCad_Application::GetAppMainWin()->GetTreeWidget()->LoadDocument(Editor()->ID());

}


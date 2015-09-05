/*
 * McCadDesign_MakeCone.cxx
 *
 *  Created on: Apr 21, 2009
 *      Author: grosse
 */

#include <McCadDesign_MakeCone.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <TDataStd_Name.hxx>
#include <QMcCadGeomeTree_TreeWidget.hxx>
#include <ShapeFix_Shape.hxx>
#include <McCadDesign_MoveTo.hxx>


McCadDesign_MakeCone::McCadDesign_MakeCone(QWidget* theParent) : QDialog(theParent), McCadDesign_Tool()
{
	theConeDialog.setupUi(this);

	connect(theConeDialog.okButton, SIGNAL(clicked()), this, SLOT(CreateCone()));
}

void McCadDesign_MakeCone::CreateCone()
{
	myOriX = GetRealValue(theConeDialog.oriX->text());
	myOriY = GetRealValue(theConeDialog.oriY->text());
	myOriZ = GetRealValue(theConeDialog.oriZ->text());

	myDirX = GetRealValue(theConeDialog.dirX->text());
	myDirY = GetRealValue(theConeDialog.dirY->text());
	myDirZ = GetRealValue(theConeDialog.dirZ->text());

	myRmin = GetRealValue(theConeDialog.radius->text());
	myRmaj = GetRealValue(theConeDialog.radius_2->text());
	myHeight = GetRealValue(theConeDialog.height->text());

	if(myRmaj <= 0 || myHeight == 0)
	{
		Editor()->Mcout("NULL dimension in Cone build!!!");
		return;
	}
	if(myRmaj == myRmin)
	{
		Editor()->Mcout("Major radius and minor radius are identical!!!");
		return;
	}

	gp_Pnt ori(0, 0, 0);
	gp_Dir dir(myDirX, myDirY, myDirZ);
	gp_Ax2 theAx(ori, dir);

	TopoDS_Shape newCone = BRepPrimAPI_MakeCone(theAx,myRmaj, myRmin, myHeight).Solid();

	if(myRmin < 1e-5) // fix shape
	{
		Handle(ShapeFix_Shape) fixer = new ShapeFix_Shape(newCone);
		fixer->Perform();
		if(fixer->Status(ShapeExtend_DONE))
			newCone = fixer->Shape();
	}

	Handle(TDocStd_Document) theTDoc = Editor()->GetDocument()->GetTDoc();
	Handle(XCAFDoc_ShapeTool) sTool = XCAFDoc_DocumentTool::ShapeTool(theTDoc->Main());

	TDF_Label newLab = sTool->AddShape(newCone, 0,0);
	TCollection_AsciiString theName("new Cone");
	TDataStd_Name::Set(newLab, theName);

	if(myOriX != 0 || myOriY != 0 || myOriZ != 0)
	{
		McCadDesign_MoveTo mover(NULL);
		mover.PerformMove(newCone, gp_Pnt(myOriX, myOriY, myOriZ));
		sTool->SetShape(newLab, newCone);
	}


	Editor()->UpdateView();
	QMcCad_Application::GetAppMainWin()->GetTreeWidget()->LoadDocument(Editor()->ID());

}

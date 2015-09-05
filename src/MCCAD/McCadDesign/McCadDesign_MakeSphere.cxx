/*
 * McCadDesign_MakeCone.cxx
 *
 *  Created on: Apr 21, 2009
 *      Author: grosse
 */

#include <McCadDesign_MakeSphere.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <TDataStd_Name.hxx>
#include <QMcCadGeomeTree_TreeWidget.hxx>
#include <McCadDesign_MoveTo.hxx>


McCadDesign_MakeSphere::McCadDesign_MakeSphere(QWidget* theParent) : QDialog(theParent), McCadDesign_Tool()
{
	theSphereDialog.setupUi(this);

	connect(theSphereDialog.okButton, SIGNAL(clicked()), this, SLOT(CreateSphere()));
}


void McCadDesign_MakeSphere::CreateSphere()
{
	myOriX = GetRealValue(theSphereDialog.oriX->text());
	myOriY = GetRealValue(theSphereDialog.oriY->text());
	myOriZ = GetRealValue(theSphereDialog.oriZ->text());

	myRadius = GetRealValue(theSphereDialog.radius->text());

	if(myRadius <= 0)
	{
		Editor()->Mcout("NULL dimension in Sphere build!!!");
		return;
	}

	gp_Pnt ori(myOriX, myOriY, myOriZ);

	TopoDS_Shape newSphere = BRepPrimAPI_MakeSphere(ori, myRadius).Solid();

	Handle(TDocStd_Document) theTDoc = Editor()->GetDocument()->GetTDoc();
	Handle(XCAFDoc_ShapeTool) sTool = XCAFDoc_DocumentTool::ShapeTool(theTDoc->Main());

	TDF_Label newLab = sTool->AddShape(newSphere, 0,0);
	TCollection_AsciiString theName("new Sphere");
	TDataStd_Name::Set(newLab, theName);

	Editor()->UpdateView();
	QMcCad_Application::GetAppMainWin()->GetTreeWidget()->LoadDocument(Editor()->ID());

}


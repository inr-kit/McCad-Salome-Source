/*
 * McCadDesign_MakeBox.cxx
 *
 *  Created on: Apr 21, 2009
 *      Author: grosse
 */
#include <BRepPrimAPI_MakeBox.hxx>
#include <McCadDesign_MakeBox.hxx>
#include <TDataStd_Name.hxx>
#include <QMcCadGeomeTree_TreeWidget.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <McCadDesign_MoveTo.hxx>

McCadDesign_MakeBox::McCadDesign_MakeBox(QWidget* theParent) : QDialog(theParent)
{
	theBoxDialog.setupUi(this);

	connect(theBoxDialog.okButton, SIGNAL(clicked()), this, SLOT(CreateBox()));
}


/**
 *
 * SLOTS
 */

void McCadDesign_MakeBox::CreateBox()
{
	myOriX = GetRealValue(theBoxDialog.Ori_X->text());
	myOriY = GetRealValue(theBoxDialog.Ori_Y->text());
	myOriZ = GetRealValue(theBoxDialog.Ori_Z->text());
	myDimX = GetRealValue(theBoxDialog.Dim_X->text());
	myDimY = GetRealValue(theBoxDialog.Dim_Y->text());
	myDimZ = GetRealValue(theBoxDialog.Dim_Z->text());

	if(myDimX == 0.0 )
	{
		myEditor->Mcout("Creation of box with NULL dimension not possible!!!");
		return;
	}
	if(myDimY == 0.0)
		myDimY = myDimX;
	if(myDimZ == 0.0)
		myDimZ = myDimX;

	gp_Pnt ori(0, 0, 0);
	TopoDS_Shape newBox = BRepPrimAPI_MakeBox(ori, myDimX, myDimY, myDimZ).Solid();

	Handle(TDocStd_Document) theTDoc = Editor()->GetDocument()->GetTDoc();
	Handle(XCAFDoc_ShapeTool) sTool = XCAFDoc_DocumentTool::ShapeTool(theTDoc->Main());
	TDF_Label newLab = sTool->AddShape(newBox, 0,0);

	TCollection_AsciiString theName("new Box");
	TDataStd_Name::Set(newLab, theName);

	if(myOriX != 0 || myOriY != 0 || myOriZ != 0)
	{
		McCadDesign_MoveTo mover(NULL);
		mover.PerformMove(newBox, gp_Pnt(myOriX, myOriY, myOriZ));
		sTool->SetShape(newLab, newBox);
	}

	Editor()->UpdateView();
	QMcCad_Application::GetAppMainWin()->GetTreeWidget()->LoadDocument(Editor()->ID());
}



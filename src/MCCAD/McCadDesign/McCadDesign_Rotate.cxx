/*
 * McCadDesign_MakeCone.cxx
 *
 *  Created on: Apr 21, 2009
 *      Author: grosse
 */

#include <McCadDesign_Rotate.hxx>
#include <TDataStd_Name.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <QMcCadGeomeTree_TreeWidget.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_BooleanList.hxx>

McCadDesign_Rotate::McCadDesign_Rotate(QWidget* theParent) : QDialog(theParent), McCadDesign_Tool()
{
    theRotateDialog.setupUi(this);

    connect(theRotateDialog.okButton, SIGNAL(clicked()), this, SLOT(Rotate()));
    connect(theRotateDialog.radButton, SIGNAL(toggled(bool)), this, SLOT(SwitchRadDeg(bool)));
    connect(theRotateDialog.angle, SIGNAL(editingFinished()), this, SLOT(UpdateAngle()));
    myAngle = 0.;
    myIsRad = false;
}


void McCadDesign_Rotate::Rotate()
{
    myOriX = GetRealValue(theRotateDialog.axX->text());
    myOriY = GetRealValue(theRotateDialog.axY->text());
    myOriZ = GetRealValue(theRotateDialog.axZ->text());

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
        Handle(AIS_Shape) aisShp = Handle(AIS_Shape)::DownCast(curIO);
        TopoDS_Shape theShape = aisShp->Shape();
        TDF_Label theLab = sTool->FindShape(theShape,1);
        theLab.ForgetAttribute(TDataStd_BooleanList::GetID());
        ic->Remove(curIO, 0);

        TopLoc_Location newLoc = theShape.Location();
        gp_Trsf theTrsf = newLoc.Transformation();

        //extract rotation
        gp_XYZ rotAxis;
        Standard_Real rotAngle;
        theTrsf.GetRotation(rotAxis, rotAngle);

        //rotate
        gp_Mat oldRot;
        oldRot.SetRotation(rotAxis, rotAngle);
        gp_Mat addRot;
        gp_XYZ addRotAxis(myOriX, myOriY, myOriZ);
        addRot.SetRotation(addRotAxis, myAngle);
        addRot *= oldRot;

        theTrsf.SetValues(addRot.Value(1,1), addRot.Value(1,2), addRot.Value(1,3), theTrsf.Value(1,4),
                                          addRot.Value(2,1), addRot.Value(2,2), addRot.Value(2,3), theTrsf.Value(2,4),
                                          addRot.Value(3,1), addRot.Value(3,2), addRot.Value(3,3), theTrsf.Value(3,4),
                                          1e-7, 1e-7);

        newLoc = TopLoc_Location(theTrsf);
        theShape.Location(newLoc);

        sTool->SetShape(theLab,theShape);
    }
    Editor()->UpdateView();
}

void McCadDesign_Rotate::SwitchRadDeg(bool theRadState)
{
    if(theRadState)
    {
            theRotateDialog.degButton->setChecked(false);
            myIsRad = true;
    }
    else
    {
            theRotateDialog.radButton->setChecked(false);
            myIsRad = false;
    }

    TCollection_AsciiString angleName(myAngle);

    if(!theRadState)
            angleName = TCollection_AsciiString(180. * myAngle/M_PI);

    theRotateDialog.angle->setText(angleName.ToCString());
}

void McCadDesign_Rotate::UpdateAngle()
{
    Standard_Real val = GetRealValue(theRotateDialog.angle->text());

    if(!myIsRad)
        val = M_PI * val/180.;

    myAngle = val;
}



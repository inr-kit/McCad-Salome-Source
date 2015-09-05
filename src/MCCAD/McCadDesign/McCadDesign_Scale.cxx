/*
 * McCadDesign_MakeCone.cxx
 *
 *  Created on: Apr 21, 2009
 *      Author: grosse
 */

#include <McCadDesign_Scale.hxx>
#include <TDataStd_Name.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <QMcCadGeomeTree_TreeWidget.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDataStd_Name.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_GTransform.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Mat.hxx>
#include <TDataStd_BooleanList.hxx>

McCadDesign_Scale::McCadDesign_Scale(QWidget* theParent) : QDialog(theParent), McCadDesign_Tool()
{
    theScaleDialog.setupUi(this);

    connect(theScaleDialog.okButton, SIGNAL(clicked()), this, SLOT(PerformScale()));
    connect(theScaleDialog.sf_X, SIGNAL(textEdited(QString)), this, SLOT(SetAxisDependent(QString)));
    connect(theScaleDialog.sf_Y, SIGNAL(textEdited(QString)), this, SLOT(SetAxisDependent(QString)));
    connect(theScaleDialog.sf_Z, SIGNAL(textEdited(QString)), this, SLOT(SetAxisDependent(QString)));
    mySf_X=1;
    mySf_Y=1;
    mySf_Z=1;
    mySf_All=1;
    myIsAll=Standard_True;
}


void McCadDesign_Scale::PerformScale()
{
    AIS_ListOfInteractive listOfSelected;
    Handle(AIS_InteractiveContext) ic = Editor()->GetDocument()->GetContext();
    for(ic->InitCurrent();ic->MoreCurrent(); ic->NextCurrent())
            listOfSelected.Append(ic->Current());

    //QMcCad_Application::GetAppMainWin()->GetTreeWidget()->GetSelected(listOfSelected);
    AIS_ListIteratorOfListOfInteractive it(listOfSelected);
    Handle(TDocStd_Document) theTDoc = Editor()->GetDocument()->GetTDoc();
    Handle(XCAFDoc_ShapeTool) sTool = XCAFDoc_DocumentTool::ShapeTool(theTDoc->Main());

    for(; it.More(); it.Next())
    {
        Handle(AIS_InteractiveObject) curIO = it.Value();
        Handle(AIS_Shape) aisShp = Handle(AIS_Shape)::DownCast(curIO);
        TopoDS_Shape theShp = aisShp->Shape();
        TDF_Label theLab = sTool->FindShape(theShp, 1);

        if(myIsAll) {
            mySf_All = GetRealValue(theScaleDialog.sf_All->text());

            if(mySf_All <= 0.0) {
                Editor()->Mcout("Invalid scaling factor! -- Factor <= 0");
                return;
            }
            mySf_X = mySf_All;
            mySf_Y = mySf_All;
            mySf_Z = mySf_All;
        }
        else {
            mySf_X = GetRealValue(theScaleDialog.sf_X->text());
            mySf_Y = GetRealValue(theScaleDialog.sf_Y->text());
            mySf_Z = GetRealValue(theScaleDialog.sf_Z->text());

            if(mySf_X <= 0 || mySf_Y <= 0 || mySf_Z <= 0.0) {
                Editor()->Mcout("Invalid scaling factor! -- Factor <= 0");
                return;
            }
        }

//        cout << mySf_X << " , " << mySf_Y << " , " << mySf_Z << endl;

        gp_Mat sclMat(mySf_X,      0,     0,
                           0, mySf_Y,     0,
                           0,      0, mySf_Z );

        gp_GTrsf theGTrsf;
        theGTrsf.SetVectorialPart(sclMat);

        BRepBuilderAPI_GTransform theScaler(theGTrsf);
        theScaler.Perform(theShp);
        theShp = theScaler.Shape();

        ic->Clear(curIO, 0);

        sTool->SetShape(theLab, theShp);
        theLab.ForgetAttribute(TDataStd_BooleanList::GetID());
    }

    Editor()->UpdateView();
    QMcCad_Application::GetAppMainWin()->GetTreeWidget()->LoadDocument(Editor()->ID());

}

void McCadDesign_Scale::SetAxisDependent(const QString& aString)
{
    myIsAll = Standard_False;
}

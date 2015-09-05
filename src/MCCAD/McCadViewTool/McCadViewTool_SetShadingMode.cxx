#include <McCadViewTool_SetShadingMode.ixx>
#include <V3d_View.hxx>
#include <AIS_DisplayMode.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_InteractiveObject.hxx>

#include <QMcCad_Application.h>
#include <QMcCadGeomeTree_TreeWidget.hxx>
#include <GeomAdaptor_Surface.hxx>

#include "../McCadMcVoid/IGeomFace.hxx"
#include "../McCadMcVoid/McCadGeomPlane.hxx"

McCadViewTool_SetShadingMode::McCadViewTool_SetShadingMode(const Handle(McCadCom_CasDocument)& theDoc, const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
  myDoc = theDoc;
  myView = theView;
  myState = theState;
  myUndoState = theUndoState;
  myRedoState = theRedoState;
  myID = McCadTool_FlatShade;
}

 void McCadViewTool_SetShadingMode::Destroy() 
{
}

 
Standard_Boolean McCadViewTool_SetShadingMode::IsNull()
{
 	return Standard_False;
} 
 
 void McCadViewTool_SetShadingMode::Execute() 
{	
    Handle(AIS_InteractiveContext) theContext = myDoc->GetContext();

    for(theContext->InitSelected();theContext->MoreSelected();theContext->NextSelected())
    {
        QMcCad_Application::GetAppMainWin()->Mcout("Yes In it");

        TopoDS_Shape shape = theContext->SelectedShape();
        TopoDS_Face face = TopoDS::Face(shape);
        //TopoDS_Face *pFace = new TopoDS_Face(face);

        if(face.ShapeType() == TopAbs_FACE)
        {
           QMcCad_Application::GetAppMainWin()->Mcout("Get the face");

           TopLoc_Location loc;
           Handle(Geom_Surface) geom_surface = BRep_Tool::Surface(face, loc);
           GeomAdaptor_Surface surf_adoptor(geom_surface);

           IGeomFace * pGeomFace = NULL;

           switch (surf_adoptor.GetType())
           {
               case GeomAbs_Plane:
               {
                    QMcCad_Application::GetAppMainWin()->Mcout("This is a plane");
                    pGeomFace = new McCadGeomPlane(surf_adoptor);
                    break;
               }
               case GeomAbs_Cylinder:
               {
                   QMcCad_Application::GetAppMainWin()->Mcout("This is a Cylinder");
                   break;
               }
               case GeomAbs_Cone:
               {
                   QMcCad_Application::GetAppMainWin()->Mcout("This is a Cone");
                   break;
               }
               case GeomAbs_Sphere:
               {
                   QMcCad_Application::GetAppMainWin()->Mcout("This is a Sphere");
                   break;
               }
               case GeomAbs_Torus:
               {
                   QMcCad_Application::GetAppMainWin()->Mcout("This is a Torus");
                   break;
               }
           }

           TCollection_AsciiString surf_info;
           surf_info = pGeomFace->GetExpression();

           QMcCad_Application::GetAppMainWin()->Mcout(surf_info.ToCString());


        }
    }

    theContext->UpdateCurrentViewer();
    Done();
}

 void McCadViewTool_SetShadingMode::UnExecute() 
{
}

 void McCadViewTool_SetShadingMode::Suspend() 
{
}

 void McCadViewTool_SetShadingMode::Resume() 
{
}

 void McCadViewTool_SetShadingMode::MousePressEvent(const McCadWin_MouseEvent& e) 
{
}

 void McCadViewTool_SetShadingMode::MouseReleaseEvent(const McCadWin_MouseEvent& e) 
{
}

 void McCadViewTool_SetShadingMode::MouseLeaveEvent() 
{
}

 void McCadViewTool_SetShadingMode::MouseEnterEvent() 
{
}


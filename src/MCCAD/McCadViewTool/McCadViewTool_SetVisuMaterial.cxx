#include <McCadViewTool_SetVisuMaterial.hxx>
#include <McCadViewTool_SetVisuMaterial.ixx>
#include <V3d_View.hxx>

McCadViewTool_SetVisuMaterial::McCadViewTool_SetVisuMaterial(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
	 myDoc = theDoc;
	 myView = theView;
	 myState = theState;
	 myUndoState = theUndoState;
	 myRedoState = theRedoState;
	 myID = McCadTool_ShapeMaterial;
}

void McCadViewTool_SetVisuMaterial::Destroy()
{
 return;
}

Standard_Boolean McCadViewTool_SetVisuMaterial::IsNull()
{
	return Standard_False;
}

void McCadViewTool_SetVisuMaterial::Execute()
{
  Handle(V3d_View) aView = myView->View();



  Done();
}

void McCadViewTool_SetVisuMaterial::UnExecute()
{
	  Done();
}

void McCadViewTool_SetVisuMaterial::Suspend()
{
  return;
}

void McCadViewTool_SetVisuMaterial::Resume()
{

  return;
}

void McCadViewTool_SetVisuMaterial::MousePressEvent(const McCadWin_MouseEvent& e)
{
   return;
}

void McCadViewTool_SetVisuMaterial::MouseReleaseEvent(const McCadWin_MouseEvent& e)
{
  return;
}

void McCadViewTool_SetVisuMaterial::MouseLeaveEvent()
{
  return;
}

void McCadViewTool_SetVisuMaterial::MouseEnterEvent()
{
 return;
}

void McCadViewTool_SetVisuMaterial::SetMaterialName(Graphic3d_NameOfMaterial theNOM)
{
	myNOM = theNOM;
}

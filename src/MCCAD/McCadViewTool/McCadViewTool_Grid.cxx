#include <McCadViewTool_Grid.ixx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

McCadViewTool_Grid::McCadViewTool_Grid(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
  myDoc = theDoc;
  myView = theView;
  myState = theState;
  myUndoState = theUndoState;
  myRedoState = theRedoState;
  myID = McCadTool_Grid;
}

void McCadViewTool_Grid::Destroy() 
{
  return;
}

Standard_Boolean McCadViewTool_Grid::IsNull()
{
	return Standard_False;
}

void McCadViewTool_Grid::Execute() 
{
	//get width and hight of the current view
	Handle(V3d_View) aView = myView->View();
	Standard_Real theWidth, theHeight;
	
	aView->Size(theWidth, theHeight);
	
	
	Handle(V3d_Viewer) aViewer = myDoc->GetContext()->CurrentViewer();
	aViewer->ActivateGrid(Aspect_GT_Rectangular, Aspect_GDM_Lines);
	aViewer->SetRectangularGridGraphicValues(theWidth + 200, theHeight + 200, 0);
	  
	Done();
}

void McCadViewTool_Grid::UnExecute() 
{
  return;
}

void McCadViewTool_Grid::Suspend() 
{
  return;
}

void McCadViewTool_Grid::Resume() 
{
  return;
}

void McCadViewTool_Grid::MousePressEvent(const McCadWin_MouseEvent& e) 
{
  return;
}

void McCadViewTool_Grid::MouseReleaseEvent(const McCadWin_MouseEvent& e) 
{ 
 return;
 
}

void McCadViewTool_Grid::MouseLeaveEvent() 
{
  return;
}

void McCadViewTool_Grid::MouseEnterEvent() 
{
  return;
}


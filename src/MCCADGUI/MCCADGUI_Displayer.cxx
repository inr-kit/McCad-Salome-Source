// Copyright (C) 2014-2015  KIT-INR/NK
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//

//

#include "MCCADGUI_Displayer.h"
#include "MCCADGUI.h"
#include "MCCAD_Constants.h"

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Application.h>
#include <SUIT_Study.h>
#include <SUIT_MessageBox.h>
#include "utilities.h"
#include <SalomeApp_Study.h>

#include <OCCViewer_ViewManager.h>
#include <SOCC_ViewModel.h>
#include <SOCC_Prs.h>
#include <SOCC_ViewWindow.h>

#include <SALOMEconfig.h>
#include <iostream>
#include CORBA_CLIENT_HEADER(MCCAD)
#include "MCCADGUI_DataObject.h"
#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>
#include <Handle_SALOME_AISShape.hxx>



#include <Material_Model.h>

#include <SUIT_Desktop.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_Session.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ResourceMgr.h>

#include <Basics_OCCTVersion.hxx>

#include <SalomeApp_Study.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <LightApp_DataObject.h>
#include <SalomeApp_TypeFilter.h>
#include <SalomeApp_Tools.h>

#include <SALOME_ListIteratorOfListIO.hxx>
#include <SALOME_ListIO.hxx>
#include <SALOME_Prs.h>

#include <SOCC_Prs.h>
#include <SOCC_ViewModel.h>

#include <SVTK_Prs.h>
#include <SVTK_ViewModel.h>



// OCCT Includes
#include <AIS_Drawer.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <StdSelect_TypeOfEdge.hxx>
#include <StdSelect_TypeOfFace.hxx>
#include <StdSelect_DisplayMode.hxx>
#include <TopoDS_Face.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <gp_Pln.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <TopoDS_Iterator.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopoDS.hxx>

#include <Prs3d_ShadingAspect.hxx>

#include <BRepMesh_IncrementalMesh.hxx>

static inline int getViewManagerId( SALOME_View* theViewFrame) {
  SUIT_ViewModel* aModel = dynamic_cast<SUIT_ViewModel*>(theViewFrame);
  SUIT_ViewManager* aViewMgr = 0;
  if (aModel != 0)
    aViewMgr = aModel->getViewManager();
  return ((aViewMgr == 0) ? -1 :aViewMgr->getGlobalId());
}


MCCADGUI_Displayer::MCCADGUI_Displayer(/*SalomeApp_Study *st*/)
{
//    if( st )
//      myApp = dynamic_cast<SalomeApp_Application*>( st->application() );
//    else
//      myApp = 0;

    /* Shading Color */
    SUIT_Session* session = SUIT_Session::session();
    SUIT_ResourceMgr* resMgr = session->resourceMgr();

    QColor col = resMgr->colorValue( "MCCAD", "shading_color", QColor( 255, 0, 0 ) );
    myShadingColor = SalomeApp_Tools::color( col );

    myDisplayMode = resMgr->integerValue("MCCAD", "display_mode", 0);
    myHasDisplayMode = false;

//    int aType = resMgr->integerValue("MCCAD", "type_of_marker", (int)Aspect_TOM_PLUS);
    myWidth = resMgr->integerValue("MCCAD", "edge_width", -1);
    myIsosWidth = resMgr->integerValue("MCCAD", "isolines_width", -1);

//    myTypeOfMarker = (Aspect_TypeOfMarker)(std::min((int)Aspect_TOM_RING3, std::max((int)Aspect_TOM_POINT, aType)));
//    myScaleOfMarker = (resMgr->integerValue("MCCAD", "marker_scale", 1)-(int)GEOM::MS_10)*0.5 + 1.0;
//    myScaleOfMarker = std::min(7.0, std::max(1., myScaleOfMarker));

    myColor = -1;
    // This color is used for shape displaying. If it is equal -1 then
    // default color is used.
//    myTexture = "";

    myWidth = -1;
//    myType = -1;

    myToActivate = true;
    myTransparency = 0.0;

    // This parameter is used for activisation/deactivisation of objects to be displayed

//    #if OCC_VERSION_LARGE > 0x06050100 // Functionnality available only in OCCT 6.5.2
//    // Activate parallel vizualisation only for testing purpose
//    // and if the corresponding env variable is set to 1
//    char* parallel_visu = getenv("PARALLEL_VISU");
//    if (parallel_visu && atoi(parallel_visu))
//    {
//      MESSAGE("Parallel visualisation on");
//      BRepMesh_IncrementalMesh::SetParallelDefault(Standard_True);
//    }
//    #endif

    myViewFrame = 0;
}

MCCADGUI_Displayer::~MCCADGUI_Displayer()
{
}

bool MCCADGUI_Displayer::canBeDisplayed( const QString& aEntry, const QString& viewer_type ) const
{
    QStringList es = aEntry.split( "_" );
    bool result = ( es[ 0 ] == "MCCADGUI" && es[ 1 ] != "root" &&viewer_type == SOCC_Viewer::Type() );
    return result; // Entry of an atom for sure
}

//=================================================================
/*!
 *  MCCADGUI_Displayer::Display
 *  Display interactive object in the current viewer
 */
//=================================================================
void MCCADGUI_Displayer::Display( const QString& aEntry,
                             const bool updateViewer,
                             SALOME_View* theViewFrame )
{
  SALOME_View* vf = theViewFrame ? theViewFrame : GetActiveView();
  if ( vf )
  {
    SALOME_Prs* prs = buildPresentation( aEntry, vf );

    if ( prs )
    {
      vf->BeforeDisplay( this, prs );
      vf->Display( prs );
      vf->AfterDisplay( this, prs );

      if ( updateViewer )
        vf->Repaint();

      int aMgrId = getViewManagerId(vf);
      SalomeApp_Study* aStudy = getStudy();
      aStudy->setObjectProperty(aMgrId, aEntry, MCCAD::propertyName( MCCAD::Visibility ), 1 );
      setVisibilityState(aEntry, Qtx::ShownState);

      delete prs;  // delete presentation because displayer is its owner
    }
  }
}


//=================================================================
/*!
 *  MCCADGUI_Displayer::Erase
 *  Erase interactive object in the current viewer
 */
//=================================================================
void MCCADGUI_Displayer::Erase( const QString& aEntry,
                            const bool forced,
                            const bool updateViewer,
                            SALOME_View* theViewFrame )
{
//  if ( theIO.IsNull() )
//    return;

  SALOME_View* vf = theViewFrame ? theViewFrame : GetActiveView();

  if ( vf ) {
    SALOME_Prs* prs = vf->CreatePrs( aEntry.toLatin1() );
    if ( prs ) {
      vf->BeforeErase( this, prs );
      vf->Erase( prs, forced );
      vf->AfterErase( this, prs );
      if ( updateViewer )
        vf->Repaint();
      delete prs;  // delete presentation because displayer is its owner

      int aMgrId = getViewManagerId(vf);
      SalomeApp_Study* aStudy = getStudy();
      aStudy->setObjectProperty(aMgrId, aEntry,MCCAD:: propertyName( MCCAD::Visibility ), 0 );

      setVisibilityState(aEntry, Qtx::HiddenState);
    }
  }
}

//=================================================================
/*!
 *  MCCADGUI_Displayer::Redisplay
 *  Redisplay (erase and then display again) interactive object
 *  in the current viewer
 */
//=================================================================
void MCCADGUI_Displayer::Redisplay(const QString& aEntry,
                                const bool updateViewer )
{
  // Remove the object permanently (<forced> == true)
  SUIT_Session* ses = SUIT_Session::session();
  SUIT_Application* app = ses->activeApplication();
  if ( app )
  {
    SUIT_Desktop* desk = app->desktop();
    QList<SUIT_ViewWindow*> wnds = desk->windows();
    SUIT_ViewWindow* wnd;
    QListIterator<SUIT_ViewWindow*> it( wnds );
    while ( it.hasNext() && (wnd = it.next()) )
    {
      SUIT_ViewManager* vman = wnd->getViewManager();
      if ( vman )
      {
        SUIT_ViewModel* vmodel = vman->getViewModel();
        if ( vmodel )
        {
          SALOME_View* view = dynamic_cast<SALOME_View*>(vmodel);
          if ( view )
          {
            if ( view->isVisible( datamodel()->findObjectIO(aEntry) ) || view == GetActiveView() )
            {
              Erase( aEntry, true, false, view );
              Display( aEntry, updateViewer, view );
            }
          }
        }
      }
    }
  }
}

//=================================================================
/*!
 *  MCCADGUI_Displayer::BeforeDisplay
 *  Called before displaying of pars. Close local context
 *  [ Reimplemented from SALOME_Displayer ]
 */
//=================================================================
void MCCADGUI_Displayer::BeforeDisplay( SALOME_View* v, const SALOME_OCCPrs* )
{
  SOCC_Viewer* vf = dynamic_cast<SOCC_Viewer*>( v );

  if ( vf )
  {
    Handle(AIS_InteractiveContext) ic = vf->getAISContext();
    if ( !ic.IsNull() )
    {
      if ( ic->HasOpenedContext() )
      ic->CloseAllContexts();
    }
  }
}



void MCCADGUI_Displayer::AfterDisplay( SALOME_View* v, const SALOME_OCCPrs* p )
{
  SalomeApp_Study* aStudy = getStudy();
  if (!aStudy) return;
  SOCC_Viewer* vf = dynamic_cast<SOCC_Viewer*>( v );
  if ( vf && !p->IsNull() ) {
    int aMgrId = getViewManagerId( vf );
    Handle(AIS_InteractiveContext) ic = vf->getAISContext();
    const SOCC_Prs* prs = dynamic_cast<const SOCC_Prs*>( p );
    if ( !ic.IsNull() && prs ) {
      AIS_ListOfInteractive objects;
      prs->GetObjects( objects );
      AIS_ListIteratorOfListOfInteractive it( objects );
      for ( ; it.More(); it.Next() ) {
        Handle(MCCAD_AISShape) sh = Handle(MCCAD_AISShape)::DownCast( it.Value() );
        if ( sh.IsNull() ) continue;
        Handle(SALOME_InteractiveObject) IO = sh->getIO();
        if ( IO.IsNull() ) continue;
        PropMap aPropMap = aStudy->getObjectPropMap( aMgrId, IO->getEntry() );
        if ( aPropMap.contains( MCCAD::propertyName( MCCAD::Transparency ) ) ) {
          double transparency = aPropMap.value(MCCAD:: propertyName( MCCAD:: Transparency )).toDouble();
          ic->SetTransparency( sh, transparency, true );
        }
      }
    }
  }
}

bool MCCADGUI_Displayer::isVisible(const QString & aEntry)
{
    bool res = false;
    SALOME_View* view = MCCADGUI_Displayer::GetActiveView();
    if ( view )
        res = view->isVisible( datamodel()->findObjectIO(aEntry ) );
    return res;
}


//=================================================================
/*!
 *  MCCADGUI_Displayer::Update
 *  Update OCC presentaion
 *  [ Reimplemented from SALOME_Displayer ]
 */
//=================================================================
void MCCADGUI_Displayer::Update( SALOME_OCCPrs* prs )
{
    SOCC_Prs* occPrs = dynamic_cast<SOCC_Prs*>( prs );
    SalomeApp_Study* study = getStudy();

    if ( !occPrs || myShape.IsNull() || !study )
      return;

    //from MCCADGUI_Displayer::update()
    // processing for usual geometry presentation
    //

    // if presentation is empty we try to create new one
    if ( occPrs->IsNull() )
    {
        // create presentation (specific for vectors)
        Handle(MCCAD_AISShape) AISShape = /*( myType == GEOM_VECTOR ) ? new GEOM_AISVector( myShape, "" )
                                                                   :*/ new MCCAD_AISShape ( myShape, "" );
        // update shape properties
        updateShapeProperties( AISShape, true );

        // add shape to the presentation
        occPrs->AddObject( AISShape );

        // In accordance with ToActivate() value object will be activated/deactivated
        // when it will be displayed
        occPrs->SetToActivate( ToActivate() );

      /*
       *  if ( AISShape->isTopLevel() && MCCAD_AISShape::topLevelDisplayMode() == MCCAD_AISShape::TopShowAdditionalWActor ) {
            // 21671: EDF 1829 GEOM : Bring to front selected objects (continuation):

            // create additional wireframe shape
            Handle(GEOM_TopWireframeShape) aWirePrs = new GEOM_TopWireframeShape(myShape);
            aWirePrs->SetWidth(AISShape->Width());
            if ( !myIO.IsNull() ) {
                aWirePrs->setIO( myIO );
                aWirePrs->SetOwner( myIO );
            }

            // add shape to the presentation
            occPrs->AddObject( aWirePrs );
        }
        */

    }
    // if presentation is found -> set again shape for it
    else
    {
        AIS_ListOfInteractive IOList;
        occPrs->GetObjects( IOList );
        AIS_ListIteratorOfListOfInteractive Iter( IOList );
        for ( ; Iter.More(); Iter.Next() )
        {
            Handle(MCCAD_AISShape) AISShape = Handle(MCCAD_AISShape)::DownCast( Iter.Value() );
            if ( AISShape.IsNull() )
                continue;

            // re-set shape (it might be changed)
            if ( AISShape->Shape() != myShape )
                AISShape->Set( myShape );

            // update shape properties
            updateShapeProperties( AISShape, false );

            // force updating
            AISShape->UpdateSelection();
            AISShape->SetToUpdate();
        }
    }

}

SalomeApp_Study* MCCADGUI_Displayer::getStudy() const
{
  return dynamic_cast<SalomeApp_Study*>( app()->activeStudy() );
}

//=================================================================
/*!
 *  MCCADGUI_Displayer::SetColor
 *  Set color for shape displaying. If it is equal -1 then default color is used.
 *  Available values are from Quantity_NameOfColor enumeration
 */
//=================================================================
void MCCADGUI_Displayer::SetColor( const int color )
{
  if ( color == -1 )
    UnsetColor();
  else
  {
    myColor = color;
    myShadingColor = Quantity_Color( (Quantity_NameOfColor)color );
  }
}

int MCCADGUI_Displayer::GetColor() const
{
  return myColor;
}

bool MCCADGUI_Displayer::HasColor() const
{
  return myColor != -1;
}

void MCCADGUI_Displayer::UnsetColor()
{
  myColor = -1;

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  QColor col = resMgr->colorValue( "MCCAD", "shading_color", QColor( 255, 0, 0 ) );
  myShadingColor = SalomeApp_Tools::color( col );
}


//=================================================================
/*!
 *  MCCADGUI_Displayer::SetWidth
 *  Set width of shape displaying. If it is equal -1 then default width is used.
 */
//=================================================================
void MCCADGUI_Displayer::SetWidth( const double width )
{
  myWidth = width;
}

double MCCADGUI_Displayer::GetWidth() const
{
  return myWidth;
}

bool MCCADGUI_Displayer::HasWidth() const
{
  return myWidth != -1;
}

void MCCADGUI_Displayer::UnsetWidth()
{
  myWidth = -1;
}


int MCCADGUI_Displayer::GetIsosWidth() const
{
  return myIsosWidth;
}

void MCCADGUI_Displayer::SetIsosWidth(const int width)
{
  myIsosWidth = width;
}

bool MCCADGUI_Displayer::HasIsosWidth() const
{
  return myIsosWidth != -1;
}


int MCCADGUI_Displayer::SetDisplayMode( const int theMode )
{
  int aPrevMode = myDisplayMode;
  if ( theMode != -1 ) {
    myDisplayMode = theMode;
    myHasDisplayMode = true;
  }
  else {
    UnsetDisplayMode();
  }
  return aPrevMode;
}

int MCCADGUI_Displayer::GetDisplayMode() const
{
  return myDisplayMode;
}

int MCCADGUI_Displayer::UnsetDisplayMode()
{
  int aPrevMode = myDisplayMode;
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  myDisplayMode = resMgr->integerValue( "MCCAD", "display_mode", 0 );
  myHasDisplayMode = false;
  return aPrevMode;
}

bool MCCADGUI_Displayer::HasDisplayMode() const
{
  return myHasDisplayMode;
}

////=================================================================
///*!
// *  MCCADGUI_Displayer::SetTransparency
// *  Set Transparency of shape displaying. If it is equal 0.0 then default Transparency is used.
// */
////=================================================================
//void MCCADGUI_Displayer::SetTransparency( const double Transparency )
//{
//  myTransparency = Transparency;
//}

//double MCCADGUI_Displayer::GetTransparency() const
//{
//  return myTransparency;
//}

//bool MCCADGUI_Displayer::HasTransparency() const
//{
//  return myTransparency != 0.0;
//}

//void MCCADGUI_Displayer::UnsetTransparency()
//{
//  myTransparency = 0.0;
//}



void MCCADGUI_Displayer::setTransparency(const QStringList& entries, const double transparency )
{
    myViewFrame = MCCADGUI_Displayer::GetActiveView();
    SalomeApp_Study* study = getStudy();
    if ( study && myViewFrame) {
        SUIT_ViewModel* viewModel = dynamic_cast<SUIT_ViewModel*>( myViewFrame );
        SUIT_ViewManager* viewMgr = ( viewModel != 0 ) ? viewModel->getViewManager() : 0;
        int viewId = ( viewMgr != 0 ) ? viewMgr->getGlobalId() : -1;

        if ( viewModel && viewId != -1 ) {
            // get properties from the study

            for ( QStringList::const_iterator it = entries.begin(), last = entries.end(); it != last; it++ )
            {
                MCCADGUI_DataObject * aObj = datamodel()->findObject(*it);
                if (!aObj)              {
                    SUIT_MessageBox::warning( app()->desktop(), QString("Warning"),QString( "Cannot find the object in this study!"));
                    return ;
                }
                //to see what it is the object
                if (aObj->isPart()) {
                    PropMap aPropMap = study->getObjectPropMap( viewId, aObj->entry() );
                    if ( aPropMap.contains( MCCAD::propertyName( MCCAD::Transparency ) ) )
                        study->setObjectProperty(viewId,aObj->entry() , MCCAD::propertyName( MCCAD:: Transparency ),  transparency);
                    Redisplay( aObj->entry().toLatin1(), /*updateviewer=*/true );
                }
                else if (aObj->isGroup() ) {
                    MCCADGUI_DataObject * bObj = dynamic_cast <MCCADGUI_DataObject *> (aObj->firstChild());
                    for (int i=0; i<aObj->childCount(); i++) {
                        PropMap aPropMap = study->getObjectPropMap( viewId, bObj->entry() );
                        if ( aPropMap.contains( MCCAD::propertyName( MCCAD:: Transparency ) ) )
                            study->setObjectProperty(viewId,bObj->entry() , MCCAD::propertyName( MCCAD:: Transparency ), transparency);
                        if (isVisible(bObj->entry()))
                            Redisplay( bObj->entry().toLatin1(), /*updateviewer=*/false );
                        else
                            Display( bObj->entry().toLatin1(), /*updateviewer=*/false, 0 );
                        bObj = dynamic_cast <MCCADGUI_DataObject *> (bObj->nextBrother());
                    }
                }
                else if (aObj->isComponent()) {
                    MCCADGUI_DataObject * bObj = dynamic_cast <MCCADGUI_DataObject *> (aObj->firstChild());
                    for (int i=0; i<aObj->childCount(); i++) {
                         MCCADGUI_DataObject * cObj =  dynamic_cast <MCCADGUI_DataObject *> (bObj->firstChild());
                         for (int j=0; j<bObj->childCount();  j++) {

                             PropMap aPropMap = study->getObjectPropMap( viewId, cObj->entry() );
                             if ( aPropMap.contains( MCCAD::propertyName( MCCAD:: Transparency ) ) )
                                 study->setObjectProperty(viewId,cObj->entry() , MCCAD::propertyName( MCCAD:: Transparency ),  transparency);
                             if (isVisible(cObj->entry()))
                                 Redisplay( cObj->entry().toLatin1(), /*updateviewer=*/false );
                             else
                                 Display( cObj->entry().toLatin1(), /*updateviewer=*/false, 0 );

                             cObj = dynamic_cast <MCCADGUI_DataObject *> (cObj->nextBrother());
                         }
                        bObj = dynamic_cast <MCCADGUI_DataObject *> (bObj->nextBrother());
                    }
                }
                else
                    MESSAGE("Cannot Redisplay this object!");
            }
        }
    }

}

/*!
 * \brief MCCADGUI_Displayer::getTransparency
 *  get the transparency value of the object
 * \param entry
 * \return
 */
double MCCADGUI_Displayer::getTransparency( const QString& entry )
{
    MCCADGUI_DataObject * aObj = datamodel()->findObject(entry);
    if (aObj->isGroup()) return -1.0;
    else if (aObj->isComponent()) return -1.0;
    else {
        myViewFrame = GetActiveView();
        SalomeApp_Study* study = getStudy();
        if ( study && myViewFrame) {
            SUIT_ViewModel* viewModel = dynamic_cast<SUIT_ViewModel*>( myViewFrame );
            SUIT_ViewManager* viewMgr = ( viewModel != 0 ) ? viewModel->getViewManager() : 0;
            int viewId = ( viewMgr != 0 ) ? viewMgr->getGlobalId() : -1;

            if ( viewModel && viewId != -1 ) {
                // get properties from the study
                PropMap aPropMap = study->getObjectPropMap( viewId, entry );
                if ( aPropMap.contains( MCCAD::propertyName( MCCAD:: Transparency ) ) ) {
                  return aPropMap.value(MCCAD::propertyName( MCCAD:: Transparency )).toDouble();
                }
            }
        }
        return -1.;
    }
}


//=================================================================
/*!
 *  MCCADGUI_Displayer::SetToActivate
 *  This method is used for activisation/deactivisation of objects to be displayed
 */
//=================================================================
void MCCADGUI_Displayer::SetToActivate( const bool toActivate )
{
  myToActivate = toActivate;
}
bool MCCADGUI_Displayer::ToActivate() const
{
  return myToActivate;
}

//=================================================================
/*!
 *  MCCADGUI_Displayer::clearTemporary
 *  Removes from selection temporary objects
 */
//=================================================================
void MCCADGUI_Displayer::clearTemporary( LightApp_SelectionMgr* theSelMgr )
{
  SALOME_ListIO selected, toSelect;
  theSelMgr->selectedObjects( selected );

  for (  SALOME_ListIteratorOfListIO it( selected ) ; it.More(); it.Next() ) {
    Handle(SALOME_InteractiveObject) io = it.Value();
    if ( !io.IsNull() && io->hasEntry() && strncmp( io->getEntry(), "TEMP_", 5 ) != 0 )
      toSelect.Append( it.Value() );
  }

  theSelMgr->setSelectedObjects( toSelect, true );
}

void MCCADGUI_Displayer::SetName( const char* theName )
{
  myName = theName;
}

void MCCADGUI_Displayer::UnsetName()
{
  myName = "";
}



/*!
 * \brief build a Presentation of this object
 * \param aEntry the object entry
 * \param view the viewframe
 * \return  the presentations
 */
SALOME_Prs* MCCADGUI_Displayer::buildPresentation( const QString& aEntry, SALOME_View* theViewFrame )
{
    /*
    const int studyID = app()->studyId();
    if ( studyID == -1 )
        return NULL;
    SOCC_Prs * prs  = dynamic_cast<SOCC_Prs*> ( LightApp_Displayer::buildPresentation( aEntry, view ));
//    SALOME_Prs * prs  = dynamic_cast<SALOME_Prs*> ( LightApp_Displayer::buildPresentation( aEntry, view ));
    if (!prs) return 0;

    //first obtain the shape
    MCCADGUI_DataObject * aObj = datamodel()->findObject(aEntry);
    if (!aObj)    {
        SUIT_MessageBox::warning( app()->desktop(), QString("Warning"),QString( "Cannot find the object in this study!"));
        return NULL;
    }
    TopoDS_Shape aShape;
    if (aObj->isPart())
        aShape = MCCADGUI_DataModel::Stream2Shape(*aObj->getPart()->getShapeStream());
    else
    {
        SUIT_MessageBox::warning( app()->desktop(), QString("Warning"),QString( "Cannot display this object!"));
        return NULL;
    }
    if (aShape.IsNull())
    {
        SUIT_MessageBox::warning( app()->desktop(), QString("Warning"),QString( "No shape to display!"));
        return NULL;
    }

    Handle(AIS_InteractiveObject) aAISshape = new AIS_Shape(aShape);
    prs->AddObject(aAISshape);
    return prs;
    */

    SALOME_Prs* prs = 0;
    internalReset();
    myViewFrame = theViewFrame ? theViewFrame : GetActiveView();
    if ( !myViewFrame ) {
        MESSAGE("cannot get activeview!")
        return 0;
    }

    prs = LightApp_Displayer::buildPresentation( aEntry, theViewFrame );
    if (!prs) {
        MESSAGE("cannot generate presentation!")
        return 0;
    }

    Handle( SALOME_InteractiveObject ) theIO = new SALOME_InteractiveObject();
    theIO->setEntry( aEntry.toLatin1().constData() );

    if ( !theIO.IsNull() )
    {
        // set interactive object
        setIO( theIO );
        // obtain the shape
        MCCADGUI_DataObject * aObj = datamodel()->findObject(aEntry);
        if (!aObj)    {
            SUIT_MessageBox::warning( app()->desktop(), QString("Warning"),QString( "Cannot find the object in this study!"));
            return NULL;
        }
        TopoDS_Shape aShape;
        if (aObj->isPart())
            aShape = MCCADGUI_DataModel::Stream2Shape(*aObj->getPart()->getShapeStream());
        else
        {
            SUIT_MessageBox::warning( app()->desktop(), QString("Warning"),QString( "Cannot display this object!"));
            return NULL;
        }
        if (aShape.IsNull())
        {
            SUIT_MessageBox::warning( app()->desktop(), QString("Warning"),QString( "No shape to display!"));
            return NULL;
        }
        SetName(aObj->name().toLatin1());
        setShape( aShape );
//        myType = GeomObject->GetType();
    }
    UpdatePrs( prs );  // Update presentation by using of the double dispatch
    return prs;

}

//=================================================================
/*!
 *  MCCADGUI_Displayer::internalReset
 *  Resets internal data
 *  [internal]
 */
//=================================================================
void MCCADGUI_Displayer::internalReset()
{
  myIO.Nullify();
  myShape.Nullify();
}

void MCCADGUI_Displayer::setIO( const Handle(SALOME_InteractiveObject)& theIO )
{
  myIO = theIO;
}

//void MCCADGUI_Displayer::updateActor( SALOME_Actor* actor )
//{
//    const int studyID = app()->studyId();
//    if ( actor && actor->hasIO() && studyID >= 0 ) {
//        actor->Update();
//    }
//}

void MCCADGUI_Displayer::setShape( const TopoDS_Shape& theShape )
{
  myShape = theShape;
}


void MCCADGUI_Displayer::updateShapeProperties( const Handle(MCCAD_AISShape)& AISShape, bool create )
{
  // check that shape is not null
  if ( AISShape.IsNull() ) return;

  // check that study is active
  SalomeApp_Study* study = getStudy();
  if ( !study ) return;

//  if ( myShape.ShapeType() != TopAbs_VERTEX && // fix pb with not displayed points
//       !TopoDS_Iterator(myShape).More() )
//    return; // NPAL15983 (Bug when displaying empty groups)

  // set interactive object

  Handle( SALOME_InteractiveObject ) anIO;

  if ( !myIO.IsNull() ) {
    AISShape->setIO( myIO );
    AISShape->SetOwner( myIO );
    anIO = myIO;
  }
  else if ( !myName.empty() ) {
    // workaround to allow selection of temporary objects
    static int tempId = 0;
    anIO = new SALOME_InteractiveObject( QString( "TEMP_%1" ).arg( tempId++ ).toLatin1().data(), "MCCAD", myName.c_str() );
    AISShape->setIO( anIO );
    AISShape->SetOwner( anIO );
  }

  // flag:  only vertex or compound of vertices is processed (specific handling)
//  bool onlyVertex = myShape.ShapeType() == TopAbs_VERTEX || isCompoundOfVertices( myShape );
  // presentation study entry (empty for temporary objects like preview)
  QString entry = !anIO.IsNull() ? QString( anIO->getEntry() ) : QString();
  // flag: temporary object
  bool isTemporary = entry.isEmpty() || entry.startsWith( "TEMP_" );
  // currently active view window's ID (-1 if no active view)
  int aMgrId = !anIO.IsNull() ? getViewManagerId( myViewFrame ) : -1;

  // get presentation properties
  PropMap propMap = getObjectProperties( study, entry, myViewFrame );

  // Temporary staff: vertex must be infinite for correct visualization
  AISShape->SetInfiniteState( myShape.Infinite() ); // || myShape.ShapeType() == TopAbs_VERTEX // VSR: 05/04/2010: Fix 20668 (Fit All for points & lines)

  // set material
//  Material_Model material;
  // if predefined color isn't set in displayer(via MCCADGUI_Displayer::SetColor() function)
//  if( !HasColor() )
//    material.fromProperties( propMap.value( GEOM::MCCAD::propertyName( GEOM::Material ) ).toString() );
  // - set front material properties
//  AISShape->SetCurrentFacingModel( Aspect_TOFM_FRONT_SIDE );
//  AISShape->SetMaterial( material.getMaterialOCCAspect( true ) );
  // - set back material properties
//  AISShape->SetCurrentFacingModel( Aspect_TOFM_BACK_SIDE );
//  AISShape->SetMaterial( material.getMaterialOCCAspect( false ) );
  // - switch to default (both sides) facing mode
//  AISShape->SetCurrentFacingModel( Aspect_TOFM_BOTH_SIDE );

  // set colors

  // - shading color
  if ( HasColor()  ) {
    // predefined color, manually set to displayer via MCCADGUI_Displayer::SetColor() function;
    // we set it to the shape not taking into account material properties
    AISShape->SetShadingColor( (Quantity_NameOfColor)GetColor() );
  }
  else
//      AISShape->SetShadingColor( Quantity_Color( Quantity_NOC_GOLDENROD ));

//  else if ( !material.isPhysical() )
  {
    // shading color from properties is used only for non-physical materials
    AISShape->SetShadingColor( SalomeApp_Tools::color( propMap.value( MCCAD::propertyName( MCCAD:: ShadingColor ) ).value<QColor>() ) );
  }

  // - wireframe color
  Handle(Prs3d_LineAspect) anAspect = AISShape->Attributes()->LineAspect();
  anAspect->SetColor( HasColor() ? (Quantity_NameOfColor)GetColor()   : /*Quantity_Color( Quantity_NOC_GOLDENROD )*/
           SalomeApp_Tools::color( propMap.value( MCCAD::propertyName( MCCAD:: WireframeColor ) ).value<QColor>() ) );
  AISShape->Attributes()->SetLineAspect( anAspect );

  // - unfree boundaries color
  anAspect = AISShape->Attributes()->UnFreeBoundaryAspect();
  anAspect->SetColor( HasColor() ? (Quantity_NameOfColor)GetColor() : /* Quantity_Color( Quantity_NOC_GOLDENROD )*/
             SalomeApp_Tools::color( propMap.value( MCCAD::propertyName( MCCAD:: WireframeColor ) ).value<QColor>() ) );
  AISShape->Attributes()->SetUnFreeBoundaryAspect( anAspect );

  // - free boundaries color
  anAspect = AISShape->Attributes()->FreeBoundaryAspect();
  anAspect->SetColor( HasColor() ? (Quantity_NameOfColor)GetColor():/*Quantity_Color( Quantity_NOC_RED ) */
              SalomeApp_Tools::color( propMap.value( MCCAD::propertyName( MCCAD:: FreeBndColor ) ).value<QColor>() ) );
  AISShape->Attributes()->SetFreeBoundaryAspect( anAspect );

  // - standalone edges color
  anAspect = AISShape->Attributes()->WireAspect();
  anAspect->SetColor( HasColor() ? (Quantity_NameOfColor)GetColor() : /*Quantity_Color( Quantity_NOC_GOLDENROD ) */
              SalomeApp_Tools::color( propMap.value( MCCAD::propertyName( MCCAD:: LineColor ) ).value<QColor>() ) );
  AISShape->Attributes()->SetWireAspect( anAspect );

  // - color for edges in shading+edges mode
  AISShape->SetEdgesInShadingColor( SalomeApp_Tools::color( propMap.value( MCCAD::propertyName( MCCAD::OutlineColor ) ).value<QColor>() ) );

  // ???
  AISShape->storeBoundaryColors();

  // set display mode
  AISShape->SetDisplayMode( HasDisplayMode() ?
                // predefined display mode, manually set to displayer via MCCADGUI_Displayer::SetDisplayMode() function
                GetDisplayMode() :  /*0 wireframe*/
                 // display mode from properties
                propMap.value( MCCAD::propertyName(MCCAD::  DisplayMode ) ).toInt() );

  // set display vectors flag
  AISShape->SetDisplayVectors( propMap.value( MCCAD::propertyName( MCCAD:: EdgesDirection ) ).toBool() );



  // set iso properties
  int uIsos = propMap.value( MCCAD::propertyName( MCCAD:: NbIsos ) ).toString().split( /*subSectionSeparator()*/ ":")[0].toInt();
  int vIsos = propMap.value( MCCAD::propertyName( MCCAD:: NbIsos ) ).toString().split(/* subSectionSeparator()*/":" )[1].toInt();
  Quantity_Color isosColor = SalomeApp_Tools::color( propMap.value( MCCAD::propertyName(MCCAD::  IsosColor ) ).value<QColor>() );
  int isosWidth = propMap.value( MCCAD::propertyName( MCCAD:: IsosWidth ) ).toInt();
  Handle(Prs3d_IsoAspect) uIsoAspect = AISShape->Attributes()->UIsoAspect();
  Handle(Prs3d_IsoAspect) vIsoAspect = AISShape->Attributes()->VIsoAspect();
  uIsoAspect->SetColor( isosColor );
  uIsoAspect->SetWidth( isosWidth );
  uIsoAspect->SetNumber( uIsos );
  vIsoAspect->SetColor( isosColor );
  vIsoAspect->SetWidth( isosWidth );
  vIsoAspect->SetNumber( vIsos );
  AISShape->Attributes()->SetUIsoAspect( uIsoAspect );
  AISShape->Attributes()->SetVIsoAspect( vIsoAspect );

  // set deflection coefficient
  // ... to avoid to small values of the coefficient, its lower value is limited
//  AISShape->SetOwnDeviationCoefficient( qMax( propMap.value( MCCAD::propertyName( Deflection ) ).toDouble(), minDeflection() ) );

  // set texture
//  if ( HasTexture() ) {
//    // predefined display texture, manually set to displayer via MCCADGUI_Displayer::SetTexture() function
//    AISShape->SetTextureFileName( TCollection_AsciiString( GetTexture().c_str() ) );
//    AISShape->SetTextureMapOn();
//    AISShape->DisableTextureModulate();
//    AISShape->SetDisplayMode( 3 );
//  }

  // set line width
  AISShape->SetWidth( HasWidth() ?
              // predefined line width, manually set to displayer via MCCADGUI_Displayer::SetLineWidth() function
              GetWidth() : /*1*/
              // libe width from properties
              propMap.value( MCCAD::propertyName( MCCAD:: LineWidth ) ).toInt() );

  // set top-level flag
  AISShape->setTopLevel( propMap.value( MCCAD::propertyName( MCCAD:: TopLevel ) ).toBool() );

  // set point marker (for vertex / compound of vertices only)
 /* if ( onlyVertex ) {
    QStringList aList = propMap.value( GEOM::MCCAD::propertyName( GEOM::PointMarker ) ).toString().split( GEOM::subSectionSeparator() );
    if ( aList.size() == 2 ) {
      // standard marker string contains "TypeOfMarker:ScaleOfMarker"
      int aTypeOfMarker = aList[0].toInt();
      double aScaleOfMarker = (aList[1].toInt() + 1) * 0.5;
      Handle(Prs3d_PointAspect) anAspect = AISShape->Attributes()->PointAspect();
      anAspect->SetScale( aScaleOfMarker );
      anAspect->SetTypeOfMarker( (Aspect_TypeOfMarker)( aTypeOfMarker-1 ) );
      anAspect->SetColor( HasColor() ?
              // predefined color, manually set to displayer via MCCADGUI_Displayer::SetColor() function
              (Quantity_NameOfColor)GetColor() :
              // color from properties
              SalomeApp_Tools::color( propMap.value( GEOM::MCCAD::propertyName( GEOM::PointColor ) ).value<QColor>() ) );
      AISShape->Attributes()->SetPointAspect( anAspect );
    }
    else if ( aList.size() == 1 ) {
      // custom marker string contains "IdOfTexture"
      int textureId = aList[0].toInt();
      Standard_Integer aWidth, aHeight;
#if OCC_VERSION_LARGE > 0x06040000 // Porting to OCCT6.5.1
      Handle(TColStd_HArray1OfByte) aTexture =
#else
    Handle(Graphic3d_HArray1OfBytes) aTexture =
#endif
    GeometryGUI::getTexture( study, textureId, aWidth, aHeight );
      if ( !aTexture.IsNull() ) {
    static int TextureId = 0;
    Handle(Prs3d_PointAspect) aTextureAspect =
      new Prs3d_PointAspect( HasColor() ?
                 // predefined color, manually set to displayer via MCCADGUI_Displayer::SetColor() function
                 (Quantity_NameOfColor)GetColor() :
                 // color from properties
                 SalomeApp_Tools::color( propMap.value( GEOM::MCCAD::propertyName( GEOM::PointColor ) ).value<QColor>() ),
                 ++TextureId,
                 aWidth, aHeight,
                 aTexture );
    AISShape->Attributes()->SetPointAspect( aTextureAspect );
      }
    }
  }


*/
  if ( create && !isTemporary && aMgrId != -1 ) {
    // set properties to the study
    study->setObjectPropMap( aMgrId, entry, propMap );
  }

  // AISShape->SetName(???); ??? necessary to set name ???
}




PropMap MCCADGUI_Displayer::getObjectProperties( SalomeApp_Study* study,
                         const QString& entry,
                         SALOME_View* view )
{
  // get default properties for the explicitly specified default view type
  PropMap propMap = getDefaultPropertyMap();

  if ( study && view ) {
      SUIT_ViewModel* viewModel = dynamic_cast<SUIT_ViewModel*>( view );
      SUIT_ViewManager* viewMgr = ( viewModel != 0 ) ? viewModel->getViewManager() : 0;
      int viewId = ( viewMgr != 0 ) ? viewMgr->getGlobalId() : -1;

      if ( viewModel && viewId != -1 ) {
          // get properties from the study
          PropMap storedMap = study->getObjectPropMap( viewId, entry );
          // overwrite default properties from stored ones (that are specified)
          for ( int prop = MCCAD::Visibility; prop <= MCCAD::LastProperty; prop++ ) {
              if ( storedMap.contains( MCCAD::propertyName( (MCCAD::Property)prop ) ) )
                  propMap.insert( MCCAD::propertyName( (MCCAD::Property)prop ),
                                  storedMap.value( MCCAD::propertyName( (MCCAD::Property)prop ) ) );
          }
          // ... specific processing for color
          // ... current implementation is to use same stored color for all aspects
          // ... (TODO) possible future improvements about free boundaries, standalone edges etc colors can be here
          if ( storedMap.contains( MCCAD::propertyName( MCCAD:: Color ) ) ) {
              propMap.insert( MCCAD::propertyName( MCCAD:: ShadingColor ),   storedMap.value( MCCAD::propertyName(MCCAD::  Color ) ) );
              propMap.insert( MCCAD::propertyName(MCCAD::  WireframeColor ), storedMap.value( MCCAD::propertyName( MCCAD:: Color ) ) );
              propMap.insert( MCCAD::propertyName(MCCAD::  LineColor ),      storedMap.value( MCCAD::propertyName(MCCAD::  Color ) ) );
              propMap.insert( MCCAD::propertyName(MCCAD::  FreeBndColor ),   storedMap.value( MCCAD::propertyName(MCCAD::  Color ) ) );
              propMap.insert( MCCAD::propertyName(MCCAD::  PointColor ),     storedMap.value( MCCAD::propertyName(MCCAD::  Color ) ) );
          }

/*          if ( !entry.isEmpty() ) {
              // get CORBA reference to geom object
              _PTR(SObject) SO( study->studyDS()->FindObjectID( entry.toStdString() ) );
              if ( SO ) {
                  CORBA::Object_var object = GeometryGUI::ClientSObjectToObject( SO );
                  if ( !CORBA::is_nil( object ) ) {
                      GEOM_Object_var geomObject = GEOM_Object::_narrow( object );
                      // check that geom object has color properly set
                      bool hasColor = false;
                      SALOMEDS::Color aSColor = getColor( geomObject, hasColor );
                      // set color from geometry object (only once, if it is not yet set in GUI)
                      // current implementation is to use same color for all aspects
                      // (TODO) possible future improvements about free boundaries, standalone edges etc colors can be here
                      if ( hasColor && !storedMap.contains( propertyName( Color ) ) ) {
                          QColor objColor = QColor::fromRgbF( aSColor.R, aSColor.G, aSColor.B );
                          propMap.insert( propertyName( ShadingColor ),   objColor );
                          propMap.insert( propertyName( WireframeColor ), objColor );
                          propMap.insert( propertyName( LineColor ),      objColor );
                          propMap.insert( propertyName( FreeBndColor ),   objColor );
                          propMap.insert( propertyName( PointColor ),     objColor );
                      }

                  }
              }
          }*/
      }
  }
  return propMap;
}

PropMap MCCADGUI_Displayer::getDefaultPropertyMap()
{
  PropMap propMap;

  // get resource manager
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  // fill in the properties map with default values

  // - visibility (false by default)
  propMap.insert( MCCAD::propertyName( MCCAD:: Visibility ), false );

  // - nb isos (take default value from preferences)
  propMap.insert( MCCAD::propertyName( MCCAD:: NbIsos ),
          QString( "%1%2%3" ).
          arg( resMgr->integerValue( "MCCAD", "iso_number_u", 1 ) ).
                  arg( /*subSectionSeparator()*/  ":").
          arg( resMgr->integerValue( "MCCAD", "iso_number_v", 1 ) ) );

  // - transparency (opacity = 1-transparency)
  propMap.insert( MCCAD::propertyName( MCCAD:: Transparency ), 0.0 );

  // - display mode (take default value from preferences)
  propMap.insert( MCCAD::propertyName( MCCAD:: DisplayMode ),
          resMgr->integerValue( "MCCAD", "display_mode", 0 ) );

  // - show edges direction flag (false by default)
  propMap.insert( MCCAD::propertyName( MCCAD:: EdgesDirection ), false );

  // - shading color (take default value from preferences)
  propMap.insert( MCCAD::propertyName( MCCAD:: ShadingColor ),
          colorFromResources( "shading_color", QColor( 255, 255, 0 ) ) );

  // - wireframe color (take default value from preferences)
  propMap.insert( MCCAD::propertyName( MCCAD:: WireframeColor ),
          colorFromResources( "wireframe_color", QColor( 255, 255, 0 ) ) );

  // - standalone edges color (take default value from preferences)
  propMap.insert( MCCAD::propertyName( MCCAD:: LineColor ),
          colorFromResources( "line_color", QColor( 255, 0, 0 ) ) );

  // - free boundaries color (take default value from preferences)
  propMap.insert( MCCAD::propertyName( MCCAD:: FreeBndColor ),
          colorFromResources( "free_bound_color", QColor( 0, 255, 0 ) ) );

  // - points color (take default value from preferences)
  propMap.insert( MCCAD::propertyName( MCCAD:: PointColor ),
          colorFromResources( "point_color", QColor( 255, 255, 0 ) ) );

  // - isos color (take default value from preferences)
  propMap.insert( MCCAD::propertyName( MCCAD:: IsosColor ),
          colorFromResources( "isos_color", QColor( 200, 200, 200 ) ) );

  // - outlines color (take default value from preferences)
  propMap.insert( MCCAD::propertyName(MCCAD::  OutlineColor ),
          colorFromResources( "edges_in_shading_color", QColor( 180, 180, 180 ) ) );

  // - deflection coefficient (take default value from preferences)
  propMap.insert( MCCAD::propertyName(MCCAD::  Deflection ),
          resMgr->doubleValue( "MCCAD", "deflection_coeff", 0.001 ) );

//  // - material (take default value from preferences)
//  Material_Model material;
//  material.fromResources( resMgr->stringValue( "MCCAD", "material", "Plastic" ) );
//  propMap.insert( MCCAD::propertyName( Material ), material.toProperties() );

  // - edge width (take default value from preferences)
  propMap.insert( MCCAD::propertyName( MCCAD:: LineWidth ),
          resMgr->integerValue( "MCCAD", "edge_width", 1 ) );

  // - isos width (take default value from preferences)
  propMap.insert( MCCAD::propertyName(MCCAD::  IsosWidth ),
          resMgr->integerValue( "MCCAD", "isolines_width", 1 ) );

//  // - point marker (take default value from preferences)
//  propMap.insert( MCCAD::propertyName(MCCAD::  PointMarker ),
//          QString( "%1%2%3" ).
//          arg( resMgr->integerValue( "MCCAD", "type_of_marker", 1 ) + 1 ).
//          arg( subSectionSeparator() ).
//          arg( resMgr->integerValue( "MCCAD", "marker_scale", 1 ) ) );

  // - top-level flag (false by default)
  propMap.insert( MCCAD::propertyName( MCCAD:: TopLevel ), false );

  return propMap;
}

QColor MCCADGUI_Displayer::colorFromResources( const QString& property, const QColor& defColor )
{
  // VSR: this method can be improved in future:
  // to improve performance, the default values from resource manager should be cached in the displayer
  return SUIT_Session::session()->resourceMgr()->colorValue( "MCCAD", property, defColor );
}



//QString MCCADGUI_Displayer::MCCAD::propertyName( Property type )
//{
//  static const char* names[] = {
//    // visibility
//    "Visibility",      // VISIBILITY_PROP
//    // transparency
//    "Transparency",    // TRANSPARENCY_PROP
//    // display mode
//    "DisplayMode",     // DISPLAY_MODE_PROP
//    // number of the iso lines (along u and v directions)
//    "Isos",            // ISOS_PROP
//    // main / shading color
//    "Color",           // COLOR_PROP
//    // "show edges direction" flag
//    "VectorMode",      // VECTOR_MODE_PROP
//    // deflection coefficient
//    "DeflectionCoeff", // DEFLECTION_COEFF_PROP
////    // point marker data
////    "MarkerType",      // MARKER_TYPE_PROP
////    // material
////    "Material",        // MATERIAL_PROP
//    // general line width
//    "EdgeWidth",       // EDGE_WIDTH_PROP
//    // isos width
//    "IsosWidth",       // ISOS_WIDTH_PROP
//    // top-level flag
//    "TopLevelFlag",    // TOP_LEVEL_PROP
//    // opacity
//    "Opacity",         // OPACITY_PROP
//    // shading color
//    "ShadingColor",    // -
//    // wireframe color
//    "WireframeColor",  // -
//    // standalone edges color
//    "LineColor",       // -
//    // free boundaries color
//    "FreeBndColor",    // -
//    // points color
//    "PointColor",      // -
//    // isos color
//    "IsosColor",       // -
//    // outlines color
//    "OutlineColor",    // -
//  };
//  return ( type >= Visibility && type <= LastProperty ) ? names[type] : QString();
//}


LightApp_Application *    MCCADGUI_Displayer::app() const
{
   return dynamic_cast<LightApp_Application *>(  SUIT_Session::session()->activeApplication() );  //works??
}

MCCADGUI_DataModel * MCCADGUI_Displayer::datamodel() const
{
    if (app())
       return dynamic_cast<MCCADGUI_DataModel*> (app()->activeModule()->dataModel());
   return 0;
}


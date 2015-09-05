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


#include "MCCADGUI_Selection.h"
#include "MCCADGUI_Displayer.h"
#include "MCCADGUI.h"

#include <LightApp_DataOwner.h>
#include <LightApp_Displayer.h>
#include <LightApp_Application.h>
#include <LightApp_Study.h>
#include <SalomeApp_Study.h>

#include <OCCViewer_ViewModel.h>

#include <SUIT_Session.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewModel.h>

#include <SALOMEDSClient_SObject.hxx>
#include <SALOMEDSClient_Study.hxx>

#include <SALOME_Prs.h>
#include <SALOME_InteractiveObject.hxx>

#include <SVTK_Prs.h>
#include <SALOME_Actor.h>

#include <SVTK_ViewModel.h>


// VTK Includes
#include <vtkActorCollection.h>

MCCADGUI_Selection::MCCADGUI_Selection()
{
}

MCCADGUI_Selection::~MCCADGUI_Selection()
{
}

QVariant MCCADGUI_Selection::parameter( const int index, const QString& p ) const
{
    if ( p == "isComponent" )
        return isComponent(index) ;
    else if ( p == "isGroup" )
        return isGroup(index) ;
    else if ( p == "isPart" )
        return isPart( index );
    else if (p == "isPartPastable")
      return isPartPastable();
    else if (p == "isGroupPastable")
      return isGroupPastable();
    else if (p == "hasBackup")
      return hasBackup(index);
    else if (p == "isDecomposed")
      return isDecomposed(index);
    else if (p == "isGEOMObj")
      return isGEOMObj(index);
    else if (p == "hasMesh")
      return hasMesh(index);
    else if (p == "isSMESHObj")
      return isSMESHObj(index);
    else if (p == "hasEnvelop")
      return hasEnvelop(index);
    else if (p == "isExplodible")
      return isExplodible(index);
    else if (p == "isMCCADObj")
      return isMCCADObj(index);
    else if (p == "isMCCADRootObj")
      return isMCCADRootObj(index);
  return LightApp_Selection::parameter( index, p );
}

bool     MCCADGUI_Selection::isComponent( const int index) const
{
    //too slow!!
//    if (!datamodel()->findObject(entry(index)))
//        return false;
//    if(datamodel())
//        return datamodel()->findObject(entry(index))->isComponent();
//    return false;

    QString aEntry = entry(index);
    if (aEntry.contains("MCCADGUI") && (aEntry != "MCCADGUI_root")  ) {
        if (aEntry.count('_') == 1) return true; // component has entry like "MCCADGUI_1"
    }
    return false;
}

bool     MCCADGUI_Selection::isGroup( const int index) const
{
    //too slow!!
//    if (!datamodel()->findObject(entry(index)))
//        return false;
//    if(datamodel())
//        return datamodel()->findObject(entry(index))->isGroup();
//    return false;
    QString aEntry = entry(index);
    if (aEntry.contains("MCCADGUI") &&  (aEntry.count('_') == 2) )
        return true; // group has entry like "MCCADGUI_1_1"

    return false;
}

bool     MCCADGUI_Selection::isPart ( const int index) const
{
    //too slow!!
//    if (!datamodel()->findObject(entry(index)))
//        return false;
//    if (datamodel())
//        return datamodel()->findObject(entry(index))->isPart();
//    return false;

    QString aEntry = entry(index);
    if (aEntry.contains("MCCADGUI") && (aEntry.count('_') == 3) )
        return true; // Part has entry like "MCCADGUI_1_1_1"

    return false;
}

bool  MCCADGUI_Selection::isPartPastable () const
{
    if(datamodel())
        return datamodel()->getPastePartsSize(); //0:false; >0 true
    return 0;
}

bool  MCCADGUI_Selection::isGroupPastable () const
{
    if(datamodel())
        return datamodel()->getPasteGroupsSize(); //0:false; >0 true
    return 0;
}

bool MCCADGUI_Selection::isVisible( const int index ) const
{
    bool res = false;
    SALOME_View* view = MCCADGUI_Displayer::GetActiveView();
    if ( view )
        res = view->isVisible( datamodel()->findObjectIO(entry(index)) );

    return res;
}


bool   MCCADGUI_Selection::hasBackup (const int index) const
{
    bool res = false;
    MCCADGUI_DataObject * aObj = datamodel()->findObject(entry(index));
    if (!aObj) return false;
    if (aObj->isPart()) {
        MCCAD_ORB::Part_var aPart = aObj->getPart();
        if (!aPart->_is_nil() )
            res = aPart->hasBackupShape();
    }
    return res;
}

bool     MCCADGUI_Selection::isDecomposed (const int index ) const
{
    bool res = true;
    MCCADGUI_DataObject * aObj = datamodel()->findObject(entry(index));
    if (!aObj) return true;
    if (aObj->isPart()) {
        MCCAD_ORB::Part_var aPart = aObj->getPart();
        if (!aPart->_is_nil())
            res = aPart->getIsDecomposed();
    }
    return res;
}

bool     MCCADGUI_Selection::isGEOMObj(const int index ) const
{
    const LightApp_DataOwner* owner = new LightApp_DataOwner (entry(index)); //get the owner
    GEOM::GEOM_Object_var aGeomObj = datamodel()->getMCCADGUImodule()->getGeom(owner, true);
    if (aGeomObj->_is_nil())
        return false;
    else return true;
}

bool   MCCADGUI_Selection::hasMesh(const int index) const
{
    bool res = false;
    MCCADGUI_DataObject * aObj = datamodel()->findObject(entry(index));
    if (!aObj) return false;
    if (aObj->isPart()) {
        MCCAD_ORB::Part_var aPart = aObj->getPart();
        if (!aPart->_is_nil() )
            res = aPart->hasMesh();
    }
    return res;
}

bool     MCCADGUI_Selection::isSMESHObj(const int index ) const
{
    const LightApp_DataOwner* owner = new LightApp_DataOwner (entry(index)); //get the owner
    SMESH::SMESH_Mesh_var aSMESHObj = datamodel()->getMCCADGUImodule()->getSmesh(owner, true);
    if (aSMESHObj->_is_nil())
        return false;
    else return true;
}

bool   MCCADGUI_Selection::hasEnvelop(const int index) const
{
    bool res = false;
    MCCADGUI_DataObject * aObj = datamodel()->findObject(entry(index));
    if (!aObj) return false;
    if (aObj->isComponent()) {
        MCCAD_ORB::Component_var aComponent = aObj->getComponent();
        if (!aComponent->_is_nil() )
            res = aComponent->hasEnvelop();
    }
    return res;
}

bool MCCADGUI_Selection::isExplodible (const int index) const
{
    bool res = false;
    MCCADGUI_DataObject * aObj = datamodel()->findObject(entry(index));
    if (!aObj) return false;
    if (aObj->isPart()) {
        MCCAD_ORB::Part_var aPart = aObj->getPart();
        if (!aPart->_is_nil() )
            res = aPart->isExplodible();
    }
    return res;
}

bool     MCCADGUI_Selection::isMCCADObj (const int  index) const
{
     // it make things slow!!
    //MCCADGUI_DataObject * aObj = datamodel()->findObject(entry(index));
    //   if (!aObj) return false;
    //else return true;

    if (entry(index).contains("MCCADGUI"))
        return true;
    else return false;
}

bool     MCCADGUI_Selection::isMCCADRootObj (const int  index) const
{
    QString aEntry =  entry(index);
    if (aEntry == "MCCADGUI_root")   return true;
    else return false;
}

 LightApp_Application *    MCCADGUI_Selection::app() const
{
    //header file should be included
    return dynamic_cast<LightApp_Application *>( study()->application() );
}

 MCCADGUI_DataModel * MCCADGUI_Selection::datamodel() const
{
    if (app())
        return dynamic_cast<MCCADGUI_DataModel*> (app()->activeModule()->dataModel());
    return 0;
}



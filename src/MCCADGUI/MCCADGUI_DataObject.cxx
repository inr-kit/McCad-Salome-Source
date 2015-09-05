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


#include "MCCADGUI_DataObject.h"
#include "MCCADGUI.h"

#include <LightApp_Study.h>
#include <LightApp_DataModel.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <CAM_DataModel.h>
#include <CAM_Module.h>

/*! Constructor */
MCCADGUI_DataObject::MCCADGUI_DataObject(SUIT_DataObject* parent, int ComponentIndex, int GroupIndex, int PartIndex )
    : LightApp_DataObject( parent ),
      CAM_DataObject( parent),
      myComponentID( ComponentIndex ),
      myGroupID( GroupIndex ),
      myPartID( PartIndex )
{
}

/*! Destructor */
MCCADGUI_DataObject::~MCCADGUI_DataObject()
{
}



/*! Returns unique object ID */
QString MCCADGUI_DataObject::entry() const
{
    QString id = "root";
    if (myComponentID > -1)    {
        id = QString::number(myComponentID);
        if ( myGroupID > -1 ) {
            id += QString( "_%1" ).arg(myGroupID);
            if ( myPartID >= 0 )
                id += QString( "_%1" ).arg( myPartID);
        }
    }

    QString aEntry = QString( "MCCADGUI_%1" ).arg( id );
    return aEntry;
}

/*! Returns a name of the Data Object. */
QString MCCADGUI_DataObject::name() const
{
    if (isPart())
    {
        MCCAD_ORB::Part_var aPart = getPart();
        if (!aPart->_is_nil())
            return aPart->getName();
        else
            return  QString("-Error-");
    }
    else if (isGroup())
    {
        MCCAD_ORB::Group_var aGroup = getGroup();
        if (!aGroup->_is_nil())
            return aGroup->getName();
        else
            return QString("-Error-");
    }
    else if (isComponent())
    {
        MCCAD_ORB::Component_var aComponent = getComponent();
        if (!aComponent->_is_nil())
            return aComponent->getName();
        else
            return QString("-Error-");
    }
    else
        return  QString("-Error-");
}

/*! Returns an icon of the Data Object. */
QPixmap MCCADGUI_DataObject::icon(const int) const
{
    static QPixmap IconPart = SUIT_Session::session()->resourceMgr()->loadPixmap( "MCCAD","Part.png", false );
    static QPixmap IconGroup = SUIT_Session::session()->resourceMgr()->loadPixmap( "MCCAD","Group.png", false );
    static QPixmap IconComponent = SUIT_Session::session()->resourceMgr()->loadPixmap( "MCCAD","Component.png", false );
    if (isPart()) return IconPart;
    else if (isGroup()) return IconGroup;
    else if (isComponent()) return IconComponent;
    else return QPixmap();
}

/*! Returns a tooltip for the object (to be displayed in the Object Browser). */
QString MCCADGUI_DataObject::toolTip(const int) const
{
    if ( isComponent() )
        return QString( QObject::tr( "Component" ) + ": %1" ).arg( getComponent()->getName());
    else if ( isGroup() )
        return QString( QObject::tr( "Group" ) + ": %1" ).arg( getGroup()->getName());
    else if (isPart())
        return QString( QObject::tr( "Part" ) + ": %1" ).arg( getPart()->getName() );
    else
        return QString();
}

MCCAD_ORB::Component_ptr MCCADGUI_DataObject::getComponent() const
{
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
//    LightApp_RootObject* rootObj = dynamic_cast<LightApp_RootObject*> ( root() );
    if ( /*rootObj && */!CORBA::is_nil( engine ) )
    {
//        const int studyID = rootObj->study()->id();
        const int studyID =    SUIT_Session::session()->activeApplication()->activeStudy()->id();
        if ( studyID > 0 )
        {
            if (myComponentID > -1)
            {
                MCCAD_ORB::Component_var aComponent;
                aComponent = engine->getComponent(studyID, myComponentID);
                if (!CORBA::is_nil(aComponent))
                    return aComponent._retn();
            }
        }
    }
    return MCCAD_ORB::Component::_nil();
}


MCCAD_ORB::Group_ptr MCCADGUI_DataObject::getGroup() const
{
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    //    LightApp_RootObject* rootObj = dynamic_cast<LightApp_RootObject*> ( root() );
        if ( /*rootObj && */!CORBA::is_nil( engine ) )
        {
    //        const int studyID = rootObj->study()->id();
            const int studyID =    SUIT_Session::session()->activeApplication()->activeStudy()->id();
        if ( studyID > 0 )
        {
            if (myGroupID > -1)
            {
                MCCAD_ORB::Group_var aGroup;
                aGroup = engine->getGroup(studyID, myGroupID);
                if (!CORBA::is_nil(aGroup))
                    return aGroup._retn();
            }
        }
    }
    return MCCAD_ORB::Group::_nil();
}

MCCAD_ORB::Part_ptr MCCADGUI_DataObject::getPart() const
{
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    //    LightApp_RootObject* rootObj = dynamic_cast<LightApp_RootObject*> ( root() );
        if ( /*rootObj && */!CORBA::is_nil( engine ) )
        {
    //        const int studyID = rootObj->study()->id();
            const int studyID =    SUIT_Session::session()->activeApplication()->activeStudy()->id();
        if ( studyID > 0 )
        {
            if (myPartID > -1)  //if myPartID > -1, myGroupID should >-1
            {
                MCCAD_ORB::Part_var aPart;
                aPart = engine->getPart(studyID, myPartID);
                if (!CORBA::is_nil(aPart))
                    return aPart._retn();
            }
        }
    }
    return MCCAD_ORB::Part::_nil();
}


/*!
 * \brief MCCADGUI_DataObject::getBaseObj
 * get the base of this part/group/component
 * \return the base
 */
MCCAD_ORB::BaseObj_ptr MCCADGUI_DataObject::getBaseObj() const
{
    if (isPart())
        return dynamic_cast <MCCAD_ORB::BaseObj_ptr> (getPart()) ;
    else if (isGroup())
        return dynamic_cast <MCCAD_ORB::BaseObj_ptr> (getGroup()) ;
    else if (isComponent())
        return dynamic_cast <MCCAD_ORB::BaseObj_ptr> (getComponent()) ;
    else
        return MCCAD_ORB::BaseObj::_nil();
}


/*! Constructor */
MCCADGUI_ModuleObject::MCCADGUI_ModuleObject( CAM_DataModel* dm, SUIT_DataObject* parent )
    : MCCADGUI_DataObject( parent, -1 ),
      LightApp_ModuleObject( dm, parent ),
      CAM_DataObject( parent )
{
}

/*! Returns a name of the root object. */
QString MCCADGUI_ModuleObject::name() const
{
    return CAM_ModuleObject::name();
}

/*! Returns an icon of the root object. */
QPixmap MCCADGUI_ModuleObject::icon(const int) const
{
    QPixmap px;
    px = SUIT_Session::session()->resourceMgr()->loadPixmap( "MCCAD", "MCCAD.png", false );

  return px;
}

/*! Returns a tooltip of the root object. */
QString MCCADGUI_ModuleObject::toolTip(const int) const
{
  return QObject::tr( "MCCAD" );
}


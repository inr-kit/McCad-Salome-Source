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


#if !defined(MCCADGUI_DATAOBJECT_H)
#define MCCADGUI_DATAOBJECT_H

#include <LightApp_DataObject.h>
#include <QString>
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(MCCAD)

/*!
 * Class       : MCCADGUI_DataObject
 * Description : Data Object for data of MCCAD component
 */
class MCCADGUI_DataObject : public virtual LightApp_DataObject
{
public:
  MCCADGUI_DataObject ( SUIT_DataObject*,int ComponentIndex = -1,  int GroupIndex = -1, int PartIndex = -1 );
  virtual ~MCCADGUI_DataObject();
    
  virtual QString entry() const;

  QString    name()    const;
  QPixmap    icon(const int = NameId)    const;
  QString    toolTip(const int = NameId) const;

  //these function is cheaper than getGroup()->getID()
  int        getComponentID() const { return myComponentID; }
  int        getGroupID() const { return myGroupID; }
  int        getPartID() const { return myPartID; }

  bool       isComponent() const { return !isPart() && !isGroup() && myComponentID > 0; }
  bool       isGroup() const { return !isPart() && myGroupID > -1; }
  bool       isPart() const { return myPartID > -1; }

  MCCAD_ORB::Component_ptr     getComponent() const;
  MCCAD_ORB::Group_ptr         getGroup() const;
  MCCAD_ORB::Part_ptr          getPart() const;
  MCCAD_ORB::BaseObj_ptr       getBaseObj() const;

  virtual    QString componentDataType() {return "MCCAD_DATA";} //a type for identifying data for this module

private:
  int       myComponentID;
  int       myGroupID;
  int       myPartID;
};

class MCCADGUI_ModuleObject : public MCCADGUI_DataObject,
                                 public LightApp_ModuleObject
{
public:
  MCCADGUI_ModuleObject ( CAM_DataModel*, SUIT_DataObject* = 0 );

  virtual QString name()    const;
  QPixmap         icon(const int = NameId)    const;
  QString         toolTip(const int = NameId) const;
};

#endif // MCCADGUI_DATAOBJECT_H

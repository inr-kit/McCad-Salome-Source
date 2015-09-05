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


#ifndef MCCADGUI_SELECTION_HeaderFile
#define MCCADGUI_SELECTION_HeaderFile

#include <LightApp_Selection.h>
#include "MCCADGUI_DataModel.h"

class MCCADGUI_Selection : public LightApp_Selection
{
public:
  MCCADGUI_Selection();
  virtual ~MCCADGUI_Selection();
  QVariant parameter( const int index, const QString& ) const;
  bool     isComponent( const int  index) const;
  bool     isGroup( const int index ) const;
  bool     isPart ( const int  index) const;
  bool     isPartPastable () const;
  bool     isGroupPastable () const;
  bool     isVisible( const int  index) const;
  bool     hasBackup (const int index ) const;
  bool     isDecomposed (const int  index) const;
  bool     isGEOMObj (const int index ) const;
  bool     hasMesh (const int  index) const;
  bool     isSMESHObj (const int  index) const;
  bool     hasEnvelop (const int  index) const;
  bool     isExplodible (const int  index) const;
  bool     isMCCADObj (const int  index) const;
  bool     isMCCADRootObj (const int  index) const;

private:
   LightApp_Application *    app() const;
   MCCADGUI_DataModel * datamodel() const;


};

#endif

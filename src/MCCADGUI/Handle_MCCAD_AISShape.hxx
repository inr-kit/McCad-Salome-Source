// Copyright (C) 2014-2015  KIT-INR/NK
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

//  GEOM OBJECT : interactive object for Geometry entities visualization
//  File   : Handle_MCCAD_AISShape.hxx
//  Module : GEOM
//
#ifndef _Handle_MCCAD_AISShape_HeaderFile
#define _Handle_MCCAD_AISShape_HeaderFile

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif

#ifndef _Handle_SALOME_AISShape_HeaderFile
#include "Handle_SALOME_AISShape.hxx"
#endif

class Standard_Transient;
class Handle_Standard_Type;
class Handle(SALOME_AISShape);
class MCCAD_AISShape;
Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(MCCAD_AISShape);

class Handle(MCCAD_AISShape) : public Handle(SALOME_AISShape) {
  public:
    inline void* operator new(size_t,void* anAddress) 
      {
        return anAddress;
      }
    inline void* operator new(size_t size) 
      { 
        return Standard::Allocate(size); 
      }
    inline void  operator delete(void *anAddress) 
      { 
        if (anAddress) Standard::Free((Standard_Address&)anAddress); 
      }
//    inline void  operator delete(void *anAddress, size_t size) 
//      { 
//        if (anAddress) Standard::Free((Standard_Address&)anAddress,size); 
//      }
    Handle(MCCAD_AISShape)():Handle(SALOME_AISShape)() {}
    Handle(MCCAD_AISShape)(const Handle(MCCAD_AISShape)& aHandle) : Handle(SALOME_AISShape)(aHandle)
     {
     }

    Handle(MCCAD_AISShape)(const MCCAD_AISShape* anItem) : Handle(SALOME_AISShape)((SALOME_AISShape *)anItem)
     {
     }

    Handle(MCCAD_AISShape)& operator=(const Handle(MCCAD_AISShape)& aHandle)
     {
      Assign(aHandle.Access());
      return *this;
     }

    Handle(MCCAD_AISShape)& operator=(const MCCAD_AISShape* anItem)
     {
      Assign((Standard_Transient *)anItem);
      return *this;
     }

    MCCAD_AISShape* operator->()
     {
      return (MCCAD_AISShape *)ControlAccess();
     }

    MCCAD_AISShape* operator->() const
     {
      return (MCCAD_AISShape *)ControlAccess();
     }

   Standard_EXPORT ~Handle(MCCAD_AISShape)();
 
   Standard_EXPORT static const Handle(MCCAD_AISShape) DownCast(const Handle(Standard_Transient)& AnObject);
};
#endif

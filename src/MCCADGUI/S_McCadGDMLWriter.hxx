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
#ifndef S_MCCADGDMLWRITER_HXX
#define S_MCCADGDMLWRITER_HXX
#include "../MCCAD/McCadMcVoid/McCadGDMLWriter.hxx"

class S_McCadGDMLWriter : public  McCadGDMLWriter
{
public:
    S_McCadGDMLWriter();
    ~S_McCadGDMLWriter();

    //Print <define> element, means print the polyhedron for visualization purpose
    //here it check if the solid is undecomposed, then jump it.
    virtual void    PrintDefine(QDomElement & ParentElement);


    virtual void    PrintMaterial(QDomElement & ParentElement);

    virtual void    PrintSolids (QDomElement & ParentElement);

    void            PrintTessellatedSolid(QDomElement & ParentElement, const TopoDS_Shape & aShape, const QString aName);

    virtual McCadVoidCell * getWholeBntBox();

    virtual void    CheckMaterialNames();

    virtual void    PrintStructure(QDomElement & ParentElement);




};

#endif // S_MCCADGDMLWRITER_HXX

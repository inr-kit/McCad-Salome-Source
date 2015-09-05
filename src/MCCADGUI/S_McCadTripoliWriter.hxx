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
#ifndef S_MCCADTRIPOLIWRITER_HXX
#define S_MCCADTRIPOLIWRITER_HXX

#include "../MCCAD/McCadMcVoid/McCadTripoliWriter.hxx"

class S_McCadTripoliWriter : public McCadTripoliWriter
{
public:
    S_McCadTripoliWriter();

    void PrintCellDesc(Standard_OStream& theStream);    /**< Print the cell card */
    void PrintMatCard(Standard_OStream& theStream);     /**< Print the material card */
    void            S_PrintComponentInfo(const int & ComponentID, Standard_OStream& theStream);
    void            PrintGroupInfo(const int & GroupID, Standard_OStream& theStream);

private:
    void PrintACellDesc( McCadSolid *pSolid, Standard_OStream& theStream);

private:
    int m_iCellNum;

};

#endif // S_MCCADTRIPOLIWRITER_HXX

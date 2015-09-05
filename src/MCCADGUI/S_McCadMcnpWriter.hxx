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
#ifndef S_MCCADMCNPWRITER_HXX
#define S_MCCADMCNPWRITER_HXX
#include "S_McCadSolid.hxx"

#include "../MCCAD/McCadMcVoid/McCadMcnpWriter.hxx" //using the one in inc is not working


class S_McCadMcnpWriter : public  McCadMcnpWriter
{
public:
    S_McCadMcnpWriter();
    ~S_McCadMcnpWriter();

    /** Print the output file, include head information, cell, void, surface, transform card */
    void            PrintFile();
    void            PrintCellDesc(Standard_OStream& theStream);
    void            S_PrintPseudoCellDesc(Standard_OStream& theStream, const int &ComponentID,
                                          int &iPseudoCellNum, const int & iEmbedCardNum);
    void            PrintMatCard(Standard_OStream& theStream);
    void            S_PrintEmbedCard(Standard_OStream& theStream);
    void            S_PrintComponentInfo(const int & ComponentID, Standard_OStream& theStream);
    void            PrintGroupInfo(const int & GroupID, Standard_OStream& theStream);

private:
    QString         myFileName;

};

#endif // S_MCCADMCNPWRITER_HXX

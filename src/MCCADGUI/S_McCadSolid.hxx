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
#ifndef S_MCCADSOLID_H
#define S_MCCADSOLID_H
#include "../MCCAD/McCadMcVoid/McCadSolid.hxx"

#include <QString>

class S_McCadSolid : public McCadSolid
{
public:
    S_McCadSolid();
    ~S_McCadSolid();
    int     GetPartID () {return myPartID;}
    int     GetGroupID () {return myGroupID;}
    int     GetComponentID() {return myComponentID;}
    bool    GetisEnvelop() {return myisEnvelop;}
    int *   GetImportances() {return myImportances;}
    QString GetRemark () {return myRemark;}
    QString GetAdditive() {return myAdditive;}
    bool    GetisDecomposed() {return myisDecomposed;}
    TopoDS_Shape GetUndecompsedSolid () {return myUndecompsedSolid; }

    void    SetPartID  (const int & aPartID) {myPartID = aPartID;}
    void    SetGroupID (const int & aGroupID) {myGroupID = aGroupID;}
    void    SetComponentID(const int & aComponentID) {myComponentID = aComponentID;}
    void    SetisEnvelop(const bool & isEnvelop) {myisEnvelop = isEnvelop;}
    void    SetImportances(const int * Importances ) {myImportances[0] = Importances[0]; myImportances[1] = Importances[1]; myImportances[2] = Importances[2]; }
    void    SetRemark (const QString & aRemark ) {myRemark = aRemark;}
    void    SetAdditive (const QString &aAdditive) {myAdditive = aAdditive;}
    void    SetisDecomposed(const bool & isDecomposed) {myisDecomposed = isDecomposed;}
    void    SetUnDecomposedSolid(const TopoDS_Shape & aSolid) {myUndecompsedSolid = aSolid ;}

private:
    int     myPartID ; // belongs to which group
    int     myGroupID ; // belongs to which group
    int     myComponentID; //belongs to which
    bool    myisEnvelop; // to see if it is the envelop for mesh
    int     myImportances[3];// importance, if -1, then use default (1)
    QString myRemark;  //remark for this part
    QString myAdditive;  //additive card for this part

    //to see if it is decomposed or not, solid which decomposed failed can be converted into tessellated solid for GDML
    bool    myisDecomposed;
    TopoDS_Shape   myUndecompsedSolid; //if decomposed failed, mySolid stores the undecomposed solid


};

#endif // S_MCCADSOLID_H

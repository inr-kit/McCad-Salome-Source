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
#include "S_McCadTripoliWriter.hxx"
#include "S_McCadSolid.hxx"
#include "MCCADGUI.h"
#include "MCCADGUI_DataModel.h"
#include "utilities.h"
#include <SUIT_Session.h>
#include <SUIT_Tools.h>
#include <SUIT_Study.h>

#include <assert.h>


S_McCadTripoliWriter::S_McCadTripoliWriter()
{
}

/*!
 * \brief S_McCadTripoliWriter::PrintCellDesc
 *  print the cell discription
 * \param theStream output file stream
 */
void S_McCadTripoliWriter::PrintCellDesc(Standard_OStream& theStream)
{
    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);

    theStream.setf(ios::left);
    theStream<<"/* ============================= Volume Data Definitions ========================= */"<<endl;

    m_iCellNum = m_iInitCellNum;

    int tmpComponentID = -1; //to get componentID, initial with a non-possible id
    int tmpGroupID = -1;

    //Loop for all the solids
    for (unsigned int i = 0; i < pData->m_SolidList.size(); i++)
    {
//qiu        McCadSolid * pSolid = m_SolidList.at(i);
        S_McCadSolid * pSolid = static_cast <S_McCadSolid *> (pData->m_SolidList.at(i));  //qiu cast to S_McCadSolid
        assert(pSolid);

        //skip the envelop solid, because it is a meshed component which works only for MCNP6!
        //Keeping in mind that the cell id m_iCellNum is not increase with i now,
        //It should be very careful when printing the Relation between Volume and Material in PrintMatCard!
        if (pSolid->GetisEnvelop()) continue;

        //qiu print Component info
        if (pSolid->GetComponentID() != tmpComponentID) {
            tmpComponentID = pSolid->GetComponentID();
            S_PrintComponentInfo(tmpComponentID, theStream);
        }
        //qiu print Group info
        if (pSolid->GetGroupID() != tmpGroupID) {
            tmpGroupID = pSolid->GetGroupID();
            PrintGroupInfo(tmpGroupID, theStream);
        }

        //output name and remark
        theStream<<"// Part: "<<pSolid->GetName();  //qiu Output the solid name
        if (!pSolid->GetRemark().trimmed().isEmpty())  //qiu Output remark
            theStream<<"  :"<<pSolid->GetRemark().trimmed().toStdString()<<endl;
        else
            theStream<<endl;

        //output cell description
        PrintACellDesc(pSolid, theStream);
    }
}


void S_McCadTripoliWriter::PrintACellDesc(McCadSolid *pSolid, Standard_OStream& theStream)
{
    vector<McCadTripoliCell *> cell_list = GenTripoliCellList(pSolid);

    vector<int> union_list;
    int iNumEpn = cell_list.size();   // The number of virtual solids

    for(int j = 0; j < iNumEpn; j++)
    {
        McCadTripoliCell *pCell = cell_list.at(j);
        //if (pCell->hasVirtualCell())
        vector<int> sub_list;
        int iNumVirCell = pCell->GetNumVirCell();
        for (int iVirCell = 0; iVirCell < iNumVirCell; iVirCell++)
        {
           McCadTripoliCell *pVirCell = pCell->GetVirtualCell(iVirCell);
           theStream<<setw(6)<<"VOLU"<<setw(8)<<++m_virtualCellNum; // Output the cell number.
           TCollection_AsciiString strCellDesc = pVirCell->GetEqua();
           theStream<<strCellDesc<<" FICTIVE ENDV"<<endl;
           sub_list.push_back(m_virtualCellNum);
        }

        if (j < iNumEpn-1 )
        {
            theStream<<setw(6)<<"VOLU"<<setw(8)<<++m_virtualCellNum; // Output the cell number.
            TCollection_AsciiString strCellDesc = pCell->GetEqua();
            theStream<<strCellDesc;
            if(!sub_list.empty())
            {
                theStream<<endl;
                theStream<<setw(14)<<""<<"VMINUS "<<sub_list.size();
                for(int iSubCell = 0; iSubCell < sub_list.size(); iSubCell++)
                {
                    theStream<<" "<<sub_list.at(iSubCell);
                }
                theStream<<endl;
                theStream<<setw(13)<<"";
            }

            theStream<<" FICTIVE ENDV"<<endl;
            sub_list.clear();
            union_list.push_back(m_virtualCellNum);
        }
        else if (j == iNumEpn-1)
        {
            theStream<<setw(6)<<"VOLU"<<setw(8)<<m_iCellNum++; // Output the cell number.
            TCollection_AsciiString strCellDesc = pCell->GetEqua();
            theStream<<strCellDesc;
            if(!sub_list.empty())
            {
                theStream<<endl;
                theStream<<setw(14)<<""<<"VMINUS "<<sub_list.size();
                for(int iSubCell = 0; iSubCell < sub_list.size(); iSubCell++)
                {
                    theStream<<" "<<sub_list.at(iSubCell);
                }
            }
        }
    }

    if (iNumEpn > 1)
    {
        theStream<<endl;
        theStream<<setw(14)<<""<<"UNION "<<union_list.size();
        for(int m = 0; m < union_list.size(); m++)
        {
            if (m % 10 == 0 && m != 0)
            {
                theStream<<endl;
                theStream<<setw(13)<<"";
            }
            theStream<<" "<<union_list.at(m);
        }
        theStream<<endl;
        theStream<<setw(14)<<""<<"ENDV"<<endl;
        union_list.clear();
        theStream<<endl;
    }
    else
    {
        theStream<<" ENDV"<<endl;
    }

    for( int k = 0; k < cell_list.size(); k++ )
    {
        McCadTripoliCell *pCell = cell_list.at(k);
        delete pCell;
        pCell = NULL;
    }
    cell_list.clear();
}


/*!
 * \brief S_McCadTripoliWriter::PrintMatCard
 *  print the material discription
 * \param theStream output file stream
 */
void S_McCadTripoliWriter::PrintMatCard(Standard_OStream& theStream)
{
    theStream.setf(ios::left);
    theStream<<"/* -------------------------- Materials Card -------------------------------- */"<<endl;
    theStream <<endl;
    //get the studyID and engine
    int studyID = SUIT_Session::session()->activeApplication()->activeStudy()->id();
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    if ( !studyID || CORBA::is_nil( engine ) )    {
        MESSAGE("Errors in study or in MCCAD engine!");
        return ;
    }
    MCCAD_ORB::MaterialList * aMaterialList = engine->getMaterialList(studyID); //don't delete!
    int n = aMaterialList->length();
    if (!n){
        MESSAGE("No material data!");
        return ;
    }

    /** Print all the material included */
    theStream<<"COMPOSITION "<< n <<endl;
    theStream<<endl;

    for(int i = 0; i < n; i++)
    {
        MCCAD_ORB::Material_var aMaterial = (*aMaterialList)[i];
        QString strMatCard = aMaterial->getMatCard();
        //Because material name might be identical or empty, we use ID+Name
        QString aMatName = QString("%1_").arg(aMaterial->getID()) + aMaterial->getName();
        theStream<<setw(8)<<""<<"POINT_WISE 300 "<<aMatName.toStdString()<<endl;
        theStream<<setw(8)<<""<<"nb(istopes)"<<endl;
        theStream<<setw(8)<<""<<strMatCard.toStdString()<<endl;
        theStream<<endl;
    }
    theStream<<"END_COMPOSITION "<<endl;
    theStream<<endl;

    //sort the Volume and Material relation
    //Be careful with iCellNum, because in PrintACellDesc, the envelop cell is skip without increase iCellNum
    //therefore the implementation here is different from McCadTripoliWriter::PrintMatCard
    Standard_Integer iCellNum = m_iInitCellNum;
    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);

    map<int, vector<int> > mapMat2Solid;
    /** Print the material name and the solids belong to this material group */
    for (unsigned int i = 0; i < (pData->m_SolidList).size(); i++)
    {
        S_McCadSolid * pSolid = static_cast <S_McCadSolid *> (pData->m_SolidList.at(i));  //qiu cast to S_McCadSolid
        assert(pSolid);

        //get the Mat ID of this solid
        if (pSolid->GetisEnvelop()) continue; //skip the envelop solid, which used only for MCNP6

        MCCAD_ORB::Group_var aGroup = engine->getGroup(studyID, pSolid->GetGroupID());
        int aMatID = aGroup->getMaterialID();

        map<int,vector<int> >::iterator iter;
        iter = mapMat2Solid.find(aMatID);
        if (iter != mapMat2Solid.end())  {
            iter->second.push_back(iCellNum);
        }
        else   {
            vector<int> cell_list;
            cell_list.push_back(iCellNum);
            mapMat2Solid.insert(pair<int,vector<int> >(aMatID,cell_list));
        }
        iCellNum ++;
    }

    //print the relation
    theStream<<"GEOMCOMP"<<endl;
    theStream<<endl;
    map<int,vector<int> >::iterator iter;
    iter = mapMat2Solid.begin();
    while (iter != mapMat2Solid.end())
    {
        //skip when material ID == 0, because Material ID should > 0
        if (iter->first == 0) continue;

        int iNum = (iter->second).size();  //contain solids
        MCCAD_ORB::Material_var aMaterial = engine->getMaterial(studyID, iter->first);
        //Because material name might be identical or empty, we use ID+Name
        QString aMatName = QString("%1_").arg(aMaterial->getID()) + aMaterial->getName();
        theStream<<setw(8)<<""<<aMatName.toStdString()<<" "<<iNum<<" ";
        for (int i = 0; i < iNum; i++ )
        {
            if ( i%10 == 0)
            {
                theStream<<endl;
                theStream<<setw(9+aMatName.size())<<"";
            }
            theStream<<(iter->second).at(i)<<" ";
        }
        theStream<<endl;
        iter++;
    }
    theStream<<endl;
    theStream<<"END_GEOMCOMP"<<endl;

}


void S_McCadTripoliWriter::S_PrintComponentInfo(const int & ComponentID, Standard_OStream& theStream)
{
    //get the studyid and engine
    int studyID = SUIT_Session::session()->activeApplication()->activeStudy()->id();
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    if ( !studyID || CORBA::is_nil( engine ) )    {
        MESSAGE("Errors in study or in MCCAD engine!");
        return ;
    }
    //find the component
    MCCAD_ORB :: Component_var aComponent = engine->getComponent(studyID, ComponentID);
    if (aComponent->_is_nil()) {
        MESSAGE("Component is nil!");
        return ;
    }
    //get infomation
    theStream <<"/*"<<endl;
    theStream<<"      "<<"=============================================================================="<<endl;
    theStream<<"      "<<setw(8)<<""<<"Component:    "<<aComponent->getName()<<endl;
    if (!QString(aComponent->getRemark()).trimmed().isEmpty())
        theStream<<"      "<<setw(8)<<""<<"INFO:       "<<QString(aComponent->getRemark()).trimmed().toStdString()<<endl;
    theStream<<"      "<<setw(8)<<""<<"Contain Group:       "<<endl;
    int n = aComponent->getGroupList()->length();
    for (int i=0; i<n; i++)
        theStream<<"      "<<setw(8)<<""<<"              "<<(*aComponent->getGroupList())[i]->getName()<<endl;
    theStream<<"      "<<"=============================================================================="<<endl;
    theStream <<"*/"<<endl;

}

void  S_McCadTripoliWriter::PrintGroupInfo(const int & GroupID, Standard_OStream& theStream)
{
    //get the studyid and engine
    int studyID = SUIT_Session::session()->activeApplication()->activeStudy()->id();
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    if ( !studyID || CORBA::is_nil( engine ) )    {
        MESSAGE("Errors in study or in MCCAD engine!");
        return ;
    }
    //find the Group
    MCCAD_ORB :: Group_var aGroup = engine->getGroup(studyID, GroupID);
    if (aGroup->_is_nil()) {
        MESSAGE("Group is nil!");
        return ;
    }
    //get infomation
    theStream <<"/*"<<endl;
    theStream<<"            ======================================================            "<<endl;
    theStream<<"      "<<setw(11)<<""<<"Group:    "<<aGroup->getName()<<endl;
    if (!QString(aGroup->getRemark()).trimmed().isEmpty())
        theStream<<"      "<<setw(11)<<""<<"INFO:       "<<QString(aGroup->getRemark()).trimmed().toStdString()<<endl;
    if (aGroup->hasMaterial()) {
        MCCAD_ORB::Material_var aMaterial = aGroup->getMaterial();
//        theStream<<"// "<<setw(8)<<""<<"Material: "<<aMaterial->getName()<<endl;
        theStream<<"      "<<setw(11)<<""<<"Material ID:   "<<aMaterial->getID()<<endl;
//        theStream<<"// "<<setw(8)<<""<<"Density:       "<<aMaterial->getDensity()<<endl;
    }
    theStream<<"      "<<setw(11)<<""<<"Contains "<<aGroup->getPartList()->length() << " cells."<<endl;
    theStream<<"            ======================================================            "<<endl;
    theStream <<"*/"<<endl;
}

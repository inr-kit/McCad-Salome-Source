#include "McCadTripoliWriter.hxx"

#include <assert.h>

#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>
#include <QString>
#include <QStringList>

#include "McCadGeomCylinder.hxx"
#include "McCadGeomSphere.hxx"
#include "McCadGeomCone.hxx"
#include <math.h> //qiu

#include "../McCadTool/McCadMathTool.hxx"

McCadTripoliWriter::McCadTripoliWriter()
{
    m_mapSymb.clear();
    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("PX","PLANEX"));
    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("PY","PLANEY"));
    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("PZ","PLANEZ"));
    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("P","PLANE"));

    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("CX","CYLX"));
    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("CY","CYLY"));
    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("CZ","CYLZ"));
    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("C/X","CYLX"));
    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("C/Y","CYLY"));
    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("C/Z","CYLZ"));

    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("KX","CONEX"));
    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("KY","CONEY"));
    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("KZ","CONEZ"));
    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("K/X","CONEX"));
    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("K/Y","CONEY"));
    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("K/Z","CONEZ"));

    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("S","SPHERE"));
    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("SX","SPHERE"));
    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("SY","SPHERE"));
    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("SZ","SPHERE"));
    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("SO","SPHERE"));

    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("TX","TORUSX"));
    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("TY","TORUSY"));
    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("TZ","TORUSZ"));

    m_mapSymb.insert(pair<TCollection_AsciiString,TCollection_AsciiString>("GQ","QUAD"));
}

McCadTripoliWriter::~McCadTripoliWriter()
{
}


/** ********************************************************************
* @brief Set the initial virtual cell number
*
* @param Standard_OStream & theStream (modify)
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
void McCadTripoliWriter::SetVirtCellNum(Standard_Integer theNum)
{
    m_virtualCellNum = theNum;
}


/** ********************************************************************
* @brief Print the head description of TRIPOLI input file.
*
* @param Standard_OStream & theStream (modify)
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
void McCadTripoliWriter::PrintHeadDesc(Standard_OStream& theStream)
{
    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);

    theStream.setf(ios::left);
    theStream<<"GEOMETRY"<<endl;
    theStream<<"TITLE title (McCad 0.4.0 generated Input)"<<endl;
    theStream<<"/*"<<endl;
    theStream<<"   * ============================================================================= *"<<endl;
    theStream<<"     * Material cells  ---- "<<pData->m_SolidList.size()<<endl;
    theStream<<"     * Void cells  -------- "<<pData->m_VoidCellList.size()<<endl;
    theStream<<"     * Surfaces    -------- "<<pData->m_GeomSurfList.size()<<endl;
    theStream<<"     * Outer Void  -------- "<<1<<endl;
    theStream<<"   * ============================================================================= *"<<endl;
    theStream<<"*/"<<endl;
    theStream<<endl;
}


/** ********************************************************************
* @brief Print the cell geometry description
*
* @param Standard_OStream & theStream
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
void McCadTripoliWriter::PrintCellDesc(Standard_OStream& theStream)
{
    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);
    Standard_Integer iCellNum = m_iInitCellNum;

    theStream.setf(ios::left);
    theStream<<"/* ============================= Volume Data Definitions ========================= */"<<endl;

    /** Update the face number after surfaces sorting. */
    for (unsigned int i = 0; i < (pData->m_SolidList).size(); i++)
    {
        if (m_bHaveMaterial)
        {
            PrintGroupInfo(i+1,theStream);
        }

        McCadSolid * pSolid = (pData->m_SolidList).at(i);
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
                theStream<<setw(6)<<"VOLU"<<setw(8)<<iCellNum++; // Output the cell number.
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
}


/** ********************************************************************
* @brief Print the group information before each group, it will check
*        the m_groupInfo, if the solid number is stored in the map, the
*        group information will be printed.
*
* @param int iSolid, Standard_OStream & theStream
* @return void
*
* @date 11/11/2013
* @author  Lei Lu
***********************************************************************/
void McCadTripoliWriter::PrintGroupInfo(const int iSolid, Standard_OStream& theStream)
{
    MaterialManager * pMatManager = m_pManager->GetMatManager();
    assert(pMatManager);

    MaterialGroup * pGroup = pMatManager->GetGroupData(iSolid);
    if(pGroup != NULL)
    {
        QString groupName = pGroup->GetGroupName();
        int iMatID = pGroup->GetMaterialID();
        double fDensity = pGroup->GetDensity();
        QString materialName = pGroup->GetMaterialName();

        int iStartNum = pGroup->GetStartNum() ;
        int iEndNum = pGroup->GetEndNum();
        int iCellNum = pGroup->GetCellNum();

        iStartNum += (m_iInitCellNum-1);
        iEndNum += (m_iInitCellNum-1);

        TCollection_AsciiString group_info;
        group_info = "From ";
        group_info += TCollection_AsciiString(iStartNum);
        group_info += " to ";
        group_info += TCollection_AsciiString(iEndNum);
        group_info += " , ";
        group_info += TCollection_AsciiString(iCellNum);
        group_info += " cells";
        int iLength = group_info.Length()%49;

        theStream<<"/*"<<endl;
        theStream<<"      "<<"o=======================================================================o"<<endl;
        theStream<<"      |"<<setw(5)<<""<<"* Group Name:    "<<setw(49)<<groupName.toStdString()<<"|"<<endl;
        theStream<<"      |"<<setw(5)<<""<<"* Material Name: "<<setw(49)<<materialName.toStdString()<<"|"<<endl;
        theStream<<"      |"<<setw(5)<<""<<"* Material ID:   "<<setw(49)<<iMatID<<"|"<<endl;
        theStream<<"      |"<<setw(5)<<""<<"* Density:       "<<setw(49)<<fDensity<<"|"<<endl;
        theStream<<"      |"<<setw(5)<<""<<"* Cell Number:   "<<group_info<<setw(49-iLength)<<""<<"|"<<endl;
        theStream<<"      "<<"o=======================================================================o"<<endl;
        theStream<<"*/"<<endl;
    }
    else
    {
        return;
    }
}



/** ********************************************************************
* @brief Print the void cell's description
*
* @param Standard_OStream & theStream
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
void McCadTripoliWriter::PrintVoidDesc(Standard_OStream& theStream)
{
    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);

    /** Print the head information of the void card */
    TCollection_AsciiString void_info;
    void_info = TCollection_AsciiString(int((pData->m_VoidCellList).size())) + " Cells";
    int iLength = void_info.Length()%50;

    theStream<<"/*"<<endl;
    theStream<<"      "<<"o=======================================================================o"<<endl;
    theStream<<"      |"<<setw(71)<<""<<"|"<<endl;
    theStream<<"      |"<<setw(5)<<""<<"* Void spaces:  "<<void_info<<setw(50 - iLength)<<""<<"|"<<endl;
    theStream<<"      |"<<setw(71)<<""<<"|"<<endl;
    theStream<<"      "<<"o=======================================================================o"<<endl;
    theStream<<"*/"<<endl;

    /** Print the detail description of void geometry */
    Standard_Integer iCellNum = (pData->m_ConvexSolidList).size() + m_iInitCellNum;
    for (unsigned int i = 0; i < (pData->m_VoidCellList).size(); i++)
    {
        McCadVoidCell * pVoid = (pData->m_VoidCellList).at(i);
        assert(pVoid);

        vector<int> complement_list;
        vector<McCadTripoliCell *> cell_list = GenTripoliVoidCellList(pVoid);

        int iNumEpn = cell_list.size();   // The number of virtual solids
        for(int j = 0; j < iNumEpn-1; j++)
        {
            McCadTripoliCell *pCell = cell_list.at(j);
            if (pCell->hasVirtualCell())
            {
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

                theStream<<setw(6)<<"VOLU"<<setw(8)<<++m_virtualCellNum; // Output the cell number.
                TCollection_AsciiString strCellDesc = pCell->GetEqua();
                theStream<<strCellDesc<<endl;
                theStream<<setw(14)<<""<<"VMINUS "<<sub_list.size();
                for(int iSubCell = 0; iSubCell < sub_list.size(); iSubCell++)
                {
                    theStream<<" "<<sub_list.at(iSubCell);
                }
                theStream<<endl;
                theStream<<setw(14)<<""<<"FICTIVE ENDV"<<endl;
                sub_list.clear();
            }
            else
            {
                theStream.setf(ios::left);
                theStream<<setw(6)<<"VOLU"<<setw(8)<<++m_virtualCellNum; // Output the cell number.
                TCollection_AsciiString strCellDesc = pCell->GetEqua();
                theStream<<strCellDesc<<" FICTIVE ENDV"<<endl;
            }
            complement_list.push_back(m_virtualCellNum);
        }

        McCadTripoliCell *pLastCell = cell_list.at(iNumEpn-1);

        theStream<<setw(6)<<"VOLU"<<setw(8)<<iCellNum++; // Output the cell number.
        TCollection_AsciiString strLastCellDesc = pLastCell->GetEqua();
        theStream<<strLastCellDesc;

        if (iNumEpn > 1)
        {
            theStream<<endl;
            theStream<<setw(14)<<""<<"VMINUS "<<complement_list.size();
            for(int m = 0; m < complement_list.size(); m++)
            {
                if (m % 10 == 0 && m != 0)
                {
                    theStream<<endl;
                    theStream<<setw(13)<<"";
                }
                theStream<<" "<<complement_list.at(m);
            }
            theStream<<endl;
            theStream<<setw(14)<<""<<"ENDV"<<endl;
            complement_list.clear();
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

    PrintOutSpace(theStream);
}


/** ********************************************************************
* @brief Print the void cell's description
*
* @param Standard_OStream & theStream
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
void McCadTripoliWriter::PrintOutSpace(Standard_OStream& theStream)
{
    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);

    McCadVoidCell * pOutVoid = pData->m_pOutVoid;
    vector<int> outer_list;

    vector<McCadTripoliCell *> cell_list;
    cell_list = GenOuterSpaceList(pOutVoid);

    int iNumEpn = cell_list.size();   // The number of virtual solids
    for(int iOutCell = 0; iOutCell < iNumEpn; iOutCell++)
    {
        McCadTripoliCell *pCell = cell_list.at(iOutCell);
        theStream<<setw(6)<<"VOLU"<<setw(8)<<++m_virtualCellNum; // Output the cell number.
        TCollection_AsciiString strCellDesc = pCell->GetEqua();
        theStream<<strCellDesc;
        if(iOutCell < iNumEpn-1)
        {
            theStream<<" FICTIVE ENDV";
            outer_list.push_back(m_virtualCellNum);
        }
        theStream<<endl;
    }

    theStream<<setw(14)<<""<<"UNION "<<outer_list.size();
    for(int m = 0; m < outer_list.size(); m++)
    {
        theStream<<" "<<outer_list.at(m);
    }
    theStream<<endl;
    theStream<<setw(14)<<""<<"ENDV"<<endl;
    outer_list.clear();

    for( int k = 0; k < cell_list.size(); k++ )
    {
        McCadTripoliCell *pCell = cell_list.at(k);
        delete pCell;
        pCell = NULL;
    }
    cell_list.clear();
}



/** ********************************************************************
* @brief Print the transform card
*
* @param Standard_OStream& theStream
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
void McCadTripoliWriter::PrintTrsfDesc(Standard_OStream& theStream)
{
}



/** ********************************************************************
* @brief Print the material card
*
* @param Standard_OStream& theStream
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
void McCadTripoliWriter::PrintMatCard(Standard_OStream& theStream)
{
    theStream.setf(ios::left);
    MaterialManager * pMatManager = m_pManager->GetMatManager();
    assert(pMatManager);

    Material * pMat = NULL;
    vector<Material *> material_list = pMatManager->GetMatList();

    /** Print all the material included */
    theStream<<"COMPOSITION "<<material_list.size()<<endl;
    theStream<<endl;

    for(int i = 0; i < material_list.size(); i++)
    {
        pMat = material_list.at(i);
        QString material_name = pMat->GetMatName();
        theStream<<setw(8)<<""<<"POINT_WISE 300 "<<material_name.toStdString()<<endl;
        theStream<<setw(8)<<""<<"nb(istopes)"<<endl;
        theStream<<endl;
    }    

    theStream<<setw(8)<<""<<"POINT_WISE 300 "<<"Mat_Air"<<endl;
    theStream<<setw(8)<<""<<"nb(istopes)"<<endl;
    theStream<<endl;

    theStream<<"END_COMPOSITION "<<endl;
    theStream<<endl;

    Standard_Integer iCellNum = m_iInitCellNum;
    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);

    map<TCollection_AsciiString, vector<int> > group_list;
    /** Print the material name and the solids belong to this material group */
    for (unsigned int i = 0; i < (pData->m_SolidList).size(); i++)
    {
        McCadSolid * pSolid = (pData->m_SolidList).at(i);
        assert(pSolid);

        TCollection_AsciiString material_name = pSolid->GetName();

        if(material_name.IsEmpty())
        {
            continue;
        }

        map<TCollection_AsciiString,vector<int> >::iterator iter;
        iter = group_list.find(material_name);
        if (iter != group_list.end())
        {
            iter->second.push_back(iCellNum + i);
        }
        else
        {
            vector<int> cell_list;
            cell_list.push_back(iCellNum + i);
            group_list.insert(pair<TCollection_AsciiString,vector<int> >(material_name,cell_list));
        }
    }

    /** ****************** print the materials of void cells ************************/
    Standard_Integer iMatSolid = (pData->m_SolidList).size();
    for (unsigned int k = 0; k < (pData->m_VoidCellList).size(); k++)
    {
        TCollection_AsciiString material_name = "Mat_Air";

        map<TCollection_AsciiString,vector<int> >::iterator iter;
        iter = group_list.find(material_name);
        if (iter != group_list.end())
        {
            iter->second.push_back(iCellNum + k + iMatSolid);
        }
        else
        {
            vector<int> cell_list;
            cell_list.push_back(iCellNum + k + iMatSolid);
            group_list.insert(pair<TCollection_AsciiString,vector<int> >(material_name,cell_list));
        }
    }

    theStream<<"GEOMCOMP"<<endl;
    theStream<<endl;

    map<TCollection_AsciiString,vector<int> >::iterator iter;
    iter = group_list.begin();
    while (iter != group_list.end())
    {
        int iNum = (iter->second).size();
        TCollection_AsciiString material_name = iter->first;
        theStream<<setw(8)<<""<<material_name<<" "<<iNum<<" ";
        for (int i = 0; i < iNum; i++ )
        {
            if ( i%10 == 0)
            {
                theStream<<endl;
                theStream<<setw(9+material_name.Length())<<"";
            }
            theStream<<(iter->second).at(i)<<" ";
        }
        theStream<<endl;
        iter++;
    }
    theStream<<endl;
    theStream<<"END_GEOMCOMP"<<endl;
}


/** ********************************************************************
* @brief Print the geometry data into a file ,include head, cells,
*        surface and transform card.
*
* @param TCollection_AsciiString theFileName
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
void McCadTripoliWriter::PrintFile()
{
    if (m_OutputFileName.IsEmpty())
    {
        cout<<"File Name is empty!"<<endl;
        return;
    }
    else
    {
        OSD_Path thePath(m_OutputFileName);
        OSD_File theFile(thePath);
        theFile.Build(OSD_ReadWrite , OSD_Protection());

        TCollection_AsciiString FileName = thePath.Name() + thePath.Extension();
        const char* strName = FileName.ToCString();
        ofstream theStream(strName);

        PrintHeadDesc(theStream);
        PrintSurfDesc(theStream);
        PrintCellDesc(theStream);
        if (m_bGenerateVoid == Standard_True)
        {
            PrintVoidDesc(theStream);
        }
        theStream<<"ENDGEOM"<<endl;
        theStream<<endl;
        if(m_bHaveMaterial)
        {
            PrintMatCard(theStream);
        }
        theFile.Close();
    }
}



/** ********************************************************************
* @brief Generate the TRIPOLI cell from a convex solid
*
* @param
* @return
*
* @date 19/12/2013
* @author  Lei Lu
***********************************************************************/
vector<McCadTripoliCell *>  McCadTripoliWriter::GenTripoliCellList(McCadSolid *& pSolid)
{
    vector<McCadTripoliCell *> cell_list;
    int iInitSurfNum = m_iInitFaceNum - 1;

    int iConvexSolid = (pSolid->GetConvexSolidList()).size();
    for(int j = 0; j < iConvexSolid; j++)
    {
        McCadConvexSolid *pConvexSolid = pSolid->GetConvexSolidList().at(j);
        assert(pConvexSolid);

        McCadTripoliCell *pTriCell = new McCadTripoliCell();

        for (Standard_Integer i = 0; i < (pConvexSolid->GetFaces()).size(); i++)
        {
            vector<int> auxface_list;    // Store some auxilury surfaces which attribute is 1
            McCadExtFace * pExtFace = (pConvexSolid->GetFaces()).at(i);
            int iFaceNum = pExtFace->GetFaceNum();
            iFaceNum > 0 ? iFaceNum += iInitSurfNum : iFaceNum -= iInitSurfNum;

            pTriCell->AddSurface(iFaceNum);


            if (pExtFace->HaveAuxSurf())
            {
                for(Standard_Integer j = 0; j < pExtFace->GetAuxFaces().size(); j++)
                {
                    McCadExtFace *pAuxFace = pExtFace->GetAuxFaces().at(j);
                    int iAuxFaceNum = pAuxFace->GetFaceNum();
                    iAuxFaceNum > 0 ? iAuxFaceNum += iInitSurfNum : iAuxFaceNum -= iInitSurfNum;

                    if (pAuxFace->GetAttri() == 1)
                    {
                       auxface_list.push_back(-iAuxFaceNum);
                    }
                    else
                    {
                       pTriCell->AddSurface(iAuxFaceNum);
                    }
                }
            }

            if (auxface_list.size() != 0)
            {
                McCadTripoliCell *pCldCell = new McCadTripoliCell();
                for (Standard_Integer k = 0; k < auxface_list.size(); k++)
                {
                    int iFaceNum = auxface_list.at(k);
                    pCldCell->AddSurface(iFaceNum);
                }
                pTriCell->AddCldCell(pCldCell); // Add the virtual cell consisted of auxiliary surfaces into parent cell
                pTriCell->SetBoolOp(2);         // Substract the virtual cell
            }
            auxface_list.clear();
        }

        cell_list.push_back(pTriCell);
    }

    return cell_list;
}




/** ********************************************************************
* @brief
*
* @param
* @return
*
* @date
* @author  Lei Lu
***********************************************************************/
vector<McCadTripoliCell *> McCadTripoliWriter::GenTripoliVoidCellList(McCadVoidCell *& pVoidCell)
{
    vector<McCadTripoliCell *> cell_list;
    int iInitSurfNum = m_iInitFaceNum - 1;

    for (Standard_Integer i = 0; i < (pVoidCell->GetCollision()).size(); i++)
    {
        McCadTripoliCell *pTriCell = new McCadTripoliCell();
        COLLISION collision = (pVoidCell->GetCollision()).at(i);
        for (Standard_Integer j = 0; j < (collision.FaceList).size(); j++)
        {
            int iCollidFaceNum = collision.FaceList.at(j);
            iCollidFaceNum > 0 ? iCollidFaceNum += iInitSurfNum : iCollidFaceNum -= iInitSurfNum;
            pTriCell->AddSurface(iCollidFaceNum);
        }

        for (Standard_Integer j = 0; j < (collision.AuxFaceList).size(); j++)
        {
            vector<int> auxiliary_face_list = collision.AuxFaceList.at(j);
            McCadTripoliCell *pCldCell = new McCadTripoliCell();
            for (Standard_Integer k = 0; k < auxiliary_face_list.size(); k++)
            {
                int iAuxFaceNum = auxiliary_face_list.at(k);
                iAuxFaceNum > 0 ? iAuxFaceNum += iInitSurfNum : iAuxFaceNum -= iInitSurfNum;
                pCldCell->AddSurface(-iAuxFaceNum);
            }
            pTriCell->AddCldCell(pCldCell); // Add the virtual cell consisted of auxiliary surfaces into parent cell
            pTriCell->SetBoolOp(2);         // Substract the virtual cell
        }
        cell_list.push_back(pTriCell);
    }

    McCadTripoliCell *pBox = new McCadTripoliCell();
    for (Standard_Integer i = 0; i < (pVoidCell->GetFaces()).size(); i++)
    {
        McCadExtFace * pExtFace = (pVoidCell->GetFaces()).at(i);
        int iFaceNum = pExtFace->GetFaceNum();
        iFaceNum > 0 ? iFaceNum += iInitSurfNum : iFaceNum -= iInitSurfNum;
        pBox->AddSurface(iFaceNum);
    }
    cell_list.push_back(pBox);

    return cell_list;
}


vector<McCadTripoliCell *> McCadTripoliWriter::GenOuterSpaceList(McCadVoidCell *& pVoidCell)
{
    vector<McCadTripoliCell *> outer_list;
    int iInitSurfNum = m_iInitFaceNum - 1;

    for (Standard_Integer i = 0; i <  (pVoidCell->GetFaces()).size(); i++)
    {
        McCadTripoliCell *pTriCell = new McCadTripoliCell();
        McCadExtFace * pExtFace =  (pVoidCell->GetFaces()).at(i);

        int iFaceNum = pExtFace->GetFaceNum();
        iFaceNum > 0 ? iFaceNum += iInitSurfNum : iFaceNum -= iInitSurfNum;
        iFaceNum *= -1;

        pTriCell->AddSurface(iFaceNum);
        outer_list.push_back(pTriCell);
    }
    return outer_list;
}



/** ********************************************************************
* @brief Print the surface list
*
* @param Standard_OStream & theStream
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
void McCadTripoliWriter::PrintSurfDesc(Standard_OStream & theStream)
{
    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);

    theStream<<"/* ============================ Surface Data Definitions ========================= */"<<endl;
    theStream<<endl;

    Standard_Integer iNum = m_iInitFaceNum; // Set the initial surface number.

    vector<IGeomFace *>::iterator iterPos;
    for (iterPos = (pData->m_GeomSurfList).begin(); iterPos != (pData->m_GeomSurfList).end(); ++iterPos)
    {
       IGeomFace * pFace = *iterPos;
       assert(pFace);
       theStream.setf(ios::left);
       theStream<<"SURF "<<setw(6)<< iNum++;            // Surface number

       theStream<<setw(4)<< GetSurfSymb(pFace);         // The symbol of surface type
       // Output the each parameter of surface
       theStream.setf(ios::fixed);
       theStream<<setprecision(7);

       vector<Standard_Real> surf_list = GetSurfPrmt(pFace);
       for (Standard_Integer i = 0; i < surf_list.size(); i++ )
       {
           if ( i%4 == 0 && i != 0 )
           {
               theStream<<endl;
               theStream<<setw(15)<<"";
           }
           theStream.unsetf(ios::left);
           theStream.setf(ios::right);
           theStream<<setw(15)<<surf_list.at(i)<<" ";
       }
       theStream<<endl;
       theStream.unsetf(ios::right);

       surf_list.clear();
    }
    theStream<<endl;
}



/** ********************************************************************
* @brief
*
* @param
* @return
*
* @date
* @author  Lei Lu
***********************************************************************/
TCollection_AsciiString McCadTripoliWriter::GetSurfSymb(IGeomFace *& pFace)
{
    TCollection_AsciiString mcnp_symb = pFace->GetSurfSymb();
    TCollection_AsciiString tri_symb;

    map<TCollection_AsciiString, TCollection_AsciiString>::iterator iter;
    iter = m_mapSymb.find(mcnp_symb);
    if(iter != m_mapSymb.end())
    {
        tri_symb = iter->second;
    }
    else
    {
        tri_symb = mcnp_symb;
    }
    return tri_symb;
}



/** ********************************************************************
* @brief Get the parameters of surface according to the format of TRIPOLI
*        surface describtion
*
* @param IGeom * pFace
* @return vector<Standard_Real> parameters list
*
* @date 09/10/2013
* @modify 10/09/2014
* @author  Lei Lu
***********************************************************************/
vector<Standard_Real> McCadTripoliWriter::GetSurfPrmt(IGeomFace *& pFace)
{
    vector<Standard_Real> prmt_list;
    TCollection_AsciiString McnpSymb = pFace->GetSurfSymb();
    if (McnpSymb == "P")
    {
        int iNum = (pFace->GetPrmtList()).size();
        for (Standard_Integer i = 0; i < iNum-1; i++ )
        {
            prmt_list.push_back(pFace->GetPrmtList().at(i));
        }
        prmt_list.push_back(-1*pFace->GetPrmtList().at(iNum-1));
        return prmt_list;
    }

    if ( McnpSymb == "CX" )
    {
        McCadGeomCylinder * pCyl = (McCadGeomCylinder*)(pFace);        
        prmt_list.push_back(pCyl->GetPoint().Y());
        prmt_list.push_back(pCyl->GetPoint().Z());
        prmt_list.push_back(pCyl->GetRadius());
        return prmt_list;
    }

    if ( McnpSymb == "CY" )
    {
        McCadGeomCylinder * pCyl = (McCadGeomCylinder*)(pFace);
        prmt_list.push_back(pCyl->GetPoint().X());
        prmt_list.push_back(pCyl->GetPoint().Z());
        prmt_list.push_back(pCyl->GetRadius());
        return prmt_list;
    }

    if ( McnpSymb == "CZ" )
    {
        McCadGeomCylinder * pCyl = (McCadGeomCylinder*)(pFace);
        prmt_list.push_back(pCyl->GetPoint().X());
        prmt_list.push_back(pCyl->GetPoint().Y());
        prmt_list.push_back(pCyl->GetRadius());
        return prmt_list;
    }

    if (McnpSymb == "SO" || McnpSymb == "SY" || McnpSymb == "SZ"
                         ||McnpSymb == "SX" || McnpSymb == "S" )
    {
        McCadGeomSphere * pSph = (McCadGeomSphere*)(pFace);
        prmt_list.push_back(pSph->GetCenter().X());
        prmt_list.push_back(pSph->GetCenter().Y());
        prmt_list.push_back(pSph->GetCenter().Z());
        prmt_list.push_back(pSph->GetRadius());
        return prmt_list;
    }

    if (McnpSymb == "K/X" || McnpSymb == "K/Y" || McnpSymb == "K/Z"
        || McnpSymb == "KX" || McnpSymb == "KY" || McnpSymb == "KZ")
    {
        McCadGeomCone * pCon = (McCadGeomCone*)(pFace);
        prmt_list.push_back(pCon->GetApex().X());
        prmt_list.push_back(pCon->GetApex().Y());
        prmt_list.push_back(pCon->GetApex().Z());
        prmt_list.push_back((pCon->GetSemiAngle()/M_PI)*180.);
        return prmt_list;
    }

    int iNum = (pFace->GetPrmtList()).size();
    for (Standard_Integer i = 0; i < iNum; i++ )
    {
        prmt_list.push_back(pFace->GetPrmtList().at(i));
    }
    return prmt_list;
}




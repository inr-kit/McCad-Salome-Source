#include "McCadMcnpWriter.hxx"

#include <assert.h>

#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>
#include <QString>
#include <QStringList>

#include "../McCadTool/McCadMathTool.hxx"

McCadMcnpWriter::McCadMcnpWriter()
{
}

McCadMcnpWriter::~McCadMcnpWriter()
{
}

/** ********************************************************************
* @brief Print the head description of Mcnp input file.
*
* @param Standard_OStream & theStream (modify)
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
void McCadMcnpWriter::PrintHeadDesc(Standard_OStream& theStream)
{
    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);

    theStream.setf(ios::left);
    theStream<<"McCad 0.4.0 generated Input "<<endl;
    theStream<<"c "<<"============================================================================="<<endl;
    theStream<<"c     * Material cells  ---- "<<pData->m_SolidList.size()<<endl;
    theStream<<"c     * Void cells  -------- "<<pData->m_VoidCellList.size()<<endl;
    theStream<<"c     * Surfaces    -------- "<<pData->m_GeomSurfList.size()<<endl;
    theStream<<"c     * Outer Void  -------- "<<1<<endl;
    theStream<<"c "<<"============================================================================="<<endl;
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
void McCadMcnpWriter::PrintCellDesc(Standard_OStream& theStream)
{
    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);

    theStream<<"c "<<"=============================== Cells Card =================================="<<endl;

    Standard_Integer iCellNum = m_iInitCellNum;   // Get the initial cell number.

    for (unsigned int i = 0; i < pData->m_SolidList.size(); i++)
    {
        McCadSolid * pSolid = pData->m_SolidList.at(i);
        assert(pSolid);

        if (m_bHaveMaterial)
        {
            PrintGroupInfo(i+1,theStream);
        }

        theStream.setf(ios::left);
        theStream<<setw(6)<<iCellNum;                   // Output the cell number.
        theStream<<setw(4)<<pSolid->GetID();            // Output the material number.
        if(!m_bHaveMaterial)
        {
            theStream<<fixed<<setprecision(4)<<setw(1)<<"";
        }
        else
        {
            theStream<<fixed<<setprecision(4)<<setw(10)<<pSolid->GetDensity();  // Output the density.
        }

        TCollection_AsciiString strCellDesc;
        for(int j = 0; j < pSolid->GetConvexSolidList().size(); j++)
        {
            McCadConvexSolid *pConvexSolid = pSolid->GetConvexSolidList().at(j);
            assert(pConvexSolid);
            //pConvexSolid->ChangeFaceNum(*m_pManager); // Update the face number after sorting            

            if ( j == 0 )
            {
               strCellDesc += "(";
            }
            else
            {
               strCellDesc += ":(";
            }

            strCellDesc += GetCellExpn(pConvexSolid);            
            strCellDesc += ")&";

            strCellDesc.LeftAdjust();   // Trim the left and right space characters of string
            strCellDesc.RightAdjust();
        }

        // Split the expression of void cell into several part according to the brackets
        // and output them into different lines.
        TCollection_AsciiString str_left;
        TCollection_AsciiString str_right= strCellDesc;

        // If the length of expression beyond 80, then split it and print respectively.
        int k = 0;
        do
        {
            k++;
            str_left = str_right;
            str_right = str_left.Split(str_left.Search("&"));// & is the seperating symbol used.
            str_left.Remove(str_left.Length());
            if (k == 1)
            {
                theStream<<str_left<<endl;
            }
            else
            {
                theStream<<setw(20)<<""<<str_left<<endl;
            }
        }while(!str_right.IsEmpty());

        // Set the importances of neutron and photon
        if(!m_bHaveMaterial)
        {
            theStream<<setw(11)<<""<<"imp:n=1 imp:p=1"<<endl;
        }
        else
        {
            theStream<<setw(20)<<""<<"imp:n=1 imp:p=1"<<endl;
        }
        iCellNum ++;
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
void McCadMcnpWriter::PrintGroupInfo(const int iSolid, Standard_OStream& theStream)
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

        theStream<<"c "<<endl;
        theStream<<"c     "<<"o=======================================================================o"<<endl;
        theStream<<"c     |"<<setw(5)<<""<<"* Group Name:    "<<setw(49)<<groupName.toStdString()<<"|"<<endl;
        theStream<<"c     |"<<setw(5)<<""<<"* Material Name: "<<setw(49)<<materialName.toStdString()<<"|"<<endl;
        theStream<<"c     |"<<setw(5)<<""<<"* Material ID:   "<<setw(49)<<iMatID<<"|"<<endl;
        theStream<<"c     |"<<setw(5)<<""<<"* Density:       "<<setw(49)<<fDensity<<"|"<<endl;
        theStream<<"c     |"<<setw(5)<<""<<"* Cell Number:   "<<group_info<<setw(49-iLength)<<""<<"|"<<endl;
        theStream<<"c     "<<"o=======================================================================o"<<endl;
        theStream<<"c "<<endl;
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
void McCadMcnpWriter::PrintVoidDesc(Standard_OStream& theStream)
{
    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);

    // Print the title of void cells geometry description.
    TCollection_AsciiString void_info;
    void_info = TCollection_AsciiString(int(pData->m_VoidCellList.size() + 1)) + " Cells";
    int iLength = void_info.Length()%50;

    theStream<<"c "<<endl;
    theStream<<"c     "<<"o=======================================================================o"<<endl;
    theStream<<"c     |"<<setw(71)<<""<<"|"<<endl;
    theStream<<"c     |"<<setw(5)<<""<<"* Void spaces:  "<<void_info<<setw(50 - iLength)<<""<<"|"<<endl;
    theStream<<"c     |"<<setw(71)<<""<<"|"<<endl;
    theStream<<"c     "<<"o=======================================================================o"<<endl;
    theStream<<"c "<<endl;

    // Set the initial output number of void cell.
    Standard_Integer iCellNum = pData->m_SolidList.size() + m_iInitCellNum;
    for (unsigned int i = 0; i < pData->m_VoidCellList.size(); i++)
    {
        McCadVoidCell * pVoid = pData->m_VoidCellList.at(i);
        assert(pVoid);

        //pVoid->ChangeFaceNum(*m_pManager);    // Update the surface number after sorting the surface list.

        theStream.setf(ios::left);
        theStream<<setw(6)<<iCellNum;   // Output the cell number.
        theStream<<setw(5)<<0;          // Output the material number.

        iCellNum ++;

        // Split the expression of void cell into several part according to the brackets
        // and output them into different lines.
        TCollection_AsciiString str_left;
        TCollection_AsciiString str_right= GetVoidExpn(pVoid);
        int j = 0;
        do{
            j++;
            str_left = str_right;
            str_right = str_left.Split(str_left.Search("&"));  // & is the seperating symbol used.
            str_left.Remove(str_left.Length());
            if (j == 1)
            {
                theStream<<str_left<<endl;
            }
            else
            {
                {
                    theStream<<setw(11)<<""<<str_left<<endl;
                }
            }

        }while(!str_right.IsEmpty());

        theStream<<setw(11)<<""<<"imp:n=1 imp:p=1"<<endl; // Output the importance.
    }

    assert(pData->m_pOutVoid);
    //pData->m_pOutVoid->ChangeFaceNum(*m_pManager); // Update the face number.

    theStream.setf(ios::left);
    theStream<<setw(6)<<iCellNum;
    theStream<<setw(5)<<0;

    theStream<<pData->m_pOutVoid->GetOutVoidExpression()<<endl;// Output the outer spaces except for void and material solids.
    theStream<<setw(11)<<""<<"imp:n=0 imp:p=0"<<endl;   // Output the importance.
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
void McCadMcnpWriter::PrintSurfDesc(Standard_OStream & theStream)
{
    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);

    theStream<<endl;
    theStream<<"c "<<"============================== Surfaces Card ================================"<<endl;
    theStream<<"c "<<endl;

    Standard_Integer iNum = m_iInitFaceNum; // Set the initial surface number.
    vector<IGeomFace *>::iterator iterPos;
    for (iterPos = pData->m_GeomSurfList.begin(); iterPos != pData->m_GeomSurfList.end(); ++iterPos)
    {
        IGeomFace * pFace = * iterPos;
        assert(pFace);
        theStream.setf(ios::left);
        theStream<<setw(6)<< iNum++;                        // Surface number
        theStream<<setw(4)<< pFace->GetTransfNum();         // Transform card number
        theStream<<setw(4)<< pFace->GetSurfSymb();          // The symbol of surface type

        // Output the each parameter of surface
        theStream.setf(ios::fixed);
        theStream<<setprecision(7);
        for (Standard_Integer i = 0; i < (pFace->GetPrmtList()).size(); i++ )
        {
            if ( i%4 == 0 && i != 0 )
            {
                theStream<<endl;
                theStream<<setw(14)<<"";
            }
            theStream.unsetf(ios::left);
            theStream.setf(ios::right);
            theStream<<setw(15)<<pFace->GetPrmtList().at(i)<<" ";
        }
        theStream<<endl;
        theStream.unsetf(ios::right);
    }
    theStream<<endl;
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
void McCadMcnpWriter::PrintTrsfDesc(Standard_OStream& theStream)
{
    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);

    vector<McCadTransfCard *>::iterator iter;
    for (iter = pData->m_TransfCardList.begin(); iter != pData->m_TransfCardList.end(); ++iter)
    {
        McCadTransfCard * pTrCard = * iter;
        assert(pTrCard);

        theStream<<"TR"<<pTrCard->GetTrNum();

        theStream.unsetf(ios::fixed);
        for (Standard_Integer i = 0; i < (pTrCard->GetPrmtList()).size(); i++ )
        {
            if ( i%4 == 0 && i != 0 )
            {
                theStream<<endl;
                theStream<<setw(3)<<"";
            }
            theStream<<setw(15)<<setprecision(8)<<(pTrCard->GetPrmtList()).at(i);
        }
        theStream<<endl;
    }
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
void McCadMcnpWriter::PrintFile()
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
        PrintCellDesc(theStream);// Lei Lu 26.11.2013

        if (m_bGenerateVoid == Standard_True)
        {
            PrintVoidDesc(theStream);
        }
        PrintSurfDesc(theStream);
        PrintTrsfDesc(theStream);
        if(m_bHaveMaterial)
        {
           PrintMatCard(theStream);
        }
//qiu not implement PrintVolumeCard(theStream);
        theFile.Close();
    }
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
void McCadMcnpWriter::PrintMatCard(Standard_OStream& theStream)
{
    theStream.setf(ios::left);
    theStream<<"c "<<"============================= Materials Card ================================"<<endl;

    MaterialManager * pMatManager = m_pManager->GetMatManager();
    assert(pMatManager);

    Material * pMat = NULL;
    vector<Material *> material_list = pMatManager->GetMatList();

    for(int i = 0; i < material_list.size(); i++)
    {
        pMat = material_list.at(i);

        theStream<<"c "<<endl;
        theStream<<"c    "<<"o===================================================================o"<<endl;
        theStream<<"c    |"<<setw(5)<<""<<"* Material ID:   "<<setw(45)<<pMat->GetMatID()<<"|"<<endl;
        theStream<<"c    |"<<setw(5)<<""<<"* Material Name: "<<setw(45)<<pMat->GetMatName().toStdString()<<"|"<<endl;
        theStream<<"c    |"<<setw(5)<<""<<"* Density:       "<<setw(45)<<pMat->GetMatDensity()<<"|"<<endl;
        theStream<<"c    "<<"o===================================================================o"<<endl;
        theStream<<"c "<<endl;

        QString strMatCard = pMat->GetMatCard();
        QStringList list;
        list = strMatCard.split("\n");

        bool bPrintMatID = false;                       // The material id has been printed or not

        for (int j = 0; j < list.size(); j++)
        {
            QString str = list.at(j).trimmed();
            if (str.startsWith("c ")||(str == "c")
                    ||str.startsWith("C ")||(str == "C"))
            {
                theStream<<str.toStdString()<<endl;
            }
            else if(!bPrintMatID)
            {
                theStream.setf(ios::left);
                theStream<<"M"<<setw(7)<<pMat->GetMatID()<<str.toStdString()<<endl;
                bPrintMatID = true;
            }
            else
            {
                theStream<<setw(8)<<""<<str.toStdString()<<endl;
            }
        }
        theStream<<"c "<<endl;
    }
}



/** ********************************************************************
* @brief Get the MCNP cell expression of solid
*
* @param
* @return TCollection_AsciiString
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
TCollection_AsciiString McCadMcnpWriter::GetCellExpn(McCadConvexSolid *& pSolid)
{
    TCollection_AsciiString szExpression;

    TCollection_AsciiString szAuxFaceUni = "";
    TCollection_AsciiString szAuxFaceSub = "";

    int iInitSurfNum = m_iInitFaceNum-1;
    int iStrLength = 20; //initial string for describing the cell no and material

    for (Standard_Integer i = 0; i < (pSolid->GetFaces()).size(); i++)
    {

        McCadExtFace * pExtFace = (pSolid->GetFaces()).at(i);

        int iFaceNum = pExtFace->GetFaceNum();
        iFaceNum > 0 ? iFaceNum += iInitSurfNum : iFaceNum -= iInitSurfNum;

        iStrLength += TCollection_AsciiString(iFaceNum).Length();

        /***************** the length of cell expression can not be larger than 80 *******/
        /***************** & is used for splitting the expression ************************/
        if (iStrLength >= 80)
        {
            szExpression += "&";
            iStrLength = 20 + TCollection_AsciiString(iFaceNum).Length();
        }
        else
        {
            szExpression += " ";
            iStrLength ++;
        }
        /**********************************************************************************/

       szExpression += TCollection_AsciiString(iFaceNum);

        if (pExtFace->HaveAuxSurf())
        {
            for(Standard_Integer j = 0; j < pExtFace->GetAuxFaces().size(); j++)
            {
                McCadExtFace *pAuxFace = pExtFace->GetAuxFaces().at(j);

                int iAuxFaceNum = pAuxFace->GetFaceNum();
                iAuxFaceNum > 0 ? iAuxFaceNum += iInitSurfNum : iAuxFaceNum -= iInitSurfNum;

                if (pAuxFace->GetAttri() == 1)
                {
                    szAuxFaceUni += TCollection_AsciiString(iAuxFaceNum);
                    szAuxFaceUni += ":";
                }
                else
                {
                    szAuxFaceSub += TCollection_AsciiString(iAuxFaceNum);
                    szAuxFaceSub += " ";
                }
            }
        }
    }

    szExpression += " ";

    if (!szAuxFaceSub.IsEmpty())
    {
        szAuxFaceSub.Remove(szAuxFaceSub.Length());

        // Avoid the length of expression beyond 80
        iStrLength += szAuxFaceSub.Length();
        if (iStrLength >= 80)
        {
            szExpression += "&";
            iStrLength = 20;
        }

        szExpression += szAuxFaceSub;
        szExpression += " ";
    }

    if (!szAuxFaceUni.IsEmpty())
    {
        szAuxFaceUni.Remove(szAuxFaceUni.Length());

        // Avoid the length of expression beyond 80
        iStrLength += szAuxFaceUni.Length();
        if (iStrLength >= 80)
        {
            szExpression += "&";
            iStrLength = 20;
        }

        if (szAuxFaceUni.Search(":") != -1 )
        {
            szExpression += "(";
            szExpression += szAuxFaceUni;
            szExpression += ") ";
        }
        else
        {
            szExpression += szAuxFaceUni;
        }
    }

    szExpression.LeftAdjust();
    szExpression.RightAdjust();

    return szExpression;
}






/** ********************************************************************
* @brief Get the void spaces description
*
* @param
* @return TCollection_AsciiString
*
* @date 16/12/2013
* @author  Lei Lu
***********************************************************************/
TCollection_AsciiString McCadMcnpWriter::GetVoidExpn(McCadVoidCell *& pVoidCell)
{
    TCollection_AsciiString szExpn;
    szExpn += "(";
    int iInitSurfNum = m_iInitFaceNum - 1;
    for (Standard_Integer i = 0; i < (pVoidCell->GetFaces()).size(); i++)
    {
        McCadExtFace * pExtFace = (pVoidCell->GetFaces()).at(i);

        int iFaceNum = pExtFace->GetFaceNum();
        iFaceNum > 0 ? iFaceNum += iInitSurfNum : iFaceNum -= iInitSurfNum;

        szExpn += TCollection_AsciiString(iFaceNum);
        szExpn += " ";
    }
    szExpn.Remove(szExpn.Length());     // Remove the last character " "
    szExpn += ")";
    szExpn += "&";

    int iStrLength; //initial string for describing the cell no and material

    for (Standard_Integer i = 0; i < (pVoidCell->GetCollision()).size(); i++)
    {
        COLLISION collision = (pVoidCell->GetCollision()).at(i);
        iStrLength = 12; //initial string for describing the cell no and material
        szExpn += "(";
        for (Standard_Integer j = 0; j < (collision.FaceList).size(); j++)
        {
            int iCollidFaceNum = collision.FaceList.at(j);
            iCollidFaceNum > 0 ? iCollidFaceNum += iInitSurfNum : iCollidFaceNum -= iInitSurfNum;

            int iNum = -1*(iCollidFaceNum);

            /**************** Avoid the length of expression beyond 80 column ***********/
            iStrLength += TCollection_AsciiString(iNum).Length(); // Add the length of number
            iStrLength += 1; // Add the length of ":"
            if (iStrLength >= 79)
            {
                szExpn += "&";
                iStrLength = 11 + TCollection_AsciiString(iNum).Length();
            }
            /****************************************************************************/

            szExpn += TCollection_AsciiString(iNum);
            szExpn += ":";
        }

        for (Standard_Integer k = 0; k < (collision.AuxFaceList).size(); k++)
        {
            vector<int> auxiliary_face_list = collision.AuxFaceList.at(k);
            if ( auxiliary_face_list.size() > 1)
            {
                szExpn += "(";
            }

            for (Standard_Integer iAux = 0; iAux < auxiliary_face_list.size(); iAux++)
            {
                int iAuxFaceNum = auxiliary_face_list.at(iAux);
                iAuxFaceNum > 0 ? iAuxFaceNum += iInitSurfNum : iAuxFaceNum -= iInitSurfNum;

                int iNum = -1*(iAuxFaceNum);

                /**************** Avoid the length of expression beyond 80 column ***********/
                iStrLength += TCollection_AsciiString(iNum).Length();
                iStrLength += 1;
                if (iStrLength >= 79)
                {
                    szExpn += "&";
                    iStrLength = 11 + TCollection_AsciiString(iNum).Length();
                }
                /****************************************************************************/

                szExpn += TCollection_AsciiString(iNum);
                szExpn += " ";                
            }

            szExpn.Remove(szExpn.Length()); // Remove the last character
            if (auxiliary_face_list.size() > 1)
            {
                szExpn += ")";
            }
            szExpn += ":";
        }

        szExpn.Remove(szExpn.Length());     // Remove the last character ":"
        szExpn += ")";
        szExpn += "&";
    }
    return szExpn;
}




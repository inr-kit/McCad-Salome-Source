#include "McCadMcnpWriter.hxx"

#include <assert.h>

#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>
#include <QString>
#include <QStringList>

#include "../McCadTool/McCadMathTool.hxx"
#include "../McCadTool/McCadConvertConfig.hxx"

McCadMcnpWriter::McCadMcnpWriter()
{
    m_iInitCellNum = McCadConvertConfig::GetInitCellNum();
    m_iInitFaceNum = McCadConvertConfig::GetInitSurfNum();
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
void McCadMcnpWriter::PrintHeadDesc(Standard_OStream& theStream) const
{
    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);

    theStream.setf(ios::left);
    theStream<<"McCad 0.5.1 generated Input "<<endl;
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

    Standard_Integer iTotalNum = pData->m_SolidList.size() + iCellNum;
    Standard_Integer iWidth = TCollection_AsciiString(iTotalNum).Length();

    iWidth < 6? iWidth = 6: iWidth += 1;
    m_iCellNumWidth = iWidth;
    m_iMatWidth = 14;
    m_iCellMatWidth = m_iCellNumWidth+m_iMatWidth;

    for (unsigned int i = 0; i < pData->m_SolidList.size(); i++)
    {
        McCadSolid * pSolid = pData->m_SolidList.at(i);
        assert(pSolid);

        if (m_bHaveMaterial)
        {
            PrintGroupInfo(i+1,theStream);
        }        

        theStream.setf(ios::left);
        theStream<<setw(m_iCellNumWidth)<<iCellNum;                   // Output the cell number.
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
        for(unsigned int j = 0; j < pSolid->GetConvexSolidList().size(); j++)
        {
            McCadConvexSolid *pConvexSolid = pSolid->GetConvexSolidList().at(j);
            assert(pConvexSolid);                     

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
                theStream<<setw(m_iCellMatWidth)<<""<<str_left<<endl;
            }
        }while(!str_right.IsEmpty());

        // Set the importances of neutron and photon
        if(!m_bHaveMaterial)
        {
            theStream<<setw(m_iCellNumWidth+5)<<""<<"imp:n=1 imp:p=1"<<endl;
        }
        else
        {
            theStream<<setw(m_iCellMatWidth)<<""<<"imp:n=1 imp:p=1"<<endl;
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

        theStream.setf(ios::left);
        theStream<<setw(m_iCellNumWidth)<<iCellNum;     // Output the cell number.
        theStream<<setw(5)<<0;                          // Output the material number.

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
                    theStream<<setw(m_iCellNumWidth+5)<<""<<str_left<<endl;
                }
            }

        }while(!str_right.IsEmpty());

        theStream<<setw(m_iCellNumWidth+5)<<""<<"imp:n=1 imp:p=1"<<endl; // Output the importance.
    }

    assert(pData->m_pOutVoid);

    theStream.setf(ios::left);
    theStream<<setw(m_iCellNumWidth)<<iCellNum;
    theStream<<setw(5)<<0;

    theStream<<pData->m_pOutVoid->GetOutVoidExpression()<<endl;// Output the outer spaces except for void and material solids.
    theStream<<setw(m_iCellNumWidth+5)<<""<<"imp:n=0 imp:p=0"<<endl;   // Output the importance.
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
void McCadMcnpWriter::PrintSurfDesc(Standard_OStream & theStream) const
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
void McCadMcnpWriter::PrintTrsfDesc(Standard_OStream& theStream)const
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
void McCadMcnpWriter::PrintFile()/*const*/
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
//qiu avoid file saving error.
        //qiu        TCollection_AsciiString FileName = thePath.Name() + thePath.Extension();
        //qiu        const char* strName = FileName.ToCString();
        //qiu        ofstream theStream(strName);
        ofstream theStream(m_OutputFileName.ToCString());  //qiu

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
        PrintVolumeCard(theStream);
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
void McCadMcnpWriter::PrintMatCard(Standard_OStream& theStream)const
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
* @brief Find the repeated surface number at the list,if there are
*        repeated surface number, do not add it into number list
*
* @param  int iFaceNum, vector<Standard_Integer> & list
* @return Standard_Boolean
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
Standard_Boolean McCadMcnpWriter::FindRepeatCell(int iFaceNum, vector<Standard_Integer> & list)
{
    Standard_Boolean bRepeat = Standard_False;
    for(int i = 0 ; i<list.size(); i++)
    {
        if(list.at(i) == iFaceNum)
        {
            bRepeat = Standard_True;
        }
    }

    return bRepeat;
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
    vector<Standard_Integer> IntCellList;
    vector<Standard_Integer> UniCellList;

    /// Generate the surface number list and remove the repeated surfaces
    for (unsigned int i = 0; i < (pSolid->GetFaces()).size(); i++)
    {
        McCadExtBndFace * pExtFace = (pSolid->GetFaces()).at(i);
        Standard_Integer iFaceNum = pExtFace->GetFaceNum();

        if(!FindRepeatCell(iFaceNum, IntCellList))
        {
            IntCellList.push_back(iFaceNum);
        }

        if (pExtFace->HaveAstSurf())
        {
            for(unsigned int j = 0; j < pExtFace->GetAstFaces().size(); j++)
            {
                McCadExtAstFace *pAstFace = pExtFace->GetAstFaces().at(j);
                Standard_Integer iAstFaceNum = pAstFace->GetFaceNum();

                if (pAstFace->IsSplitFace())
                {
                    if(!FindRepeatCell(iAstFaceNum,UniCellList))
                    {
                        UniCellList.push_back(iAstFaceNum);
                    }
                }
                else
                {
                    if(!FindRepeatCell(iAstFaceNum,IntCellList))
                    {
                        IntCellList.push_back(iAstFaceNum);
                    }
                }
            }
        }
    }

    /// Generate the expression of cell
    TCollection_AsciiString szExpression;

    int iStrLength = m_iCellMatWidth + 2; //initial string for describing the cell no and material

    for (unsigned int i = 0; i < IntCellList.size(); i++)
    {
        Standard_Integer iFaceNum = IntCellList.at(i);
        iStrLength += TCollection_AsciiString(iFaceNum).Length();

        /***************** the length of cell expression can not be larger than 80 *******/
        /***************** & is used for splitting the expression ************************/
        if (iStrLength >= 80)
        {
            szExpression += "&";
            iStrLength = m_iCellMatWidth + 2 + TCollection_AsciiString(iFaceNum).Length();
        }
        else
        {
            szExpression += " ";
            iStrLength ++;
        }
        /**********************************************************************************/

        szExpression += TCollection_AsciiString(iFaceNum);
    }

    szExpression += " ";

    if(UniCellList.size() > 1)
    {
        szExpression += "(";

        for (unsigned int j = 0; j < UniCellList.size(); j++)
        {
            Standard_Integer iFaceNum = UniCellList.at(j);
            szExpression += TCollection_AsciiString(iFaceNum);
            iStrLength += TCollection_AsciiString(iFaceNum).Length();

            /***************** the length of cell expression can not be larger than 80 *******/
            /***************** & is used for splitting the expression ************************/
            if (iStrLength >= 80)
            {
                szExpression += "&";
                iStrLength = m_iCellMatWidth + 2 + TCollection_AsciiString(iFaceNum).Length();
            }
            else
            {
                szExpression += ":";
                iStrLength ++;
            }
            /**********************************************************************************/
        }

        szExpression.Remove(szExpression.Length());     // Remove the last character ":"
        szExpression += ") ";
    }    

    szExpression.LeftAdjust();
    szExpression.RightAdjust();

    IntCellList.clear();
    UniCellList.clear();

    return szExpression;
}


///** ********************************************************************
//* @brief Get the MCNP cell expression of solid
//*
//* @param
//* @return TCollection_AsciiString
//*
//* @date 31/8/2012
//* @author  Lei Lu
//***********************************************************************/
//TCollection_AsciiString McCadMcnpWriter::GetCellExpnOld(McCadConvexSolid *& pSolid)
//{
//    TCollection_AsciiString szExpression;

//    TCollection_AsciiString szAstFaceUni = "";
//    TCollection_AsciiString szAstFaceSub = "";

//    int iInitSurfNum = m_iInitFaceNum-1;
//    int iStrLength = 20; //initial string for describing the cell no and material

//    for (Standard_Integer i = 0; i < (pSolid->GetFaces()).size(); i++)
//    {
//        McCadExtBndFace * pExtFace = (pSolid->GetFaces()).at(i);

//        int iFaceNum = pExtFace->GetFaceNum();
//        iFaceNum > 0 ? iFaceNum += iInitSurfNum : iFaceNum -= iInitSurfNum;
//        //iFaceNum = RefreshSurfNum(iFaceNum);
//        iStrLength += TCollection_AsciiString(iFaceNum).Length();

//        /***************** the length of cell expression can not be larger than 80 *******/
//        /***************** & is used for splitting the expression ************************/
//        if (iStrLength >= 80)
//        {
//            szExpression += "&";
//            iStrLength = 20 + TCollection_AsciiString(iFaceNum).Length();
//        }
//        else
//        {
//            szExpression += " ";
//            iStrLength ++;
//        }
//        /**********************************************************************************/

//       szExpression += TCollection_AsciiString(iFaceNum);

//       if (pExtFace->HaveAstSurf())
//       {
//            for(unsigned j = 0; j < pExtFace->GetAstFaces().size(); j++)
//            {
//                McCadExtAstFace *pAstFace = pExtFace->GetAstFaces().at(j);

//                int iAstFaceNum = pAstFace->GetFaceNum();
//                iAstFaceNum > 0 ? iAstFaceNum += iInitSurfNum : iAstFaceNum -= iInitSurfNum;
//                //iAstFaceNum = RefreshSurfNum(iAstFaceNum);

//                if (pAstFace->IsSplitFace())
//                {
//                    szAstFaceUni += TCollection_AsciiString(iAstFaceNum);
//                    szAstFaceUni += ":";
//                }
//                else
//                {
//                    szAstFaceSub += TCollection_AsciiString(iAstFaceNum);
//                    szAstFaceSub += " ";
//                }
//            }
//        }
//    }

//    szExpression += " ";

//    if (!szAstFaceSub.IsEmpty())
//    {
//        szAstFaceSub.Remove(szAstFaceSub.Length());

//        // Avoid the length of expression beyond 80
//        iStrLength += szAstFaceSub.Length();
//        if (iStrLength >= 80)
//        {
//            szExpression += "&";
//            iStrLength = 20;
//        }

//        szExpression += szAstFaceSub;
//        szExpression += " ";
//    }

//    if (!szAstFaceUni.IsEmpty())
//    {
//        szAstFaceUni.Remove(szAstFaceUni.Length());

//        // Avoid the length of expression beyond 80
//        iStrLength += szAstFaceUni.Length();
//        if (iStrLength >= 80)
//        {
//            szExpression += "&";
//            iStrLength = 20;
//        }

//        if (szAstFaceUni.Search(":") != -1 )
//        {
//            szExpression += "(";
//            szExpression += szAstFaceUni;
//            szExpression += ") ";
//        }
//        else
//        {
//            szExpression += szAstFaceUni;
//        }
//    }

//    szExpression.LeftAdjust();
//    szExpression.RightAdjust();

//    return szExpression;
//}



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

    for (Standard_Integer i = 0; i < (pVoidCell->GetBndFaces()).size(); i++)
    {
        McCadExtBndFace * pExtFace = (pVoidCell->GetBndFaces()).at(i);
        int iFaceNum = pExtFace->GetFaceNum();        

        szExpn += TCollection_AsciiString(iFaceNum);
        szExpn += " ";
    }
    szExpn.Remove(szExpn.Length());     // Remove the last character " "
    szExpn += ")";
    szExpn += "&";

    int iStrLength; //initial string for describing the cell no and material

    for (Standard_Integer i = 0; i < pVoidCell->GetCollisions().size(); i++)
    {
        McCadVoidCollision *pCollision = pVoidCell->GetCollisions().at(i);
        iStrLength = 12; //initial string for describing the cell no and material
        szExpn += "(";
        for (Standard_Integer j = 0; j < pCollision->GetFaceNumList().size(); j++)
        {
            int iCollidFaceNum = pCollision->GetFaceNumList().at(j);
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

        if ( pCollision->GetAstFaceNumList().size() > 1)
        {
            szExpn += "(";

            for (Standard_Integer k = 0; k < pCollision->GetAstFaceNumList().size(); k++)
            {
                int iAstFaceNum = pCollision->GetAstFaceNumList().at(k);
                //iAstFaceNum > 0 ? iAstFaceNum += iInitSurfNum : iAstFaceNum -= iInitSurfNum;

                int iNum = -1*(iAstFaceNum);

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

                //szExpn.Remove(szExpn.Length()); // Remove the last character
            }
            szExpn.Remove(szExpn.Length());     // Remove the last character " "
            szExpn += ") ";
        }  

        szExpn.Remove(szExpn.Length());     // Remove the last character " "
        szExpn += ")";
        szExpn += "&";
    }
    return szExpn;
}




/** ********************************************************************
* @brief Print the volume card.
*
* @param Standard_OStream & theStream
* @return void
*
* @date 12/05/2015
* @author  Lei Lu
***********************************************************************/
void McCadMcnpWriter::PrintVolumeCard(Standard_OStream& theStream)const
{
    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);

    theStream<<"c "<<"=============================== Volume Card =================================="<<endl;

    Standard_Integer iCellNum = m_iInitCellNum;   // Get the initial cell number.

    for (unsigned int i = 0; i < pData->m_SolidList.size(); i++)
    {
        McCadSolid * pSolid = pData->m_SolidList.at(i);
        assert(pSolid);

        theStream.setf(ios::left);
        theStream<<setw(6)<<iCellNum;                   // Output the cell number.

        Standard_Real fVolume = 0;
        for(int j = 0; j < pSolid->GetConvexSolidList().size(); j++)
        {
            McCadConvexSolid *pConvexSolid = pSolid->GetConvexSolidList().at(j);
            assert(pConvexSolid);
            fVolume += pConvexSolid->GetVolume();
        }

        theStream<<setw(10)<<fVolume<<endl;
        iCellNum++;
    }
}

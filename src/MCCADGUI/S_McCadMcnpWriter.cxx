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
#include "S_McCadMcnpWriter.hxx"
#include "S_McCadSolid.hxx"
#include "MCCADGUI.h"
#include "MCCADGUI_DataModel.h"
#include "utilities.h"
#include "../MCCAD/McCadTool/McCadConvertConfig.hxx"

//OCC
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>

//SALOME
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Tools.h>
#include <SUIT_Study.h>


S_McCadMcnpWriter::S_McCadMcnpWriter()
{
//    m_iNumSolid = 0;                   // Initial the number of convex solid.
    m_bHaveMaterial = Standard_False;
    myFileName = "";
}

S_McCadMcnpWriter::~S_McCadMcnpWriter()
{
//    ReleaseMemory();
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
* 2013-12-06 Modified by Yuefeng Qiu
***********************************************************************/
void S_McCadMcnpWriter::PrintFile()
{
//    myFileName = aFileName;
    if (m_OutputFileName.IsEmpty())
    {
        cout<<"File Name is empty!"<<endl;
        return;
    }
    else
    {
//        TCollection_AsciiString theFileName = aFileName;  //tmp str
        OSD_Path thePath(m_OutputFileName);
        OSD_File theFile(thePath);
        theFile.Build(OSD_ReadWrite , OSD_Protection());

//        TCollection_AsciiString FileName = thePath.Name() + thePath.Extension();
//        const char* strName = FileName.ToCString();
        ofstream theStream(m_OutputFileName.ToCString());

        PrintHeadDesc(theStream);
        PrintCellDesc(theStream);
        if (m_bGenerateVoid == Standard_True)
        {
            PrintVoidDesc(theStream);
        }
        PrintSurfDesc(theStream);
        PrintTrsfDesc(theStream);
        if (m_bHaveMaterial)
            PrintMatCard(theStream);

        SUIT_Session* session = SUIT_Session::session();
        SUIT_ResourceMgr* resMgr = session->resourceMgr();
        bool isMCNP6 = resMgr->integerValue("MCCAD", "target_code", 0) == 1 ? true : false;
        if (isMCNP6) {
            S_PrintEmbedCard(theStream);
        }

        theFile.Close();
    }
}


/** ********************************************************************
* @brief Print the cell geometry description
*
* @param Standard_OStream & theStream
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
* 2013-12-06 modified by Yuefeng Qiu
***********************************************************************/
void S_McCadMcnpWriter::PrintCellDesc(Standard_OStream& theStream)
{
    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);

    theStream<<"c "<<"o---------------------------- Cells Card ----------------------------------o"<<endl;
    theStream<<"c "<<endl;

    Standard_Integer iCellNum = McCadConvertConfig::GetInitCellNum();   // Get the initial cell number.
	
	Standard_Integer iTotalNum = pData->m_SolidList.size() + iCellNum;
    Standard_Integer iWidth = TCollection_AsciiString(iTotalNum).Length();

    iWidth < 6? iWidth = 6: iWidth += 1;
    m_iCellNumWidth = iWidth;
    m_iMatWidth = 14;
	
	if (m_bHaveMaterial)
	{
		m_iCellMatWidth = m_iCellNumWidth+m_iMatWidth;
	}
	else
	{
		m_iCellMatWidth = m_iCellNumWidth + 5;
	}	    	

    SUIT_Session* session = SUIT_Session::session();
    SUIT_ResourceMgr* resMgr = session->resourceMgr();
//    Standard_Integer iPseudoCellNum =McCadConvertConfig::GetInitPseudoCellNum(); //get the initial pseudo cell number
//    Standard_Integer iEmbedCardNum =McCadConvertConfig::GetInitEmbedCardNum();
    Standard_Integer iPseudoCellNum = resMgr->integerValue("MCCAD","init_pseudo_cell_no", 90000);
    Standard_Integer iEmbedCardNum = resMgr->integerValue("MCCAD","embed_card_no", 2);

    int tmpComponentID = -1; //to get componentID, initial with a non-possible id
    int tmpGroupID = -1;
    for (unsigned int i = 0; i < pData->m_SolidList.size(); i++)
    {
//qiu        McCadSolid * pSolid = m_SolidList.at(i);
        S_McCadSolid * pSolid = static_cast <S_McCadSolid *> (pData->m_SolidList.at(i));  //qiu cast to S_McCadSolid
        assert(pSolid);

//qiu        if (m_bHaveMaterial)
//qiu        {
//qiu            PrintlGroupInfo(i+1,theStream);
//qiu        }
        //qiu print Component info
        if (pSolid->GetComponentID() != tmpComponentID) {
            tmpComponentID = pSolid->GetComponentID();
            S_PrintComponentInfo(tmpComponentID, theStream);
        }
        //qiu print Group info, if envelop cell the groupID is wrong! avoid it.
        if (!pSolid->GetisEnvelop() && pSolid->GetGroupID() != tmpGroupID) {
            tmpGroupID = pSolid->GetGroupID();
            PrintGroupInfo(tmpGroupID, theStream);
        }

        //output name and remark
        theStream<<"c Part:"<<pSolid->GetName();  //qiu Output the solid name
        if (!pSolid->GetRemark().trimmed().isEmpty())  //qiu Output remark
            theStream<<"  $"<<pSolid->GetRemark().trimmed().toStdString()<<endl;
        else
            theStream<<endl;	
			
        //output cell num, material and density
        theStream.setf(ios::left);
        theStream<<setw(m_iCellNumWidth)<<iCellNum;                   // Output the cell number.
        theStream<<setw(4)<<pSolid->GetID();            // Output the material number.
//qiu        if(!m_bHaveMaterial)
        if (pSolid->GetID() == 0) //if no material
        {
            theStream<<fixed<<setprecision(4)<<setw(1)<<"";
        }
        else
        {
            theStream<<fixed<<setprecision(4)<<setw(10)<<pSolid->GetDensity();  // Output the density.
        }

        //output cell description
        for(int j = 0; j < pSolid->GetConvexSolidList().size(); j++)
        {
            McCadConvexSolid *pConvexSolid = pSolid->GetConvexSolidList().at(j);
            assert(pConvexSolid);
//            pConvexSolid->ChangeFaceNum(pData); // Update the face number after sorting

//            TCollection_AsciiString strCellDesc = pConvexSolid->GetExpression();
            TCollection_AsciiString strCellDesc = GetCellExpn(pConvexSolid);// qiu using the new updates
            strCellDesc.LeftAdjust();   // Trim the left and right space characters of string
            strCellDesc.RightAdjust();
//qiu replace & with enter and white space, & indicate a new-line
            QString QstrCellDesc = strCellDesc.ToCString();

			if (m_bHaveMaterial)
			{
				QstrCellDesc.replace("&","\n                    ");
			}
			else
			{
				QstrCellDesc.replace("&","\n           ");
			}

            if ( j == 0 )
            {
 //               theStream<<"("<<strCellDesc<<")"<<endl;
                theStream<<"("<<QstrCellDesc.toStdString()<<")"<<endl;
            }
            else
            {
//                theStream<<setw(20)<<""<<":("<<strCellDesc<<")"<<endl;
                theStream<<setw(m_iCellMatWidth)<<""<<":("<<QstrCellDesc.toStdString()<<")"<<endl;
            }
        }

//qiu        // Set the importances of neutron and photon
//qiu        if(!m_bHaveMaterial)
//qiu        {
//qiu            theStream<<setw(11)<<""<<"imp:n=1 imp:p=1"<<endl;
//qiu        }
//qiu        else
//qiu        {
//qiu            theStream<<setw(20)<<""<<"imp:n=1 imp:p=1"<<endl;
//qiu        }
        //qiu output importance and additive card
        if (pSolid->GetID() == 0)
            theStream<<setw(m_iCellNumWidth)<<"";
        else
            theStream<<setw(m_iCellMatWidth)<<"";
        int aIMPn = pSolid->GetImportances()[0];
        int aIMPp = pSolid->GetImportances()[1];
//        int aIMPe = pSolid->GetImportances()[2];
        theStream <<"imp:n="<<(aIMPn == -1 ? 1 : aIMPn) <<" imp:p="<<(aIMPp == -1 ? 1 : aIMPp) /*<<" imp:e="<<(aIMPe == -1 ? 1 : aIMPe)*/;
        if (pSolid->GetisEnvelop())
            theStream <<" fill="<<iEmbedCardNum;
        theStream <<" "<<pSolid->GetAdditive().trimmed().toStdString()<<endl;


        //qiu ### Output MCNP6 pseudo cell description
        if (pSolid->GetisEnvelop()) {
            S_PrintPseudoCellDesc(theStream, pSolid->GetComponentID(),iPseudoCellNum, iEmbedCardNum);
            iEmbedCardNum ++;
        }
        iCellNum ++;
    }
}


/*!
 * \brief S_McCadMcnpWriter::S_PrintPseudoCellDesc
 *  print MCNP6 pseudo cell description
 *  if one S_McCadSolid is marked as envelop, then this method will be call
 * \param theStream  the output file stream
 * \param ComponentID the component which is fully meshed
 * \param iPseudoCellNum  the current pseudo cell number, increase all the way
 * \param iEmbedCardNum the current Embed card no
 */
void S_McCadMcnpWriter::S_PrintPseudoCellDesc(Standard_OStream& theStream, const int &ComponentID,
                                                    int & iPseudoCellNum,  const int & iEmbedCardNum)
{
    int studyID = SUIT_Session::session()->activeApplication()->activeStudy()->id();
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    if ( !studyID || CORBA::is_nil( engine ) )    {
        MESSAGE("Errors in study or in MCCAD engine!");
        return ;
    }
    //find the component
    MCCAD_ORB::Component_var aComponent = engine->getComponent(studyID, ComponentID);
    if (aComponent->_is_nil()) {
        MESSAGE("This component is nil!");
        return ;
    }
    MCCAD_ORB::GroupList * aGroupList = aComponent->getGroupList();
    //*********LOOP for Groups*****
    for (int j=0; j<aGroupList->length(); j++)
    {
        MCCAD_ORB::Group_var aGroup = (*aGroupList)[j];
        PrintGroupInfo(aGroup->getID(), theStream);  //print group info

        int MaterialID = 0;
        double Density = 0.0;
        if ( aGroup->hasMaterial()) {
            MCCAD_ORB::Material_var aMaterial = aGroup->getMaterial();  //this material should not be nil
            MaterialID = aMaterial->getID();
            Density = aMaterial->getDensity();
        }
        MCCAD_ORB::PartList * aPartList = aGroup->getPartList();
        //*********LOOP for Parts*****
        for (int k=0; k<aPartList->length(); k++)
        {
            MCCAD_ORB::Part_var aPart = (*aPartList)[k];
            //output name and remark
            theStream<<"c Part:"<<aPart->getName();  //qiu Output the solid name
            if (!QString(aPart->getRemark()).trimmed().isEmpty())  //qiu Output remark
                theStream<<"  $"<<QString(aPart->getRemark()).trimmed().toStdString()<<endl;
            else
                theStream<<endl;

            theStream.setf(ios::left);
            theStream<<setw(6)<<iPseudoCellNum;                   // Output the cell number.
            theStream<<setw(4)<<MaterialID;            // Output the material number.
            if (!MaterialID) //if no material
                theStream<<fixed<<setprecision(7)<<setw(1)<<"";
            else
                theStream<<fixed<<setprecision(7)<<setw(10)<<Density;  // Output the density.
            theStream<<resetiosflags(ios::fixed)<<setw(4)<<0;  //output the surface description: 0
            MCCAD_ORB::FixArray_var aIMP = aPart->getImportance();  //get importance
            theStream <<" imp:n="<<(aIMP[0] == -1 ? 1 : aIMP[0]) <<" imp:p="<<(aIMP[1] == -1 ? 1 : aIMP[1]) /*<<" imp:e="<<(aIMP[2] == -1 ? 1 : aIMP[2])*/;
            theStream <<" u="<<iEmbedCardNum ;  //output the universal card for this pseudo cell
            if (!QString(aPart->getAdditive()).trimmed().isEmpty())
                theStream <<" "<< QString(aPart->getAdditive()).trimmed().toStdString()<<endl;
            else
                theStream <<endl;

            iPseudoCellNum++; //!!IMPORTANT
        }
    }

    //print the background cell
    theStream<<"c Background cell" <<endl;
    theStream.setf(ios::left);
    theStream<<setw(6)<<iPseudoCellNum;                   // Output the cell number.
    theStream<<setw(4)<<0;            // Output the material number 0 .
    theStream<<fixed<<setprecision(7)<<setw(1)<<"";  //output the density 0
    theStream<<setw(4)<<0;  //output the surface description: 0
    theStream <<" u="<<iEmbedCardNum ;  //output the universal card for background
    theStream <<" imp:n=1"<<" imp:p=1"/*<<" imp:e=1"*/<<endl;

    iPseudoCellNum++; //!!IMPORTANT
}



/** ********************************************************************
* @brief Print the material card
*
* @param Standard_OStream& theStream
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
* 2013-12-06 Modified by Yuefeng Qiu
***********************************************************************/
void S_McCadMcnpWriter::PrintMatCard(Standard_OStream& theStream)
{
    /*theStream<<"c    "<<"o---------------------------------------------------------------------o"<<endl;
    theStream<<"c    "<<"|                                                                     |"<<endl;
    theStream<<"c    "<<"|   o-------------------------------------------------------------o   |"<<endl;
    theStream<<"c    "<<"|   |               M A T E R I A L S    S E C T I O N            |   |"<<endl;
    theStream<<"c    "<<"|   o-------------------------------------------------------------o   |"<<endl;
    theStream<<"c    "<<"|                                                                     |"<<endl;
    theStream<<"c    "<<"o---------------------------------------------------------------------o"<<endl;
    theStream<<"c "<<endl;*/

    theStream<<"c "<<"o-------------------------- Materials Card --------------------------------o"<<endl;
    theStream<<"c "<<endl;

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
    for(int i = 0; i < n; i++)
    {
        MCCAD_ORB::Material_var aMaterial = (*aMaterialList)[i];
        QString strMatCard = aMaterial->getMatCard();
        QStringList list;
        list = strMatCard.split("\n");

        bool bPrintMatID = false;                       // The material id has been printed or not

        for (int j = 0; j < list.size()-1; j++)
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
                theStream<<"M"<<setw(7)<<aMaterial->getID()<<str.toStdString()<<endl;
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

void S_McCadMcnpWriter::S_PrintEmbedCard(Standard_OStream& theStream)
{

    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);

    //get  initial number
//    Standard_Integer iPseudoCellNum = McCadConvertConfig::GetInitPseudoCellNum(); //get the initial pseudo cell number
//    Standard_Integer iEmbedCardNum = McCadConvertConfig::GetInitEmbedCardNum();

    //get the preference for MCNP6 conversion
    SUIT_Session* session = SUIT_Session::session();
    SUIT_ResourceMgr* resMgr = session->resourceMgr();
    Standard_Integer iPseudoCellNum = resMgr->integerValue("MCCAD","init_pseudo_cell_no", 90000);
    Standard_Integer iEmbedCardNum = resMgr->integerValue("MCCAD","embed_card_no", 2);
    int iMeshType = resMgr->integerValue("MCCAD", "MeshType", 0);  //actually only one option.
    int iFileType = resMgr->integerValue("MCCAD", "EeoutFileType", 1);  //default ascii
    int iOverlap =  resMgr->integerValue("MCCAD", "OverlapTreat", 2);  //default AVERAGE
    double dLengthFactor = resMgr->doubleValue("MCCAD", "LengthFactor", 1.0); //default 1

    //get the studyID and engine
    int studyID = SUIT_Session::session()->activeApplication()->activeStudy()->id();
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    if ( !studyID || CORBA::is_nil( engine ) )    {
        MESSAGE("Errors in study or in MCCAD engine!");
        return ;
    }
    //Loop all the solid to check if some are envelop cells
    for (unsigned int i = 0; i < pData->m_SolidList.size(); i++)
    {
        S_McCadSolid * pSolid = static_cast <S_McCadSolid *> (pData->m_SolidList.at(i));  //qiu cast to S_McCadSolid
        assert(pSolid);
        if (pSolid->GetisEnvelop()) {
            int ComponentID = pSolid->GetComponentID();
            MCCAD_ORB::Component_var aComponent = engine->getComponent(studyID, ComponentID);
            if (aComponent->_is_nil()) {
                MESSAGE("The Component is nill!");
                return ;
            }
            //###start to print
            //print he card and mesh type
            theStream.setf(ios::left);
            theStream<<"c Embed Card for Component: " <<aComponent->getName()<<endl;
            theStream<<"embed"<<iEmbedCardNum<<setw(5)<<"";         //output the embed card no
            QString aTmpStr = "";
            if (iMeshType == 0)
                aTmpStr = "meshgeo=abaqus";
            theStream<<aTmpStr.toStdString()<<endl;  //print the mesh type
            //print the mesh file input, default output file name + .inp
            aTmpStr = QString("mgeoin=") + SUIT_Tools::file(QString(m_OutputFileName.ToCString()), /*withExten=*/true).toLower() + QString(".inp");
            theStream<<setw(7)<<""<<aTmpStr.toStdString()<<endl;
            //print the eeout file name, default the Component name + .eeout
            aTmpStr = QString("meeout=") + SUIT_Tools::file(QString(m_OutputFileName.ToCString()), /*withExten=*/false) + QString(aComponent->getName()) + QString(".eeout");
            theStream<<setw(7)<<""<<aTmpStr.toStdString()<<endl;
            //print the eeout file type
            aTmpStr = "filetype=";
            aTmpStr += iFileType == 0? "binary" : "ascii";
            theStream<<setw(7)<<""<<aTmpStr.toStdString()<<endl;
            aTmpStr = "length=" + QString("%1").arg(dLengthFactor);
            theStream<<setw(7)<<""<<aTmpStr.toStdString()<<endl;
            //print the overlap treament
            switch (iOverlap) {
            case 0:
                aTmpStr = "overlap=EXIT";
                break;
            case 1 :
                aTmpStr = "overlap=ENTRY";
                break;
            case 2:
                aTmpStr = "overlap=AVERAGE";
                break;
            }
            theStream<<setw(7)<<""<<aTmpStr.toStdString()<<endl;

            //print the match map of pseudo cell and mesh
            theStream<<setw(7)<<""<<"matcell=";
            int partCount = aComponent->getNumParts();
            for (int i=0; i<partCount; i++) {
                theStream<<i+1<<" "<<iPseudoCellNum + i <<" ";
                if (i%5 == 0 && i != partCount-1) theStream <<"\n               ";
            }
            theStream <<endl;
            iPseudoCellNum += partCount;  //point to background cell
            //print the background cell
            aTmpStr = "background=" + QString("%1").arg(iPseudoCellNum);
            theStream<<setw(7)<<""<<aTmpStr.toStdString()<<endl;
            iPseudoCellNum ++; //for next component
            iEmbedCardNum ++;       //for next component
        }
    }

}

void S_McCadMcnpWriter::S_PrintComponentInfo(const int & ComponentID, Standard_OStream& theStream)
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
    theStream<<"c "<<"=============================================================================="<<endl;
    theStream<<"c "<<setw(8)<<""<<"Component:    "<<aComponent->getName()<<endl;
    if (!QString(aComponent->getRemark()).trimmed().isEmpty())
        theStream<<"c "<<setw(8)<<""<<"INFO:       "<<QString(aComponent->getRemark()).trimmed().toStdString()<<endl;
    theStream<<"c "<<setw(8)<<""<<"Contain Group:       "<<endl;
    int n = aComponent->getGroupList()->length();
    for (int i=0; i<n; i++)
        theStream<<"c "<<setw(8)<<""<<"              "<<(*aComponent->getGroupList())[i]->getName()<<endl;
    theStream<<"c "<<"=============================================================================="<<endl;

}

void            S_McCadMcnpWriter::PrintGroupInfo(const int & GroupID, Standard_OStream& theStream)
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
    theStream<<"c "<<"            ======================================================            "<<endl;
    theStream<<"c "<<setw(11)<<""<<"Group:    "<<aGroup->getName()<<endl;
    if (!QString(aGroup->getRemark()).trimmed().isEmpty())
        theStream<<"c "<<setw(11)<<""<<"INFO:       "<<QString(aGroup->getRemark()).trimmed().toStdString()<<endl;
    if (aGroup->hasMaterial()) {
        MCCAD_ORB::Material_var aMaterial = aGroup->getMaterial();
//        theStream<<"c "<<setw(8)<<""<<"Material: "<<aMaterial->getName()<<endl;
        theStream<<"c "<<setw(11)<<""<<"Material ID:   "<<aMaterial->getID()<<endl;
//        theStream<<"c "<<setw(8)<<""<<"Density:       "<<aMaterial->getDensity()<<endl;
    }
    theStream<<"c "<<setw(11)<<""<<"Contains "<<aGroup->getPartList()->length() << " cells."<<endl;
    theStream<<"c "<<"            ======================================================            "<<endl;
}


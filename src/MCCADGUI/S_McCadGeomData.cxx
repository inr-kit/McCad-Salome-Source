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
#include "S_McCadGeomData.hxx"
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

S_McCadGeomData::S_McCadGeomData()
{
    m_iNumSolid = 0;                   // Initial the number of convex solid.
}

S_McCadGeomData::S_McCadGeomData(const int & studyID, Standard_Boolean bGenVoid)
{
    m_iNumSolid = 0;                   // Initial the number of convex solid.
   m_bGenerateVoid = bGenVoid;
   S_InputData(studyID);
}

/** ********************************************************************
* @brief get the geometry data from MCCAD engine, geneate new S_McCadSolid
*  and append them to the list
*
* @param[in] StudyID : ID to find data from which study
* @return void
*
* @date 06/12/2013
* @author Yuefeng Qiu
*********************************************************************/
void  S_McCadGeomData::S_InputData(const int & studyID)
{
    cout<<endl;
    cout<<"Step.1  ===================== Load the input solid data ====================="<<endl<<endl;

    //get the option of target code, and void generation
    //get the target MC code
    SUIT_Session* session = SUIT_Session::session();
    SUIT_ResourceMgr* resMgr = session->resourceMgr();
    bool isMCNP6 = false, isGDML = false;
    if (resMgr->integerValue("MCCAD", "target_code", 0) == 1) //if MCNP6 is chosen
        isMCNP6 = true;
    else if (resMgr->integerValue("MCCAD", "target_code", 0) == 3) //if GDML is chosen
        isGDML = true;

    //work should do for McCadGeomData
    m_bGenerateVoid = McCadConvertConfig::GenerateVoid(); // Turn on/off the void generating switch
    m_listConvexSolid = new TopTools_HSequenceOfShape;      // create a shape list to store the convex material solids
    m_listVoidCell = new TopTools_HSequenceOfShape;         // create a shape list to store the void solids
    Standard_Integer iNumMatSolid = 0;

    //get component list
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    if ( !studyID || CORBA::is_nil( engine ) )    {
        MESSAGE("Errors in study or in MCCAD engine!");
        return ;
    }
    MCCAD_ORB::ComponentList * aComponentList = engine->getData(studyID);
    if (!aComponentList)    {
        MESSAGE("component list is emtpy!");
                return ;
    }

    //loop all component, group and part to generate data
    //!!!since all the object is checked before conversion start, here is not check again!
    for (int i=0; i< aComponentList->length(); i++)
    {
        //*********LOOP for Components*****
        MCCAD_ORB::Component_var aComponent = (*aComponentList)[i];

        if (isMCNP6 && aComponent->hasEnvelop()) {//if target code is MCNP6, and the component has the envelop
          TopoDS_Shape aTmpShape = MCCADGUI_DataModel::Stream2Shape(*aComponent->getEnvelopStream()); //get the envelop shape
          S_McCadSolid * pSolidList = S_GenSMcCadSolid(aTmpShape); //generate a S_McCadSolid with envelop shape
          if (pSolidList == NULL) {
              MESSAGE("Component: " << aComponent->getName() << " contain not valid shape!");
              delete pSolidList;
              continue;
          }
          //set property
          pSolidList->SetMaterial(0, 0.0);  //set the material to be 0, density to be 0.0
          pSolidList->SetName(aComponent->getName());
          pSolidList->SetComponentID(aComponent->getID()); //only set the component id
          pSolidList->SetisEnvelop(true);  //!!!IMPORTANT, the solid will be consider as envelop
//          pSolidList->SetImportances() //no need , use default value
          int aImp[3] = {1,1,1};
           pSolidList->SetImportances(aImp);
          pSolidList->SetRemark(aComponent->getRemark());
//          pSolidList->SetAdditive()  //no need
          //Push the solid to the list
          m_SolidList.push_back(static_cast <McCadSolid *> (pSolidList)); //need to cast because m_SolidList accepts only McCadSolid.
          iNumMatSolid ++;
          continue;  //jump to next component and skip all the group and part
        }

        MCCAD_ORB::GroupList * aGroupList = aComponent->getGroupList();
        //*********LOOP for Groups*****
        for (int j=0; j<aGroupList->length(); j++)
        {
            MCCAD_ORB::Group_var aGroup = (*aGroupList)[j];
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
                //add a judgement to deal with parts without being decomposed
                S_McCadSolid * pSolidList;
                TopoDS_Shape aTmpShape = MCCADGUI_DataModel::Stream2Shape(*aPart->getShapeStream());
                if (aPart->getIsDecomposed())
                {
                    pSolidList = S_GenSMcCadSolid(aTmpShape); //generate a S_McCadSolid with this shape
                    if (pSolidList == NULL) {
                        MESSAGE("Part: " << aPart->getName() << " contain not valid shape!");
                        delete pSolidList;
                        continue;
                    }
                }
                else if (isGDML)
                {
                    //this code is for those undecomposed solid, they will be
                    pSolidList = new S_McCadSolid;
                    pSolidList->SetisDecomposed(false);
                    pSolidList->SetUnDecomposedSolid(aTmpShape);
                }
                else {
                    MESSAGE("Part: " << aPart->getName() << " is not decomposed!");
                    continue;
                }

                //set property
                pSolidList->SetMaterial(MaterialID, Density);  //set the material id and density
                pSolidList->SetName(aPart->getName());
                pSolidList->SetPartID(aPart->getID());
                pSolidList->SetGroupID(aGroup->getID());
                pSolidList->SetComponentID(aComponent->getID()); //only set the component id
                MCCAD_ORB::FixArray_var aIMP_var = aPart->getImportance();
                int aIMP [3] = {aIMP_var[0], aIMP_var[1], aIMP_var[2]};
                pSolidList->SetImportances(aIMP);   //set importance
                pSolidList->SetRemark(aPart->getRemark());
                pSolidList->SetAdditive(aPart->getAdditive())  ;
                //Push the solid to the list
                m_SolidList.push_back(static_cast <McCadSolid *> (pSolidList)); //need to cast because m_SolidList accepts only McCadSolid.
                iNumMatSolid ++;
            }
        }
    }
    cout<< iNumMatSolid << "  Solids are read." << endl;

}




/** ********************************************************************
* @brief Get a material solid from the shape, and generate the surfaces list.
* and return a new S_McCadSolid
*
* @param[in] TopoDS_Shape & aShape OCC shape
* @return S_McCadSolid a new S_McCadSolid
*
* @date 31/8/2012
* @author  Lei Lu
* 2013-12-05 Modified by  Yuefeng Qiu
*********************************************************************/
S_McCadSolid * S_McCadGeomData::S_GenSMcCadSolid( const TopoDS_Shape & aShape)
{
        TopoDS_Shape theShape = aShape;
        TopExp_Explorer exp;
//        double aMinSolidVol = McCadConvertConfig::GetMinIptSolidVod();
        // If the solid is compound solid, explode each its lumps into solids firstly
        if( theShape.ShapeType() == TopAbs_COMPSOLID || theShape.ShapeType() == TopAbs_COMPOUND )
        {
            cout<<"yes. it is compoundsolid;"<<endl;
            int j = 0;
//qiu            McCadSolid * pSolidList = new McCadSolid();
            S_McCadSolid * pSolidList = new S_McCadSolid();

            for(exp.Init(theShape,TopAbs_SOLID); exp.More(); exp.Next())
            {
                j++;
                TopoDS_Solid tmpSolid = TopoDS::Solid(exp.Current());
                McCadConvexSolid *pConvexSolid = NULL;

//qiu                if(pConvexSolid->GetVolume() >= -0.01)        // Set the minimum volume tolerance
//qiu                if(pConvexSolid->GetVolume() >= aMinSolidVol)
//qiu                {
//qiu                    pConvexSolid = new McCadConvexSolid(tmpSolid);
//qiu                }
//qiu                else
//qiu                {
//qiu                    cout<<"Detected a solid with small volume: "<<pConvexSolid->GetVolume()<<endl;
//qiu                    continue;
//qiu                }
                pConvexSolid = new McCadConvexSolid(tmpSolid); //qiu
                // If the void generating switch is on, descrete the surfaces and get the sample points
                if (m_bGenerateVoid)
                {
                  //  cout<<"Processing the " <<i<<" - "<<j<<"convex solid"<<endl;
                   // pConvexSolid->GenDescPoints(m_bGenerateVoid);
                }

                AddGeomSurfList(pConvexSolid->GetSTLFaceList());    // Travelse the boundary faces, add in face list
                /*AddGeomAuxSurfList*/AddGeomAstSurfList(pConvexSolid->GetSTLFaceList()); // Judge whether need add auxiliary faces
                pSolidList->AddConvexSolid(pConvexSolid);           // Add the convex solid into a complicate solid

                pConvexSolid->DeleteRepeatFaces(m_bGenerateVoid);    // Delete the repeated faces of solid
                m_ConvexSolidList.push_back(pConvexSolid);          // Add the convex solid in the solid list
                m_iNumSolid++;
            }
//qiu            m_SolidList.push_back(pSolidList);
            return pSolidList;
        }
        // If the solid is single solid, add to convex solid list directly
        else if( theShape.ShapeType() == TopAbs_SOLID)
        {
//            TopoDS_Solid tmpSolid = TopoDS::Solid(solid_list->Value(i));
            TopoDS_Solid tmpSolid = TopoDS::Solid(theShape);
            McCadConvexSolid *pConvexSolid = NULL;
//qiu            McCadSolid * pSolidList = new McCadSolid();
            S_McCadSolid * pSolidList = new S_McCadSolid(); //qiu
//qiu                if(pConvexSolid->GetVolume() >= -0.01)        // Set the minimum volume tolerance
//qiu                if(pConvexSolid->GetVolume() >= -0.01)        // Set the minimum volume tolerance
//qiu                if(pConvexSolid->GetVolume() >= aMinSolidVol)
//qiu                {
//qiu                    pConvexSolid = new McCadConvexSolid(tmpSolid);
//qiu                }
//qiu                else
//qiu                {
//qiu                    cout<<"Detected a solid with small volume: "<<pConvexSolid->GetVolume()<<endl;
//qiu                    continue;
//qiu                }
            pConvexSolid = new McCadConvexSolid(tmpSolid); //qiu

            // If the void generating switch is on, descrete the surfaces and get the sample points
            if (m_bGenerateVoid)
            {
               // pConvexSolid->GenDescPoints(m_bGenerateVoid);
            }

            AddGeomSurfList(pConvexSolid->GetSTLFaceList());    // Travelse the boundary faces, add in face list
            /*AddGeomAuxSurfList*/AddGeomAstSurfList(pConvexSolid->GetSTLFaceList()); // Judge whether need add auxiliary faces
            pSolidList->AddConvexSolid(pConvexSolid);           // Add the convex solid into a complicate solid

            pConvexSolid->DeleteRepeatFaces(1);                  // Delete the repeated faces of solid
            m_ConvexSolidList.push_back(pConvexSolid);          // Add the convex solid in the solid list
            m_iNumSolid++;
//qiu            m_SolidList.push_back(pSolidList);
            return pSolidList;
        }
        else return NULL;
}

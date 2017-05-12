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
#include "S_McCadGDMLWriter.hxx"
#include "S_McCadSolid.hxx"
#include "MCCADGUI.h"
#include "MCCADGUI_DataModel.h"
#include "utilities.h"
//SALOME
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Tools.h>
#include <SUIT_Study.h>

//OCC
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <BRepBndLib.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <Poly_Triangulation.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <BRepPrimAPI_MakeBox.hxx>

S_McCadGDMLWriter::S_McCadGDMLWriter()
{
}
S_McCadGDMLWriter::~S_McCadGDMLWriter()
{
}
void   S_McCadGDMLWriter:: PrintDefine(QDomElement & ParentElement)
{
    //print the define element
    QDomElement theElement = m_Doc.createElement(("define"));
    ParentElement.appendChild(theElement);

    //get the data
    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);

    //loop the solid list
    for (unsigned int i = 0; i < pData->m_SolidList.size(); i++)
    {
        S_McCadSolid * pSolid = static_cast <S_McCadSolid *> (pData->m_SolidList.at(i));  //qiu cast to S_McCadSolid
        assert(pSolid);

        //check if decomposed, if not don't need to decomposed it
        if (!pSolid->GetisDecomposed())
            continue;

        QString aName (pSolid->GetName().ToCString()) ; //get the solid name
        //print the polyhedron for the whole pSolid
        PrintPolyhedron(theElement, pSolid, aName);

        int ConvexSolidCnt = pSolid->GetConvexSolidList().size();
        if (ConvexSolidCnt == 1) continue; //only one convex solid: print only once
        for(int j = 0; j < ConvexSolidCnt; j++)
        {
            McCadConvexSolid *pConvexSolid = pSolid->GetConvexSolidList().at(j);
            assert(pConvexSolid);

            //name for the convex solid, if only one convex solid, using the aName without surfix
            QString bName /*;
            if (ConvexSolidCnt == 1) bName = aName;
            else bName*/ = aName + QString("_%1").arg(j);
            PrintPolyhedron(theElement, pConvexSolid, bName);
        }
    }

    //print the world box polyhedron
    //!! COMMENT OUT because the worldBox is failed in genearted surface in McCad
    //!! Using a hug box instead!
//    McCadVoidCell * pVoid = getWholeBntBox();
//    PrintPolyhedron(theElement,pVoid, "WorldBox");

}


void    S_McCadGDMLWriter::PrintMaterial(QDomElement & ParentElement)
{
    QDomElement theElement = m_Doc.createElement(("materials"));
    ParentElement.appendChild(theElement);

    //get the studyID and engine
    int studyID = SUIT_Session::session()->activeApplication()->activeStudy()->id();
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    if ( !studyID || CORBA::is_nil( engine ) )    {
        MESSAGE("Errors in study or in MCCAD engine!");
        return ;
    }
    MCCAD_ORB::MaterialList * aMaterialList = engine->getMaterialList(studyID);
    int n = aMaterialList->length();
    if (!n){
        MESSAGE("No material data!");
        return ;
    }

    for(int i = 0; i < n; i++)
    {
        MCCAD_ORB::Material_var aMaterial = (*aMaterialList)[i];

        //create a Material element
        QDomElement MaterialElement = m_Doc.createElement(("material"));
        MaterialElement.setAttribute("name", QString("Mat_%1").arg(aMaterial->getName())); //add "Mat_" to avoid repeated name in GDML
        //create the density element
        QDomElement DensityElement = m_Doc.createElement(("D"));
        DensityElement.setAttribute("value", aMaterial->getDensity());
        DensityElement.setAttribute("unit", "g/cm3"); //We use g/cm3 as defaut unit, in future should be selectable.
        MaterialElement.appendChild(DensityElement);
//        QDomText txtMatcard = m_Doc.createTextNode(aMaterial->GetMatCard());
//        MaterialElement.appendChild(txtMatcard);
        theElement.appendChild(MaterialElement);
    }

}


void      S_McCadGDMLWriter::PrintSolids (QDomElement & ParentElement)
{
    QDomElement theElement = m_Doc.createElement(("solids"));
    ParentElement.appendChild(theElement);

    //get the data
    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);

    //loop the solid list
    for (unsigned int i = 0; i < pData->m_SolidList.size(); i++)
    {
        S_McCadSolid * pSolid = static_cast <S_McCadSolid *> (pData->m_SolidList.at(i));  //qiu cast to S_McCadSolid
        assert(pSolid);
        //print convex solid
        QString aName (pSolid->GetName().ToCString()) ; //get the solid name
        int SubSolidCount =0;

        //if undecomposed solid, print as tessellated solid
        //if has compound, explode it and make a union solid at the end
        if (!pSolid->GetisDecomposed())
        {
            TopoDS_Shape aTmpShape = pSolid->GetUndecompsedSolid();
            if( aTmpShape.ShapeType() == TopAbs_COMPSOLID || aTmpShape.ShapeType() == TopAbs_COMPOUND )
            {

                TopExp_Explorer exp;
                //first we want to see how many solid inside
                for(exp.Init(aTmpShape,TopAbs_SOLID); exp.More(); exp.Next())
                    SubSolidCount ++;
                //then print the solids
                int j=0;
                for(exp.Init(aTmpShape,TopAbs_SOLID); exp.More(); exp.Next()) {
                    TopoDS_Shape bTmpShape = TopoDS::Solid(exp.Current());
                    QString bName;
                    if (SubSolidCount == 1) bName = aName;
                    else bName = aName + QString("_%1").arg(j);
                    PrintTessellatedSolid(theElement, bTmpShape, bName);
                    j++;
                }

                //make boolean operation to Union
                if ( SubSolidCount == 1)
                    continue;  //no need for the union
                else {
                    PrintHalfSpaceBoolean(theElement, aName, SubSolidCount, false); //don't print polyhedron
                }

            }
            else if (aTmpShape.ShapeType() == TopAbs_SOLID){
                PrintTessellatedSolid(theElement, aTmpShape, aName);
            }
            else {
                MESSAGE("The part "<<pSolid->GetName().ToCString()<<"contains no solid!!");
                continue;
            }
        }
        // for decomposed solid, normal procedures
        else
        {
            SubSolidCount = pSolid->GetConvexSolidList().size();
            for(int j = 0; j < SubSolidCount; j++)
            {
                McCadConvexSolid *pConvexSolid = pSolid->GetConvexSolidList().at(j);
                assert(pConvexSolid);
                //name for the convex solid, if only one convex solid, using the aName without surfix
                QString bName;
                if (SubSolidCount == 1) bName = aName;
                else bName = aName + QString("_%1").arg(j);
                PrintHalfSpaceSolid(theElement, pConvexSolid, bName);
            }
            //make boolean operation to Union
            if ( SubSolidCount == 1)
                continue;  //no need for the union
            else {
                PrintHalfSpaceBoolean(theElement, aName, SubSolidCount);
            }
        }
    }

    //print the world solid
    //first we make this box
    //!! COMMENT OUT because the worldBox is failed in genearted surface in McCad
    //!! Using a hug box instead!
//    McCadVoidCell * pVoid = getWholeBntBox();
//    PrintHalfSpaceSolid(theElement, pVoid, "WorldBox");
//    delete pVoid;

    PrintWorldBox(theElement);

}


void  S_McCadGDMLWriter::PrintTessellatedSolid(QDomElement & ParentElement, const TopoDS_Shape & aShape, const QString aName)
{
    //creat the Tessellated solid node
    QDomElement theElement = m_Doc.createElement(("tessellated"));
    ParentElement.appendChild(theElement);

    theElement.setAttribute("name", aName);
    theElement.setAttribute("lunit", "cm");

/* These are not working because m_Doc does not append the "gdml" element yet!
    //get the <define> node
    QDomElement docElem = m_Doc.documentElement();
//    QDomNodeList DefineNodeList = docElem.elementsByTagName("define"); //get the list of node with tag name "define", actually only one
    QDomNodeList DefineNodeList = docElem.elementsByTagName("gdml");
    if (DefineNodeList.size() == 0) {
        cout<<"cannot find gdml node!"<<endl;
    }
    DefineNodeList = DefineNodeList.at(0).toElement().elementsByTagName("define");
    QDomElement DefineElement;

    if (DefineNodeList.size() == 0) {
        cout <<"the Define elemenet is null!" <<endl;
    }
    else if (DefineNodeList.size() == 1){
        DefineElement  = DefineNodeList.at(0).toElement();
    }
    else
        cout <<"the Define elemenet is abnormal!" <<endl;

//    QDomNode DefineNode = ParentElement.previousSibling();
//    QDomElement DefineElement = DefineNode.toElement();
//comment out    if (DefineNode.isNull())  // because isNull is true when element has no content.
//      if (!DefineNode.isElement())
//        cout <<"the Define elemenet is null!" <<endl;
//    else
//        cout <<"the Name : " <<DefineNode.nodeName().toStdString()<<endl;
*/
//    cout <<"name of current parent: " <<ParentElement.nodeName().toStdString()<<endl;
     QDomElement DefineElement =  ParentElement.parentNode().toElement()
             .elementsByTagName("define").at(0).toElement();
    if (DefineElement.isNull())
        cout <<"the Define elemenet is null!" <<endl;
//    else
//        cout <<"the Name : " <<DefineElement.nodeName().toStdString()<<endl;

    SUIT_Session* session = SUIT_Session::session();
    SUIT_ResourceMgr* resMgr = session->resourceMgr();
//    Standard_Real aDeflection= resMgr->doubleValue("MCCAD","STL_Coefficient", 0.001); //STL_Coefficient is removed!!
    Standard_Real aDeflection= resMgr->doubleValue("MCCAD","STL_Deflection", 0.001);

    TopoDS_Shape theShape = aShape;
    double  theDeflection = aDeflection;//temperory
    MeshShape(theShape, theDeflection);

    //get the node list and facet list
    //we create the nodes and facets element for the gdml directly here

    int NodeTotal=0;  //each time a face is mesh, the total node will be updated
    TopExp_Explorer ex;
    TopLoc_Location aLoc;
    for (ex.Init(theShape, TopAbs_FACE); ex.More(); ex.Next())
    {
        const TopoDS_Face& aFace = TopoDS::Face(ex.Current());
        Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(aFace,aLoc);
        if(aPoly.IsNull()) {
            return;
        }
        //get the transformation
        gp_Trsf myTransf;
        Standard_Boolean identity = true;
        if(!aLoc.IsIdentity()){
          identity = false;
          myTransf = aLoc.Transformation();
        }
        //get the nodes and facets
        const TColgp_Array1OfPnt& Nodes = aPoly->Nodes();
        Standard_Integer nbNodesInFace = aPoly->NbNodes();
        for( int i = 1; i <= nbNodesInFace; i++) { // i start with 1 ??
          gp_Pnt P = Nodes(i);
          if(!identity)
            P.Transform(myTransf);
          //make a position element under the <define> node

          QDomElement aNodeElement = m_Doc.createElement(("position"));
          aNodeElement.setAttribute("name", aName + QString("_v%1").arg(i + NodeTotal));
          aNodeElement.setAttribute("unit", "cm");
          aNodeElement.setAttribute("x",P.X()/10); //unit cm
          aNodeElement.setAttribute("y",P.Y()/10);
          aNodeElement.setAttribute("z",P.Z()/10);
          DefineElement.appendChild(aNodeElement);
        }

        const Poly_Array1OfTriangle& Triangles = aPoly->Triangles();
        Standard_Integer nbTriInFace = aPoly->NbTriangles();
        for( int i = 1; i <= nbTriInFace; i++){  // i start with 1 ??
          // Get the triangle
          Standard_Integer N1,N2,N3;
          Triangles(i).Get(N1,N2,N3);
          N1 += NodeTotal ; //node id should start from 1
          N2 += NodeTotal ;
          N3 += NodeTotal ;

          QDomElement aFacetElement = m_Doc.createElement(("triangular"));
          //IMPORTANT!! this must do otherwise the visualization will be wrong!
          if (aFace.Orientation() == TopAbs_REVERSED)
          {
              aFacetElement.setAttribute("vertex1",aName + QString("_v%1").arg(N3));// reverse the order if face is reversed!!
              aFacetElement.setAttribute("vertex2",aName + QString("_v%1").arg(N2));
              aFacetElement.setAttribute("vertex3",aName + QString("_v%1").arg(N1));
          }
          else
          {
              aFacetElement.setAttribute("vertex1",aName + QString("_v%1").arg(N1));
              aFacetElement.setAttribute("vertex2",aName + QString("_v%1").arg(N2));
              aFacetElement.setAttribute("vertex3",aName + QString("_v%1").arg(N3));
          }

          theElement.appendChild(aFacetElement);
        }
        NodeTotal += nbNodesInFace;  //MUST DO!!
    }

}

McCadVoidCell * S_McCadGDMLWriter::getWholeBntBox()
{
    Bnd_Box bnd_box;
    McCadGeomData * pData = m_pManager->GetGeomData();

    for (unsigned int i = 0; i < pData->m_ConvexSolidList.size(); i++)
    {
        McCadConvexSolid  *pSolid = pData->m_ConvexSolidList[i];
        assert(pSolid);
        BRepBndLib::Add((TopoDS_Shape)(*pSolid),bnd_box); // Add each convex solid into bundary box
    }

    //also add the undecomposed solid
    for (unsigned int i=0; i<pData->m_SolidList.size(); i++)
    {
        S_McCadSolid * pSolid = static_cast <S_McCadSolid *> (pData->m_SolidList.at(i));  //qiu cast to S_McCadSolid
        if (!pSolid->GetisDecomposed())
            BRepBndLib::Add(pSolid->GetUndecompsedSolid(),bnd_box); // Add each convex solid into bundary box
    }

    bnd_box.SetGap(1.0); // Set the gap between the boundary box and material solids
    Standard_Real dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
    bnd_box.Get(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);

    gp_Pnt bnd_max_pnt(dXmax,dYmax,dZmax);
    gp_Pnt bnd_min_pnt(dXmin,dYmin,dZmin);

    // Create the boundary box.
    McCadVoidCell *pVoid = new McCadVoidCell(BRepPrimAPI_MakeBox(bnd_min_pnt,bnd_max_pnt).Solid());
    pVoid->SetBntBox(dXmin,dYmin,dZmin,dXmax,dYmax,dZmax);
    return pVoid;
}


void    S_McCadGDMLWriter::CheckMaterialNames()
{

    //get the studyID and engine
    int studyID = SUIT_Session::session()->activeApplication()->activeStudy()->id();
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    if ( !studyID || CORBA::is_nil( engine ) )    {
        MESSAGE("Errors in study or in MCCAD engine!");
        return ;
    }
    MCCAD_ORB::MaterialList * aMaterialList = engine->getMaterialList(studyID);
    int n = aMaterialList->length();
    if (!n){
        MESSAGE("No material data!");
        return ;
    }

    vector <QString> aNameList;

    for(int i = 0; i < n; i++)
    {
        MCCAD_ORB::Material_var aMaterial = (*aMaterialList)[i];
        aNameList.push_back(aMaterial->getName());
    }
    CheckNames(aNameList);
    for(int i = 0; i < n; i++)
    {
        MCCAD_ORB::Material_var aMaterial = (*aMaterialList)[i];
        aMaterial->setName(aNameList.at(i).toStdString().c_str());
    }

}


void     S_McCadGDMLWriter::PrintStructure(QDomElement & ParentElement)
{
    QDomElement theElement = m_Doc.createElement(("structure"));

    //get the data
    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);
    //get the studyID and engine
    int studyID = SUIT_Session::session()->activeApplication()->activeStudy()->id();
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    if ( !studyID || CORBA::is_nil( engine ) )    {
        MESSAGE("Errors in study or in MCCAD engine!");
        return ;
    }


    //loop the solid list
    for (unsigned int i = 0; i < pData->m_SolidList.size(); i++)
    {
        S_McCadSolid * pSolid = static_cast <S_McCadSolid *> (pData->m_SolidList.at(i));  //qiu cast to S_McCadSolid
        assert(pSolid);
        QString aSolidName (pSolid->GetName().ToCString()) ; //get the solid name

        //get the material name
        QString aMatName = "unknown"; //if cannot find the material or no material, set name to "unknown"
        MCCAD_ORB::Material_var aMaterial = engine->getMaterial(studyID, pSolid->GetID());
        if (!aMaterial->_is_nil())
            aMatName = aMaterial->getName(); //using the mat id in the solid to get mat name
        //create the volume element
        PrintVolume(theElement, aMatName, aSolidName);
    }

    //create the world volume
    QDomElement WorldVolElement = m_Doc.createElement(("volume"));
    WorldVolElement.setAttribute("name", QString("World"));
    QDomElement MaterialRefElement = m_Doc.createElement(("materialref"));
    MaterialRefElement.setAttribute("ref", "Air");
    QDomElement SolidRefElement = m_Doc.createElement("solidref");
    SolidRefElement.setAttribute("ref","WorldBox"); //ref to the solid named "WorldBox"
    WorldVolElement.appendChild(MaterialRefElement);
    WorldVolElement.appendChild(SolidRefElement);

    for (unsigned int i = 0; i < pData->m_SolidList.size(); i++)
    {
        McCadSolid * pSolid = pData->m_SolidList.at(i);
        assert(pSolid);
        QString aSolidName (pSolid->GetName().ToCString()) ; //get the solid name

        //create the physcial volume element
        PrintPhysicalVolume(WorldVolElement, aSolidName);
    }

    theElement.appendChild(WorldVolElement);
    ParentElement.appendChild(theElement);
}



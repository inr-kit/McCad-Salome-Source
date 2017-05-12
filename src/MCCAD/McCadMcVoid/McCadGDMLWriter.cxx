#include "McCadGDMLWriter.hxx"

#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <Bnd_Box.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>

#include <BRepBndLib.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <Poly_Triangulation.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <BRepPrimAPI_MakeBox.hxx>


#include "assert.h"

McCadGDMLWriter::McCadGDMLWriter()
{
}
McCadGDMLWriter::~McCadGDMLWriter()
{
}

void    McCadGDMLWriter::CheckNames(vector<QString> aNameList)
{
    QString aBackup;  //back up the name
    QString aNewName; //new name if repeat
    int n=1;

    for (unsigned int i=0; i<aNameList.size(); i++)
    {
        aBackup = aNameList[i];
        for (unsigned int j=0; i<j; j++)
        {
            if (aNameList[i] == aNameList[j]) //repeated name
            {
                aNewName = aBackup + QString("_%1").arg(n);
                n++;
            }
        }
        if (aNewName != aBackup) // if the name is changed
            aNameList[i] = aNewName;
        n =1; //reset
    }
}
void    McCadGDMLWriter::CheckSolidNames()
{
    //get the data
    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);
    //check the solid names
    vector <QString> aNameList;
    for (unsigned int i = 0; i < pData->m_SolidList.size(); i++)  {
        TCollection_AsciiString atmpName = pData->m_SolidList.at(i)->GetName();
        aNameList.push_back(QString(atmpName.ToCString()));
        cout <<"Solid name : " <<atmpName.ToCString()<<endl; //for test
    }
    CheckNames(aNameList);
    for (unsigned int i = 0; i < pData->m_SolidList.size(); i++)  {
        QString atmpName = aNameList.at(i);
        cout <<"After rename name : " <<atmpName.toStdString()<<endl; //for test
        pData->m_SolidList.at(i)->SetName(TCollection_AsciiString(atmpName.toStdString().c_str()));
    }
}
void    McCadGDMLWriter::CheckMaterialNames()
{
    //get the data
    MaterialManager * pMatManager = m_pManager->GetMatManager();
    assert(pMatManager); //qiu comment out because error: mat manager is create late!
//    if (!pMatManager) return;

    vector<Material *> aMaterialList = pMatManager->GetMatList();
    vector <QString> aNameList;

    for(unsigned int i = 0; i < aMaterialList.size(); i++)
        aNameList.push_back(aMaterialList.at(i)->GetMatName());
    CheckNames(aNameList);
    for (unsigned int i = 0; i < aMaterialList.size(); i++)  {
        aMaterialList.at(i)->SetMatName(aNameList.at(i));
    }
}


void    McCadGDMLWriter::PrintFile()
{
    //check the solid and material name
    CheckSolidNames();
    CheckMaterialNames();

    //open the document
    QFile *file = new QFile(QString(m_OutputFileName.ToCString()));
    if(!file->open(QFile::WriteOnly|QFile::Truncate))
        return;
    //the heading instruction
    QDomProcessingInstruction instruction;
    instruction = m_Doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    m_Doc.appendChild(instruction);

    QDomElement root=m_Doc.createElement(("gdml"));    //create the gdml Element
    root.setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    root.setAttribute("xsi:noNamespaceSchemaLocation","gdml.xsd" );   //default schema location: current directory
    m_Doc.appendChild(root);

    //let's  print it
    PrintDefine(root);
    PrintMaterial(root);
    PrintSolids(root);
    PrintStructure(root);
    PrintSetup(root);


    QTextStream out(file);
    m_Doc.save(out,4);
    file->close();
}

void    McCadGDMLWriter::PrintDefine(QDomElement & ParentElement)
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
        McCadSolid * pSolid = pData->m_SolidList.at(i);
        assert(pSolid);
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



void McCadGDMLWriter::MeshShape(const TopoDS_Shape theShape, double & theDeflection)
{

  Standard_Real aDeflection = theDeflection <= 0 ? 0.0001 : theDeflection;

  //If deflection <= 0, than return default deflection
  if(theDeflection <= 0)
    theDeflection = aDeflection;

  // Is shape triangulated?
//  Standard_Boolean alreadymeshed = Standard_True;
//  TopExp_Explorer ex;
//  TopLoc_Location aLoc;
//  for (ex.Init(theShape, TopAbs_FACE); ex.More(); ex.Next()) {
//    const TopoDS_Face& aFace = TopoDS::Face(ex.Current());
//    Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(aFace,aLoc);
//    if(aPoly.IsNull()) {
//  alreadymeshed = Standard_False;
//  break;
//    }
//  }

  //the judgement of triangulation is comment out because we would like to mesh it
  //everytime with different deflection.
    Standard_Boolean alreadymeshed = Standard_False;  // force to mesh again

  if(!alreadymeshed) {
    Bnd_Box B;
    BRepBndLib::Add(theShape, B);
    if ( B.IsVoid() )
  return; // NPAL15983 (Bug when displaying empty groups)
    Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

    // This magic line comes from Prs3d_ShadedShape.gxx in OCCT
    aDeflection = max(aXmax-aXmin, max( aYmax-aYmin, aZmax-aZmin)) * aDeflection * 4;

    //Clean triangulation before compute incremental mesh
    BRepTools::Clean(theShape);

    //Compute triangulation
    BRepMesh_IncrementalMesh MESH(theShape,aDeflection);
  }
}

void    McCadGDMLWriter::PrintPolyhedron(QDomElement & ParentElement, const TopoDS_Shape * aShape, const QString & aName)
{
    //write the name attribute
    QDomElement theElement = m_Doc.createElement(("Polyhedron"));
    ParentElement.appendChild(theElement);
    theElement.setAttribute("name", QString("Poly_")+ aName); //add "Poly_" to avoid repeated name in GDML

    //mesh the solid
    TopoDS_Shape theShape = *aShape;
    double  theDeflection = 0.001;
    MeshShape(theShape, theDeflection);

    //get the node list and facet list
    //we create the nodes and facets element for the gdml directly here


    QDomElement NodesElement = m_Doc.createElement(("Nodes"));
    NodesElement.setAttribute("lunit", "cm");
    QDomElement FacetsElement = m_Doc.createElement(("Facets"));
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
          //make a node element
          QDomElement aNodeElement = m_Doc.createElement(("Node"));
          aNodeElement.setAttribute("x",P.X()/10); //unit cm
          aNodeElement.setAttribute("y",P.Y()/10);
          aNodeElement.setAttribute("z",P.Z()/10);
          NodesElement.appendChild(aNodeElement);
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

          QDomElement aFacetElement = m_Doc.createElement(("Facet"));
          //IMPORTANT!! this must do otherwise the visualization will be wrong!
          if (aFace.Orientation() == TopAbs_REVERSED)
          {
              aFacetElement.setAttribute("n1",N3); // reverse the order if face is reversed!!
              aFacetElement.setAttribute("n2",N2);
              aFacetElement.setAttribute("n3",N1);
          }
          else
          {
              aFacetElement.setAttribute("n1",N1);
              aFacetElement.setAttribute("n2",N2);
              aFacetElement.setAttribute("n3",N3);
          }

          FacetsElement.appendChild(aFacetElement);
        }
        NodeTotal += nbNodesInFace;  //MUST DO!!
    }

    theElement.appendChild(NodesElement);
    theElement.appendChild(FacetsElement);

}

void    McCadGDMLWriter::PrintPolyhedron(QDomElement & ParentElement, McCadSolid * pSolid, const QString & aName)
{
    //write the name attribute
    QDomElement theElement = m_Doc.createElement(("Polyhedron"));
    ParentElement.appendChild(theElement);
    theElement.setAttribute("name", QString("Poly_")+ aName); //add "Poly_" to avoid repeated name in GDML

    int ConvexSolidCnt = pSolid->GetConvexSolidList().size();
    int NodeTotal=0;  //each time a face is mesh, the total node will be updated
    QDomElement NodesElement = m_Doc.createElement(("Nodes"));
    NodesElement.setAttribute("lunit", "cm");
    QDomElement FacetsElement = m_Doc.createElement(("Facets"));
    double  theDeflection = 0.001;

    for(int j = 0; j < ConvexSolidCnt; j++)
    {
        McCadConvexSolid *pConvexSolid = pSolid->GetConvexSolidList().at(j);
        assert(pConvexSolid);

        //mesh the solid
        TopoDS_Shape theShape = *pConvexSolid;
        MeshShape(theShape, theDeflection);

        //get the node list and facet list
        //we create the nodes and facets element for the gdml directly here
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
              //make a node element
              QDomElement aNodeElement = m_Doc.createElement(("Node"));
              aNodeElement.setAttribute("x",P.X()/10); //unit cm
              aNodeElement.setAttribute("y",P.Y()/10);
              aNodeElement.setAttribute("z",P.Z()/10);
              NodesElement.appendChild(aNodeElement);
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

              QDomElement aFacetElement = m_Doc.createElement(("Facet"));
              if (aFace.Orientation() == TopAbs_REVERSED)
              {
                  aFacetElement.setAttribute("n1",N3); // reverse the order if face is reversed!!
                  aFacetElement.setAttribute("n2",N2);
                  aFacetElement.setAttribute("n3",N1);
              }
              else
              {
                  aFacetElement.setAttribute("n1",N1);
                  aFacetElement.setAttribute("n2",N2);
                  aFacetElement.setAttribute("n3",N3);
              }
              FacetsElement.appendChild(aFacetElement);
            }
            NodeTotal += nbNodesInFace;  //MUST DO!!
        }
    }

    theElement.appendChild(NodesElement);
    theElement.appendChild(FacetsElement);


}


void    McCadGDMLWriter::PrintMaterial(QDomElement & ParentElement)
{
    QDomElement theElement = m_Doc.createElement(("materials"));
    ParentElement.appendChild(theElement);

    //get the data
    MaterialManager * pMatManager = m_pManager->GetMatManager();
    assert(pMatManager);

    vector<Material *> aMaterialList = pMatManager->GetMatList();

    for(unsigned int i = 0; i < aMaterialList.size(); i++)
    {
        //create a Material element
        QDomElement MaterialElement = m_Doc.createElement(("material"));
        Material * aMaterial = aMaterialList.at(i);
        MaterialElement.setAttribute("name", QString("Mat_") + aMaterial->GetMatName()); //add "Mat_" to avoid repeated name in GDML
        //create the density element
        QDomElement DensityElement = m_Doc.createElement(("D"));
        DensityElement.setAttribute("value", aMaterial->GetMatDensity());
        DensityElement.setAttribute("unit", "g/cm3"); //We use g/cm3 as defaut unit, in future should be selectable.
        MaterialElement.appendChild(DensityElement);
//        QDomText txtMatcard = m_Doc.createTextNode(aMaterial->GetMatCard());
//        MaterialElement.appendChild(txtMatcard);
        theElement.appendChild(MaterialElement);
    }

}


void     McCadGDMLWriter::PrintSolids (QDomElement & ParentElement)
{
    QDomElement theElement = m_Doc.createElement(("solids"));
    ParentElement.appendChild(theElement);

    //get the data
    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);

    //loop the solid list
    for (unsigned int i = 0; i < pData->m_SolidList.size(); i++)
    {
        McCadSolid * pSolid = pData->m_SolidList.at(i);
        assert(pSolid);
        //print convex solid
        QString aName (pSolid->GetName().ToCString()) ; //get the solid name
        int ConvexSolidCnt = pSolid->GetConvexSolidList().size();
        for(int j = 0; j < ConvexSolidCnt; j++)
        {
            McCadConvexSolid *pConvexSolid = pSolid->GetConvexSolidList().at(j);
            assert(pConvexSolid);
            //name for the convex solid, if only one convex solid, using the aName without surfix
            QString bName;
            if (ConvexSolidCnt == 1) bName = aName;
            else bName = aName + QString("_%1").arg(j);
            PrintHalfSpaceSolid(theElement, pConvexSolid, bName);
        }

        //make boolean operation to Union
        if ( pSolid->GetConvexSolidList().size() == 1)
            continue;  //no need for the union
        else {
            PrintHalfSpaceBoolean(theElement, aName, ConvexSolidCnt);
        }
    }

    //print the world solid
    //first we make this box
//no need    TopoDS_Solid aBndBox = *(pData->m_pOutVoid);  //McCadVoidCell is inherit from TopoDS_Solid
//no need    McCadConvexSolid * pWorldBox = new McCadConvexSolid(aBndBox);
    //!! COMMENT OUT because the worldBox is failed in genearted surface in McCad
    //!! Using a hug box instead!
//    McCadVoidCell * pVoid = getWholeBntBox();
//    PrintHalfSpaceSolid(theElement, pVoid, "WorldBox");
//    delete pVoid;
    PrintWorldBox(theElement);

}

 void    McCadGDMLWriter::PrintWorldBox (QDomElement & ParentElement)
{
    //build a element
    QDomElement theElement = m_Doc.createElement(("box"));
    theElement.setAttribute("name", "WorldBox");
    theElement.setAttribute("x", "100000");
    theElement.setAttribute("y", "100000");
    theElement.setAttribute("z", "100000");
    ParentElement.appendChild(theElement);
}


void    McCadGDMLWriter::PrintHalfSpaceSolid(QDomElement & ParentElement,
                            McCadConvexSolid *pConvexSolid, const QString & aName)
{
    //build a element
    QDomElement theElement = m_Doc.createElement(("HalfSpaceSolid"));
    ParentElement.appendChild(theElement);

    theElement.setAttribute("name", aName);
    theElement.setAttribute("lunit", "cm");

    //print the surface //unit cm
    PrintSurfaces(theElement, pConvexSolid);

    //print the bounadry box //unit cm
    PrintBoundaryBox(theElement, pConvexSolid);

    //print other values (volume size, surface area, polyhedron) //unit cm
    PrintVolumeArea(theElement, pConvexSolid, aName);
    PrintPolyhedronRef(theElement, aName);

}

//void    McCadGDMLWriter::PrintHalfSpaceSolid(QDomElement & ParentElement,
//                            McCadVoidCell *pVoid, const QString & aName)
//{
//    //build a element
//    QDomElement theElement = m_Doc.createElement(("HalfSpaceSolid"));
//    ParentElement.appendChild(theElement);
//    theElement.setAttribute("name", aName);
//    theElement.setAttribute("lunit", "cm");

//    //print the surface
//    PrintSurfaces(theElement, pVoid);

//    //print the bounadry box
//    PrintBoundaryBox(theElement, pVoid);

//    //print other values (volume size, surface area, polyhedron)
//    PrintVolumeArea(theElement, pVoid, aName);
//    PrintPolyhedronRef(theElement, aName);

//}


void    McCadGDMLWriter::PrintSurfaces(QDomElement & ParentElement, McCadConvexSolid *pConvexSolid)
{
    //get the data
    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);

    //print the surface element
    QDomElement theElement = m_Doc.createElement(("Surfaces"));
    ParentElement.appendChild(theElement);

    //remove the repeat surface
    vector<Standard_Integer> IntSurfList;
    vector<Standard_Integer> UniSurfList;
    checkRepeatFaces(pConvexSolid,IntSurfList,UniSurfList );
    for (unsigned int  i = 0; i < IntSurfList.size(); i++)
    {
        int iFaceNum = IntSurfList.at(i); //get the surface number
        int theSense = iFaceNum > 0 ?  1 : -1; //if face number > 0: positive; else negative
        //using the face number to get the face from list

//        cout <<"iFaceNum   " <<iFaceNum<<endl;

        IGeomFace * pFace = findSurface(abs(iFaceNum) -1); //?? correct?  //SHOULD -1 because iFaceNum start from 1!!
        if (pFace == NULL)    {
            QDomComment aComment=   m_Doc.createComment("FatalError: McCadGDMLWriter::Cannot get the surface of number");
            ParentElement.appendChild(aComment);
            return;
        }

        PrintASurface(theElement, pFace, theSense);  //print the surface
    }
    for (unsigned int  i = 0; i < UniSurfList.size(); i++)
    {
        QDomComment aComment=   m_Doc.createComment("Warning: McCadGDMLWriter::PrintSurfaces does not support Union Auxiliary surface!!");
        theElement.appendChild(aComment);

        int iFaceNum = UniSurfList.at(i); //get the surface number
        int theSense = iFaceNum > 0 ?  1 : -1; //if face number > 0: positive; else negative
        //using the face number to get the face from list

//        cout <<"iFaceNum   " <<iFaceNum<<endl;

        IGeomFace * pFace = findSurface(abs(iFaceNum) -1); //?? correct?  //SHOULD -1 because iFaceNum start from 1!!
        if (pFace == NULL)    {
            QDomComment aComment=   m_Doc.createComment("FatalError: McCadGDMLWriter::Cannot get the surface of number");
            ParentElement.appendChild(aComment);
            return;
        }

        PrintASurface(theElement, pFace, theSense);  //print the surface
    }
 /*
    for (unsigned int  i = 0; i < (pConvexSolid->GetFaces()).size(); i++)
    {
        McCadExtBndFace * pExtFace = (pConvexSolid->GetFaces()).at(i);
        int iFaceNum = pExtFace->GetFaceNum(); //get the surface number
        int theSense = iFaceNum > 0 ?  1 : -1; //if face number > 0: positive; else negative
        //using the face number to get the face from list

//        cout <<"iFaceNum   " <<iFaceNum<<endl;

        IGeomFace * pFace = findSurface(abs(iFaceNum) -1); //?? correct?  //SHOULD -1 because iFaceNum start from 1!!
        if (pFace == NULL)    {
            cout<<"FatalError: McCadGDMLWriter::Cannot get the surface of number "<<iFaceNum<<" !! ";
            return;
        }

        PrintASurface(theElement, pFace, theSense);  //print the surface

        if (pExtFace->HaveAstSurf())
        {
            for(unsigned int j = 0; j < pExtFace->GetAstFaces().size(); j++)
            {
                McCadExtAstFace *pAuxFace = pExtFace->GetAstFaces().at(j);
                if (pAuxFace->IsSplitFace()) //union aux surface
                {
                  QDomComment aComment=   m_Doc.createComment("Warning: McCadGDMLWriter::PrintSurfaces does not support Union Auxiliary surface!!");
                  theElement.appendChild(aComment);
//                    cout<<"FatalError: McCadGDMLWriter::PrintSurfaces does not support Union Auxiliary surface!! ";
//2016-04-25 we want to print all the surface anyway
                  //return;
                  int iAuxFaceNum = pAuxFace->GetFaceNum();
                  theSense = iAuxFaceNum >0 ? 1 : -1 ;
                  IGeomFace * qFace = findSurface(abs(iAuxFaceNum) -1);
                  if (qFace == NULL)    {
                      QDomComment aComment=   m_Doc.createComment("FatalError: McCadGDMLWriter::Cannot get the surface of number!");
                      ParentElement.appendChild(aComment);
//                        cout<<"FatalError: McCadGDMLWriter::Cannot get the surface of number "<<iFaceNum<<" !! ";
                      return;
                  }
                  PrintASurface(theElement, qFace, theSense);  //print the surface
                }
                else  //intersect aux surface
                {
                    int iAuxFaceNum = pAuxFace->GetFaceNum();
                    theSense = iAuxFaceNum >0 ? 1 : -1 ;
                    //qiu A BUG HERE!!
//                    IGeomFace * qFace = findSurface(abs(iFaceNum) -1); //?? correct?
                    IGeomFace * qFace = findSurface(abs(iAuxFaceNum) -1); //?? correct?
                    if (qFace == NULL)    {
                        QDomComment aComment=   m_Doc.createComment("FatalError: McCadGDMLWriter::Cannot get the surface of number!");
                        ParentElement.appendChild(aComment);
//                        cout<<"FatalError: McCadGDMLWriter::Cannot get the surface of number "<<iFaceNum<<" !! ";
                        return;
                    }
                    PrintASurface(theElement, qFace, theSense);  //print the surface
                }
            }
        }

    }
*/
}


//void    McCadGDMLWriter::PrintSurfaces(QDomElement & ParentElement, McCadVoidCell *pVoid)
//{
//    //get the data
//    McCadGeomData *pData = m_pManager->GetGeomData();
//    assert(pData);

//    //print the surface element
//    QDomElement theElement = m_Doc.createElement(("Surfaces"));
//    ParentElement.appendChild(theElement);

//    for (unsigned int  i = 0; i < (pVoid->GetBndFaces()).size(); i++)
//    {
//        McCadExtBndFace * pExtFace = (pVoid->GetBndFaces()).at(i);
//        int iFaceNum = pExtFace->GetFaceNum(); //get the surface number
//        int theSense = iFaceNum > 0 ?  1 : -1; //if face number > 0: positive; else negative
//        //using the face number to get the face from list

////        cout <<"iFaceNum   " <<iFaceNum<<endl;

//        IGeomFace * pFace = findSurface(abs(iFaceNum) -1); //?? correct?
//        if (pFace == NULL)    {
//            cout<<"FatalError: McCadGDMLWriter::Cannot get the surface of number "<<iFaceNum<<" !! ";
//            return;
//        }

//        PrintASurface(theElement, pFace, theSense);  //print the surface
//        if (pExtFace->HaveAstSurf())
//        {
//            for(unsigned int j = 0; j < pExtFace->GetAstFaces().size(); j++)
//            {
//                McCadExtAstFace *pAuxFace = pExtFace->GetAstFaces().at(j);
//                if (pAuxFace->IsSplitFace()) //union aux surface
//                {
//                    QDomComment aComment=   m_Doc.createComment("Warning: McCadGDMLWriter::PrintSurfaces does not support Union Auxiliary surface!!");
//                    theElement.appendChild(aComment);
//  //                    cout<<"FatalError: McCadGDMLWriter::PrintSurfaces does not support Union Auxiliary surface!! ";
//  //2016-04-25 we want to print all the surface anyway
//                    //return;
//                    int iAuxFaceNum = pAuxFace->GetFaceNum();
//                    theSense = iAuxFaceNum >0 ? 1 : -1 ;
//                    IGeomFace * qFace = findSurface(abs(iAuxFaceNum) -1);
//                    if (qFace == NULL)    {
//                        QDomComment aComment=   m_Doc.createComment("FatalError: McCadGDMLWriter::Cannot get the surface of number!");
//                        ParentElement.appendChild(aComment);
//  //                        cout<<"FatalError: McCadGDMLWriter::Cannot get the surface of number "<<iFaceNum<<" !! ";
//                        return;
//                    }
//                    PrintASurface(theElement, qFace, theSense);  //print the surface
//                }
//                else  //intersect aux surface
//                {
//                    int iAuxFaceNum = pAuxFace->GetFaceNum();
//                    theSense = iAuxFaceNum >0 ? 1 : -1 ;
//                    //qiu A BUG HERE!!
////                    IGeomFace * qFace = findSurface(abs(iFaceNum) -1); //?? correct?
//                    IGeomFace * qFace = findSurface(abs(iAuxFaceNum) -1); //?? correct?
//                    if (qFace == NULL)    {
//                        QDomComment aComment=   m_Doc.createComment("FatalError: McCadGDMLWriter::Cannot get the surface of number!");
//                        ParentElement.appendChild(aComment);
////                        cout<<"FatalError: McCadGDMLWriter::Cannot get the surface of number "<<iFaceNum<<" !! ";
//                        return;
//                    }
//                    PrintASurface(theElement, qFace, theSense);  //print the surface
//                }
//            }
//        }
//    }

//}

IGeomFace *   McCadGDMLWriter::findSurface(const int & aIdx)
{
    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);
//    for (unsigned int i=0; i<pData->m_GeomSurfList.size(); i++)
//    {
//        IGeomFace * aSurf = pData->m_GeomSurfList.at(i);
//        if (aSurf->GetSurfNum() == aIdx)
//            return aSurf;
//    }
//    return NULL;
    return pData->m_GeomSurfList.at(aIdx);
}


void    McCadGDMLWriter::PrintASurface(QDomElement & ParentElement, IGeomFace * pFace, const int & theSense)
{
    QString aSymb = pFace->GetSurfSymb().ToCString();

    if (aSymb == "P") {
        QDomElement theElement = m_Doc.createElement(("Plane"));
        theElement.setAttribute("Sense", theSense);
        theElement.setAttribute("A",RoundDouble(pFace->GetPrmtList().at(0)));
        theElement.setAttribute("B",RoundDouble(pFace->GetPrmtList().at(1)));
        theElement.setAttribute("C",RoundDouble(pFace->GetPrmtList().at(2)));
        theElement.setAttribute("D",RoundDouble(pFace->GetPrmtList().at(3)));
        ParentElement.appendChild(theElement);
    }
    else if (aSymb == "PX") {
        QDomElement theElement = m_Doc.createElement(("Plane"));
        theElement.setAttribute("Sense", theSense);
        theElement.setAttribute("Axis","X");
        theElement.setAttribute("D",RoundDouble(pFace->GetPrmtList().at(0)));
        ParentElement.appendChild(theElement);
    }
    else if (aSymb == "PY") {
        QDomElement theElement = m_Doc.createElement(("Plane"));
        theElement.setAttribute("Sense", theSense);
        theElement.setAttribute("Axis","Y");
        theElement.setAttribute("D",RoundDouble(pFace->GetPrmtList().at(0)));
        ParentElement.appendChild(theElement);
    }
    else if (aSymb == "PZ") {
        QDomElement theElement = m_Doc.createElement(("Plane"));
        theElement.setAttribute("Sense", theSense);
        theElement.setAttribute("Axis","Z");
        theElement.setAttribute("D",RoundDouble(pFace->GetPrmtList().at(0)));
        ParentElement.appendChild(theElement);
    }
    else if (aSymb == "SO") {
        QDomElement theElement = m_Doc.createElement(("Sphere"));
        theElement.setAttribute("Sense", theSense);
        theElement.setAttribute("X",0.);
        theElement.setAttribute("Y",0.);
        theElement.setAttribute("Z",0.);
        theElement.setAttribute("Radius",RoundDouble(pFace->GetPrmtList().at(0)));
        ParentElement.appendChild(theElement);
    }
    else if (aSymb == "SX") {
        QDomElement theElement = m_Doc.createElement(("Sphere"));
        theElement.setAttribute("Sense", theSense);
        theElement.setAttribute("X",RoundDouble(pFace->GetPrmtList().at(0)));
        theElement.setAttribute("Y",0.);
        theElement.setAttribute("Z",0.);
        theElement.setAttribute("Radius",RoundDouble(pFace->GetPrmtList().at(1)));
        ParentElement.appendChild(theElement);
    }
    else if (aSymb == "SY") {
        QDomElement theElement = m_Doc.createElement(("Sphere"));
        theElement.setAttribute("Sense", theSense);
        theElement.setAttribute("X",0.);
        theElement.setAttribute("Y",RoundDouble(pFace->GetPrmtList().at(0)));
        theElement.setAttribute("Z",0.);
        theElement.setAttribute("Radius",RoundDouble(pFace->GetPrmtList().at(1)));
        ParentElement.appendChild(theElement);
    }
    else if (aSymb == "SZ") {
        QDomElement theElement = m_Doc.createElement(("Sphere"));
        theElement.setAttribute("Sense", theSense);
        theElement.setAttribute("X",0.);
        theElement.setAttribute("Y",0.);
        theElement.setAttribute("Z",RoundDouble(pFace->GetPrmtList().at(0)));
        theElement.setAttribute("Radius",RoundDouble(pFace->GetPrmtList().at(1)));
        ParentElement.appendChild(theElement);
    }
    else if (aSymb == "S") {
        QDomElement theElement = m_Doc.createElement(("Sphere"));
        theElement.setAttribute("Sense", theSense);
        theElement.setAttribute("X",RoundDouble(pFace->GetPrmtList().at(0)));
        theElement.setAttribute("Y",RoundDouble(pFace->GetPrmtList().at(1)));
        theElement.setAttribute("Z",RoundDouble(pFace->GetPrmtList().at(2)));
        theElement.setAttribute("Radius",RoundDouble(pFace->GetPrmtList().at(3)));
        ParentElement.appendChild(theElement);
    }
    else if (aSymb == "CX") {
        QDomElement theElement = m_Doc.createElement(("Cylinder"));
        theElement.setAttribute("Sense", theSense);
        theElement.setAttribute("Axis","X");
        theElement.setAttribute("Center1",0.);
        theElement.setAttribute("Center2",0.);
        theElement.setAttribute("Radius",RoundDouble(pFace->GetPrmtList().at(0)));
        ParentElement.appendChild(theElement);
    }
    else if (aSymb == "CY") {
        QDomElement theElement = m_Doc.createElement(("Cylinder"));
        theElement.setAttribute("Sense", theSense);
        theElement.setAttribute("Axis","Y");
        theElement.setAttribute("Center1",0.);
        theElement.setAttribute("Center2",0.);
        theElement.setAttribute("Radius",RoundDouble(pFace->GetPrmtList().at(0)));
        ParentElement.appendChild(theElement);
    }
    else if (aSymb == "CZ") {
        QDomElement theElement = m_Doc.createElement(("Cylinder"));
        theElement.setAttribute("Sense", theSense);
        theElement.setAttribute("Axis","Z");
        theElement.setAttribute("Center1",0.);
        theElement.setAttribute("Center2",0.);
        theElement.setAttribute("Radius",RoundDouble(pFace->GetPrmtList().at(0)));
        ParentElement.appendChild(theElement);
    }
    else if (aSymb == "C/X") {
        QDomElement theElement = m_Doc.createElement(("Cylinder"));
        theElement.setAttribute("Sense", theSense);
        theElement.setAttribute("Axis","X");
        theElement.setAttribute("Center1",RoundDouble(pFace->GetPrmtList().at(0)));
        theElement.setAttribute("Center2",RoundDouble(pFace->GetPrmtList().at(1)));
        theElement.setAttribute("Radius",RoundDouble(pFace->GetPrmtList().at(2)));
        ParentElement.appendChild(theElement);
    }
    else if (aSymb == "C/Y") {
        QDomElement theElement = m_Doc.createElement(("Cylinder"));
        theElement.setAttribute("Sense", theSense);
        theElement.setAttribute("Axis","Y");
        theElement.setAttribute("Center1",RoundDouble(pFace->GetPrmtList().at(0)));
        theElement.setAttribute("Center2",RoundDouble(pFace->GetPrmtList().at(1)));
        theElement.setAttribute("Radius",RoundDouble(pFace->GetPrmtList().at(2)));
        ParentElement.appendChild(theElement);
    }
    else if (aSymb == "C/Z") {
        QDomElement theElement = m_Doc.createElement(("Cylinder"));
        theElement.setAttribute("Sense", theSense);
        theElement.setAttribute("Axis","Z");
        theElement.setAttribute("Center1",RoundDouble(pFace->GetPrmtList().at(0)));
        theElement.setAttribute("Center2",RoundDouble(pFace->GetPrmtList().at(1)));
        theElement.setAttribute("Radius",RoundDouble(pFace->GetPrmtList().at(2)));
        ParentElement.appendChild(theElement);
    }
    else if (aSymb == "KX") {
        QDomElement theElement = m_Doc.createElement(("Cone"));
        theElement.setAttribute("Sense", theSense);
        theElement.setAttribute("Axis","X");
        theElement.setAttribute("X",RoundDouble(pFace->GetPrmtList().at(0)));
        theElement.setAttribute("Y",0.);
        theElement.setAttribute("Z",0.);
        theElement.setAttribute("T2",RoundDouble(pFace->GetPrmtList().at(1)));
        ParentElement.appendChild(theElement);
    }
    else if (aSymb == "KY") {
        QDomElement theElement = m_Doc.createElement(("Cone"));
        theElement.setAttribute("Sense", theSense);
        theElement.setAttribute("Axis","Y");
        theElement.setAttribute("X",0.);
        theElement.setAttribute("Y",RoundDouble(pFace->GetPrmtList().at(0)));
        theElement.setAttribute("Z",0.);
        theElement.setAttribute("T2",RoundDouble(pFace->GetPrmtList().at(1)));
        ParentElement.appendChild(theElement);
    }
    else if (aSymb == "KZ") {
        QDomElement theElement = m_Doc.createElement(("Cone"));
        theElement.setAttribute("Sense", theSense);
//        theElement.setAttribute("Axis","Y"); //BUG 2016-03-23!!
        theElement.setAttribute("Axis","Z");
        theElement.setAttribute("X",0.);
        theElement.setAttribute("Y",0.);
        theElement.setAttribute("Z",RoundDouble(pFace->GetPrmtList().at(0)));
        theElement.setAttribute("T2",RoundDouble(pFace->GetPrmtList().at(1)));
        ParentElement.appendChild(theElement);
    }
    else if (aSymb == "K/X") {
        QDomElement theElement = m_Doc.createElement(("Cone"));
        theElement.setAttribute("Sense", theSense);
        theElement.setAttribute("Axis","X");
        theElement.setAttribute("X",RoundDouble(pFace->GetPrmtList().at(0)));
        theElement.setAttribute("Y",RoundDouble(pFace->GetPrmtList().at(1)));
        theElement.setAttribute("Z",RoundDouble(pFace->GetPrmtList().at(2)));
        theElement.setAttribute("T2",RoundDouble(pFace->GetPrmtList().at(3)));
        ParentElement.appendChild(theElement);
    }
    else if (aSymb == "K/Y") {
        QDomElement theElement = m_Doc.createElement(("Cone"));
        theElement.setAttribute("Sense", theSense);
        theElement.setAttribute("Axis","Y");
        theElement.setAttribute("X",RoundDouble(pFace->GetPrmtList().at(0)));
        theElement.setAttribute("Y",RoundDouble(pFace->GetPrmtList().at(1)));
        theElement.setAttribute("Z",RoundDouble(pFace->GetPrmtList().at(2)));
        theElement.setAttribute("T2",RoundDouble(pFace->GetPrmtList().at(3)));
        ParentElement.appendChild(theElement);
    }
    else if (aSymb == "K/Z") {
        QDomElement theElement = m_Doc.createElement(("Cone"));
        theElement.setAttribute("Sense", theSense);
        theElement.setAttribute("Axis","Z");
        theElement.setAttribute("X",RoundDouble(pFace->GetPrmtList().at(0)));
        theElement.setAttribute("Y",RoundDouble(pFace->GetPrmtList().at(1)));
        theElement.setAttribute("Z",RoundDouble(pFace->GetPrmtList().at(2)));
        theElement.setAttribute("T2",RoundDouble(pFace->GetPrmtList().at(3)));
        ParentElement.appendChild(theElement);
    }
    else if (aSymb == "GQ") {
        QDomElement theElement = m_Doc.createElement(("Quadric"));
        theElement.setAttribute("Sense", theSense);
        theElement.setAttribute("A",RoundDouble(pFace->GetPrmtList().at(0)));
        //ATTENTION, some parameters should /2, difference between Geant4 half-space solid
        //and MCNP please check the development documents and MCNP manual!
        theElement.setAttribute("B",RoundDouble(pFace->GetPrmtList().at(3)/2.0));
        theElement.setAttribute("C",RoundDouble(pFace->GetPrmtList().at(5)/2.0));
        theElement.setAttribute("D",RoundDouble(pFace->GetPrmtList().at(6)/2.0));
        theElement.setAttribute("E",RoundDouble(pFace->GetPrmtList().at(1)));
        theElement.setAttribute("F",RoundDouble(pFace->GetPrmtList().at(4)/2.0));
        theElement.setAttribute("G",RoundDouble(pFace->GetPrmtList().at(7)/2.0));
        theElement.setAttribute("H",RoundDouble(pFace->GetPrmtList().at(2)));
        theElement.setAttribute("I",RoundDouble(pFace->GetPrmtList().at(8)/2.0));
        theElement.setAttribute("J",RoundDouble(pFace->GetPrmtList().at(9)));
        ParentElement.appendChild(theElement);
    }
    else if (aSymb == "TX") {
        QDomElement theElement = m_Doc.createElement(("Torus"));
        theElement.setAttribute("Sense", theSense);
        theElement.setAttribute("CenterX",RoundDouble(pFace->GetPrmtList().at(0)));
        theElement.setAttribute("CenterY",RoundDouble(pFace->GetPrmtList().at(1)));
        theElement.setAttribute("CenterZ",RoundDouble(pFace->GetPrmtList().at(2)));
        theElement.setAttribute("AxisX",1.);
        theElement.setAttribute("AxisY",0.);
        theElement.setAttribute("AxisZ",0.);
        theElement.setAttribute("MaxRadius",RoundDouble(pFace->GetPrmtList().at(3)));
        //ATTENTION!! the A,B,C in MCNP torus parameters means r, b, a in Geant4
        //Half-space solid, therefore the order should be switched!
//        theElement.setAttribute("MinRadius1",RoundDouble(pFace->GetPrmtList().at(4)));
//        theElement.setAttribute("MinRadius2",RoundDouble(pFace->GetPrmtList().at(5)));
        theElement.setAttribute("MinRadius1",RoundDouble(pFace->GetPrmtList().at(5)));
        theElement.setAttribute("MinRadius2",RoundDouble(pFace->GetPrmtList().at(4)));
        ParentElement.appendChild(theElement);

    }
    else if (aSymb == "TY") {
        QDomElement theElement = m_Doc.createElement(("Torus"));
        theElement.setAttribute("Sense", theSense);
        theElement.setAttribute("CenterX",RoundDouble(pFace->GetPrmtList().at(0)));
        theElement.setAttribute("CenterY",RoundDouble(pFace->GetPrmtList().at(1)));
        theElement.setAttribute("CenterZ",RoundDouble(pFace->GetPrmtList().at(2)));
        theElement.setAttribute("AxisX",0.);
        theElement.setAttribute("AxisY",1.);
        theElement.setAttribute("AxisZ",0.);
        theElement.setAttribute("MaxRadius",RoundDouble(pFace->GetPrmtList().at(3)));
        //ATTENTION!! the A,B,C in MCNP torus parameters means r, b, a in Geant4
        //Half-space solid, therefore the order should be switched!
//        theElement.setAttribute("MinRadius1",RoundDouble(pFace->GetPrmtList().at(4)));
//        theElement.setAttribute("MinRadius2",RoundDouble(pFace->GetPrmtList().at(5));
        theElement.setAttribute("MinRadius1",RoundDouble(pFace->GetPrmtList().at(5)));
        theElement.setAttribute("MinRadius2",RoundDouble(pFace->GetPrmtList().at(4)));
        ParentElement.appendChild(theElement);
    }
    else if (aSymb == "TZ") {
        QDomElement theElement = m_Doc.createElement(("Torus"));
        theElement.setAttribute("Sense", theSense);
        theElement.setAttribute("CenterX",RoundDouble(pFace->GetPrmtList().at(0)));
        theElement.setAttribute("CenterY",RoundDouble(pFace->GetPrmtList().at(1)));
        theElement.setAttribute("CenterZ",RoundDouble(pFace->GetPrmtList().at(2)));
        theElement.setAttribute("AxisX",0.);
        theElement.setAttribute("AxisY",0.);
        theElement.setAttribute("AxisZ",1.);
        theElement.setAttribute("MaxRadius",RoundDouble(pFace->GetPrmtList().at(3)));
        //ATTENTION!! the A,B,C in MCNP torus parameters means r, b, a in Geant4
        //Half-space solid, therefore the order should be switched!
//        theElement.setAttribute("MinRadius1",RoundDouble(pFace->GetPrmtList().at(4)));
//        theElement.setAttribute("MinRadius2",RoundDouble(pFace->GetPrmtList().at(5)));
        theElement.setAttribute("MinRadius1",RoundDouble(pFace->GetPrmtList().at(5)));
        theElement.setAttribute("MinRadius2",RoundDouble(pFace->GetPrmtList().at(4)));
        ParentElement.appendChild(theElement);
    }
}

void    McCadGDMLWriter::PrintBoundaryBox(QDomElement & ParentElement,  TopoDS_Solid *pConvexSolid)
{
    //create the elemenet
    QDomElement theElement = m_Doc.createElement(("BoundaryBox"));

    //calculate the boundary box
    Bnd_Box theBB;
    BRepBndLib::Add(*pConvexSolid, theBB);
//    theBB.SetGap(0.0);  //better to set a small gap
//    theBB.SetGap(0.001); //!! this does not work very well!!
    Standard_Real Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
    theBB.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
    Standard_Real MarginX = (Xmax - Xmin)/1000;
    Standard_Real MarginY = (Ymax - Ymin)/1000;
    Standard_Real MarginZ = (Zmax - Zmin)/1000;

    QDomElement LowerPoint = m_Doc.createElement(("LowerPoint"));
    QDomElement UpperPoint = m_Doc.createElement(("UpperPoint"));
    //2016-03-29 here we increase an obsolute margin of 0.1 cm
    // in order to prevent bounding "outside" checking failure for
    //tiny geometries which are far from the global origin
    LowerPoint.setAttribute("x", (Xmin-MarginX)/10 -0.1); //unit convert mm->cm
    LowerPoint.setAttribute("y", (Ymin-MarginY)/10 -0.1);
    LowerPoint.setAttribute("z", (Zmin-MarginZ)/10 -0.1);
    UpperPoint.setAttribute("x", (Xmax+MarginX)/10 +0.1);
    UpperPoint.setAttribute("y", (Ymax+MarginY)/10 +0.1);
    UpperPoint.setAttribute("z", (Zmax+MarginZ)/10 +0.1);

    theElement.appendChild(LowerPoint);
    theElement.appendChild(UpperPoint);
    ParentElement.appendChild(theElement);
}

void    McCadGDMLWriter::PrintVolumeArea (QDomElement & ParentElement, TopoDS_Solid *pConvexSolid, const QString & aName)
{
  //create the elements
    QDomElement VolumeElement = m_Doc.createElement(("VolumeSize"));
    QDomElement SurfaceAreaElement = m_Doc.createElement(("SurfaceArea"));
    ParentElement.appendChild(VolumeElement);
    ParentElement.appendChild(SurfaceAreaElement);

    //calculate
    //from http://www.opencascade.org/org/forum/thread_17898/?forum=3
    TopoDS_Shape aShape = *pConvexSolid;
    double Volume =0., Area =0.;
    GProp_GProps System;
    BRepGProp::SurfaceProperties(aShape, System);
    Area = System.Mass()/100;  //correct?? //unit cm2
    BRepGProp::VolumeProperties(aShape, System);
    Volume = System.Mass()/1000;//correct?? //unit cm3

    VolumeElement.setAttribute("value", Volume);
    SurfaceAreaElement.setAttribute("value",Area);



}
void    McCadGDMLWriter::PrintPolyhedronRef (QDomElement & ParentElement, const QString & aName)
{
  //create the elements

    QDomElement PolyhedronElement = m_Doc.createElement(("VisualPolyhedron"));
    PolyhedronElement.setAttribute("ref", QString("Poly_") + aName); //add "Poly_" to avoid repeated name in GDML, see also PrintPolyhedron()
    ParentElement.appendChild(PolyhedronElement);

}

McCadVoidCell * McCadGDMLWriter::getWholeBntBox()
{
    Bnd_Box bnd_box;
    McCadGeomData * pData = m_pManager->GetGeomData();

    for (unsigned int i = 0; i < pData->m_ConvexSolidList.size(); i++)
    {
        McCadConvexSolid  *pSolid = pData->m_ConvexSolidList[i];
        assert(pSolid);
        BRepBndLib::Add((TopoDS_Shape)(*pSolid),bnd_box); // Add each convex solid into bundary box
    }

    bnd_box.SetGap(1.0); // Set the gap between the boundary box and material solids
    Standard_Real dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
    bnd_box.Get(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);

    // Interger the dimension parameters of boundary box.
//no need    McCadMathTool::Integer(dXmin);
//no need    McCadMathTool::Integer(dYmin);
//no need    McCadMathTool::Integer(dZmin);
//no need    McCadMathTool::Integer(dXmax);
//no need    McCadMathTool::Integer(dYmax);
//no need    McCadMathTool::Integer(dZmax);

    gp_Pnt bnd_max_pnt(dXmax,dYmax,dZmax);
    gp_Pnt bnd_min_pnt(dXmin,dYmin,dZmin);

    // Create the boundary box.
    McCadVoidCell *pVoid = new McCadVoidCell(BRepPrimAPI_MakeBox(bnd_min_pnt,bnd_max_pnt).Solid());
    pVoid->SetBntBox(dXmin,dYmin,dZmin,dXmax,dYmax,dZmax);
    return pVoid;
}


void    McCadGDMLWriter::PrintHalfSpaceBoolean(QDomElement & ParentElement,
                                               const QString SolidName, const int &SubSolidCnt, const bool isPrintPoly)
{
    if (SubSolidCnt == 1) return ;//no need to make a boolean on a convex solid

    //create the element
    QDomElement theElement = m_Doc.createElement(("HalfSpaceBoolean"));
    ParentElement.appendChild(theElement);
    theElement.setAttribute("name", SolidName);
    QDomElement theUnionElement = m_Doc.createElement(("Union"));

    QString aName = SolidName ; //get the solid name
    for(int j = 0; j < SubSolidCnt; j++)
    {
        //name for the convex solid, if only one convex solid, using the aName without surfix
        QString bName = aName + QString("_%1").arg(j);
        QDomElement solidElement = m_Doc.createElement(("Solid"));
        solidElement.setAttribute("ref", bName);
        theUnionElement.appendChild(solidElement);
    }
    theElement.appendChild(theUnionElement);

    //Print polyhedron reference
    if (isPrintPoly)
        PrintPolyhedronRef(theElement, aName);

}

void   McCadGDMLWriter:: PrintStructure(QDomElement & ParentElement)
{
    QDomElement theElement = m_Doc.createElement(("structure"));
    ParentElement.appendChild(theElement);

    //get the data
    McCadGeomData *pData = m_pManager->GetGeomData();
    assert(pData);

    MaterialManager * pMatManager = m_pManager->GetMatManager();
    assert(pMatManager);



    //loop the solid list
    for (unsigned int i = 0; i < pData->m_SolidList.size(); i++)
    {
        McCadSolid * pSolid = pData->m_SolidList.at(i);
        assert(pSolid);
        QString aSolidName (pSolid->GetName().ToCString()) ; //get the solid name

        //get the material name
        QString aMatName = pMatManager->GetMaterial(pSolid->GetID())->GetMatName(); //use Material id to get material
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
}

void    McCadGDMLWriter::PrintVolume(QDomElement & ParentElement, const QString MaterialName, const QString SolidName)
{
    QDomElement theElement = m_Doc.createElement(("volume"));
    ParentElement.appendChild(theElement);
    //we add a "LV_" here because we want to make it differ from solidname
    theElement.setAttribute("name", QString("LV_") + SolidName);

    QDomElement MaterialRefElement = m_Doc.createElement(("materialref"));
    MaterialRefElement.setAttribute("ref", QString("Mat_") + MaterialName); // add "Mat_" to avoid repeat name in GDML

    QDomElement SolidRefElement = m_Doc.createElement("solidref");
    SolidRefElement.setAttribute("ref",SolidName);

    theElement.appendChild(MaterialRefElement);
    theElement.appendChild(SolidRefElement);
}

void   McCadGDMLWriter:: PrintPhysicalVolume(QDomElement & ParentElement, const QString SolidName)
{
    QDomElement theElement = m_Doc.createElement(("physvol"));
    ParentElement.appendChild(theElement);
    QDomElement VolumeRefElement = m_Doc.createElement("volumeref");
    //see PrintVolume() for the volume name
    VolumeRefElement.setAttribute("ref",QString("LV_") + SolidName);

    theElement.appendChild(VolumeRefElement);
}

void   McCadGDMLWriter::  PrintSetup (QDomElement & ParentElement)
{
    QDomElement theElement = m_Doc.createElement(("setup"));
    ParentElement.appendChild(theElement);
    theElement.setAttribute("name", "Default"); //what so ever
    theElement.setAttribute("version", "1.0"); //just copy the example

    QDomElement WorldElement = m_Doc.createElement(("world"));
    WorldElement.setAttribute("ref", "World");

    theElement.appendChild(WorldElement);

}

/*!
 * \brief McCadGDMLWriter::RoundDouble
 *  round a double, truncate the digit
 * \param aDouble
 * \return the qstring of the digit
 */
QString   McCadGDMLWriter::RoundDouble(const double & aDouble)
{
    //round the digit to 7
    return QString::number(aDouble, 'f', 7);
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
Standard_Boolean FindRepeatCell(int iFaceNum, vector<Standard_Integer> & list)
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
void      McCadGDMLWriter::checkRepeatFaces(McCadConvexSolid *& pSolid,
                                            vector<Standard_Integer> & IntSurfList, vector<Standard_Integer> UniSurfList)
{

    //copy from McCadMcnpWriter::GetCellExpn
    /// Generate the surface number list and remove the repeated surfaces
    for (unsigned int i = 0; i < (pSolid->GetFaces()).size(); i++)
    {
        McCadExtBndFace * pExtFace = (pSolid->GetFaces()).at(i);
        Standard_Integer iFaceNum = pExtFace->GetFaceNum();

        if(!FindRepeatCell(iFaceNum, IntSurfList))
        {
            IntSurfList.push_back(iFaceNum);
        }

        if (pExtFace->HaveAstSurf())
        {
            for(unsigned int j = 0; j < pExtFace->GetAstFaces().size(); j++)
            {
                McCadExtAstFace *pAstFace = pExtFace->GetAstFaces().at(j);
                Standard_Integer iAstFaceNum = pAstFace->GetFaceNum();

                if (pAstFace->IsSplitFace())
                {
                    if(!FindRepeatCell(iAstFaceNum,UniSurfList))
                    {
                        UniSurfList.push_back(iAstFaceNum);
                    }
                }
                else
                {
                    if(!FindRepeatCell(iAstFaceNum,IntSurfList))
                    {
                        IntSurfList.push_back(iAstFaceNum);
                    }
                }
            }
            if(UniSurfList.size() == 1)
            {
                cout<<"WARNING!!: found a single Union Asistant surface!!"<<endl;
//                IntSurfList.push_back(UniSurfList.at(0));
            }
        }
    }

}



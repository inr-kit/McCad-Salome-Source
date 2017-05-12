#include "McCadDecompose.hxx"
#include <assert.h>

#include <TopTools_HSequenceOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopOpeBRepTool_PurgeInternalEdges.hxx>

#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <BRepLib_FuseEdges.hxx>
#include <BRepTools_ReShape.hxx>

#include <ShapeFix_Solid.hxx>

#include "McCadRepair.hxx"

#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>

#include "../McCadTool/McCadMathTool.hxx"

McCadDecompose::McCadDecompose()
{
    m_pGeoData = new McCadDcompGeomData();
}

McCadDecompose::~McCadDecompose()
{
    delete m_pGeoData;
    m_pGeoData = NULL;
}


/** ***************************************************************************
* @brief  Perform the decomposition and saving work
* @param
* @return void
*
* @date 06/04/2016
* @modify 06/04/2016
* @author  Lei Lu
******************************************************************************/
void  McCadDecompose::Perform()
{
    Decompose();
    SaveDecomposedSolids(m_fileName);
}



/** ***************************************************************************
* @brief  Load the models, if the model is compound solid, separate it firstly
* @param  Handle_TopTools_HSequenceOfShape & solid_list
* @return void
*
* @date 13/05/2015
* @modify 13/05/2015
* @author  Lei Lu
******************************************************************************/
void McCadDecompose::InputData(Handle_TopTools_HSequenceOfShape & solid_list,
                               TCollection_AsciiString theFileName)
{
    SetFileName(theFileName);

    cout<<"*******************************************************************"<<endl;
    cout<<"****          The 1st step: Load the input models              ****"<<endl<<endl;
    cout<<"*******************************************************************"<<endl;
    cout<<endl;

    m_pGeoData->InputSolids(solid_list);

    cout<<"-- There are "<<solid_list->Length()<<" solids are loaded."<<endl<<endl;
}





/** ***************************************************************************
* @brief  Decompose the input solids
* @param
* @return Void
*
* @date 13/05/2015
* @modify 13/10/2015
* @author  Lei Lu
******************************************************************************/
void McCadDecompose::Decompose()
{
    //STEPControl_Writer wrt;
    cout<<"*******************************************************************"<<endl;
    cout<<"****          The 2ed step: Perform the decomposing            ****"<<endl;
    cout<<endl;
    cout<<"*******************************************************************"<<endl;
    cout<<endl;

    Standard_Integer iSolidNum = 0 ;

    Handle_TopTools_HSequenceOfShape InputSolidList = m_pGeoData->GetInputSolidList();    

    for(int i = 1; i <= InputSolidList->Length(); i ++)
    {
        iSolidNum++;
        cout<<"-- Decomposing the "<<iSolidNum<<" solid"<<endl;
        Standard_Integer iLevel = 0;

        TopoDS_Shape theShape = InputSolidList->Value(i);

        /** Repair the geometry of solid */
        TopoDS_Shape newShape = McCadRepair::RemoveSmallFaces(theShape); // Remove the small faces in solid
        TopoDS_Solid tmpSolid = TopoDS::Solid(newShape);
        TopoDS_Solid newSolid = McCadRepair::RepairSolid(tmpSolid);      // Repair the solid

        // Set the deflection for solid meshing
        Standard_Real deflection = CalMeshDeflection(newSolid);

//      TopOpeBRepTool_PurgeInternalEdges fuseEdge(newSolid,true);
//      fuseEdge.Perform();
//      TopoDS_Solid solid = TopoDS::Solid(fuseEdge.Shape());

        Handle(ShapeFix_Solid) genericFix = new ShapeFix_Solid;

        genericFix->Init(newSolid);
        genericFix->Perform();
        TopoDS_Solid theSolid =TopoDS::Solid(genericFix->Solid());

        //Handle_TopTools_HSequenceOfShape resultSolidList = new TopTools_HSequenceOfShape();

        McCadDcompSolid *pMcCadSolid = new McCadDcompSolid(theSolid);

        pMcCadSolid->SetDeflection(deflection);                 // Set the deflection

        vector<McCadDcompSolid* > *pOutputSolidList = new vector<McCadDcompSolid*>();// The output solid list */
        vector<McCadDcompSolid* > *pErrorSolidList  = new vector<McCadDcompSolid*>();// The output solid list */
        if (pMcCadSolid->Decompose(pOutputSolidList,pErrorSolidList,iLevel,1))        // Decompose solid
        {
            delete pMcCadSolid;
            pMcCadSolid = NULL;
        }

        m_pGeoData->AddDecompSolid(pOutputSolidList);   // Combine the solids and create a compund solid
        m_pGeoData->AddErrorSolid(pErrorSolidList);     // Save the error solids

        DeleteList(pOutputSolidList);   // Delete the output list and the solids inside
        DeleteList(pErrorSolidList);    // Delete the error list and solids inside
    }
}




/** ***************************************************************************
* @brief  Delete the solid list and remove the solids inside
* @param  vector<McCadDcompSolid*> *& pSolidList
* @return void
*
* @date 18/07/2016
* @modify 18/07/2016
* @author  Lei Lu
******************************************************************************/
void McCadDecompose::DeleteList(vector<McCadDcompSolid*> *& pSolidList)
{
    /// Delete the decomposed solids
    for(unsigned int i = 0; i < pSolidList->size(); i++)
    {
        McCadDcompSolid *pSolid = pSolidList->at(i);
        pSolidList->erase(pSolidList->begin()+i);

        delete pSolid;
        pSolid = NULL;
    }
    pSolidList->clear();
    delete pSolidList;
    pSolidList = NULL;
}




/** ***************************************************************************
* @brief  Save the decomposed solids
* @param  TCollection_AsciiString theFileName
* @return void
*
* @date 16/05/2016
* @modify 16/05/2016
* @author  Lei Lu
******************************************************************************/
void McCadDecompose::SaveDecomposedSolids(TCollection_AsciiString theFileName)
{
    cout<<"*******************************************************************"<<endl;
    cout<<"****          The 3rd step: Save the decomposed result        *****"<<endl;
    cout<<endl;
    cout<<"*******************************************************************"<<endl;
    cout<<endl;

    m_pGeoData->SaveSolids(m_fileName,m_iCombSymb);

    cout<<endl<<endl;
}




/** ***************************************************************************
* @brief  Calculate the deflection of surface meshing
* @param
* @return Standard_Real
*
* @date 06/04/2016
* @modify 06/04/2016
* @author  Lei Lu
******************************************************************************/
Standard_Real McCadDecompose::CalMeshDeflection(TopoDS_Solid &theSolid)
{
    /** Calculate the boundary box of face **/
    Bnd_Box theBB;
    BRepBndLib::Add(theSolid,theBB);
    theBB.SetGap(0.0);
    Standard_Real Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
    theBB.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);

    /** Generate the meshes of face **/
    Standard_Real deflection // The tolerance setting of the mesh
             = McCadMathTool::MaxValue((Xmax-Xmin),(Ymax-Ymin),(Zmax-Zmin))/100.0;  // Set deflection value

    return deflection;
}




/** ***************************************************************************
* @brief  Set the file name
* @param
* @return void
*
* @date 06/04/2016
* @modify 06/04/2016
* @author  Lei Lu
******************************************************************************/
void McCadDecompose::SetFileName( TCollection_AsciiString theFileName)
{
    m_fileName = theFileName;
}



/** ***************************************************************************
* @brief  Set the symbol of combination, the decomposed solid will be combined
*         into a independent solid or seperated solids
* @param
* @return void
*
* @date 06/04/2016
* @modify 06/04/2016
* @author  Lei Lu
******************************************************************************/
void McCadDecompose::SetCombine(Standard_Integer iCombSymb)
{
    m_iCombSymb = iCombSymb;
}

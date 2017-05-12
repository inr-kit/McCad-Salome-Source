#include "McCadDcompGeomData.hxx"
#include <TopTools_HSequenceOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#include <BRep_Builder.hxx>

#include "McCadDcompSolid.hxx"

#include "../McCadTool/McCadGeneTool.hxx"

McCadDcompGeomData::McCadDcompGeomData()
{
    m_OutCompSolidList = new TopTools_HSequenceOfShape;
    m_ErrCompSolidList = new TopTools_HSequenceOfShape;
}


McCadDcompGeomData::~McCadDcompGeomData()
{
    cout<<"Clear all data"<<endl;

    for(unsigned int i = 1; i <= m_InputSolidList->Length(); i++)
    {
        TopoDS_Shape tmpSol = m_InputSolidList->Value(i);
        tmpSol.Free();
    }
    m_InputSolidList->Clear();

    for(unsigned int i = 1; i <= m_OutCompSolidList->Length(); i++)
    {
        TopExp_Explorer ex;
        for (ex.Init(m_OutCompSolidList->Value(i), TopAbs_SOLID); ex.More(); ex.Next())
        {
            TopoDS_Solid tmpSol = TopoDS::Solid(ex.Current());
            tmpSol.Free();
        }
    }
    m_OutCompSolidList->Clear();

    for(unsigned int i = 1; i <= m_ErrCompSolidList->Length(); i++)
    {
        TopExp_Explorer ex;
        for (ex.Init(m_ErrCompSolidList->Value(i), TopAbs_SOLID); ex.More(); ex.Next())
        {
            TopoDS_Solid tmpSol = TopoDS::Solid(ex.Current());
            tmpSol.Free();
        }
    }
    m_ErrCompSolidList->Clear();
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
void McCadDcompGeomData::InputSolids( Handle_TopTools_HSequenceOfShape & solid_list )
{   
    m_InputSolidList = new TopTools_HSequenceOfShape;

    for (Standard_Integer i=1; i<= solid_list->Length(); i++)
    {        
        if ((solid_list->Value(i)).ShapeType() == TopAbs_COMPSOLID
                || (solid_list->Value(i)).ShapeType() == TopAbs_COMPOUND)
        {
            TopExp_Explorer ex;
            for (ex.Init(solid_list->Value(i), TopAbs_SOLID); ex.More(); ex.Next())
            {
                TopoDS_Solid tmpSol = TopoDS::Solid(ex.Current());               
                m_InputSolidList->Append(tmpSol);
            }
        }
        else if ((solid_list->Value(i)).ShapeType() == TopAbs_SOLID)
        {            
            m_InputSolidList->Append(solid_list->Value(i));
        }
    }   
    cout<<endl<<endl;
}




/** ***************************************************************************
* @brief  Add the error solids can not be decomposed into the list
* @param  McCadDcompSolid *& theSolid
* @return void
*
* @date   18/07/2016
* @modify 18/07/2016
* @author Lei Lu
******************************************************************************/
void McCadDcompGeomData::AddErrorSolid(vector<McCadDcompSolid *> *& pSolidList)
{
    if(pSolidList->empty())
    {
        return;
    }

    TopoDS_CompSolid comp_solid;
    BRep_Builder comp_builder;
    comp_builder.MakeCompSolid(comp_solid);

    for(unsigned int i = 0; i < pSolidList->size(); i++)
    {
        McCadDcompSolid *pSolid = pSolidList->at(i);
        comp_builder.Add(comp_solid, *pSolid);
    }

    /// Add into geometry data the compund solid list
    m_ErrCompSolidList->Append(comp_solid);
}




/** ***************************************************************************
* @brief  Add the compund solid
* @param  TopoDS_Compound &CompSolid
* @return void
*
* @date 01/06/2015
* @modify
* @author  Lei Lu
******************************************************************************/
void McCadDcompGeomData::AddDecompSolid(vector<McCadDcompSolid *> *& pSolidList)
{
    if(pSolidList->empty())
    {
        return;
    }

    //TopoDS_CompSolid comp_solid;
    TopoDS_Compound comp_solid;
    BRep_Builder comp_builder;
    comp_builder.MakeCompound(comp_solid);

    for(unsigned int i = 0; i < pSolidList->size(); i++)
    {
        McCadDcompSolid *pSolid = pSolidList->at(i);
        comp_builder.Add(comp_solid, *pSolid);
    }

    /// Add into geometry data the compund solid list
    m_OutCompSolidList->Append(comp_solid);
}




/** ***************************************************************************
* @brief  Get the stroed input solid for decomposition
* @param  Handle_TopTools_HSequenceOfShape
* @return void
*
* @date 01/06/2015
* @modify
* @author  Lei Lu
******************************************************************************/
Handle_TopTools_HSequenceOfShape McCadDcompGeomData::GetInputSolidList() const
{
    return m_InputSolidList;
}




/** ***************************************************************************
* @brief  Combine solids as a compound solid for saving
* @param
* @return void
*
* @date 01/06/2015
* @modify
* @author  Lei Lu
******************************************************************************/
Handle_TopTools_HSequenceOfShape McCadDcompGeomData::CombineSolids()
{
    /**< The output compound solid */
    Handle_TopTools_HSequenceOfShape OutCombSolid = new TopTools_HSequenceOfShape;

    TopoDS_Compound compounds;
    BRep_Builder comp_builder;
    comp_builder.MakeCompound(compounds);


    for(Standard_Integer i = 1; i <= m_OutCompSolidList->Length(); i++)
    {
        TopoDS_Shape shape = m_OutCompSolidList->Value(i);
        comp_builder.Add(compounds, shape);
    }

    OutCombSolid->Append(compounds);
    return OutCombSolid;
}




/** ***************************************************************************
* @brief  Save the combined solids and error solids
* @param
* @return void
*
* @date 01/06/2015
* @modify
* @author  Lei Lu
******************************************************************************/
void McCadDcompGeomData::SaveSolids(TCollection_AsciiString theFileName, Standard_Integer iCombine)
{
    cout<<"-- Saving the decomposed model."<<endl;

    TCollection_AsciiString fileExtension;
    TCollection_AsciiString tempName = theFileName;
    fileExtension = tempName.Split(tempName.SearchFromEnd(".")-1);

    TCollection_AsciiString outName = tempName + "_cut" + fileExtension;
    TCollection_AsciiString errName = tempName + "_err" + fileExtension;

//    TCollection_AsciiString vorName = "Decomposed_";
//    TCollection_AsciiString outName = vorName + theFileName;
    Standard_CString output_filename = outName.ToCString();

    if(iCombine == 1)
    {        
        McCadGeneTool::WriteFile(output_filename, m_OutCompSolidList);
    }
    else if (iCombine == 0)
    {        
        Handle_TopTools_HSequenceOfShape OutSolidList = new TopTools_HSequenceOfShape;
        for(Standard_Integer i = 1; i <= m_OutCompSolidList->Length(); i++)
        {
            TopExp_Explorer ex;
            for (ex.Init(m_OutCompSolidList->Value(i), TopAbs_SOLID); ex.More(); ex.Next())
            {
                TopoDS_Solid tmpSol = TopoDS::Solid(ex.Current());
                OutSolidList->Append(tmpSol);
            }
        }
        McCadGeneTool::WriteFile(output_filename, OutSolidList);
        OutSolidList->Clear();
    }
    else if (iCombine == 2)
    {
        Handle_TopTools_HSequenceOfShape CompSolid =  CombineSolids();
        McCadGeneTool::WriteFile(output_filename, CompSolid);
        for(unsigned int i = 1; i <= CompSolid->Length(); i++)
        {
            TopoDS_Shape shape = CompSolid->Value(i);
            shape.Free();
        }
        CompSolid->Clear();
    }

    cout<<endl<<endl;
    cout<<"-- Saving the error solids."<<endl;

//    TCollection_AsciiString vorNameErr = "Error_";
//    TCollection_AsciiString errName = vorNameErr + theFileName;
    Standard_CString error_filename = errName.ToCString();

    Handle_TopTools_HSequenceOfShape OutErrSolid = new TopTools_HSequenceOfShape;

    for(Standard_Integer i = 1; i <= m_ErrCompSolidList->Length(); i++)
    {
        TopoDS_Shape shape = m_ErrCompSolidList->Value(i);
        TopExp_Explorer ex;
        for (ex.Init(shape, TopAbs_SOLID); ex.More(); ex.Next())
        {
            TopoDS_Solid tmpSol = TopoDS::Solid(ex.Current());
            OutErrSolid->Append(tmpSol);
        }
    }

    McCadGeneTool::WriteFile(error_filename, OutErrSolid);
    cout<<endl<<endl;
}






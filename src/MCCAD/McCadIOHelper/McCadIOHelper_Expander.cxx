#include <McCadIOHelper_Expander.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Solid.hxx>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

using namespace std;


//
//	CTORS
//
McCadIOHelper_Expander::McCadIOHelper_Expander(){
    m_explodedShapes = new TopTools_HSequenceOfShape;
}


McCadIOHelper_Expander::McCadIOHelper_Expander(const Handle_TopTools_HSequenceOfShape& theHSeqOfShp){
    m_shapesToExplode = theHSeqOfShp;
    m_explodedShapes  = Explode();
}

Handle(TopTools_HSequenceOfShape) McCadIOHelper_Expander::Explode(const Handle_TopTools_HSequenceOfShape& aHSeqOfShp){
    m_shapesToExplode = aHSeqOfShp;
    return Explode();
}

Handle_TopTools_HSequenceOfShape McCadIOHelper_Expander::GetExplodedShapes(){
    return m_explodedShapes;
}

Handle_TopTools_HSequenceOfShape McCadIOHelper_Expander::Explode()
{
    int cnt(0);

    m_explodedShapes = new TopTools_HSequenceOfShape;

    TopExp_Explorer ex;
    for (Standard_Integer i=1; i<= m_shapesToExplode->Length(); i++) {
        for (ex.Init(m_shapesToExplode->Value(i), TopAbs_SOLID); ex.More(); ex.Next()){
            cnt++;
            TopoDS_Solid tmpSol = TopoDS::Solid(ex.Current());
            m_explodedShapes->Append(tmpSol);
        }
    }

    return m_explodedShapes;
}


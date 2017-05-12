#ifndef MCCADDCOMPGEOMDATA_HXX
#define MCCADDCOMPGEOMDATA_HXX

#include <Handle_TopTools_HSequenceOfShape.hxx>

#include <vector>
#include <TopoDS_Solid.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Compound.hxx>

class McCadDecompose;
class McCadDcompSolid;

using namespace std;

class McCadDcompGeomData
{
public:

    void* operator new(size_t,void* anAddress)
    {
        return anAddress;
    }
    void* operator new(size_t size)
    {
        return Standard::Allocate(size);
    }
    void  operator delete(void *anAddress)
    {
        if (anAddress) Standard::Free((Standard_Address&)anAddress);
    }

public:

    McCadDcompGeomData();
    ~McCadDcompGeomData();

    void InputSolids( Handle_TopTools_HSequenceOfShape & solid_list );  /**< Input and store the solids */
    Handle_TopTools_HSequenceOfShape GetInputSolidList() const;         /**< Get the input solid list */

    void AddErrorSolid(vector<McCadDcompSolid*> *& pSolidList);     /**< Add the error solid */
    void AddDecompSolid(vector<McCadDcompSolid*> *& pSolidList);    /**< Add the compound solid */

    void SaveSolids(TCollection_AsciiString theFileName, Standard_Integer iCombine);      /**< Save the decomposed solids and error solids */
    //qiu add method to get solids
    Handle_TopTools_HSequenceOfShape getOutCompSolids () {return m_OutCompSolidList;} /**< Get the output compound solid */
    Handle_TopTools_HSequenceOfShape getErrCompSolids () {return m_ErrCompSolidList;} /**< Get the error compound solid */

private:

    Handle_TopTools_HSequenceOfShape m_InputSolidList;      /**< loaded solid list */
    Handle_TopTools_HSequenceOfShape m_OutCompSolidList;    /**< The output compound solid */
    Handle_TopTools_HSequenceOfShape m_ErrCompSolidList;    /**< The output compound solid */

private:
    Handle_TopTools_HSequenceOfShape CombineSolids();       /**< Combine the solid into a independent solid */

};


#endif // MCCADDECOMPGEOMDATA_HXX

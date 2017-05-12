#ifndef MCCADDECOMPOSE_HXX
#define MCCADDECOMPOSE_HXX

#include <Handle_TopTools_HSequenceOfShape.hxx>
#include <vector>
#include <TopoDS_Solid.hxx>
#include <TCollection_AsciiString.hxx>

#include "McCadDcompSolid.hxx"

using namespace std;

class McCadDecompose
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

Standard_EXPORT    McCadDecompose();
Standard_EXPORT    ~McCadDecompose();

Standard_EXPORT    void InputData(Handle_TopTools_HSequenceOfShape & solid_list,
                   TCollection_AsciiString theFileName);            /**< Load the step file */
Standard_EXPORT    void Perform();                                                 /**< Perform the decomposition and saving */
    //qiu make Decompose() becasue McCad-SALOME use it without saving file
Standard_EXPORT    void Decompose();                                                   /**< Start decomposition */

Standard_EXPORT    void SetFileName( TCollection_AsciiString theFileName);         /**< File name for saving */
Standard_EXPORT    void SetCombine(Standard_Integer iCombSymb);                    /**< Combine the decomposed solids or not */
    //qiu add method to get solids
Standard_EXPORT    Handle_TopTools_HSequenceOfShape getOutCompSolids () {return m_pGeoData->getOutCompSolids();} /**< Get the output compound solid */
Standard_EXPORT    Handle_TopTools_HSequenceOfShape getErrCompSolids () {return m_pGeoData->getErrCompSolids();} /**< Get the error compound solid */

private:

    Standard_Real m_fDeflection;            /**< The deflection for surface meshing */
    McCadDcompGeomData * m_pGeoData;        /**< Store the geometry data */
    TCollection_AsciiString m_fileName;     /**< File name for saving */
    Standard_Integer m_iCombSymb;           /**< Combine the decomposed solids or not */

private:

    Standard_Real CalMeshDeflection(TopoDS_Solid &theSolid);            /**< Calculate the deflection of surface meshing*/
    void SaveDecomposedSolids(TCollection_AsciiString theFileName);     /**< Save the decomposed solids */
    void DeleteList(vector<McCadDcompSolid*> *& pSolidList);            /**< Delete the solid list and solids*/

};

#endif // MCCADDECOMPOSE_HXX

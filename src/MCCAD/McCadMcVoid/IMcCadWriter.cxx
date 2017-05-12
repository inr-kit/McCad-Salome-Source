#include "IMcCadWriter.hxx"

IMcCadWriter::IMcCadWriter()
{
}

IMcCadWriter::~IMcCadWriter()
{
}

/** ********************************************************************
* @brief Set the name of output file.
* @param TCollection_AsciiString theFileName
* @return void
*
* @date 31/10/2012
* @modify 18/12/2013
* @author  Lei Lu
***********************************************************************/
void IMcCadWriter::SetFileName(TCollection_AsciiString theFileName)
{
    m_OutputFileName = theFileName;
}


/** ********************************************************************
* @brief Set the manager pointer.
* @param McCadVoidCellManager * pManager
* @return void
*
* @date 31/10/2012
* @modify 18/12/2013
* @author  Lei Lu
***********************************************************************/
void IMcCadWriter::SetManager(McCadVoidCellManager *pManager)
{
    m_pManager = pManager;
}


/** ********************************************************************
* @brief Set the material switch to control the output of material card
* @param Standard_Boolean bMat
* @return void
*
* @date 31/10/2012
* @modify 18/12/2013
* @author  Lei Lu
***********************************************************************/
void IMcCadWriter::SetMaterial(Standard_Boolean bMat)
{
    m_bHaveMaterial = bMat;
}


/** ********************************************************************
* @brief Set the void switch to control the output of void card
* @param Standard_Boolean bVoid
* @return void
*
* @date 31/10/2012
* @modify 18/12/2013
* @author  Lei Lu
***********************************************************************/
void IMcCadWriter::SetVoid(Standard_Boolean bVoid)
{
    m_bGenerateVoid = bVoid;
}

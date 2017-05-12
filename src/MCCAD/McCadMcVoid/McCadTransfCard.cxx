#include "McCadTransfCard.hxx"

McCadTransfCard::McCadTransfCard()
{
}

McCadTransfCard::McCadTransfCard(gp_Ax3 theAxis,gp_Pnt theApex)
{
    m_Axis = theAxis;
    m_pntApex = theApex;
    GenerateTransfCard();  /// Generate the transform matrix
}



/** ********************************************************************
* @brief Given the axis and rotated point, generate the transform card
*
* @param
* @return void
*
* @date 31/04/2013
* @author  Lei Lu
************************************************************************/
void McCadTransfCard::GenerateTransfCard()
{
    gp_Trsf theTrsf;
    gp_Ax3 stdAxis;
    theTrsf.SetTransformation(m_Axis, stdAxis);
    gp_XYZ vector = theTrsf.TranslationPart();

    if(Abs(m_pntApex.X()) < 1.0e-7)
    {
        vector.SetX(0.0);
    }
    if(Abs(m_pntApex.Y()) < 1.0e-7)
    {
        vector.SetY(0.0);
    }
    if(Abs(m_pntApex.Z()) < 1.0e-7)
    {
        vector.SetZ(0.0);
    }
    m_PrmtList.push_back(m_pntApex.X());
    m_PrmtList.push_back(m_pntApex.Y());
    m_PrmtList.push_back(m_pntApex.Z());

    gp_Mat matrix = theTrsf.HVectorialPart();

    for (int i = 1; i <= 3; i++)
    {
        gp_XYZ vec = matrix.Column(i);
        if(Abs(vec.X()) < 1.e-10)
        {
            vec.SetX(0.0);
        }
        if(Abs(vec.Y()) < 1.e-10)
        {
            vec.SetY(0.0);
        }
        if(Abs(vec.Z()) < 1.e-10)
        {
            vec.SetZ(0.0);
        }

        m_PrmtList.push_back(vec.X());
        m_PrmtList.push_back(vec.Y());
        m_PrmtList.push_back(vec.Z());
    }

}



/** ********************************************************************
* @brief Set the transform card
*
* @param Standard_Integer iNum
* @return void
*
* @date 31/04/2013
* @author  Lei Lu
************************************************************************/
void McCadTransfCard::SetTrNum(Standard_Integer iNum)
{
    m_iTrNum = iNum;
}




/** ********************************************************************
* @brief Get the transform card
*
* @param
* @return Standard_Integer
*
* @date 31/04/2013
* @author  Lei Lu
************************************************************************/
Standard_Integer McCadTransfCard::GetTrNum()
{
    return m_iTrNum;
}



/** ********************************************************************
* @brief Get the coefficients list
*
* @param
* @return vector<Standard_Real>
*
* @date 31/04/2013
* @author  Lei Lu
************************************************************************/
vector<Standard_Real> McCadTransfCard::GetPrmtList()
{
    return m_PrmtList;
}

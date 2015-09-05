#include "McCadTransfCard.hxx"

McCadTransfCard::McCadTransfCard()
{
}


McCadTransfCard::McCadTransfCard(gp_Ax3 theAxis,gp_Pnt theApex)
{
    gp_Trsf theTrsf;
    gp_Ax3 stdAxis;
    theTrsf.SetTransformation(theAxis, stdAxis);
    gp_XYZ vector = theTrsf.TranslationPart();

    if(Abs(theApex.X()) < 1.0e-7)
    {
        vector.SetX(0.0);
    }
    if(Abs(theApex.Y()) < 1.0e-7)
    {
        vector.SetY(0.0);
    }
    if(Abs(theApex.Z()) < 1.0e-7)
    {
        vector.SetZ(0.0);
    }
    m_PrmtList.push_back(theApex.X());
    m_PrmtList.push_back(theApex.Y());
    m_PrmtList.push_back(theApex.Z());

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

void McCadTransfCard::SetTrNum(Standard_Integer iNum)
{
    m_iTrNum = iNum;
}

Standard_Integer McCadTransfCard::GetTrNum()
{
    return m_iTrNum;
}

vector<Standard_Real> McCadTransfCard::GetPrmtList()
{
    return m_PrmtList;
}

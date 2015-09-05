#include "McCadTripoliCell.hxx"

McCadTripoliCell::McCadTripoliCell()
{





}


TCollection_AsciiString McCadTripoliCell::GetEqua()
{
    if (!m_Equation.IsEmpty())
    {
        return m_Equation;
    }

    TCollection_AsciiString strDesc = "EQUA ";

    int iNumPlusFace = plus_list.size();
    int iNumMinusFace = minus_list.size();

    for(int i = 0; i < iNumPlusFace; i++)
    {
        if (i == 0)
        {
            strDesc += "PLUS ";
            strDesc += TCollection_AsciiString(iNumPlusFace);
            strDesc += " ";
        }
        strDesc += TCollection_AsciiString(plus_list.at(i));
        strDesc += " ";
    }

    for(int i = 0; i < iNumMinusFace; i++)
    {
        if (i == 0)
        {
            strDesc += "MINUS ";
            strDesc += TCollection_AsciiString(iNumMinusFace);
            strDesc += " ";
        }
        strDesc += TCollection_AsciiString(abs(minus_list.at(i)));
        strDesc += " ";
    }

    strDesc.LeftAdjust();   // Trim the left and right space characters of string
    strDesc.RightAdjust();
    return strDesc;
}



void McCadTripoliCell::SetVirtual(bool isVirtual)
{

}

void McCadTripoliCell::SetBoolOp(int theBoolOp)
{
    m_boolOp = theBoolOp;
}


void McCadTripoliCell::SetCellNum(int theCellNum)
{
    m_CellNum = theCellNum;
}

void McCadTripoliCell::AddSurface(int theSurfNum)
{
    if(theSurfNum > 0)
    {
        plus_list.push_back(theSurfNum);
    }
    else if(theSurfNum < 0)
    {
        minus_list.push_back(theSurfNum);
    }
}

void McCadTripoliCell::AddCldCell(McCadTripoliCell* pCell)
{
    m_childCellList.push_back(pCell);
}

bool McCadTripoliCell::hasVirtualCell()
{
    if(!m_childCellList.empty())
    {
        return true;
    }
    else
    {
        return false;
    }
}

McCadTripoliCell * McCadTripoliCell::GetVirtualCell(int index)
{
    if(m_childCellList.empty())
    {
        return NULL;
    }
    else
    {
        return m_childCellList.at(index);
    }
}

int McCadTripoliCell::GetNumVirCell()
{
    if(m_childCellList.empty())
    {
        return 0;
    }
    else
    {
        return m_childCellList.size();
    }
}



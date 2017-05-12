#include "McCadTripoliCell.hxx"

McCadTripoliCell::McCadTripoliCell()
{
    m_IsVirtual = false;
}

McCadTripoliCell::~McCadTripoliCell()
{
}


/** ********************************************************************
* @brief From the input surfaces and their directions, write the equation*
*
* @param
* @return TCollection_AsciiString
*
* @date 31/04/2013
* @author  Lei Lu
************************************************************************/
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
        strDesc += TCollection_AsciiString(minus_list.at(i));
        strDesc += " ";
    }

    strDesc.LeftAdjust();   // Trim the left and right space characters of string
    strDesc.RightAdjust();
    return strDesc;
}



/** ********************************************************************
* @brief Set this Boolean operation of the cell
*
* @param bool isVirtual
* @return void
*
* @date 31/04/2013
* @author  Lei Lu
************************************************************************/
void McCadTripoliCell::SetBoolOp(int theBoolOp)
{
    m_boolOp = theBoolOp;
}



/** ********************************************************************
* @brief Add surface into the face list, according to the directions,
*        add them into plus_list or minus_list
*
* @param int theSurfNum
* @return void
*
* @date 31/04/2013
* @author  Lei Lu
************************************************************************/
void McCadTripoliCell::AddSurface(int theSurfNum)
{
    if(theSurfNum > 0)
    {
        plus_list.push_back(theSurfNum);
    }
    else if(theSurfNum < 0)
    {
        minus_list.push_back(abs(theSurfNum));  // Remove the minus symbol
    }
}



/** ********************************************************************
* @brief Add another cells as one virtual child cell of current cell
*
* @param int theSurfNum
* @return void
*
* @date 31/04/2013
* @author  Lei Lu
************************************************************************/
void McCadTripoliCell::AddChildCell(McCadTripoliCell* pCell)
{
    m_childCellList.push_back(pCell);
}



/** ********************************************************************
* @brief If the cell has virtual cell or not
*
* @param
* @return bool
*
* @date 31/04/2013
* @author  Lei Lu
************************************************************************/
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



/** ********************************************************************
* @brief Get virtual cell from virtual cell list with given index
*
* @param int index
* @return McCadTripoliCell *
*
* @date 31/04/2013
* @author  Lei Lu
************************************************************************/
McCadTripoliCell* McCadTripoliCell::GetVirtualCell(int index)
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



/** ********************************************************************
* @brief Get the number of virtual cells
*
* @param int index
* @return McCadTripoliCell *
*
* @date 31/04/2013
* @author  Lei Lu
************************************************************************/
int McCadTripoliCell::GetNumVirCell()
{
    return m_childCellList.size();
}



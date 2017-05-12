#include "McCadVoidCollision.hxx"

McCadVoidCollision::McCadVoidCollision()
{
}

McCadVoidCollision::~McCadVoidCollision()
{
}


/** ********************************************************************
* @brief Set the collied solid number
* @param int iSolidNum
* @return void
*
* @date 26/07/2016
* @author  Lei Lu
***********************************************************************/
void McCadVoidCollision::SetColliedSolidNum(int iSolidNum)
{
    m_iSolidNum = iSolidNum;
}


/** ********************************************************************
* @brief Add one boundary surface in collied surface list
* @param int iFaceNum
* @return void
*
* @date 26/07/2016
* @author  Lei Lu
***********************************************************************/
void McCadVoidCollision::AddColliedFace(int iFaceNum)
{
    //Check the repeated cell number
    Standard_Boolean bRepeat = Standard_False;
    for(unsigned int i = 0 ; i < m_FaceNumList.size(); i++)
    {
        if(m_FaceNumList.at(i) == iFaceNum)
        {
            bRepeat = Standard_True;
            break;
        }
    }

    if(bRepeat == Standard_False)
    {
        m_FaceNumList.push_back(iFaceNum);
    }
}




/** ********************************************************************
* @brief Add assisted surface in collied assisted surface list
* @param int iAstFaceNum
* @return void
*
* @date 26/07/2016
* @author  Lei Lu
***********************************************************************/
void McCadVoidCollision::AddColliedAstFace(int iAstFaceNum)
{
    Standard_Boolean bRepeat = Standard_False;
    for(unsigned int i = 0 ; i < m_AstFaceNumList.size(); i++)
    {
        if(m_AstFaceNumList.at(i) == iAstFaceNum)
        {
            bRepeat = Standard_True;
            break;
        }
    }

    if(bRepeat == Standard_False)
    {
        m_AstFaceNumList.push_back(iAstFaceNum);
    }
}




/** ********************************************************************
* @brief Get the collied solid number
* @param
* @return Standard_Integer
*
* @date 26/07/2016
* @author  Lei Lu
***********************************************************************/
Standard_Integer McCadVoidCollision::GetColliedSolidNum()
{
    return m_iSolidNum;
}



/** ********************************************************************
* @brief Get the collied boundary surface list
* @param
* @return vector<int>
*
* @date 26/07/2016
* @author  Lei Lu
***********************************************************************/
vector<int> McCadVoidCollision::GetFaceNumList()
{
    return m_FaceNumList;
}



/** ********************************************************************
* @brief Get the collied assisted surface list
* @param
* @return vector<int>
*
* @date 26/07/2016
* @author  Lei Lu
***********************************************************************/
vector<int> McCadVoidCollision::GetAstFaceNumList()
{
    return m_AstFaceNumList;
}



/** ********************************************************************
* @brief Change the boundary face number at collied face list
* @param int index, int value
* @return void
*
* @date 26/07/2016
* @author  Lei Lu
***********************************************************************/
void McCadVoidCollision::ChangeFaceNum(int index, int value)
{  
    m_FaceNumList.at(index) = value;
}



/** ********************************************************************
* @brief Change the assisted face number at collied face list
* @param int index, int value
* @return void
*
* @date 26/07/2016
* @author  Lei Lu
***********************************************************************/
void McCadVoidCollision::ChangeAstFaceNum(int index, int value)
{
    m_AstFaceNumList.at(index) = value;
}



/** ********************************************************************
* @brief Change the collied solid number
* @param int iSolidNum
* @return void
*
* @date 26/07/2016
* @author  Lei Lu
***********************************************************************/
void McCadVoidCollision::ChangeSolidNum(int iSolidNum )
{
    m_iSolidNum = iSolidNum;
}

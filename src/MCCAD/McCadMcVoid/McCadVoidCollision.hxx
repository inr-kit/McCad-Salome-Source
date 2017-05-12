#ifndef MCCADVOIDCOLLISION_HXX
#define MCCADVOIDCOLLISION_HXX

#include <vector>
#include <Standard.hxx>

using namespace std;

class McCadVoidCollision
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

    McCadVoidCollision();
    ~McCadVoidCollision();

public:

    void SetColliedSolidNum(int iSolidNum);     /**< Set the number of collied solid */
    void AddColliedFace(int iFaceNum);          /**< Add the boundary surface numbers of collied solid */
    void AddColliedAstFace(int iAstFaceNum);    /**< Add the assisted surface numbers of collied solid */

    Standard_Integer GetColliedSolidNum();      /**< Get the collied solid number */
    vector<int> GetFaceNumList();               /**< Get the face number of collied solid */
    vector<int> GetAstFaceNumList();            /**< Get the assisted surface number of collied solid */
    void ChangeFaceNum(int index, int value);   /**< Change the surface number by giving index */
    void ChangeSolidNum(int iSolidNum );        /**< Change the collied solid number */
    void ChangeAstFaceNum(int index, int value);/**< Change the assisted surface number of collied solid */

private:

    int m_iSolidNum;                    /**< Collied solid number */
    vector<int> m_FaceNumList;          /**< Collied boundary surface numbers */
    vector<int> m_AstFaceNumList;       /**< Collied assisted surface numbers */
};

#endif // MCCADVOIDCOLLISION_HXX

#ifndef MCCADVOIDGENERATOR_HXX
#define MCCADVOIDGENERATOR_HXX

#include "McCadVoidCellManager.hxx"

using namespace std;

class McCadVoidGenerator
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

Standard_EXPORT    McCadVoidGenerator();
Standard_EXPORT    McCadVoidGenerator(const McCadVoidCellManager * pManager);
Standard_EXPORT    ~McCadVoidGenerator();

private:
    McCadVoidCellManager * m_pManager;

public:

    /** Read the void models. */
Standard_EXPORT    void ReadVoidCells();
    /** Generate the void cells according to the convex solid readed. */
Standard_EXPORT    void GenVoidCells();
    /** Detect the collision between the void cell and material solids */
Standard_EXPORT    void VoidSolidCollision();
    /** Detect the collision between the void cell and boundary surface of material solids */
Standard_EXPORT    void VoidFaceCollision();
Standard_EXPORT    void GenVoidSurfList();
    /** Split the void box when it collides with too many material solids */
Standard_EXPORT    void SplitVoidCell();
    /** Split the void box when it collides with too many material solids */
Standard_EXPORT    void Process();
};

#endif // MCCADVOIDGENERATOR_HXX

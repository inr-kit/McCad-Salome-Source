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

    McCadVoidGenerator();
    McCadVoidGenerator(const McCadVoidCellManager * pManager);
    ~McCadVoidGenerator();

private:
    McCadVoidCellManager * m_pManager;

public:

    /** Read the void models. */
    void ReadVoidCells();
    /** Generate the void cells according to the convex solid readed. */
    void GenVoidCells();
    /** Detect the collision between the void cell and material solids */
    void VoidSolidCollision();
    /** Detect the collision between the void cell and boundary surface of material solids */
    void VoidFaceCollision();
    void GenVoidSurfList();
    /** Split the void box when it collides with too many material solids */
    void SplitVoidCell();
    /** Split the void box when it collides with too many material solids */
    void Process();
};

#endif // MCCADVOIDGENERATOR_HXX

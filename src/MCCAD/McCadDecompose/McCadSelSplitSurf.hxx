#ifndef MCCADSELSPLITSURF_HXX
#define MCCADSELSPLITSURF_HXX

#include "McCadSurface.hxx"
#include <vector>

using namespace std;

class McCadSelSplitSurf
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

    McCadSelSplitSurf();
    ~McCadSelSplitSurf();

public:

    /**< Merge Surfaces, delete the repeated surfaces, and count the repeated times*/
    void MergeSplitSurfaces(vector<McCadSurface*> & face_list);
    /**< Select the splitting surfaces from split surfaces list according to the characters of splitting surfaces */
    void GenSplitSurfList(vector<McCadSurface*> & face_list,vector<McCadSurface *> &selface_list);

    /**< Judge the solid has splitting planes through only planes */
    Standard_Boolean HasPlnSplitOnlyPln(vector<McCadSurface*> & face_list);
    /**< Are there splitting surface with internal loop and has no collision with other boundary surfaces */
    Standard_Boolean HasSurfThroughNoBndSurf(vector<McCadSurface*> & face_list);

private:

    /**< Sort the surfaces with different conditions*/
    void SortSurfaces(vector<McCadSurface*> & face_list);
};

#endif // MCCADSELSPLITSURF_HXX

#ifndef MCCADTRIPOLICELL_HXX
#define MCCADTRIPOLICELL_HXX

#include <TCollection_AsciiString.hxx>
#include <vector>

using namespace std;

class McCadTripoliCell
{
public:
Standard_EXPORT    McCadTripoliCell();

private:

    bool m_IsVirtual;       // is virtual cell
    int m_boolOp;           // boolean operations
    int m_CellNum;          // cell number
    vector<McCadTripoliCell *> m_childCellList; // the child cells included

    vector<int> plus_list;  // the plus surfaces list
    vector<int> minus_list; // the minus surfaces list

    TCollection_AsciiString m_Equation; // the generated TRIPOLI equation

public:
Standard_EXPORT    TCollection_AsciiString GetEqua();
Standard_EXPORT    void SetVirtual(bool isVirtual);
Standard_EXPORT    void SetBoolOp(int theBoolOp);
Standard_EXPORT    void SetCellNum(int theCellNum);
Standard_EXPORT    void AddSurface(int theSurfNum);

Standard_EXPORT    void AddCldCell(McCadTripoliCell* pCell);
Standard_EXPORT    bool hasVirtualCell();

Standard_EXPORT    McCadTripoliCell * GetVirtualCell(int index);
Standard_EXPORT    int GetNumVirCell();
};

#endif // MCCADTRIPOLICELL_HXX

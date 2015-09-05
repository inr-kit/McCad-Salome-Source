#ifndef MCCADTRIPOLICELL_HXX
#define MCCADTRIPOLICELL_HXX

#include <TCollection_AsciiString.hxx>
#include <vector>

using namespace std;

class McCadTripoliCell
{
public:
    McCadTripoliCell();

private:

    bool m_IsVirtual;       // is virtual cell
    int m_boolOp;           // boolean operations
    int m_CellNum;          // cell number
    vector<McCadTripoliCell *> m_childCellList; // the child cells included

    vector<int> plus_list;  // the plus surfaces list
    vector<int> minus_list; // the minus surfaces list

    TCollection_AsciiString m_Equation; // the generated TRIPOLI equation

public:
    TCollection_AsciiString GetEqua();
    void SetVirtual(bool isVirtual);
    void SetBoolOp(int theBoolOp);
    void SetCellNum(int theCellNum);
    void AddSurface(int theSurfNum);

    void AddCldCell(McCadTripoliCell* pCell);
    bool hasVirtualCell();

    McCadTripoliCell * GetVirtualCell(int index);
    int GetNumVirCell();
};

#endif // MCCADTRIPOLICELL_HXX

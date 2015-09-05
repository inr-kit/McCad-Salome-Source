#ifndef QMCCAD_READLISTDIALOG_HXX
#define QMCCAD_READLISTDIALOG_HXX

#include <QDialog>
#include <ui_McCad_LoadVisualizationList.h>
#include <TCollection_AsciiString.hxx>
#include <list>

using namespace std;

class QMcCad_ReadListDialog : public QDialog
{
    Q_OBJECT

public:
    QMcCad_ReadListDialog();
    TCollection_AsciiString GetFileName();
    double GetLowerBound();
    double GetUpperBound();
    bool FileSelected();
    list<int> GetCellNumberList();
    list<double> GetValueList();
    int GetNbColors();
    bool Done();

   public slots:
    void SlotOpenFile();
    void SlotDyeSolids();

private:
    bool ParseFile();

    bool myIsDone;

    Ui_McCad_LoadVisualizationList myUI;
    bool myFileSelected;
    double myLowerBound, myUpperBound;
    TCollection_AsciiString myFileName;
    list<int> myCellNumberList;
    list<double> myValueList;    
};

#endif // QMCCAD_READLISTDIALOG_HXX

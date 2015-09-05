#include "MCCADGUI_CreatePartOp.h"
#include "MCCADGUI_GenPartDialog.h"
#include "MCCADGUI_DataModel.h"
#include "MCCADGUI.h"
#include "MCCADGUI_DataObject.h"

#include <LightApp_Module.h>
#include <LightApp_Application.h>
#include <SUIT_Desktop.h>
#include <iostream>

using namespace std;


MCCADGUI_CreatePartOp::MCCADGUI_CreatePartOp()
  : MCCADGUI_Operation(),
    myDlg( 0 )
{

}

/*! Destructor */
MCCADGUI_CreatePartOp::~MCCADGUI_CreatePartOp()
{
  if ( myDlg )
    delete myDlg;
}

/*! Returns Operation dialog */
LightApp_Dialog* MCCADGUI_CreatePartOp::dlg() const
{
  if ( !myDlg )
    const_cast<MCCADGUI_CreatePartOp*>( this )->myDlg = new MCCADGUI_GenPartDialog( module()->getApp()->desktop() );

  return myDlg;
}

void MCCADGUI_CreatePartOp::onApply()
{
//    cout << "on Apply() !!"<<endl;
//    const int studyID = dataModel()->getStudyId();

    vector <float>  XDim;
    vector <float>  YDim;
    vector <float>  ZDim;

    MCCADGUI_GenPartDialog * Dialog = dynamic_cast<MCCADGUI_GenPartDialog *> (dlg()) ;
    if (!Dialog->getData(XDim, YDim, ZDim))
        abort();
    if (Dialog->getName().isEmpty())
        abort();

    //get part group id
//    MCCADGUI * theModule = dynamic_cast<MCCADGUI *> (module()) ;  //obtain the XYSMESHGUI module
//    if (!theModule)
//        abort();

    //for debug
//    MCCADGUI_DataObject * aObj = dataModel()->findObject("MCCADGUI_1");
//    QString atmpEntry = aObj->entry();
//    int atmp  = aObj->getPartGroupIndex();
//    int btmp = aObj->getPartIndex();
//    QString atmpName = aObj->name();

    QStringList aEntryList;
//    theModule->selected(aEntryList,false);  //only one seleted item
    MCCADGUIModule()->selected(aEntryList,false);
    if (aEntryList.size() != 1)
    {
        cout << "seletion is invalid!!"<<endl;
        return;
    }

    QString aEntry = aEntryList.at(0);  //obtain the entry
    int aGroupId = dataModel()->getGroupID(aEntry);

    if (!dataModel()->generatePart(Dialog->getName(), aGroupId, XDim, YDim, ZDim))
    {
        cout << "generate part failed!"<<endl;
        return;
    }
}



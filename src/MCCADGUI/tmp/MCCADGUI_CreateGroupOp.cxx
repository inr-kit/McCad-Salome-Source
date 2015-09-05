#include "MCCADGUI_CreateGroupOp.h"
#include <MCCADGUI_DataModel.h>
#include <LightApp_Module.h>
#include <LightApp_Application.h>
#include <SUIT_Desktop.h>
#include "MCCADGUI_DataObject.h"

#include <qinputdialog.h>

MCCADGUI_CreateGroupOp::MCCADGUI_CreateGroupOp()
: MCCADGUI_Operation()
{
}

MCCADGUI_CreateGroupOp::~MCCADGUI_CreateGroupOp()
{
}
/*! Operation itself. */
void MCCADGUI_CreateGroupOp::startOperation()
{
    MCCADGUI_DataModel* dm = dataModel();

    bool bOk;
    QString aName = QInputDialog::getText( module()->getApp()->desktop(),
                                           tr( "Create Part Group" ),
                                           tr( "Group Name"),
                                           QLineEdit::Normal,
                                           "NewGroup",
                                           &bOk);

    if (dm->generateGroup(aName))
        commit();
    else
        abort();
    return;
}

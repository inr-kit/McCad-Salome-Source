// Copyright (C) 2014-2015  KIT-INR/NK
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//
#include "MCCADGUI_Operation.h"
#include "MCCADGUI_DataModel.h"
#include "MCCADGUI.h"

#include <SUIT_MessageBox.h>
#include <LightApp_Application.h>
#include <LightApp_Dialog.h>

MCCADGUI_Operation::MCCADGUI_Operation()
: LightApp_Operation(),
//  myDlg(0),
  LastName("NewPart")
{
}

MCCADGUI_Operation::~MCCADGUI_Operation()
{
//    if ( myDlg )
//      delete myDlg;
}

/*! Called when Operation is started. */
void MCCADGUI_Operation::startOperation()
{
//    cout <<"run here!!!" <<endl;
//  if( dlg() )
//  {
//    disconnect( dlg(), SIGNAL( dlgOk() ),     this, SLOT( onOk() ) );
//    disconnect( dlg(), SIGNAL( dlgApply() ),  this, SLOT( onApply() ) );
//    disconnect( dlg(), SIGNAL( dlgClose() ),  this, SLOT( onClose() ) );

//    connect( dlg(), SIGNAL( dlgOk() ),     this, SLOT( onOk() ) );
//    connect( dlg(), SIGNAL( dlgApply() ),  this, SLOT( onApply() ) );
//    connect( dlg(), SIGNAL( dlgClose() ),  this, SLOT( onClose() ) );
//    dlg()->show();
//    MCCADGUI_TemplateDlg * aDlg = dynamic_cast <MCCADGUI_TemplateDlg *> (dlg());
//    aDlg->updateNameList();
//  }
}

/*! Called when Operation is aborted. */
void MCCADGUI_Operation::abortOperation()
{
  finish();
}

/*! Called when Operation is committed (normally ended). */
void MCCADGUI_Operation::commitOperation()
{
    SUIT_MessageBox::information( module()->getApp()->desktop(), QString("Message"),
                              QString( "Operation Done."));
  finish();
}

/*! Called when Operation is done. Finalization steps. */
void MCCADGUI_Operation::finish()
{
  if( dlg() )
    dlg()->hide();

//  module()->getApp()->updateObjectBrowser();  //this method force updating datamodel, not used

//  module()->updateObjBrowser(false);
  MCCADGUIModule()->updateObjBrowser();
}


/*! Returns a Data Model. */
MCCADGUI_DataModel* MCCADGUI_Operation::dataModel() const
{
  return dynamic_cast<MCCADGUI_DataModel*>( module()->dataModel() );
}

/*! Returns MCCADGUI class instance. */
MCCADGUI* MCCADGUI_Operation::MCCADGUIModule() const
{
  return dynamic_cast<MCCADGUI*>( module() );
}

/*! slot. Called when user presses OK button of the dialog (if it was set). */
void MCCADGUI_Operation::onOk()
{
  onApply();
  commit();
}

/*! slot. Called when user presses Apply button of the dialog (if it was set). */
void MCCADGUI_Operation::onApply()
{
}

/*! slot. Called when user presses Close button of the dialog (if it was set). */
void MCCADGUI_Operation::onClose()
{
  commit();
}

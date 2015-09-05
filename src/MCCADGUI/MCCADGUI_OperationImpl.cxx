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
#include "MCCADGUI_OperationImpl.h"
#include "MCCADGUI_Dialogs.h"
#include "MCCADGUI_DataModel.h"
#include "MCCADGUI.h"
#include "MCCADGUI_DataObject.h"

#include <LightApp_Module.h>
#include <LightApp_Application.h>
#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Tools.h>
#include "utilities.h"

#include <qinputdialog.h>

////###############EditMaterialOp#############//

//MCCADGUI_EditMaterialOp::MCCADGUI_EditMaterialOp()
//: MCCADGUI_Operation()
////  LastDir("/home/untitled.inp")

//{
//    myDlg = 0;
//}

//MCCADGUI_EditMaterialOp::~MCCADGUI_EditMaterialOp()
//{
//    if ( myDlg )
//      delete myDlg;
//}

///*! Returns Operation dialog */
//LightApp_Dialog* MCCADGUI_EditMaterialOp::dlg() const
//{
//  if ( !myDlg )
//    const_cast<MCCADGUI_EditMaterialOp*>( this )->myDlg
//          = new MCCADGUI_MCardDialog( module()->getApp()->desktop(), dataModel() );
//  return myDlg;
//}

///*! Operation itself. */
//void MCCADGUI_EditMaterialOp::startOperation()
//{
//    if( dlg() )
//      {
//        disconnect( dlg(), SIGNAL( dlgOk() ),     this, SLOT( onOk() ) );
//        disconnect( dlg(), SIGNAL( dlgClose() ),  this, SLOT( onClose() ) );

//        connect( dlg(), SIGNAL( dlgOk() ),     this, SLOT( onOk() ) );
//        connect( dlg(), SIGNAL( dlgClose() ),  this, SLOT( onClose() ) );
//      }

//    MCCADGUI_Operation::startOperation();
//    MCCADGUI_MCardDialog * aDialog = dynamic_cast <MCCADGUI_MCardDialog * > (dlg());
//    aDialog->Refresh();  //load the existing material in this study
//    aDialog->show();
//}

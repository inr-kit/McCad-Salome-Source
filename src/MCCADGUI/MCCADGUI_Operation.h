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
#ifndef MCCADGUI_OPERATION_H
#define MCCADGUI_OPERATION_H
#include <LightApp_Operation.h>
#include "MCCADGUI_Dialogs.h"

class MCCADGUI_DataModel;
class MCCADGUI;

class MCCADGUI_Operation: public LightApp_Operation
{
    Q_OBJECT

public:
    MCCADGUI_Operation();
    virtual ~MCCADGUI_Operation();

protected:
  virtual void startOperation();
  virtual void abortOperation();
  virtual void commitOperation();
  virtual void finish();

  MCCADGUI_DataModel*   dataModel() const;
  MCCADGUI*             MCCADGUIModule() const;

protected slots:
  virtual void onOk();
  virtual void onApply();
  virtual void onClose();

protected:
//  LightApp_Dialog*  myDlg;
//  MCCADGUI_TemplateDlg * myDlg;
  QString           LastName;

};

#endif // MCCADGUI_OPERATION_H

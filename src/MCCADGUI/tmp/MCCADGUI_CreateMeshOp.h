#ifndef MCCADGUI_CREATEMESHOP_H
#define MCCADGUI_CREATEMESHOP_H
#include <MCCADGUI_Operation.h>


class MCCADGUI_GenPartDialog;



class MCCADGUI_CreatePartOp: public MCCADGUI_Operation
{

    Q_OBJECT

public:
    MCCADGUI_CreatePartOp();
    virtual ~MCCADGUI_CreatePartOp();

    virtual LightApp_Dialog* dlg() const;

protected slots:
  virtual void onApply();

private:

    MCCADGUI_GenPartDialog* myDlg;

};

#endif // MCCADGUI_CREATEMESHOP_H

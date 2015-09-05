#ifndef MCCADGUI_CREATEGROUPOP_H
#define MCCADGUI_CREATEGROUPOP_H
#include <MCCADGUI_Operation.h>


class MCCADGUI_CreateGroupOp: public MCCADGUI_Operation
{

    Q_OBJECT

public:
    MCCADGUI_CreateGroupOp();
    virtual ~MCCADGUI_CreateGroupOp();

protected:
  virtual void startOperation();

};

#endif // MCCADGUI_CREATEGROUPOP_H

#ifndef MCCADGUI_GENMESHDIALOG_H
#define MCCADGUI_GENMESHDIALOG_H
#include <LightApp_Dialog.h>
#include "ui_GenPartDialog.h"
#include <vector>
#include <QString>

using namespace std;



class MCCADGUI_GenPartDialog: public LightApp_Dialog , public Ui::GenPartDialog
{
    Q_OBJECT

public:
    MCCADGUI_GenPartDialog(QWidget* );
    virtual ~MCCADGUI_GenPartDialog();

//    virtual bool acceptData();
//    bool isOk() {return *isOK; }
    bool getData (vector <float> & xDim, vector <float> & yDim, vector <float> & zDim);
    QString getName();



private:
    QString aName ;
    vector <float>  XDim;
    vector <float>  YDim;
    vector <float>  ZDim;



};

#endif // MCCADGUI_GENMESHDIALOG_H

#include "MCCADGUI_GenPartDialog.h"

MCCADGUI_GenPartDialog::MCCADGUI_GenPartDialog(QWidget* parent)
: LightApp_Dialog( parent, 0, false, true, OK | Apply | Close )
{
    aName = "";
    setupUi(this);
}

MCCADGUI_GenPartDialog::~MCCADGUI_GenPartDialog()
{
}

///*!
// * \brief this method is inherited from acceptData() in QtxDialog.
// *  this method is called when Dialog box is accepted, for checking
// *  data correctness and so on
// * \return \c true if checking pass
// */
//bool MCCADGUI_GenPartDialog::acceptData()
//{

//    aName = Gen_le_name->text();
//    QStringList Xstring = Gen_le_X->text().split(" ");
//    QStringList Ystring = Gen_le_Y->text().split(" ");
//    QStringList Zstring = Gen_le_Z->text().split(" ");

//    XDim.clear();
//    YDim.clear();
//    ZDim.clear();


//    //check and load the intervals into member
//    for (int i = 0; i < Xstring.size(); i++)
//    {
//        XDim.push_back(QString(Xstring.at(i)).toFloat(isOK));
//        if (!(*isOK))
//            return false;
//    }
//    for (int i = 0; i < Ystring.size(); i++)
//    {
//        YDim.push_back(QString(Ystring.at(i)).toFloat(isOK));
//        if (!(*isOK))
//            return false;
//    }
//    for (int i = 0; i < Zstring.size(); i++)
//    {
//        ZDim.push_back(QString(Zstring.at(i)).toFloat(isOK));
//        if (!(*isOK))
//            return false;
//    }
//    return true;
//}

/*!
 * \brief get Name
 * \return  the name
 */
QString MCCADGUI_GenPartDialog::getName()
{
    aName = Gen_le_name->text();
    return aName;
}

/*!
 * \brief return the intervals
 * \param xDim
 * \param yDim
 * \param zDim
 * \return
 */
bool MCCADGUI_GenPartDialog::getData (vector <float> & xDim, vector <float> & yDim, vector <float> & zDim)
{



    QStringList Xstring = Gen_le_X->text().trimmed().split(" ");
    QStringList Ystring = Gen_le_Y->text().trimmed().split(" ");
    QStringList Zstring = Gen_le_Z->text().trimmed().split(" ");

    XDim.clear();
    YDim.clear();
    ZDim.clear();

    bool isOK;
    //check and load the intervals into member
    for (int i = 0; i < Xstring.size(); i++)
    {
//        XDim.push_back(QString(Xstring.at(i)).toFloat(isOK));
        XDim.push_back(Xstring.at(i).toFloat(&isOK));
        if (!isOK)
            return false;
    }
    for (int i = 0; i < Ystring.size(); i++)
    {
//        YDim.push_back(QString(Ystring.at(i)).toFloat(isOK));
        YDim.push_back(Ystring.at(i).toFloat(&isOK));
        if (!isOK)
            return false;
    }
    for (int i = 0; i < Zstring.size(); i++)
    {
//        ZDim.push_back(QString(Zstring.at(i)).toFloat(isOK));
         ZDim.push_back(Zstring.at(i).toFloat(&isOK));
        if (!isOK)
            return false;
    }

    xDim = XDim;
    yDim = YDim;
    zDim = ZDim;
    return true;
}



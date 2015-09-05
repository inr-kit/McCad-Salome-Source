/*
 * McCadDesign_MakeBox.hxx
 *
 *  Created on: Apr 21, 2009
 *      Author: grosse
 */

#ifndef MCCADDESIGN_MoveTO_HXX_
#define MCCADDESIGN_MoveTO_HXX_

#include "ui_McCadDesign_MoveTo.h"
#include <QMcCad_Editor.h>
#include <McCadDesign_Tool.hxx>

//! \brief Moves object to location


class McCadDesign_MoveTo : public QDialog, public McCadDesign_Tool
{
	Q_OBJECT

public:
	McCadDesign_MoveTo(QWidget* theParent = NULL);
	void PerformMove(TopoDS_Shape& theShp, gp_Pnt locPnt);

public slots:
	void CreateMove();
	void ConfirmMoveToPnt(const QString& dummyStr );

private:
	//fields
	Standard_Real myOriX, myOriY, myOriZ, myDistX, myDistY, myDistZ;
	Ui::moveToDialog theMoveToDialog;
	Standard_Real myMoveToPnt;
	Standard_Boolean myExternCall;
};

#endif /* MCCADDESIGN_MoveTo_HXX_ */

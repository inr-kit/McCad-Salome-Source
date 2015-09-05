/*
 * McCadDesign_MakeBox.hxx
 *
 *  Created on: Apr 21, 2009
 *      Author: grosse
 */

#ifndef MCCADDESIGN_ROTATE_HXX_
#define MCCADDESIGN_ROTATE_HXX_

#include "ui_McCadDesign_Rotate.h"
#include <QMcCad_Editor.h>
#include <McCadDesign_Tool.hxx>

//! \brief Performs a rotation of a shape registered in the TDocStd_Document


class McCadDesign_Rotate : public QDialog, public McCadDesign_Tool
{
	Q_OBJECT

public:
	McCadDesign_Rotate(QWidget* theParent = NULL);

public slots:
	void Rotate();
	void SwitchRadDeg(bool radState);
	void UpdateAngle();

private:
	//fields
	Standard_Real myOriX, myOriY, myOriZ, myAngle;
	bool myIsRad;
	Ui::rotateDialog theRotateDialog;
};

#endif /* MCCADDESIGN_Rotate_HXX_ */

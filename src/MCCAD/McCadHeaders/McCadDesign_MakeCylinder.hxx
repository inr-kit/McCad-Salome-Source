/*
 * McCadDesign_MakeBox.hxx
 *
 *  Created on: Apr 21, 2009
 *      Author: grosse
 */

#ifndef MCCADDESIGN_MAKECylinder_HXX_
#define MCCADDESIGN_MAKECylinder_HXX_

#include "ui_McCadDesign_MakeCylinder.h"
#include <QMcCad_Editor.h>
#include <McCadDesign_Tool.hxx>

//! \brief creates a cylinder


class McCadDesign_MakeCylinder : public QDialog, public McCadDesign_Tool
{
	Q_OBJECT

public:
	McCadDesign_MakeCylinder(QWidget* theParent = NULL);

public slots:
	void CreateCylinder();

private:
	//fields
	Standard_Real myOriX, myOriY, myOriZ, myDirX, myDirY, myDirZ, myHeight, myRadius;

	Ui::CylinderDialog theCylinderDialog;
	QMcCad_Editor* myEditor;
};

#endif /* MCCADDESIGN_MAKECylinder_HXX_ */

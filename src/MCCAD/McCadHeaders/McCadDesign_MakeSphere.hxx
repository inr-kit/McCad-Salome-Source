/*
 * McCadDesign_MakeBox.hxx
 *
 *  Created on: Apr 21, 2009
 *      Author: grosse
 */

#ifndef MCCADDESIGN_MAKESPHERE_HXX_
#define MCCADDESIGN_MAKESPHERE_HXX_

#include "ui_McCadDesign_MakeSphere.h"
#include <QMcCad_Editor.h>
#include <McCadDesign_Tool.hxx>

//! \brief Creates a sphere

class McCadDesign_MakeSphere : public QDialog, public McCadDesign_Tool
{
	Q_OBJECT

public:
	McCadDesign_MakeSphere(QWidget* theParent = NULL);

public slots:
	void CreateSphere();

private:
	//fields
	Standard_Real myOriX, myOriY, myOriZ, myRadius;

	Ui::SphereDialog theSphereDialog;
	QMcCad_Editor* myEditor;
};

#endif /* MCCADDESIGN_MAKESphere_HXX_ */

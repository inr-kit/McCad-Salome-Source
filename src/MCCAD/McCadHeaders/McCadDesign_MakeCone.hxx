/*
 * McCadDesign_MakeBox.hxx
 *
 *  Created on: Apr 21, 2009
 *      Author: grosse
 */

#ifndef MCCADDESIGN_MAKECONE_HXX_
#define MCCADDESIGN_MAKECONE_HXX_

#include "ui_McCadDesign_MakeCone.h"
#include <QMcCad_Editor.h>
#include <McCadDesign_Tool.hxx>

//! \brief Creates a cone

class McCadDesign_MakeCone : public QDialog, public McCadDesign_Tool
{
	Q_OBJECT

public:
	McCadDesign_MakeCone(QWidget* theParent = NULL);

public slots:
	void CreateCone();

private:
	//fields
	Standard_Real myOriX, myOriY, myOriZ, myDirX, myDirY, myDirZ, myHeight, myRmin, myRmaj;

	Ui::ConeDialog theConeDialog;
	QMcCad_Editor* myEditor;
};

#endif /* MCCADDESIGN_MAKECone_HXX_ */

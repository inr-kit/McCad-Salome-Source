/*
 * McCadDesign_MakeBox.hxx
 *
 *  Created on: Apr 21, 2009
 *      Author: grosse
 */

#ifndef MCCADDESIGN_COPYTO_HXX_
#define MCCADDESIGN_COPYTO_HXX_

#include "ui_McCadDesign_CopyTo.h"
#include <QMcCad_Editor.h>
#include <McCadDesign_Tool.hxx>
#include <QObject>

//! \brief implements a copy algorithm for solids registered in the TDocStd_Document

class McCadDesign_CopyTo : public QDialog, public McCadDesign_Tool
{
	Q_OBJECT

public:
	McCadDesign_CopyTo(QWidget* theParent = NULL);

public slots:
	void CreateCopy();

private:
	//fields
	Standard_Real myOriX, myOriY, myOriZ;
	Ui::copyToDialog theCopyToDialog;
};

#endif /* MCCADDESIGN_CopyTo_HXX_ */

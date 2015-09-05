/*
 * McCadDesign_MakeBox.hxx
 *
 *  Created on: Apr 21, 2009
 *      Author: grosse
 */

#ifndef MCCADDESIGN_SCALE_HXX_
#define MCCADDESIGN_SCALE_HXX_

#include "ui_McCadDesign_Scale.h"
#include <QMcCad_Editor.h>
#include <McCadDesign_Tool.hxx>

//! \brief Axis(in)dependent scaling


class McCadDesign_Scale : public QDialog, public McCadDesign_Tool
{
	Q_OBJECT

public:
	McCadDesign_Scale(QWidget* theParent = NULL);

public slots:
	void PerformScale();
	void SetAxisDependent(const QString& aString);

private:
	//fields
	Standard_Real mySf_X, mySf_Y, mySf_Z, mySf_All;
	Standard_Boolean myIsAll;
	Ui::scaleDialog theScaleDialog;
};

#endif /* MCCADDESIGN_Scale_HXX_ */

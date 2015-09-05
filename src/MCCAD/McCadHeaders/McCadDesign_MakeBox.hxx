/*
 * McCadDesign_MakeBox.hxx
 *
 *  Created on: Apr 21, 2009
 *      Author: grosse
 */

#ifndef MCCADDESIGN_MAKEBOX_HXX_
#define MCCADDESIGN_MAKEBOX_HXX_

#include "ui_McCadDesign_MakeBox.h"
#include <McCadDesign_Tool.hxx>
#include <QMcCad_Editor.h>

//! \brief creates a box


class McCadDesign_MakeBox : public QDialog, public McCadDesign_Tool
{
	Q_OBJECT

public:
	McCadDesign_MakeBox(QWidget* theParent = NULL);
	//void SetCurrentEditor(QMcCad_Editor* theEditor);


public slots:
	void CreateBox();

signals:

private:

	//functions
	//Standard_Real GetRealValue(const QString& theVal);

	//fields
	Standard_Real myOriX, myOriY, myOriZ, myDimX, myDimY, myDimZ;

	Ui::MakeBoxDialog theBoxDialog;
	QMcCad_Editor* myEditor;

};

#endif /* MCCADDESIGN_MAKEBOX_HXX_ */

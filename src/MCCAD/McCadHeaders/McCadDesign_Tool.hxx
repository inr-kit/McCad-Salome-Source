/*
 * McCadDesign_MakeBox.hxx
 *
 *  Created on: Apr 21, 2009
 *      Author: grosse
 */

#ifndef MCCADDESIGN_TOOL_HXX_
#define MCCADDESIGN_TOOL_HXX_

#include <QMcCad_Editor.h>

//! \brief Base class for all McCadDesign Tools


class McCadDesign_Tool
{
public:
	McCadDesign_Tool();
	virtual void SetCurrentEditor(QMcCad_Editor* theEditor);
	QMcCad_Editor* Editor();

protected:
	Standard_Real GetRealValue(const QString& theVal);
private:
	QMcCad_Editor* myEditor;
};

#endif /* MCCADDESIGN_TOOL_HXX_ */

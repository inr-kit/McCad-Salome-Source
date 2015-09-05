/*
 * McCadDesign_MakeBox.cxx
 *
 *  Created on: Apr 21, 2009
 *      Author: grosse
 */

#include <McCadDesign_Tool.hxx>

McCadDesign_Tool::McCadDesign_Tool()
{
}

void McCadDesign_Tool::SetCurrentEditor(QMcCad_Editor* theEditor)
{
	myEditor = theEditor;
}


QMcCad_Editor* McCadDesign_Tool::Editor()
{
	return myEditor;
}

/**
 *
 * private functions
 */
Standard_Real McCadDesign_Tool::GetRealValue(const QString& theVal)
{
	if(theVal.length() == 0)
		return 0.0;

	char aStr[theVal.length()];
	QByteArray ba = theVal.toAscii();
	strcpy(aStr, ba.data());
	TCollection_AsciiString anAsciiStr(aStr);
	if(!anAsciiStr.IsRealValue())
		return 0.0;

	return anAsciiStr.RealValue();
	//return anAsciiStr;
}


#include "McCadGeneTool.hxx"

McCadGeneTool::McCadGeneTool()
{
}

TCollection_AsciiString McCadGeneTool::ToAsciiString(const QString& theStr)
{
//qiu     char aStr[theStr.length()];
//qiu    QByteArray ba = theStr.toAscii();
//qiu    strcpy(aStr, ba.data());
//qiu    TCollection_AsciiString anAsciiStr(aStr);
	TCollection_AsciiString anAsciiStr(theStr.toStdString().c_str());
    return anAsciiStr;
}

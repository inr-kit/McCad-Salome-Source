#include <McCadGUI_BaseDialog.ixx>
#include <iostream>
#include <fstream>

McCadGUI_BaseDialog::McCadGUI_BaseDialog()
{
	myType = McCadGUI_Message;
	myLevel = McCadGUI_Warning_Level;
	myText = " ";
	// std::streambuf *myBuffer = myStream.rdbuf();
	// std::cout.rdbuf(myStream.rdbuf());
	// myStream.rdbuf(cout.rdbuf());
}

void McCadGUI_BaseDialog::Destroy()
{

}

void McCadGUI_BaseDialog::SetType(const McCadGUI_TypeOfDialog theType)
{
	myType = theType;
}

McCadGUI_TypeOfDialog McCadGUI_BaseDialog::GetType() const
{
	return myType;
}

void McCadGUI_BaseDialog::SetLevel(const McCadGUI_LevelOfDialog theLevel)
{
	myLevel = theLevel;
}

McCadGUI_LevelOfDialog McCadGUI_BaseDialog::GetLevel() const
{
	return myLevel;
}

void McCadGUI_BaseDialog::SetText(const TCollection_AsciiString& theText)
{
	myText = theText;
}

TCollection_AsciiString McCadGUI_BaseDialog::GetText() const
{
	return myText;
}

void McCadGUI_BaseDialog::Print()
{
	// myStream << myText << endl;
}

void McCadGUI_BaseDialog::Print(const TCollection_AsciiString& theText)
{
	myText = theText;
	//  myStream << myText << endl;
}

void McCadGUI_BaseDialog::Print(const McCadGUI_LevelOfDialog theLevel,	const TCollection_AsciiString& theText)
{
	myLevel = theLevel;
	myText = theText;
	// myStream << myText << endl;
}

void McCadGUI_BaseDialog::SetStream(Standard_OStream& theStream)
{
	// std::streambuf *myBuffer = myStream.rdbuf();
	// myStream.rdbuf(theStream.rdbuf());
}

Quantity_Color McCadGUI_BaseDialog::GetColor() const
{
    Quantity_Color aColor(Quantity_NOC_WHITE);
    return aColor;
}

void McCadGUI_BaseDialog::SetColor(const Quantity_Color& theColor)
{
}

TCollection_AsciiString McCadGUI_BaseDialog::SelectedFile() const
{
    TCollection_AsciiString aSelectedFile("none");
    return aSelectedFile;
}

TCollection_AsciiString McCadGUI_BaseDialog::SelectedFilter() const
{
    TCollection_AsciiString aSelectedFilter("none");
    return aSelectedFilter;
}

void McCadGUI_BaseDialog::SetSelectedFilter(
		const TCollection_AsciiString& theMask)
{
}

void McCadGUI_BaseDialog::SetSelection(
		const TCollection_AsciiString& theFileName)
{
}

Standard_Integer McCadGUI_BaseDialog::GetInteger() const
{
    Standard_Integer anInt(0);
    return anInt;
}

Standard_Real McCadGUI_BaseDialog::GetReal() const
{
    Standard_Real aReal(0.0);
    return aReal;
}

void McCadGUI_BaseDialog::SetTotalSteps(const Standard_Integer theTotalSteps)
{
}

void McCadGUI_BaseDialog::SetProgress(const Standard_Integer theProgressPositin)
{
}

void McCadGUI_BaseDialog::SetLabelText(
		const TCollection_AsciiString& theLabelText)
{
}

void McCadGUI_BaseDialog::SetCancelButtonText(
		const TCollection_AsciiString& theCancelButtonText)
{
}

void McCadGUI_BaseDialog::Cancel()
{
}


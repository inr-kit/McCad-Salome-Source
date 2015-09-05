#include <McCadGUI_FileDialog.ixx>

McCadGUI_FileDialog::McCadGUI_FileDialog()
{
  myFileName = " ";
  myFilter = " ";
}

TCollection_AsciiString McCadGUI_FileDialog::SelectedFile() const
{

  return myFileName;

}

TCollection_AsciiString McCadGUI_FileDialog::SelectedFilter() const
{
  return myFilter;
}

void McCadGUI_FileDialog::SetSelectedFilter(const TCollection_AsciiString& theMask)
{
  myFilter = theMask;
}

void McCadGUI_FileDialog::SetSelection(const TCollection_AsciiString& theFileName)
{

  myFileName = theFileName;
}



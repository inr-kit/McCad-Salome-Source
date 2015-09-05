#include <McCadGUI_ColorDialog.ixx>

McCadGUI_ColorDialog::McCadGUI_ColorDialog()
{
}

Quantity_Color McCadGUI_ColorDialog::GetColor() const
{
  return myColor;
}

void McCadGUI_ColorDialog::SetColor(const Quantity_Color& theColor)
{
  myColor = theColor;
}





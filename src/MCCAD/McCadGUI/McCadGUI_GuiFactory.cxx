#include <McCadGUI_GuiFactory.ixx>
#include <McCadGUI_ColorDialog.hxx>
#include <McCadGUI_FileDialog.hxx>
#include <McCadGUI_InputDialog.hxx>
#include <McCadGUI_ProgressDialog.hxx>


McCadGUI_GuiFactoryPtr McCadGUI_GuiFactory::myInstance = 0;

McCadGUI_GuiFactory::McCadGUI_GuiFactory()
{
	myCurrentType = McCadGUI_None;
}

McCadGUI_GuiFactoryPtr McCadGUI_GuiFactory::Instance()
{
	if (myInstance == 0)
	{
		myInstance = new McCadGUI_GuiFactory;
	}
	return myInstance;
}

void McCadGUI_GuiFactory::SetType(const McCadGUI_TypeOfDialog theType)
{
	myCurrentType = theType;
}

McCadGUI_TypeOfDialog McCadGUI_GuiFactory::GetType() const
{
	return myCurrentType;
}

McCadGUI_BaseDialogPtr McCadGUI_GuiFactory::MakeDialog() const
{
	if (myCurrentType == McCadGUI_None)
		return 0;
	switch (myCurrentType)
	{
	case McCadGUI_Base:
                return new McCadGUI_BaseDialog();
	case McCadGUI_Progress:
                return new McCadGUI_ProgressDialog();
	case McCadGUI_File:
                return new McCadGUI_FileDialog();
	case McCadGUI_Message:
                return new McCadGUI_BaseDialog();
	case McCadGUI_Error:
                return new McCadGUI_BaseDialog();
	case McCadGUI_Debug:
                return new McCadGUI_BaseDialog();
	case McCadGUI_Input:
                return new McCadGUI_InputDialog();
	case McCadGUI_Color:
                return new McCadGUI_ColorDialog();
	default:
		return 0;
	}
}


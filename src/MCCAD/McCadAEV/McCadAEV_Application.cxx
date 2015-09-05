#include <McCadAEV_Application.ixx>
#include <TCollection_ExtendedString.hxx>
#include <McCadEXPlug.hxx>
#include <McCadEXPlug_ExchangePlugin.hxx>
#include <McCadEXPlug_PluginManager.hxx>


McCadAEV_Application::McCadAEV_Application()
{
	myPluginManager = McCadEXPlug_PluginManager::Instance();
}

void McCadAEV_Application::Destroy()
{
	//delete myPluginManager;
}
OSD_Directory McCadAEV_Application::ResourceDir() const
{
	return myResourceDir;
}

void McCadAEV_Application::SetResourceDir(const OSD_Directory& theDir)
{
	myResourceDir = theDir;

}

void McCadAEV_Application::SetPluginPath(const OSD_Path& thePath)
{
	myPluginPath = thePath;
}

OSD_Path McCadAEV_Application::GetPluginPath() const
{
	return myPluginPath;
}

Standard_Boolean McCadAEV_Application::LoadPlugins()
{
	if (myPluginManager == NULL)
		myPluginManager = McCadEXPlug_PluginManager::Instance();

	if (!McCadEXPlug::PathExists(myPluginPath))
	{
		cout << "The Plugin Path seems not to exist!!  :  " << myPluginPath.Name().ToCString() << endl;
		return false;
	}
	if (!McCadEXPlug::PathIsReadable(myPluginPath))
	{
		cout << "The Plugin Path seems not Readable!!  :  " << myPluginPath.Name().ToCString() << endl;
		return false;
	}
	return myPluginManager->LoadSharedLibrary(myPluginPath);
}

Standard_Boolean McCadAEV_Application::UnLoadPlugins()
{
	if (myPluginManager == NULL)
		myPluginManager = McCadEXPlug_PluginManager::Instance();
	myPluginManager->UnLoadSharedLibrary();
	return true;
}

Handle(TColStd_HSequenceOfAsciiString) McCadAEV_Application::Formats() const
{
	return myPluginManager->GetFormats();
}

McCadEXPlug_ExchangePluginPtr McCadAEV_Application::GetPlugin(	const TCollection_AsciiString& theExtension) const
{
	return myPluginManager->GetPlugin(theExtension);
}

Handle(TColStd_HSequenceOfAsciiString) McCadAEV_Application::GetExtensions() const
{
	return myPluginManager->GetExtensions();

}

Standard_Boolean McCadAEV_Application::CheckExtension(	const TCollection_AsciiString& theExtension) const
{
	return myPluginManager->CheckExtension(theExtension);
}

void McCadAEV_Application::CloseEditor(const McCadAEV_EditorPtr& theEdPtr)
{
}

void McCadAEV_Application::OpenEditor(const McCadAEV_EditorPtr& theEdPtr)
{
}

void McCadAEV_Application::AddEditor(const McCadAEV_EditorPtr& theEdPtr)
{
}


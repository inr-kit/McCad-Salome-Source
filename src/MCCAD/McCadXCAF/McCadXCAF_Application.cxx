#include <McCadXCAF_Application.ixx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFApp_Application.hxx>

McCadXCAF_Application::McCadXCAF_Application()
{
}

 void McCadXCAF_Application::Formats(TColStd_SequenceOfExtendedString& theFormats)
{
	//theFormats.Append(TCollection_ExtendedString("XmlXCAF"));
	 XCAFApp_Application::GetApplication()->Formats(theFormats);
	 //theFormats.Append("McCad_Document");
}

 Standard_CString McCadXCAF_Application::ResourcesName()
{
	 return XCAFApp_Application::GetApplication()->ResourcesName();
	//return Standard_CString("XCAF");
	// return "Standard";
}


 const Handle(McCadXCAF_Application)& McCadXCAF_Application::GetApplication()
 {
	 static Handle(McCadXCAF_Application) anApp = new McCadXCAF_Application;
	 return anApp;
 }


void McCadXCAF_Application::InitDocument (const Handle(TDocStd_Document)& theDoc) const
{
	TDF_Label aL = theDoc->Main().FindChild (1, Standard_True); //0:1:1
	XCAFDoc_DocumentTool::Set (aL.FindChild (1, Standard_True), Standard_False); //0:1:1:1
}

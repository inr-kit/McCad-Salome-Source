#include <McCadAEV_Viewer.ixx>
//qiu #include <Graphic3d_GraphicDevice.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Aspect_DisplayConnection.hxx>

McCadAEV_Viewer::McCadAEV_Viewer()
{
	myViewer = NULL;
	myContext = NULL;
	myCollector = NULL;
	myName = "McCadViewer";
}

void McCadAEV_Viewer::Destroy()
{

}

void McCadAEV_Viewer::Update()
{
}

void McCadAEV_Viewer::Reconfigure()
{

}

Handle(AIS_InteractiveContext) McCadAEV_Viewer::GetContext() const
{
    return myContext;
}

void McCadAEV_Viewer::SetContext(const Handle(AIS_InteractiveContext)& theContext)
{
	myContext = theContext;
}

Handle(V3d_Viewer) McCadAEV_Viewer::MakeCasViewer(const Standard_CString theDisplay,
		const Standard_ExtString theName, const Standard_CString theDomain,
		const Standard_Real theViewSize,  const V3d_TypeOfOrientation theViewProj,
		const Standard_Boolean theCompMode,	const Standard_Boolean theDefCompMode)
{
//qiu	static Handle(Graphic3d_GraphicDevice) defaultdevice;
//qiu	if(defaultdevice.IsNull())
//qiu		defaultdevice = new Graphic3d_GraphicDevice(theDisplay);
    static Handle(Graphic3d_GraphicDriver) defaultdevice;
//	if(defaultdevice.IsNull())
//		defaultdevice = new Graphic3d_GraphicDriver(theDisplay);

        Handle(V3d_Viewer) aCascadeViewer = new V3d_Viewer(defaultdevice, theName, theDomain,
                                                           theViewSize, theViewProj, Quantity_NOC_GRAY30,
                                                           V3d_ZBUFFER, V3d_GOURAUD, V3d_WAIT,
                                                           theCompMode, theDefCompMode, V3d_TEX_NONE);
	return aCascadeViewer;
}

Handle(V3d_Viewer) McCadAEV_Viewer::GetViewer() const
{
	return myViewer;
}

void McCadAEV_Viewer::SetViewer(const Handle(V3d_Viewer)& theViewer)
{
	myViewer = theViewer;
}

TCollection_AsciiString McCadAEV_Viewer::GetName() const
{
	return myName;
}

void McCadAEV_Viewer::SetName(const TCollection_AsciiString& theName)
{
	myName = theName;
}


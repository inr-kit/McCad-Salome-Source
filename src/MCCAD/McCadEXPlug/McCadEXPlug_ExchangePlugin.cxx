#include <McCadEXPlug_ExchangePlugin.ixx>
#include <TDocStd_Document.hxx>

McCadEXPlug_ExchangePlugin::McCadEXPlug_ExchangePlugin()
{
}

Standard_Boolean McCadEXPlug_ExchangePlugin::Export(const Handle(TopTools_HSequenceOfShape)& theEntitySequence)
{
	return Standard_True;
}

Standard_Boolean McCadEXPlug_ExchangePlugin::Export(const Handle(McCadDiscDs_HSequenceOfDiscSolid)& theVEntitySequence)
{
	return Standard_True;
}

Handle(TopTools_HSequenceOfShape) McCadEXPlug_ExchangePlugin::Import()
{
	Handle(TopTools_HSequenceOfShape) newHSeq = new TopTools_HSequenceOfShape;
        return newHSeq;
}


// Import/Export to/from TDocStd_Document
Standard_Boolean McCadEXPlug_ExchangePlugin::ImportToDocument(Handle(TDocStd_Document)& theTDoc)
{   
    return Standard_True;
}

Standard_Boolean McCadEXPlug_ExchangePlugin::ExportFromDocument(const Handle(TDocStd_Document)& theTDoc)
{
	return Standard_True;
}

//________________



Standard_Integer McCadEXPlug_ExchangePlugin::GetModelType() const
{
	return 0;
}

void McCadEXPlug_ExchangePlugin::SetModelType(const Standard_Integer theType)
{
}

TCollection_AsciiString McCadEXPlug_ExchangePlugin::GetFilename()
{
	TCollection_AsciiString a(" ");
	return a;
}

void McCadEXPlug_ExchangePlugin::SetFilename(
		const TCollection_AsciiString& theFileName)
{
}

Standard_Integer McCadEXPlug_ExchangePlugin::GetSize() const
{
	return 0;
}

Handle(TColStd_HSequenceOfAsciiString) McCadEXPlug_ExchangePlugin::GetExtensions() const
{
    return NULL;
}

TCollection_AsciiString McCadEXPlug_ExchangePlugin::GetExtension() const
{
    TCollection_AsciiString anExtension(".none");
    return anExtension;
}

TCollection_AsciiString McCadEXPlug_ExchangePlugin::GetFormat() const
{
    TCollection_AsciiString aFormat("none");
    return aFormat;
}

Standard_Boolean McCadEXPlug_ExchangePlugin::ExportEnabled() const
{
    return Standard_False;
}

Standard_Boolean McCadEXPlug_ExchangePlugin::ImportEnabled() const
{
    return Standard_False;
}


McCadGUI_ProgressDialogPtr McCadEXPlug_ExchangePlugin::GetProgressDialog()
{
	return myProgress;
}


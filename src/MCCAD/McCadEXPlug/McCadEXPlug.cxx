#include <OSD_File.hxx>
#include <McCadEXPlug.hxx>

Standard_Boolean McCadEXPlug::PathExists(const OSD_Path& thePath)
{
	OSD_Path tmpPath = thePath;
	OSD_File theFile;
	theFile.SetPath(tmpPath);
	return theFile.Exists();
}

Standard_Boolean McCadEXPlug::PathIsReadable(const OSD_Path& thePath)
{
	OSD_Path tmpPath = thePath;
	OSD_File theFile;
	theFile.SetPath(tmpPath);
	return theFile.IsReadable();
}


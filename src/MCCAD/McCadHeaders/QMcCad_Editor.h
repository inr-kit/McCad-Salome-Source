#ifndef QMcCad_Editor_H
#define QMcCad_Editor_H

#include <map>
#include <QtGui/QMainWindow>
#include <QtCore/QString>
#include <QtGui/QPrinter>
#include <TCollection_AsciiString.hxx>

#include <McCadAEV_Editor.hxx>
#include <McCadCom_CasDocument.hxx>
#include <McCadCom_CasView.hxx>
#include <McCadDiscDs_HSequenceOfDiscSolid.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>

#include <cstdlib>
#include <string>
#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <list>
#include <algorithm>
#include <time.h>
#include <map>

#include <QtCore/QByteArray>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QCloseEvent>

#include <Aspect_FormatOfSheetPaper.hxx>

#include <AIS_InteractiveObject.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <TopTools_HSequenceOfShape.hxx>

#include <QMcCad_Application.h>
#include <QMcCad_Viewer.h>
#include <QMcCad_View.h>

#include <TopoDS.hxx>
#include <TopoDS_Solid.hxx>
#include <McCadCSGAdapt_SolidAnalyser.hxx>
#include <McCadCSGTool_Decomposer.hxx>

#include <McCadEXPlug.hxx>

#include <McCadEXPlug_ExchangePlugin.hxx>
#include <McCadEXPlug_PluginManager.hxx>
#include <McCadGUI_GuiFactory.hxx>

#include <McCadGUI_BaseDialog.hxx>
#include <McCadGUI_BaseDialogPtr.hxx>
#include <McCadGUI_TypeOfDialog.hxx>

#include <McCadEXPlug_PluginMaker.hxx>

#ifndef  _OSD_File_HeaderFile
#include <OSD_File.hxx>
#endif
#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>

#include <AIS_Shape.hxx>
#include <AIS_InteractiveObject.hxx>

#include <McCadCSGTool.hxx>
#include <McCadGTOOL.hxx>
#include <McCadCSGGeom_HSequenceOfCell.hxx>
#include <McCadCSGTool_Decomposer.hxx>
#include <McCadTDS_HSequenceOfExtSolid.hxx>
#include <McCadCSGTool_Extender.hxx>
#include <McCadCSGTool_CellBuilder.hxx>
#include <McCadCSGGeom_Cell.hxx>
#include <McCadCSGGeom_Surface.hxx>
#include <McCadCSGAdapt_SolidAnalyser.hxx>
#include <McCadCSGAdapt_ShapeAnalyser.hxx>
#include <McCadTDS_ExtSolid.hxx>
#include <McCadCSGAdapt_FaceAnalyser.hxx>
#include <McCadCSGBuild_SolidFromCSG.hxx>

#include <McCadCSGGeom_Plane.hxx>
#include <McCadCSGGeom_Cylinder.hxx>
#include <McCadCSGGeom_Cone.hxx>
#include <McCadCSGGeom_Sphere.hxx>
#include <McCadCSGGeom_Torus.hxx>

#include <McCadCSGGeom_DataMapOfIntegerSurface.hxx>
#include <McCadCSGGeom_DataMapIteratorOfDataMapOfIntegerSurface.hxx>

#include <McCadEXPlug.hxx>

#include <McCadEXPlug_ExchangePlugin.hxx>
#include <McCadEXPlug_PluginManager.hxx>
#include <McCadGUI_GuiFactory.hxx>

#include <McCadGUI_BaseDialog.hxx>
#include <McCadGUI_BaseDialogPtr.hxx>
#include <McCadGUI_TypeOfDialog.hxx>

#include <McCadEXPlug_PluginMaker.hxx>
#include <McCadMcWrite_McnpInputGenerator.hxx>

#include <McCadDiscDs_DiscFace.hxx>
#include <McCadDiscDs_DiscSolid.hxx>
#include <McCadDiscDs_HSequenceOfDiscSolid.hxx>
#include <McCadDiscDs_HSequenceOfDiscFace.hxx>
#include <McCadDiscretization_Solid.hxx>
#include <McCadDiscretization_Face.hxx>

#include <McCadMcVoid.hxx>
#include <McCadMcVoid_Generator.hxx>
#include <McCadMcVoid_ForwardCollision.hxx>

#include <TColStd_HSequenceOfAsciiString.hxx>

#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <TColStd_DataMapIteratorOfDataMapOfIntegerInteger.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopExp_Explorer.hxx>

#include <TopoDS_Face.hxx>
#include <TopAbs_Orientation.hxx>

#include <TColStd_HSequenceOfReal.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <TColgp_HSequenceOfPnt.hxx>
#include <TopTools_HSequenceOfShape.hxx>

#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <gp_Pnt.hxx>

#include <ElCLib.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRep_Tool.hxx>

#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>

#include <AIS_ListIteratorOfListOfInteractive.hxx>

#include <QMcCad_Application.h>
///////////////////////////////////////////////////////////
#include <McCadViewTool_FrontProject.hxx>

#ifndef  _OSD_File_HeaderFile
#include <OSD_File.hxx>
#endif
#include <QtCore/QThread>
#include <QtCore/QReadWriteLock>

#include <QMcCad_ActionMaster.h>
#include <qprogressbar.h>

#include <QMcCad_VGParamDialog.h>

#include <TDocStd_Document.hxx>

#include <McCadMDReader_DataMapOfLabelAsciiString.hxx>
#include <TPrsStd_AISViewer.hxx>
#include <TDF_LabelSequence.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <TPrsStd_AISViewer.hxx>
#include <McCadMessenger_MessageType.hxx>
//#include <McCadXCAF_Application.hxx>

#include <QMcCadGeomeTree_TreeWidget.hxx>

#include "../McCadTool/MaterialManager.hxx"
#include "../McCadTool/Material.hxx"

using namespace std;

class QMcCad_Viewer;
class QMcCad_View;
class QMcCad_ActionMaster;

class QMcCad_Editor : public QMainWindow, public McCadAEV_Editor
 {
    Q_OBJECT

public:

    QMcCad_Editor(QWidget* theParent, const char* theName=NULL, const Standard_Integer& theID = -1);
    ~QMcCad_Editor();

    Handle(McCadCom_CasDocument) GetDocument();
    QMcCad_View* GetView();
    QMcCad_Viewer* GetViewer();
    void SetView(QMcCad_View* theView);

    void Import( const QString& theFileName);
    bool Save();
    //void Close();
    bool Save(const TCollection_AsciiString& saveName );
    bool Export(const TCollection_AsciiString& theFileName);

    bool Export();

    void Hardcopy(const QString& theFileName);
    void Print(QPrinter* thePrinter);

    void SetCurrentFilter(QString theFilter);
    QString GetCurrentFilter();

    void Mcout(const QString& aMessage, McCadMessenger_MessageType msgType = McCadMessenger_DefaultMsg);

    void SetDocument(Handle(McCadCom_CasDocument) theDoc);
    void SetViewer(QMcCad_Viewer* theViewer);

    QMcCad_ActionMaster* GetActionMaster();

    QString GetQFileName();

    void SetID(Standard_Integer theID);
    Standard_Integer ID();

    TDF_LabelSequence* GetLabelSequence();
    Handle(TPrsStd_AISViewer) GetTViewer();

    void UpdateWindowTitle();
    void UpdateView();

    void GradientBackground();

    void MakeRotationMoviePictures();

    void SetIsLoaded(Standard_Boolean status);

    MaterialManager* GetMatManager();
    //void SetMatManager(MaterialManager *&theManager);

    // Lei Lu, 21.11.2013
    Standard_Boolean AddTreeItem(TCollection_AsciiString, QTreeWidgetItem *&); // Add a lable and corresponding tree item
    QTreeWidgetItem * GetTreeItem(TCollection_AsciiString);         // Get a tree item from the lable input
    TCollection_AsciiString GetSolidFromItem(QTreeWidgetItem *& pItem);
    void DeleteSelectedItem(TCollection_AsciiString theLabel);

    void DeleteTreeItem();                                          // Delete the tree items when delete this editor

protected:
    void closeEvent(QCloseEvent* e);

private:
    void InitDocument(); // create View and viewer
    void InitDocument(Handle(McCadCom_CasDocument) theDoc);
    void InitViewer(); // create View and viewer
    void InitTDoc();

public slots:
	void SlotViewModified();
	void updateProgress(int);
	virtual void keyPressEvent(QKeyEvent* e);
	virtual void keyReleaseEvent(QKeyEvent* e);

signals:
	void keyPressed(QKeyEvent*);
	void keyReleased(QKeyEvent*);
	void updateMainTitle();
	void saveAs();
	void destroyed(int);
	void updateProgress(int, QString);

private:
	QProgressBar* myProgressBar;
    Handle(McCadCom_CasDocument) myDoc;
    QMcCad_Viewer* myViewer;
    QString myCurrentFilter;
	Standard_Integer myID;
	TDF_LabelSequence myLabelSequence;
	TCollection_AsciiString mySaveName;
	Standard_Boolean myIsLoaded;

    MaterialManager * m_pMatManager;
    /** create a map to store the relationship of tree item and each solid in view, TreeMap, Lei Lu*/
    QMap<TCollection_AsciiString, QTreeWidgetItem*> m_IDLabelItemMap;
    QMap<QTreeWidgetItem*, TCollection_AsciiString> m_ItemSolidMap;
};


#endif


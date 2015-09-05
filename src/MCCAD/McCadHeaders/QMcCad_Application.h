#ifndef QMcCad_Application_H
#define QMcCad_Application_H

#include <QtGui/QMainWindow>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QMap>
#include <V3d_Coordinate.hxx>

#include <McCadAEV_Application.hxx>
#include <McCadCom_CasDocument.hxx>
#include <QMcCad_LogWindow.h>
#include <qprogressdialog.h>
#include <Graphic3d_NameOfMaterial.hxx>
#include <Handle_McCadXCAF_Application.hxx>
#include <McCadGUI_ProgressDialog.hxx>
#include <McCadMessenger_MessageType.hxx>
#include <McCadMessenger_Singleton.hxx>
#include <string>
#include <QMcCadMessenger_MainWindowMessage.hxx>

//class QMcCad_PDialog;
class QWorkspace;
// class QAssistantClient;
class QMcCad_ActionFactory;
class QMcCad_ActionMaster;
class QMcCad_Editor;
class QMcCad_View;
class QTextEdit;
class QAction;
class QEvent;
class QLabel;
class FindDialog;
class Spreadsheet;
class McCadXCAF_Application;
class QMcCadGeomeTree_TreeWidget;

//! \brief Main GUI class

/*! Application Class; this is the GUI's core class
 *
 */


class QMcCad_Application : public QMainWindow, public McCadAEV_Application
{
  Q_OBJECT

public:

  //! ctor
  QMcCad_Application (QWidget *parent = 0, const char *name = 0);

  //! Returns the QWorkspace
  QWorkspace* GetWorkSpace();

  //! Return current Editor <br>
  QMcCad_Editor* GetEditor();

  //! Get Editor by its ID
  QMcCad_Editor* GetEditor(int theID);

  //! Returns the view from the current Editor
  QMcCad_View*  GetView();

  //! Returns the Document
  Handle_McCadCom_CasDocument GetDocument();

  //! Returns a pointer on the tree widget
  QMcCadGeomeTree_TreeWidget* GetTreeWidget();

  //! This function is used to define wether a file should be opened right after start up <br>
  //! for when a file is passed in the command line as argument.
  void SetOpenFile(const TCollection_AsciiString& inName);

  //! return singleton this
  static QMcCad_Application* GetAppMainWin()
    {
      return myApp;
    }

  //! returns a pointer on the Action master. The action master links all qactions
  QMcCad_ActionMaster* GetActionMaster()
    {
      return myAcM;
    }

  //! Returns a Handle on the TDocStd_Application - this enables OCAF (open cascade application framework)
  Handle_TDocStd_Application GetXCAFApp();

  //! cout into log window
//  void Mcout(const QString& aMessage, McCadGUI_MessageType = McCadGUI_DefaultMsg);

  //! cout into log window
  void Mcout(const std::string& message, McCadMessenger_MessageType = McCadMessenger_DefaultMsg);

  //! return name of current file of current editor as QString
  QString GetCurrentFile();

  //! set the background color in current editor's view window
  void SetBgColor(const QColor theCol) ;

  //! Receive the background color of current editor's view window
  QColor GetBgColor();

  //! Create a new TDocStd_Document
  Standard_Boolean NewDocument(Handle(TDocStd_Document)& theTDoc) ;

 protected:
  //! handles a close event
  void closeEvent(QCloseEvent *event);

  //! handles context menues
  void contextMenuEvent(QContextMenuEvent *event);



private slots:

  void SlotFileNew();
  void SlotFileOpen();
  void SlotFileOpen(QString& fileName);
  void SlotFileClose();
  void SlotFileSave();
  void SlotFileSaveAs();
  void SlotImport();
  void SlotExport();
  void SlotHardcopy();
  void SlotPrint();
  void SlotQuit();
  void OpenRecentFile();

  void SlotAbout();
  void SlotHelp();
  void SlotUpdateMenus();
  void SlotEditorClosed(int);

  void SlotUpdateMainTitle();


 private:

  void CreateActions();
  void CreateMenus();
  void CreateToolBars();
  void CreateStatusBar();
  void CreateLogWindow();
  //void CreateCADPropDock();
  void CreateTreeView();
  void CreateProgressDialog();
  void ReadSettings();
  void WriteSettings();


  void SetCurrentFile(const QString &fileName);

  void UpdateRecentFileItems();


  QString StrippedName(const QString &fullFileName);

  QStringList myRecentFiles;
  enum { MaxRecentFiles = 10 };
  int myRecentFileIds[MaxRecentFiles];
  QAction *myRecentFileActions[MaxRecentFiles];

  QMenu *myContextMenu;
  QMenu *myFileMenu;
  QMenu *myEditMenu;
  QMenu *myViewMenu;
  QMenu *myZoomSubMenu;
  QMenu *myProjectionSubMenu;
  QMenu *myShadeSubMenu;
  QMenu *myInsertMenu;
  QMenu *myAssistMenu;
  QMenu *myDesignMenu;
  QMenu *myModifyMenu;
  QMenu *myConvertMenu;
  QMenu *myBooleanSubMenu;

  QMenu *myWindowMenu;
  QMenu *myHelpMenu;

  QToolBar *myFileToolBar;
  QToolBar *myViewToolBar;
  QToolBar *myConvertBar;

  // QActions
  QAction *myActionFileNew;
  QAction *myActionFileOpen;
  QAction *myActionFileClose;
  QAction *myActionFileSave;
  QAction *myActionFileSaveAs;
  QAction *myActionImportGeom;
  QAction *myActionExportGeom;
  QAction *myActionHardCopy;
  QAction *myActionPrint;
  QAction *myActionEditUndo;
  QAction *myActionEditRedo;
  QAction *myActionEditCut;
  QAction *myActionEditCopy;
  QAction *myActionEditPaste;
  QAction *myActionSelect;
  QAction *myActionSelectAll;
  QAction *myActionDeselectAll;
  QAction *myActionRedraw;
  QAction *myActionZoomWindow;
 // QAction *myActionZoomDynamic;
 // QAction *myActionZoomScale;
  QAction *myActionFitAll;
  QAction *myActionZoomIn;
  QAction *myActionZoomOut;
  QAction *myActionPan;
  QAction *myActionRotate;
  QAction *myActionBackProject;
  QAction *myActionFrontProject;
  QAction *myActionTopProject;
  QAction *myActionBottomProject;
  QAction *myActionLeftProject;
  QAction *myActionRightProject;
  QAction *myActionWireFrame;
  QAction *myActionFlatShade;
  QAction *myActionMeasureDistance;
  QAction *myActionMeasureAngle;
  QAction *myActionMeasureArea;
  QAction *myActionMeasureVolume;
  QAction *myActionPropertyCurve;
  QAction *myActionPropertySurface;
  QAction *myActionMakeBox;
  QAction *myActionMakeCone;
  QAction *myActionMakeCylinder;
  QAction *myActionMakeHalfSpace;
  QAction *myActionMakeSphere;
  QAction *myActionMakeTorus;
  QAction *myActionMakeWedge;
  QAction *myActionModelErase;
  QAction *myActionModelCopy;
  QAction *myActionModelMirror;
  QAction *myActionModelMove;
  QAction *myActionModelRotate;
  QAction *myActionModelScale;
  QAction *myActionModelStrech;
  QAction *myActionSolidUnion;
  QAction *myActionSolidIntersection;
  QAction *myActionSolidDifference;
  QAction *myActionBackgroundColor;
  QAction *myActionShapeColor;
  QAction *myActionShapeMaterial;
  QAction *myActionShapeTransparency;
  QAction *myActionAntialiasing;
  QAction *myActionSetHiddenLine;
  QAction *myActionSetGrid;
  QAction *myActionSetAxis;
  QAction *myActionQuit;
  QAction *myActionAbout;
  QAction *myActionHelp;
  QAction *myActionConvertModel;
  QAction *myActionGenerateVoids;
  QAction *myActionExplodeModel;
  QAction *myActionDecomposeModel;
  QAction *myActionDisplaySelectedOnly;
  QAction *myActionHideSelected;
  QAction *myActionShowAll;
  QAction *myActionRedisplaySelected;
  QAction *myActionAddMaterialGroup;
  QAction *myActionAddToMaterialGroup;
  QAction *myActionClippingPlane;
  QAction *myActionReadValueList;

  QList<QAction*> myFileActionList;
  QList<QAction*> myEditActionList;
  QList<QAction*> myViewActionList;
  QList<QAction*> myAssistActionList;
  QList<QAction*> myDesignActionList;
  QList<QAction*> myModifyActionList;
  QList<QAction*> myWindowActionList;
  QList<QAction*> myConvertActionList;


 private:

  McCadMessenger_Singleton* m_messenger;

  QLabel *myReadyLabel;
  QMcCad_ActionMaster* myAcM;
  static QMcCad_Application* myApp;

  QWorkspace* myWS;
  QPrinter *myPrinter;
  QMcCad_LogWindow* myOut;
  QString myCurrentFileName;
//  static QAssistantClient* myAssistant;

  Handle_McCadXCAF_Application myXCAFApp;
  Standard_Integer myEditorIDCnt;

  QMcCadGeomeTree_TreeWidget* myTreeView;
  QProgressDialog* myPDialog;
  QString myWorkingDirectory;
  QMap<int, QMcCad_Editor*> myIDEditorMap;
  QColor myBgCol;
  bool myLoadFileAtStartUp;

};

#endif

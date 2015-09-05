#ifndef QMcCad_ActionMaster_H
#define QMcCad_ActionMaster_H

#include <QtCore/QObject>
#include <QtCore/QReadWriteLock>

#include <McCadTool_Task.hxx>
#include <McCadTool_TaskHistory.hxx>
//#include "QMcCad_Editor.h"



class QMcCad_ActionMaster : public QObject
{
  Q_OBJECT

    public:

  QMcCad_ActionMaster();
  virtual ~QMcCad_ActionMaster();

  Handle(McCadTool_Task) GetCurrentAction();
  void SetCurrentAction(McCadTool_TaskID theID);
  void Mcout(const QString & aMessage);

public slots:

    //edit

  void SlotUndo();
  void SlotRedo();
  void SlotCut();
  void SlotCopy();
  void SlotPast();
  void SlotSelect();
  void SlotSelectAll();
  void SlotDeselectAll();

  // view
  void SlotRedraw();
  void SlotZooomWindow();
  void SlotZoomDynamic();
  void SlotDispSelOnly();
  void SlotRedisplaySelected();
  void SlotShowAll();
  void SlotHideSelected();
  //void SlotZoomScale();
  void SlotZoomIn();
  void SlotZoomOut();
  void SlotFitAll();
  void SlotPan();
  void SlotRotate();
  void SlotBackProject();
  void SlotFrontProject();
  void SlotTopProject();
  void SlotBottomProject();
  void SlotLeftProject();
  void SlotRightProject();
  void SlotWireFrame();
  void SlotShade();

  // assist
  void SlotDistance();
  void SlotAngle();
  void SlotArea();
  void SlotVolume();
  void SlotCurveProperty();
  void SlotSurfaceProperty();
  void SlotReadValueList();

  // model
  void SlotMakeBox();
  void SlotMakeCone();
  void SlotMakeCylinder();
  void SlotMakeHalfSpace();
  void SlotMakeSphere();
  void SlotMakeTorus();
  void SlotMakeWedge();

  // modify
  void SlotErase();
  void SlotModelCopy();
  void SlotMirror();
  void SlotMove();
  void SlotModelRotate();
  void SlotScale();
  void SlotStrech();
  void SlotUnion();
  void SlotIntersection();
  void SlotDifference();

  //window
  void SlotBackgroundColor();
  void SlotShapeColor();
  void SlotShapeMaterial();
  void SlotShapeTransparency();
  void SlotAntialiasing();
  void SlotSetHiddenLine();
  void SlotSetAxis();
  void SlotSetGrid();

  //QMcCadGeomeTree related stuff
  void SlotAddMaterialGroup();
  void SlotAddToMaterialGroup();

  //void SlotEditMaterialCards(); // Lei 14/10/2013
  void SlotClippingPlane();

 signals:
//MYMOD
  void ActionStarted();


 private:
McCadTool_TaskHistory myTaskHistory;
  //  QMcCad_Application* myMainAppWin;

};

#endif

// EOF

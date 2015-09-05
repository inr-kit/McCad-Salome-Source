#ifndef QMcCad_View_H
#define QMcCad_View_H

#include <QtOpenGL/QGLWidget>
#include <QtGui/QWidget>
#include <AIS_InteractiveContext.hxx>
#include <V3d_Viewer.hxx>

#include <McCadCom_CasView.hxx>
#include <McCadCom_CasDocument.hxx>
#include <V3d_View.hxx>
#include <Handle_McCadTool_Task.hxx>
#include <McCadTool_TaskID.hxx>

#include <qrubberband.h>

class QMcCad_Viewer;
class QMcCad_ActionMaster;


class QMcCad_View :  public QGLWidget, public McCadCom_CasView
{
    Q_OBJECT

public:
    QMcCad_View(QWidget* theParent, QMcCad_Viewer* theViewer,  Handle(McCadCom_CasDocument) theDoc);
    ~QMcCad_View();
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void ClippingPlaneDialog();
    void SetClippingPlaneParameters(Standard_Real a, Standard_Real b, Standard_Real c, Standard_Real d, Standard_Boolean state);

signals:

   void InitFinished();
   void viewModified();
   void SigSelectionChanged();

protected:
   void initializeGL();
   void resizeGL(int width, int height);
   void paintGL();

public slots:

  virtual void mousePressEvent ( QMouseEvent* e);
  virtual void mouseReleaseEvent ( QMouseEvent* e);
  virtual void mouseMoveEvent ( QMouseEvent* e);
  //virtual void paintEvent ( QPaintEvent * e);
  // virtual void resizeEvent ( QResizeEvent* e);

  virtual void LeaveEvent(QEvent*);
  virtual void EnterEvent(QEvent*);
  virtual void FocusInEvent(QFocusEvent*);
  virtual void FocusOutEvent(QFocusEvent*);
  virtual void wheelEvent(QWheelEvent* e);
  virtual void keyPressEvent(QKeyEvent* e);
  virtual void keyReleaseEvent(QKeyEvent* e);
  void SlotInitTask();
  void PreExecute();

 private:

  void Select();
  void ShiftSelect();
  void DrawRectangle();
  void Execute();
  Standard_Boolean ConvertToPlane(const Standard_Integer Xs, const Standard_Integer Ys, Standard_Real& X, Standard_Real& Y, Standard_Real& Z);


 private:

  QMcCad_Viewer* myViewer;
  QMcCad_ActionMaster* myAcM;
  Standard_Integer myXmin;
  Standard_Integer myYmin;
  Standard_Integer myXmax;
  Standard_Integer  myYmax;
  Standard_Boolean  myIsInit;
  Standard_Boolean  mySelectionMode;
  Standard_Boolean  myRectangleMode;
  Standard_Boolean myShiftSelect;
  Standard_Boolean myControlDown;
  //CAD mouse cursor
  Standard_Boolean myLMBpushed;
  Standard_Boolean myMMBpushed;
  McCadTool_TaskID myCurrentTID;
  QRubberBand* myRubberBand;
  Standard_Boolean myAxisSet;
  Standard_Real myCPA, myCPB, myCPC, myCPD;
  Standard_Boolean myCPVisState;

};

#endif

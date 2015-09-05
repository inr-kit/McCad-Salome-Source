#ifndef QMcCad_ActionFactory_H
#define QMcCad_ActionFactory_H


#include <QtGui/QAction>
#include <QtCore/QObject>
#include <QtGui/QWidget>

#include <McCadTool_TaskID.hxx>

class QMcCad_ActionFactory : public QObject 
{
  Q_OBJECT
    
    public:

  QMcCad_ActionFactory(QWidget* theWin);

  virtual ~QMcCad_ActionFactory();
  
  QAction* CreateAction(McCadTool_TaskID theID, QObject* aMaster);

private:
  QWidget* myWin;
};

#endif


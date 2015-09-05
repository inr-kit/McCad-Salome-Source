#ifndef QMcCad_Viewer_H
#define QMcCad_Viewer_H

#include <QtGui/QWidget>

#include <McCadAEV_Viewer.hxx>
#include <McCadCom_CasView.hxx>

class QMcCad_Editor;
class QMcCad_View;
class QMcCad_ActionMaster;

class QMcCad_Viewer : public McCadAEV_Viewer
{
	// Q_OBJECT

public:

	QMcCad_Viewer(QMcCad_Editor* theEditor);
	virtual ~QMcCad_Viewer();

	virtual void Update();
	virtual void Reconfigure();

	QMcCad_Editor* GetEditor();

	QMcCad_View* GetView();

	void SetView(QMcCad_View* theView);

	void Mcout(const QString & aMessage);

	QMcCad_ActionMaster* GetActionMaster();

	bool IsConfigured()
	{
		return myIsConfigured;
	}

	void SetViewer(Handle(V3d_Viewer)& theViewer)
	{
		myViewer = theViewer;
	}

	Handle(V3d_Viewer) GetViewer() const
	{
		return myViewer;
	}

private:

	QMcCad_View* myView;
	QMcCad_Editor* myEditor;
	bool myIsConfigured;
};

#endif


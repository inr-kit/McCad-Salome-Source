/*
 * QMcCad_TrnapsparencyDialog.h
 *
 *  Created on: Jul 18, 2008
 *      Author: grosse
 */
#ifndef QMCCAD_ClippingPlaneDialog_H_
#define QMCCAD_ClippingPlaneDialog_H_

#include <QPushButton>
#include <QDialog>
#include <ui_QMcCad_ClippingPlane.h>
#include <McCadCSGGeom_Unit.hxx>
#include <V3d_Plane.hxx>
#include <gp_Dir.hxx>
#include <QMcCad_View.h>
#include <QMcCad_Application.h>

class QMcCad_ClippingPlaneDialog : public QDialog
{
	Q_OBJECT

public:
	QMcCad_ClippingPlaneDialog(QWidget* theFather = NULL);
	void SetInitialValues(Standard_Real& a, Standard_Real& b, Standard_Real& c, Standard_Real& d, Standard_Boolean& visuState);

public slots:
	void IsAccepted();
	void VisuStateChanged(int chVal);
	void SliderValChanged(int slVal);
	void IsCanceled();
	void LineEdited();

private:

// Functions
	void UpdateClippingPlane(Standard_Real a, Standard_Real b, Standard_Real c, Standard_Real d);
	void GetFormData( TCollection_AsciiString& locX, TCollection_AsciiString& locY, TCollection_AsciiString& locZ ,
			            TCollection_AsciiString& dirX, TCollection_AsciiString& dirY, TCollection_AsciiString& dirZ,
			            TCollection_AsciiString& a, TCollection_AsciiString& b, TCollection_AsciiString& c, TCollection_AsciiString& d);
// Fields
	Ui::ui_ClippingPlaneDialog uiClipDial;
	Standard_Real myA, myB, myC, myD;
	Standard_Boolean myVisuState;
	Handle(V3d_Plane) myPln3D;
	gp_Dir myDir;
	Standard_Boolean myEntriesChanged;
};

#endif /* QMCCAD_ClippingPlaneDialog_H_ */

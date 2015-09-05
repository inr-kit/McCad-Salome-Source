/*
 * QMcCad_ClippingPlaneDialog.cpp
 *
 *  Created on: Jul 18, 2008
 *      Author: grosse
 */
#include <QMcCad_ClippingPlaneDialog.h>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_InteractiveContext.hxx>
#include <QMcCad_Application.h>
#include <QMcCad_Editor.h>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <V3d_Plane.hxx>
#include <V3d_Viewer.hxx>
#include <gp_Pln.hxx>
#include <gp_Ax1.hxx>

TCollection_AsciiString GetAStr(QString str)
{
	char aStr[str.length()];
	QByteArray ba = str.toAscii();
	strcpy(aStr, ba.data());
	TCollection_AsciiString anAsciiStr(aStr);
	return anAsciiStr;
	//return anAsciiStr.RealValue();
}

QMcCad_ClippingPlaneDialog::QMcCad_ClippingPlaneDialog(QWidget* theFather) : QDialog(theFather)
{
	uiClipDial.setupUi(this);
	myA=0;
	myB=0;
	myC=0;
	myD=0;

	myPln3D = NULL;

	myVisuState = Standard_False;
	myEntriesChanged = Standard_False;

	connect(uiClipDial.plnSlider, SIGNAL(valueChanged(int)), this, SLOT(SliderValChanged(int)));
	connect(uiClipDial.checkBox, SIGNAL(stateChanged(int)), this, SLOT(VisuStateChanged(int)));
	connect(uiClipDial.okButton, SIGNAL(clicked()), this, SLOT(IsAccepted()));
	connect(uiClipDial.cancelButton, SIGNAL(clicked()), this, SLOT(IsCanceled()));
        connect(uiClipDial.dirX, SIGNAL(editingFinished()), this, SLOT(LineEdited()));
        connect(uiClipDial.dirY, SIGNAL(editingFinished()), this, SLOT(LineEdited()));
        connect(uiClipDial.dirZ, SIGNAL(editingFinished()), this, SLOT(LineEdited()));
        connect(uiClipDial.locX, SIGNAL(editingFinished()), this, SLOT(LineEdited()));
        connect(uiClipDial.locY, SIGNAL(editingFinished()), this, SLOT(LineEdited()));
        connect(uiClipDial.locZ, SIGNAL(editingFinished()), this, SLOT(LineEdited()));
}


void QMcCad_ClippingPlaneDialog::LineEdited()
{
    myEntriesChanged = Standard_True;
}


void QMcCad_ClippingPlaneDialog::SliderValChanged(int slVal)
{
	//TCollection_AsciiString valStr(slVal);
	Standard_Real v(slVal);
	v /= 100.;

	if(myA == 0 && myB == 0 && myC == 0 && myD == 0)
		return;

	/*TCollection_AsciiString locX, locY, locZ, dirX, dirY, dirZ, a, b, c, d;
	GetFormData(locX, locY, locZ, dirX, dirY, dirZ, a, b, c, d);*/
	Standard_Real a,b,c,d;
	gp_Pln pln(myA, myB, myC, myD);
	gp_Pnt loc = pln.Location();
	gp_Pnt newLoc(loc.X()+myDir.X()*v, loc.Y()+myDir.Y()*v, loc.Z()+myDir.Z()*v);
	pln.SetLocation(newLoc);
	pln.Coefficients(a, b, c, d);
	if(myPln3D != NULL)
		QMcCad_Application::GetAppMainWin()->GetView()->View()->SetPlaneOff();

	// show vals
	TCollection_AsciiString aTAS(a), bTAS(b), cTAS(c), dTAS(d);
	uiClipDial.A->setText(aTAS.ToCString());
	uiClipDial.B->setText(bTAS.ToCString());
	uiClipDial.C->setText(cTAS.ToCString());
	uiClipDial.D->setText(dTAS.ToCString());
	TCollection_AsciiString xLocTAS(newLoc.X()), yLocTAS(newLoc.Y()), zLocTAS(newLoc.Z());
	uiClipDial.locX->setText(xLocTAS.ToCString());
	uiClipDial.locY->setText(yLocTAS.ToCString());
	uiClipDial.locZ->setText(zLocTAS.ToCString());

	if(myVisuState)
		UpdateClippingPlane(a,b,c,d);
}

void QMcCad_ClippingPlaneDialog::VisuStateChanged(int chVal)
{
	myVisuState = chVal;

	if(!myVisuState)
	{
		if(myPln3D != NULL)
			QMcCad_Application::GetAppMainWin()->GetView()->View()->SetPlaneOff();

		QMcCad_Application::GetAppMainWin()->GetEditor()->GetViewer()->GetViewer()->Update();
		return;
	}

	//if(myA == 0 && myB == 0 && myC == 0 && myC == 0)
		IsAccepted();
	/*else
		UpdateClippingPlane(myA, myB, myC, myD);*/
}

void QMcCad_ClippingPlaneDialog::IsCanceled()
{
	VisuStateChanged(0);
}


void QMcCad_ClippingPlaneDialog::SetInitialValues(Standard_Real& a, Standard_Real& b, Standard_Real& c, Standard_Real& d, Standard_Boolean& visuState)
{
	myVisuState = visuState;
	if(myVisuState)
		uiClipDial.checkBox->setCheckState(Qt::Checked);

	myA = a;
	myB = b;
	myC = c;
	myD = d;

	gp_Pln tmpPln(a,b,c,d);
	gp_Ax1 axis = tmpPln.Axis();
	gp_Dir dir = axis.Direction();

	if(a!=0 || b!=0 || c!=0 || d!=0)
	{
		TCollection_AsciiString xdir(dir.X());
		TCollection_AsciiString ydir(dir.Y());
		TCollection_AsciiString zdir(dir.Z());

		uiClipDial.dirX->setText(xdir.ToCString());
		uiClipDial.dirY->setText(ydir.ToCString());
		uiClipDial.dirZ->setText(zdir.ToCString());
	}

	SliderValChanged(0);
}


void QMcCad_ClippingPlaneDialog::IsAccepted()
{
	TCollection_AsciiString locX, locY, locZ, dirX, dirY, dirZ, a, b, c, d;

	if(!myVisuState) // canceled
		return;

	locX = GetAStr(uiClipDial.locX->text());
	locY = GetAStr(uiClipDial.locY->text());
	locZ = GetAStr(uiClipDial.locZ->text());
	dirX = GetAStr(uiClipDial.dirX->text());
	dirY = GetAStr(uiClipDial.dirY->text());
	dirZ = GetAStr(uiClipDial.dirZ->text());
	a = GetAStr(uiClipDial.A->text());
	b = GetAStr(uiClipDial.B->text());
	c = GetAStr(uiClipDial.C->text());
	d = GetAStr(uiClipDial.D->text());

	if(!QMcCad_Application::GetAppMainWin()->GetView()->View()->IfMorePlanes())
		return;

	if(!a.IsEmpty() && !b.IsEmpty() && !c.IsEmpty() && !d.IsEmpty() && !myEntriesChanged)
	{
		if(a.IsRealValue() && b.IsRealValue() && c.IsRealValue() && d.IsRealValue())
		{
			myA = a.RealValue();
			myB = b.RealValue();
			myC = c.RealValue();
			myD = d.RealValue();

			gp_Pln aPln(myA, myB, myC, myD);
			gp_Ax1 ax1 = aPln.Axis();
			myDir = ax1.Direction();
		}
		else
			return;
	}
	else if(!locX.IsEmpty() && !locY.IsEmpty() && !locZ.IsEmpty() &&
			!dirX.IsEmpty() && !dirY.IsEmpty() && !dirZ.IsEmpty()  )
	{
		myEntriesChanged = Standard_False;
		if(locX.IsRealValue() && locY.IsRealValue() && locZ.IsRealValue() &&
		   dirX.IsRealValue() && dirY.IsRealValue() && dirZ.IsRealValue()  )
		{
			gp_Dir aDir(dirX.RealValue(), dirY.RealValue(), dirZ.RealValue());
			myDir = aDir;
			gp_Pln pln(gp_Pnt(locX.RealValue(), locY.RealValue(), locZ.RealValue()), myDir );
			pln.Coefficients(myA,myB,myC,myD);
		}
		else
			return;
	}
	else
	{
		// False Entry ...
		return;
	}
	if(!myVisuState)
	{
		if(myPln3D != NULL)
			QMcCad_Application::GetAppMainWin()->GetView()->View()->SetPlaneOff(myPln3D);
		return;
	}

	QMcCad_Application::GetAppMainWin()->GetView()->SetClippingPlaneParameters(myA, myB, myC, myD, myVisuState);

	UpdateClippingPlane(myA, myB, myC, myD);
}

void QMcCad_ClippingPlaneDialog::UpdateClippingPlane(Standard_Real a, Standard_Real b, Standard_Real c, Standard_Real d)
{
        Handle(V3d_Viewer) theViewer = QMcCad_Application::GetAppMainWin()->GetEditor()->GetViewer()->GetViewer();
#ifdef OCC650
        myPln3D = new V3d_Plane(theViewer, a, b, c, d);
#else
        myPln3D = new V3d_Plane(a, b, c, d);
#endif
	QMcCad_Application::GetAppMainWin()->GetView()->View()->SetPlaneOn(myPln3D);
	theViewer->Update();
}


void QMcCad_ClippingPlaneDialog::GetFormData(TCollection_AsciiString& locX, TCollection_AsciiString& locY, TCollection_AsciiString& locZ ,
				TCollection_AsciiString& dirX, TCollection_AsciiString& dirY, TCollection_AsciiString& dirZ,
				TCollection_AsciiString& a, TCollection_AsciiString& b, TCollection_AsciiString& c, TCollection_AsciiString& d)
{
	locX = GetAStr(uiClipDial.locX->text());
	locY = GetAStr(uiClipDial.locY->text());
	locZ = GetAStr(uiClipDial.locZ->text());
	dirX = GetAStr(uiClipDial.dirX->text());
	dirY = GetAStr(uiClipDial.dirY->text());
	dirZ = GetAStr(uiClipDial.dirZ->text());
	a = GetAStr(uiClipDial.A->text());
	b = GetAStr(uiClipDial.B->text());
	c = GetAStr(uiClipDial.C->text());
	d = GetAStr(uiClipDial.D->text());
}

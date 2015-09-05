/*
 * QMcCad_VGParamDialog.cpp
 *
 *  Created on: Jul 18, 2008
 *      Author: grosse
 */
#include <QMcCad_TransparencyDialog.h>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_InteractiveContext.hxx>
#include <QMcCad_Application.h>
#include <QMcCad_Editor.h>
#include <AIS_ListIteratorOfListOfInteractive.hxx>


QMcCad_TransparencyDialog::QMcCad_TransparencyDialog(QWidget* theFather) : QDialog(theFather)
{
	uiTranspDial.setupUi(this);
	connect(uiTranspDial.okButton, SIGNAL(clicked()), this, SLOT(IsAccepted()));
	connect(uiTranspDial.horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(SetTransparency(int)));
}

void QMcCad_TransparencyDialog::IsAccepted()
{
	SetTransparency(0);
}


void QMcCad_TransparencyDialog::SetTransparency(int val)
{
	Handle(AIS_InteractiveContext) theContext = QMcCad_Application::GetAppMainWin()->GetEditor()->GetDocument()->GetContext();

	AIS_ListOfInteractive ioList;
	for(theContext->InitCurrent(); theContext->MoreCurrent(); theContext->NextCurrent())
		ioList.Append(theContext->Current());

	AIS_ListIteratorOfListOfInteractive it(ioList);

	for ( ; it.More(); it.Next() )
	{
		Handle(AIS_InteractiveObject) io = it.Value();
		Standard_Real tVal = Standard_Real(val)/100.;
		io->SetTransparency(tVal);
	}
	theContext->UpdateCurrentViewer();
}

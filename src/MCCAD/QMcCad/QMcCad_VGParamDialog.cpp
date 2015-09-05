/*
 * QMcCad_VGParamDialog.cpp
 *
 *  Created on: Jul 18, 2008
 *      Author: grosse
 */
#include <QMcCad_VGParamDialog.h>

QMcCad_VGParamDialog::QMcCad_VGParamDialog(QWidget* theFather) : QDialog(theFather)
{
	uiVGParams.setupUi(this);
	myAccepted = false;
	myAcceptButton = uiVGParams.acceptButton;
	myCM = uiVGParams.checkCM;
	myMM = uiVGParams.checkMM;
	connect(myCM, SIGNAL(stateChanged(int)), this, SLOT(SlotCMStateChanged(int)));
	connect(myMM, SIGNAL(stateChanged(int)), this, SLOT(SlotMMStateChanged(int)));
	connect(myAcceptButton, SIGNAL(clicked()), this, SLOT(IsAccepted()));
}

bool QMcCad_VGParamDialog::GetWriteCollision()
{
	return uiVGParams.checkCollision->checkState();
}

bool QMcCad_VGParamDialog::GetWriteDiscretModel()
{
	return uiVGParams.checkDiscrete->checkState();
}

double QMcCad_VGParamDialog::GetTolerance()
{
	return (uiVGParams.tolSet->text()).toFloat();
}

double QMcCad_VGParamDialog::GetXRes()
{
	return (uiVGParams.xRes->text()).toDouble();
}

double QMcCad_VGParamDialog::GetYRes()
{
	return (uiVGParams.yRes->text()).toDouble();
}

int QMcCad_VGParamDialog::GetMaxSamplePnts()
{
	return (uiVGParams.maxSamplePoints->text()).toInt();
}

int QMcCad_VGParamDialog::GetMinSamplePnts()
{
	return (uiVGParams.minSamplePoints->text()).toInt();
}

int QMcCad_VGParamDialog::GetMaxComplemented()
{
	return (uiVGParams.maxComplementCells->text()).toDouble();
}

int QMcCad_VGParamDialog::GetMaxPreDec()
{
	return (uiVGParams.maxPredecCells->text()).toInt();
}

int QMcCad_VGParamDialog::GetInitialCellNumber()
{
	return (uiVGParams.initCellNb->text()).toInt();
}

int QMcCad_VGParamDialog::GetInitialSurfaceNumber()
{
	return (uiVGParams.initSurfNb->text()).toInt();
}

double QMcCad_VGParamDialog::GetMinDecFace()
{
	return (uiVGParams.minDecFaceArea->text()).toDouble();
}

double QMcCad_VGParamDialog::GetMinReDecFace()
{
	return (uiVGParams.minReDecFaceArea->text()).toDouble();
}

double QMcCad_VGParamDialog::GetMinVoidVolume()
{
	return (uiVGParams.minVoidVolume->text()).toDouble();
}

double QMcCad_VGParamDialog::GetMinInputVolume()
{
	return (uiVGParams.minInputVolume->text()).toDouble();
}

/*QString* QMcCad_VGParamDialog::GetBoundingBox()
{
	return &(uiVGParams.lineBoundingBox->text());
}*/

QPushButton* QMcCad_VGParamDialog::GetAcceptButton()
{
	return myAcceptButton;
}

bool QMcCad_VGParamDialog::Accepted()
{
	return myAccepted;
}

void QMcCad_VGParamDialog::IsAccepted()
{
	myAccepted = true;
	accept();
}

McCadCSGGeom_Unit QMcCad_VGParamDialog::GetUnits()
{
	return myUnit;
}

void QMcCad_VGParamDialog::SlotCMStateChanged(int theState)
{
	if(theState == 2)
	{
		myUnit = McCadCSGGeom_CM;
		myMM->setCheckState(Qt::Unchecked);
	}
	else
	{
		if(myMM->checkState()==Qt::Unchecked)
			myCM->setCheckState(Qt::Checked);
	}
}

void QMcCad_VGParamDialog::SlotMMStateChanged(int theState)
{
	if(theState == 2)
	{
		myUnit = McCadCSGGeom_MM;
		myCM->setCheckState(Qt::Unchecked);
	}
	else
	{
		if(myCM->checkState()==Qt::Unchecked)
				myMM->setCheckState(Qt::Checked);
	}
}

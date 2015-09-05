/*
 * QMcCad_VGParamDialog.h
 *
 *  Created on: Jul 18, 2008
 *      Author: grosse
 */
#ifndef QMCCAD_VGPARAMDIALOG_H_
#define QMCCAD_VGPARAMDIALOG_H_

#include <QPushButton>
#include <QDialog>
#include <ui_VGParamDialog.h>
#include <McCadCSGGeom_Unit.hxx>

class QMcCad_VGParamDialog : public QDialog
{
	Q_OBJECT

public:
	QMcCad_VGParamDialog(QWidget* theFather = NULL);
	QPushButton* GetAcceptButton();
	QPushButton* GetCancelButton();
	QPushButton* GetLoadButton();

	bool GetWriteCollision();
	bool GetWriteDiscretModel();

	double GetTolerance();
	double GetXRes();
	double GetYRes();
	int GetMaxSamplePnts();
	int GetMinSamplePnts();
	int GetMaxComplemented();
	int GetMaxPreDec();
	int GetInitialCellNumber();
	int GetInitialSurfaceNumber();
	double GetMinDecFace();
	double GetMinReDecFace();
	double GetMinVoidVolume();
	double GetMinInputVolume();
	//QString* GetBoundingBox();
	bool Accepted();
	McCadCSGGeom_Unit GetUnits();


public slots:
	void IsAccepted();
	void SlotMMStateChanged(int theState);
	void SlotCMStateChanged(int theState);

signals:
	void CanContinue();

private:
	QPushButton* myAcceptButton;
	QPushButton* myLoadButton;
	QPushButton* myCancelButton;
	QCheckBox* myCM;
	QCheckBox* myMM;
	McCadCSGGeom_Unit myUnit;

	bool myAccepted;
	Ui::VGParamDialog uiVGParams;

};

#endif /* QMCCAD_VGPARAMDIALOG_H_ */

/*
 * QMcCad_TrnapsparencyDialog.h
 *
 *  Created on: Jul 18, 2008
 *      Author: grosse
 */
#ifndef QMCCAD_TRANSPARENCYDIALOG_H_
#define QMCCAD_TRANSPARENCYDIALOG_H_

#include <QPushButton>
#include <QDialog>
#include <ui_QMcCad_TransparencyDialog.h>
#include <McCadCSGGeom_Unit.hxx>

class QMcCad_TransparencyDialog : public QDialog
{
	Q_OBJECT

public:
	QMcCad_TransparencyDialog(QWidget* theFather = NULL);


public slots:
	void IsAccepted();
	void SetTransparency(int value);

private:
	Ui::TransDialog uiTranspDial;
};

#endif /* QMCCAD_TransparencyDialog_H_ */

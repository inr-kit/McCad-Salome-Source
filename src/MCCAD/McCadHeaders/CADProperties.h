#ifndef MCCAD_CAD_PROPERTIES_H
#define MCCAD_CAD_PROPERTIES_H

#include "ui_CADProperties.h"
#include <QWidget>
#include <McCadMDReader_DataMapOfLabelAsciiString.hxx>
#include <TCollection_AsciiString.hxx>

#include <TDocStd_Document.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>

#include <TDF_Label.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDF_Tool.hxx>

#include <TDataStd_Name.hxx>
#include <TDF_ChildIterator.hxx>
#include <Graphic3d_NameOfMaterial.hxx>

class CADProperties : public QWidget
{
	Q_OBJECT

public:
	CADProperties(QWidget* theFather = NULL);

	QSlider* GetSlider();
	QToolButton* GetColorButton();
	//QToolButton* GetMaterialButton();
	QComboBox* GetMaterialCombo();
	QToolButton* GetViewSolidButton();
	QToolButton* GetViewWireFrameButton();
	QToolButton* GetBackgroundColorButton();
	QTreeWidget* GetTreeWidget();

	void NewEditorAdded(const int theEditorID, const QString& theName);
	//void AddAssembliesToTree(int theEditorID, const McCadMDReader_DataMapOfLabelAsciiString& theLabelNameMap);
	void AddAssembliesToTree(int theEditorID, const Handle(TDocStd_Document) theTDoc );
	void EditorClosed(const int theEditorID);
	void SetModelName(const int theEditorID, const QString& theName);

signals:
	void MaterialSelected(Graphic3d_NameOfMaterial);


private slots:
	void ResetSlider();
	void SlotItemSelectionChanged();
	void SlotMaterialSelected(QString);

private:

	void FillMaterialBox();
	Standard_Integer CountNumberOfColons(const TCollection_AsciiString theLabelString);

	Ui::CADProperties uiProps;
	QSlider* myTransSlider;
	QLCDNumber* myTransLCD;
	QToolButton* myColorButton;
	//QToolButton* myMaterialButton;
	QComboBox* myMaterialCombo;
	QToolButton* myViewSolidButton;
	QToolButton* myViewWireFrameButton;
	QToolButton* myBackgroundColorButton;
	QTreeWidget* myTreeWidget;
	QMap<int, QTreeWidgetItem*> myEditorTreeMap;
	QMap<QTreeWidgetItem*, TDF_Label> myQTWItemLabelMap;
	QList<TDF_Label> myLabelList;
};

#endif

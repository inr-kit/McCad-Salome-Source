// Copyright (C) 2014-2015  KIT-INR/NK
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//
#ifndef MCCADGUI_DIALOGS_H
#define MCCADGUI_DIALOGS_H
#include <LightApp_Dialog.h>
#include "MCCADGUI_DataModel.h"
#include "ui_MCardDialog.h"
#include "ui_SetPropertyDialog.h"
#include "ui_ImportObjDialog.h"
#include "ui_TransformDialog.h"

#include <QDialog>
#include <QWidget>
#include <QFileDialog>
#include <QPushButton>
#include <QComboBox>
#include <QStringList>
#include <QLineEdit>
#include <QColor>
const double Null = 4.44444444444e34;

class MCCADGUI_MCardDialog:  public QDialog,  Ui::MCCADGUI_MCardDialog//public LightApp_Dialog,
{
    Q_OBJECT

public:
    MCCADGUI_MCardDialog(QWidget*parent, MCCADGUI_DataModel * dm );
    void LoadMatData();
//    void SetCurrentEditor(QMcCad_Editor* theEditor);
    bool CheckInput(QString &strErrorMsg);
    void ClearData();
//    void SetEditGroup(QString theGroupName);
//    void LoadMaterials();
    void DisplayMaterial(int iItem);
    void Refresh();

public slots:
    void AddMaterial();
    void DelMaterial();
    void ModifyMaterial();
    void onOK();
    void onApply();
    void onCancel();
    void onColorButton();

//    void SaveMaterial();
    void SaveXMLFile();
    void LoadXMLFile();
    void ShowSelectedMaterial(QTreeWidgetItem*item, int colume);

    void onMatEdited() {m_isFresh = false;}; //tringer then material is edited

private:

//    Ui::MCardDialog m_MatDialog;
//    QMcCad_Editor* m_Editor;

    unsigned int  m_ID;
    double        m_Density;
    QString       m_Name;
    QString       m_Card;
    QColor        m_Color;

    QString       m_EditGroupName;          // The group name of being edited group

    Standard_Integer m_NumOfMat;            // Number of Materials
    vector<Standard_Integer> m_MatIDList;   // The list stores the ID of materials.
    MCCADGUI_DataModel * dm ;
    //for recording if the Materials are consistent with that in the library,
    //if Refresh is called, m_isFresh is true; if the inputs are modified, then false
    bool          m_isFresh;

};


/*!
 * \brief The MCCADGUI_SetPropertyDialog class
 * for setting property of
 */
class MCCADGUI_SetPropertyDialog:  public QDialog, Ui::MCCADGUI_SetPropertyDialog
{
    Q_OBJECT

public:
   MCCADGUI_SetPropertyDialog(QWidget*parent, MCCADGUI_DataModel * dm );



   void     Refresh();
   void     DisplayProperty(const QString & aEntry) ;
   void     checkInput();


private slots:
   void     onClose();
   void     onApply();
   void     onEditMaterial();
   void     onColorButton() ;
   void     onSelectChg();


private:
   MCCADGUI_DataModel * dm ;
   QColor   m_Color;
   //for checking and associate actions
   bool     isMultipleSelect;
   bool     isAllGroup;
   bool     isMaterialChg;
   bool     isImpChg ; //is importance changed
   bool     isColorChg;
   bool     isRemarkChg;
   bool     isAddCardChg;


};

/*!
 * \brief The MCCADGUI_ImportObjDialog class
 * for setting property of
 */
class MCCADGUI_ImportObjDialog:  public QDialog, Ui::MCCADGUI_ImportObjDialog
{
    Q_OBJECT

public:
   MCCADGUI_ImportObjDialog(QWidget*parent, MCCADGUI_DataModel * dm );
   void     SetSourceObjName(const QStringList &aNameList);
   void     SetMCCADObjName(const QStringList & aNameList);

private slots:
   void     onOK();
   void     onCancel();
   void     onSelectChg();


private:
   MCCADGUI_DataModel * dm ;
//   bool     isOK;
};


class QSlider;
//=================================================================================
// class    : MCMEHSTRANGUI_TransparencyDlg
// purpose  : dialog box that allows to modify transparency of displayed objects
//=================================================================================
class MCCADGUI_TransparencyDlg : public QDialog
{
    Q_OBJECT

public:
    MCCADGUI_TransparencyDlg( QWidget* parent, const QStringList& entries );

private:
    QSlider*  mySlider;
    QStringList myEntries;

private slots:
  void ClickOnCancel();
  void ValueHasChanged( int ) ;
  void accept();

};

/*!
 * \brief The MCCADGUI_SetPropertyDialog class
 * for setting property of
 */
class MCCADGUI_TransformDialog:  public QDialog, Ui::MCCADGUI_TransformDialog
{
    Q_OBJECT

public:
   MCCADGUI_TransformDialog(QWidget*parent, MCCADGUI_DataModel * dm );


private slots:
   void     onClose();
   void     onApply();



private:
   MCCADGUI_DataModel * dm ;



};



#endif   //MCCADGUI_DIALOGS_H

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
#include "MCCADGUI_Dialogs.h"
#include "MCCADGUI.h"
#include "MCCADGUI_DataModel.h"
#include "MCCADGUI_Displayer.h"
#include <LightApp_Study.h>
#include <SUIT_Tools.h>
#include <SUIT_MessageBox.h>
#include <SUIT_DataObjectIterator.h>
#include "MCCAD_Constants.h"

#include <QString>
#include <QStringList>
#include <QMap>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QLayout>
#include <QVariant>
#include <QToolTip>
#include <QWhatsThis>
#include <QApplication>
#include <QGroupBox>
#include <QColorDialog>


#include <utilities.h>

#include CORBA_CLIENT_HEADER(MCCAD)




MCCADGUI_MCardDialog::MCCADGUI_MCardDialog(QWidget*parent, MCCADGUI_DataModel *dm )  :QDialog(parent)
//    :    LightApp_Dialog( parent, /*name */ "Material Property"
//                     , /*modal */false, /*allow resize */ false, /*buttons*/  None)
{
    //configure the dialog box
    setupUi(this);
    QIntValidator *vInt = new QIntValidator(1,1000,this);
    IDEdit->setValidator(vInt);
    QDoubleValidator *vDbl = new QDoubleValidator(-10000.0,10000.0,4,this);
    densityEdit->setValidator(vDbl);
    m_Color = QColor(255, 170, 0);
    m_isFresh = false;


    this->dm  = dm;

    connect(okButton, SIGNAL(clicked()), this, SLOT(onOK())); //qiu
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(onCancel())); //qiu
    connect(applyButton, SIGNAL(clicked()), this, SLOT(onApply()));
    connect(colorButton, SIGNAL(clicked()), this, SLOT(onColorButton()));

    connect(btnDelMat, SIGNAL(clicked()), this, SLOT(DelMaterial()));
    //connect(btnMdfMat, SIGNAL(clicked()), this, SLOT(ModifyMaterial()));
//    connect(okButton, SIGNAL(clicked()), this, SLOT(SaveMaterial()));
    connect(saveXMLButton, SIGNAL(clicked()), this, SLOT(SaveXMLFile()));
    connect(loadXMLButton, SIGNAL(clicked()), this, SLOT(LoadXMLFile()));
    connect(mMatTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(ShowSelectedMaterial(QTreeWidgetItem*,int)));

    //if material are modified, then they are not up-to-date and need to be Refresh()
    connect(IDEdit, SIGNAL(textEdited(QString)), this, SLOT(onMatEdited()));
    connect(densityEdit, SIGNAL(textEdited(QString)), this, SLOT(onMatEdited()));
    connect(nameEdit, SIGNAL(textEdited(QString)), this, SLOT(onMatEdited()));
    connect(mCardText, SIGNAL(textEdited(QString)), this, SLOT(onMatEdited()));


}


///** ********************************************************************
//* @brief Load Materials and display them
//*
//* @param
//* @return void
//*
//* @date 28/10/2013
//* @author  Lei Lu
//************************************************************************/
//void MCCADGUI_MCardDialog::LoadMaterials()
//{
//    QString strInfo = QString("Material Group \"%1\" is being edited").arg(m_EditGroupName);
//    label_1->setText(strInfo);

//    int iMatNum = m_Editor->GetMatManager()->GetMatNum(strSelectedGrp); // The material number of selected group

//    vector<int> MatIDList = m_Editor->GetMatManager()->GetMatIDList();
//    for(int i = 0; i< MatIDList.size(); i++)
//    {
//        QTreeWidgetItem* itemAdd = new QTreeWidgetItem();
//        QString itemText = "M " + QString::number(MatIDList.at(i));
//        itemAdd->setText(0,itemText);
//        itemAdd->setText(1,QString::number(MatIDList.at(i)));
//        mMatTree->addTopLevelItem(itemAdd);

//        if (iMatNum == MatIDList.at(i))
//        {
//            mMatTree->setItemSelected(itemAdd,true); // Select the material number of being edited group
//            DisplayMaterial(iMatNum);
//        }
//    }
//}



/** ********************************************************************
* @brief Update and refresh the displayed material information
*
* @param
* @return void
*
* @date 14/11/2013
* @author  Lei Lu
************************************************************************/
void MCCADGUI_MCardDialog::Refresh()
{
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = dm->getStudyID();
    if ( !studyID && CORBA::is_nil( engine ) )
    {
        MESSAGE("Errors in study or in engine!");
        return ;
    }

    MCCAD_ORB::MaterialList * aMaterialList = engine->getMaterialList(studyID);
    int n = aMaterialList->length();
    if (n == 0) return;
//    vector<int> MatIDList = m_Editor->GetMatManager()->GetMatIDList();
    for(int i = 0; i< n; i++)
    {
        QTreeWidgetItem* itemAdd = new QTreeWidgetItem();
//        QString itemText = "M " + QString::number(MatIDList.at(i));
        QString itemText = "M " + QString::number((*aMaterialList)[i]->getID());
        itemAdd->setText(0,itemText);
        itemAdd->setText(1,QString::number((*aMaterialList)[i]->getID()));
        mMatTree->addTopLevelItem(itemAdd);
    }

    if (n != 0 )    {
        DisplayMaterial((int)(*aMaterialList)[0]->getID());   // Load the detail information of the first material
    }
    m_isFresh = true; //material is up-to-date
}



/** ********************************************************************
* @brief Add material in the map and a item on the tree
*
* @param
* @return void
*
* @date 21/10/2013
* @author  Lei Lu
************************************************************************/
void MCCADGUI_MCardDialog::AddMaterial()
{
    int iOrgID;  // Record the selected material id and compared with the new one
    QTreeWidgetItem* itemCur = mMatTree->currentItem();
    if (NULL != itemCur)
    {
        iOrgID = itemCur->text(1).toInt();  // Get the material ID in the second column
    }
    else
    {
        iOrgID = 0;
    }

    if(m_ID == iOrgID)  // if the new material id is same as selected material, then modify it.
    {
        ModifyMaterial();
    }                   // else add a new material
    else if(dm->generateMaterial(m_ID,m_Name,m_Density,m_Card, m_Color))
    {
        QTreeWidgetItem* itemAdd = new QTreeWidgetItem();
        QString itemText = "M " + IDEdit->text();
        itemAdd->setText(0,itemText);
        itemAdd->setText(1,IDEdit->text());
        mMatTree->addTopLevelItem(itemAdd);
        ClearData();
    }
    else                // if the material has already existed, popup the massage box.
    {
        QMessageBox msg;

        QString strMsg = QString("Material %1 has been already existed").arg(m_ID);
        msg.setText(strMsg);
        msg.exec();
    }
}


/** ********************************************************************
* @brief Check the input data, make sure they are not empty
*
* @param  strErrorMsg
* @return bool,  strErrorMsg
*
* @date 21/10/2013
* @author  Lei Lu
************************************************************************/
bool MCCADGUI_MCardDialog::CheckInput(QString &strErrorMsg)
{
    if ( IDEdit->text().isEmpty() )
    {
        strErrorMsg = "Material ID is missing";
        return false;
    }

    if ( densityEdit->text().isEmpty() )
    {
        strErrorMsg = "Density is missing";
        return false;
    }

    if ( nameEdit->text().isEmpty() )
    {
        strErrorMsg = "Material name is missing";
        return false;
    }

    m_Name = nameEdit->text();
    m_ID = IDEdit->text().toInt();
    m_Density = densityEdit->text().toDouble();
    m_Card = mCardText->toPlainText();

    if( m_ID == 0 )
    {
        strErrorMsg = "Material ID should be between 1 and 1000";
        return false;
    }

    if( m_Density == 0 )
    {
        strErrorMsg = "Value of density is wrong";
        return false;
    }

    return true;
}


/** ********************************************************************
* @brief Delete the selected item and the related material in map
*
* @param
* @return void
*
* @date 21/10/2013
* @author  Lei Lu
************************************************************************/
void MCCADGUI_MCardDialog::DelMaterial()
{
    QTreeWidgetItem* itemCur = mMatTree->currentItem();
    if (NULL == itemCur)
    {
        return;
    }

    int index = mMatTree->indexOfTopLevelItem(itemCur);
    int iDelItem = itemCur->text(1).toInt();            // Get the material ID

    if(dm->deleteMaterial(iDelItem))
    {
        mMatTree->takeTopLevelItem(index);  // Delete the item on the tree

        delete itemCur;         // Delete the iterCur pointer
        itemCur = NULL;

        ClearData();            // Clear the edit box
    }

    return;
}



/** ********************************************************************
* @brief Modify the selected item and the related material in map
*
* @param
* @return void
*
* @date 23/10/2013
* @author  Lei Lu
************************************************************************/
void MCCADGUI_MCardDialog::ModifyMaterial()
{
    QTreeWidgetItem* itemCur = mMatTree->currentItem();
    if (NULL == itemCur)
    {
        return;
    }

    m_Card = mCardText->toPlainText();
    m_Name = nameEdit->text();
    m_ID = IDEdit->text().toInt();
    m_Density = densityEdit->text().toDouble();

    int index = itemCur->text(1).toInt();            // Get the material ID
    if (dm->updateMaterial(index,m_ID,m_Name,m_Density,m_Card, m_Color)) //!!!!
    {
        QString itemText = "M " + IDEdit->text();
        itemCur->setText(0,itemText);
        itemCur->setText(1,IDEdit->text());
    }
    else
    {
        return;
    }
}

/*!
 * \brief when okButton is click, save and close
 */
void MCCADGUI_MCardDialog::onOK()
{
    QString strMessage;
    if(!CheckInput(strMessage))  {     //check if no data , just quit
        this->done(0);
        return ;
    }
    if (m_isFresh == false) //only if the material is not up-to-date
        AddMaterial();
    this->done(0);
}

/*!
 * \brief when applyButton is click, save
 */
void MCCADGUI_MCardDialog::onApply()
{
    QString strMessage;
    if(!CheckInput(strMessage))             // Check the input data and their formats
    {
        QMessageBox msg;
        msg.setText(strMessage);
        msg.exec();
        return;
    }
    if (m_isFresh == false) //only if the material is not up-to-date
        AddMaterial();

}

/*!
 * \brief when cancelButton is click, close
 */
void MCCADGUI_MCardDialog::onCancel()
{
    this->done(0);
}


/*!
 * \brief MCCADGUI_MCardDialog::onColorButton
 * open a color dialog to select color
 */
void MCCADGUI_MCardDialog::onColorButton()
{
 QColor aColor = QColorDialog::getColor(Qt::white, this);
 m_Color = aColor;

 colorButton->setStyleSheet(QString("QPushButton { background:rgb(%1, %2, %3) }").arg(aColor.red()).arg(aColor.green()).arg(aColor.blue()));

}



/** ********************************************************************
* @brief Reponse to the item click, update the edit box
*        with the selected material.
*
* @param
* @return void
*
* @date 23/10/2013
* @author  Lei Lu
************************************************************************/
void MCCADGUI_MCardDialog::ShowSelectedMaterial(QTreeWidgetItem* item,int colume)
{
    QTreeWidgetItem* itemCur = mMatTree->currentItem();
    if (NULL == itemCur)
    {
        return;
    }

    int iItem = item->text(1).toInt();          // Get the material ID
    DisplayMaterial(iItem);                     // Display the material whose ID is iItem

    return;
}


/** ********************************************************************
* @brief Display the deteil selected material.
*
* @param
* @return void
*
* @date 13/11/2013
* @author  Lei Lu
************************************************************************/
void MCCADGUI_MCardDialog::DisplayMaterial(int iItem)
{
//    Material *pMaterial = NULL;
//    pMaterial = m_Editor->GetMatManager()->GetMaterial(iItem);
    //get the engine and study
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = dm->getStudyID();
    if ( !studyID && CORBA::is_nil( engine ) )
    {
        MESSAGE("Errors in study or in engine!");
        return ;
    }

    //get the original material, and check the newID if ok
    MCCAD_ORB::Material_var aMaterial = engine->getMaterial(studyID, iItem);
    if (aMaterial->_is_nil()) {
        MESSAGE("Could not find the material in the list!");
        return ;
    }

//    if (NULL != pMaterial)
//    {
    QString strID = QString::number(aMaterial->getID());
    QString strDensity = QString::number(aMaterial->getDensity());
    QString strName = aMaterial->getName();
    QString strMatCard = aMaterial->getMatCard();

    IDEdit->setText(strID);
    densityEdit->setText(strDensity);
    nameEdit->setText(strName);
    mCardText->setText(strMatCard);
//    }
    return;
}



///** ********************************************************************
//* @brief Add material in the map and a item on the tree
//*
//* @param
//* @return void
//*
//* @date 21/10/2013
//* @author  Lei Lu
//************************************************************************/
//void MCCADGUI_MCardDialog::SaveMaterial()
//{
//    QTreeWidgetItem* itemCur = mMatTree->currentItem();
//    if (NULL == itemCur)
//    {
//        return;
//    }

//    int iItem = itemCur->text(1).toInt();            // Get the material ID
//    m_Editor->GetMatManager()->InsertMatGroup(m_EditGroupName,iItem);
//    //m_Editor->GetMatManager()->SaveXMLfile();
//}



/** ********************************************************************
* @brief Clear the edit box
*
* @param
* @return void
*
* @date 23/10/2013
* @author  Lei Lu
************************************************************************/
void MCCADGUI_MCardDialog::ClearData()
{
    mCardText->clear();
    nameEdit->clear();
    IDEdit->clear();
    densityEdit->clear();
}


///** ********************************************************************
//* @brief Set the material group now being edited
//*
//* @param
//* @return void
//*
//* @date 28/10/2013
//* @author  Lei Lu
//************************************************************************/
//void MCCADGUI_MCardDialog::SetEditGroup(QString theGroupName)
//{
//    m_EditGroupName = theGroupName;
//}


///** ********************************************************************
//* @brief Set the editor, connect the editor with material manager
//*
//* @param
//* @return void
//*
//* @date 28/10/2013
//* @author  Lei Lu
//************************************************************************/
//void MCCADGUI_MCardDialog::SetCurrentEditor(QMcCad_Editor* theEditor)
//{
//    m_Editor = theEditor;
//}


/*!
 * \brief MCCADGUI_MCardDialog::SaveXMLFile
 *  Save the material to XML file, as the DataModel to do this
 *  If the current material edit is no empty, add this material and then save
 */
void MCCADGUI_MCardDialog::SaveXMLFile()
{

    QString strMessage;
    if(CheckInput(strMessage))   //if check ok, save current material first
        AddMaterial();

    QString fileFilters("*.xml");
    QString theFileName = QFileDialog::getSaveFileName(this, tr("Save As"),	".", fileFilters );

    if (!theFileName.isEmpty())
    {
        if(!theFileName.endsWith(".xml"))
        {
            theFileName.append(".xml");
        }
        dm->saveMaterial2Xml(theFileName);
    }
}



/** ********************************************************************
* @brief Load the material information as a xml file
*
* @param
* @return void
*
* @date 04/11/2013
* @author  Lei Lu
************************************************************************/
void MCCADGUI_MCardDialog::LoadXMLFile()
{
    QString fileFilters("*.xml");
    QString theFileName = QFileDialog::getOpenFileName(this, tr("Load"),	".", fileFilters );

    if (!theFileName.isEmpty())
    {
        if(!theFileName.endsWith(".xml"))
        {
            theFileName.append(".xml");
        }
        dm->loadMaterialXml(theFileName);
        Refresh();
    }
}


MCCADGUI_SetPropertyDialog::MCCADGUI_SetPropertyDialog(QWidget*parent, MCCADGUI_DataModel * dm ): QDialog(parent)
{
    setupUi(this);
    QIntValidator *vInt = new QIntValidator(0,1000000,this);
    le_imp_n->setValidator(vInt);
    le_imp_p->setValidator(vInt);
    le_imp_e->setValidator(vInt);

    this->dm  = dm; //assign the data model
    isMultipleSelect = false;
    isAllGroup = true;

    connect(Btn_close, SIGNAL(clicked()), this, SLOT(onClose())); //qiu
    connect(Btn_apply, SIGNAL(clicked()), this, SLOT(onApply()));
    connect(Btn_edit_mat, SIGNAL(clicked()), this, SLOT(onEditMaterial()));
    connect(Btn_color, SIGNAL(clicked()), this, SLOT(onColorButton()));
    connect(dm->getMCCADGUImodule(),SIGNAL(selectChg()),this,SLOT(onSelectChg()) );


    Refresh(); //refresh the dialog
}





/*!
 * \brief MCCADGUI_SetPropertyDialog::Refresh
 * refresh the dialog box to update the shown info
 */
void    MCCADGUI_SetPropertyDialog:: Refresh()
{
    MCCADGUI * theGUI = dm->getMCCADGUImodule();
    QStringList aSelectedList;
    theGUI->selected(aSelectedList, /*multiple objects*/ true);

    if (aSelectedList.size() > 1)
        isMultipleSelect = true;
    else if (aSelectedList.size() == 1)
        isMultipleSelect = false;
    else return;

    //check if all groups
    isAllGroup = false; //default false for case of no selection
    for (int i=0; i<aSelectedList.size(); i++) {
        if (!dm->findObject(aSelectedList[i])->isGroup()) {  //when one of the object is not a group
            isAllGroup = false;
            break;
        }
        else isAllGroup = true;
    }

    //get the material list, and update the combox box
    if (isAllGroup) {
        MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
        const int studyID = dm->getStudyID();
        if ( !studyID && CORBA::is_nil( engine ) )
        {
            MESSAGE("Errors in study or in engine!");
            return ;
        }
        MCCAD_ORB::MaterialList * aMaterialList = engine->getMaterialList(studyID);
        int n = aMaterialList->length();
        cmb_mat_id->clear(); //clear the old data
        cmb_mat_id->addItem("-----",0 );
        for(int i = 0; i< n; i++)
        {
            //style: M1--name
            QString aTmpStr = QString("M") + QString("%1--").arg((*aMaterialList)[i]->getID()) + QString ((*aMaterialList)[i]->getName());
            cmb_mat_id->addItem(aTmpStr,(*aMaterialList)[i]->getID() );
        }
    }

    //display the property of the first object
    DisplayProperty(aSelectedList[0]);
}

/*!
 * \brief DisplayProperty
 *  display the property of this object
 * \param aEntry
 */
void      MCCADGUI_SetPropertyDialog::DisplayProperty(const QString & aEntry)
{

    //####first display the properties ####
    //combo box
    MCCADGUI_DataObject * aObj = dm->findObject(aEntry);
    if (!aObj) return;
        //for material group box
//        if (!isMultipleSelect) {
    if (aObj->isGroup()) {
        MCCAD_ORB::Group_var aGroup = aObj->getGroup() ;
        MCCAD_ORB::Material_var aMaterial = aGroup->getMaterial();
        if (!aMaterial->_is_nil()) {
            int aIndex = cmb_mat_id->findData(aMaterial->getID()); //find the index of the material in the combox according to ID
            cmb_mat_id->setCurrentIndex(aIndex);
        }
    }
//        }
//        else {
//            int aIndex = cmb_mat_id->findData(0);  //find the ----
//            cmb_mat_id->setCurrentIndex(aIndex);
//        }

    //for color
        MCCAD_ORB::BaseObj_var aBaseObj = aObj->getBaseObj();
//        if (!isMultipleSelect) {
            if (!aBaseObj->_is_nil()) {
                MCCAD_ORB::MColor aMColor;
                aBaseObj->getColor(aMColor);
                QColor aColor = QColor(aMColor[0], aMColor[1], aMColor[2]);
                m_Color = aColor;
                Btn_color->setStyleSheet(QString("QPushButton { background:rgb(%1, %2, %3) }").arg(aColor.red()).arg(aColor.green()).arg(aColor.blue()));
            }
//        }
//        else {
//            m_Color = QColor(0,0,0) ; //use black color to indecate the no change on the color
//        }



    //for importances
//    if (isMultipleSelect) {
//        le_imp_n->clear();  //clear the data, to show that the data is not set
//        le_imp_p->clear();
//        le_imp_e->clear();
//    }
    if (aObj->isPart()) {
        MCCAD_ORB::Part_var aPart = aObj->getPart();
        if (!aPart->_is_nil()) {
             MCCAD_ORB::FixArray_var aIMP = aPart->getImportance();
             le_imp_n->setText(aIMP[0] == -1 ? "" :QString("%1").arg(aIMP[0]));
             le_imp_p->setText(aIMP[1] == -1 ? "" :QString("%1").arg(aIMP[1]));
             le_imp_e->setText(aIMP[2] == -1 ? "" :QString("%1").arg(aIMP[2]));
        }
    }

    //for the remark
//    if (!isMultipleSelect) {
        tb_remark->setText(aBaseObj->getRemark()); //set the remark
//    }
//    else {
//        tb_remark->clear();
//    }

    //for additive card

//    if  (isMultipleSelect) {
//        tb_add_card->clear();
//    }
        if (aObj->isPart()) {
            MCCAD_ORB::Part_var aPart = aObj->getPart();
            if (!aPart->_is_nil()) {
                tb_add_card->setText(QString(aPart->getAdditive()));
            }
        }


        //####second controld the enable/disable checkable/ of groupboxes####
        //material group box
        if (!isAllGroup)
            gb_Material->setEnabled(false); //deactivate the group box
        else {
            gb_Material->setEnabled(true); //activate the group box

            if (isMultipleSelect)   //if multiple groups, default not checked
                gb_Material->setChecked(false);
            else gb_Material->setChecked(true);  //if single group, default checked
        }
        //important group box
        if (isMultipleSelect)  {
            gb_Imp->setChecked(false);
            gb_Graphic->setChecked(false);
            gb_Remark->setChecked(false);
            gb_Additive->setChecked(false);
        }
        else if (aObj->isComponent()) {
              gb_Imp->setChecked(false);
              gb_Graphic->setChecked(false);
              gb_Remark->setChecked(true);
              gb_Additive->setChecked(false);
        }
        else if (aObj->isGroup()) {
              gb_Imp->setChecked(false);
              gb_Graphic->setChecked(false);
              gb_Remark->setChecked(true);
              gb_Additive->setChecked(false);
        }
        else if (aObj->isPart()) {
              gb_Imp->setChecked(true);
              gb_Graphic->setChecked(false);
              gb_Remark->setChecked(true);
              gb_Additive->setChecked(true);
        }

}


/*!
 * \brief MCCADGUI_SetPropertyDialog::checkInput
 * for checking the inputs
 */
void     MCCADGUI_SetPropertyDialog::checkInput()
{
    //check if material are change
    if (isAllGroup) { //if all are groups, check the material group box
        if (isMultipleSelect) { //if more than one group
            if (!gb_Material->isChecked()) //if the material group box is not check
                isMaterialChg = false;
        }
        else isMaterialChg = true;  // to force updating material of this single group
    }
    else isMaterialChg = false; //  material not edit
    //check if importance changed
//    if (isMultipleSelect)
        isImpChg = gb_Imp->isChecked()?   true : false;
//    else  isImpChg = true; //force update the importance

   //check color changed
//    if (isMultipleSelect)
        isColorChg = gb_Graphic->isChecked()?  true : false;
//    else  isColorChg = true; //force update

    //check remark changed
//    if (isMultipleSelect)
        isRemarkChg = gb_Remark->isChecked()?  true : false;
//    else  isRemarkChg = true;
    //check Additive changed
//    if (isMultipleSelect)
        isAddCardChg = gb_Additive->isChecked()?  true : false;
//    else  isAddCardChg = true;

}



/*!
 * \brief MCCADGUI_SetPropertyDialog::onClose
 * close the dialog without any action
 */
void     MCCADGUI_SetPropertyDialog::onClose()
{
    this->done(0);
}

/*!
 * \brief MCCADGUI_SetPropertyDialog::onApply
 *  take all necessary actions
 */
void     MCCADGUI_SetPropertyDialog::onApply()
{
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = dm->getStudyID();
    if ( !studyID && CORBA::is_nil( engine ) )
    {
        MESSAGE("Errors in study or in engine!");
        return ;
    }

    checkInput();  //first check the input

    MCCADGUI * theGUI = dm->getMCCADGUImodule();
    QStringList aSelectedList;
    theGUI->selected(aSelectedList, /*multiple objects*/ true);


    //for material
    if (isAllGroup && isMaterialChg) {
        int aIndex = cmb_mat_id->currentIndex();
        int MaterialID = cmb_mat_id->itemData(aIndex).toInt();
        if ( MaterialID != 0) {
//            MCCAD_ORB::Material_var aMaterial = engine->getMaterial(studyID, MaterialID);
//            MCCAD_ORB::MColor aMColor ;
//            aMaterial->getColor(aMColor);
//            for (int i=0; i<aSelectedList.size() ; i++) {
//                MCCADGUI_DataObject * aObj  = dm->findObject(aSelectedList[i]);
//                MCCAD_ORB::Group_var aGroup = aObj->getGroup();
//                aGroup->setMaterial(aMaterial);  //duplciate the material reference for the group
//                if (chk_recolor->isChecked()) { //if assign the material color for the group
//                    aGroup->setColor(aMColor);
//                }
//            }

            dm->assignMaterial(aSelectedList, MaterialID, chk_recolor->isChecked()); // call the dm to finish this work
            if (chk_recolor->isChecked()) {
                //invoke the GUI to update the color of these objects
                MCCAD_ORB::Material_var aMaterial = engine->getMaterial(studyID, MaterialID);
                if (!aMaterial->_is_nil()) {
                    MCCAD_ORB::MColor aMColor ;
                    aMaterial->getColor(aMColor);
                    QColor aColor = QColor(aMColor[0], aMColor[1],aMColor[2]);
                    theGUI->setProperty(aSelectedList, MCCAD::Color, aColor  );
                    MCCADGUI_Displayer * d = dynamic_cast <MCCADGUI_Displayer * > (theGUI->displayer());
                    theGUI->redisplay(d);
                }
            }
        }
    }

    //for graphic
    if(isColorChg) {
        MCCAD_ORB::MColor aMColor ;
        aMColor[0]= m_Color.red();
        aMColor[1] = m_Color.green();
        aMColor[2] = m_Color.blue();
        for (int i=0; i<aSelectedList.size() ; i++) {
            MCCADGUI_DataObject * aObj  = dm->findObject(aSelectedList[i]);
            if (aObj->isPart()) {
                MCCAD_ORB::Part_var aPart = aObj->getPart();
                aPart->setColor(aMColor);
            }
            else if (aObj->isGroup()) {
                MCCAD_ORB::Group_var aGroup = aObj->getGroup();
                aGroup->setColor(aMColor);
            }
            else if (aObj->isComponent()) {
                MCCAD_ORB::Component_var aComponent = aObj->getComponent();
                aComponent->setColor(aMColor);
            }
            else continue;
        }
        //set the color property for the GUI
        theGUI->setProperty(aSelectedList, MCCAD::Color, m_Color  );
        MCCADGUI_Displayer * d = dynamic_cast <MCCADGUI_Displayer * > (theGUI->displayer());
        theGUI->redisplay(d);
    }

    if (isImpChg) {
        MCCAD_ORB::FixArray aIMP ;
        aIMP[0] = le_imp_n->text().trimmed().isEmpty() ? -1 : le_imp_n->text().trimmed().toInt(); // -1: no importance
        aIMP[1] = le_imp_p->text().trimmed().isEmpty() ? -1 : le_imp_p->text().trimmed().toInt();
        aIMP[2] = le_imp_e->text().trimmed().isEmpty() ? -1 : le_imp_e->text().trimmed().toInt();
        dm->assignImportance(aSelectedList, aIMP);  //dont get the return bool, not useful
    }
    if (isRemarkChg) {
        dm->assignRemark(aSelectedList, tb_remark->toPlainText());
    }
    if (isAddCardChg) {
        dm->assignAdditive(aSelectedList, tb_add_card->toPlainText());
    }

//    this->done(0);

}



/*!
 * \brief MCCADGUI_SetPropertyDialog::onEditMaterial
 *  open the edit material dialog
 */
void  MCCADGUI_SetPropertyDialog::onEditMaterial()
{
    MCCADGUI_MCardDialog aMatDialog (dm->getMCCADGUImodule()->application()->desktop(),dm );
    aMatDialog.Refresh();
    aMatDialog.exec();

    Refresh(); //refresh the material
}

/*!
 * \brief MCCADGUI_SetPropertyDialog::onColorButton
 * open a color dialog to select color
 */
void MCCADGUI_SetPropertyDialog::onColorButton()
{
 QColor aColor = QColorDialog::getColor(Qt::white, this);
 m_Color = aColor;
 Btn_color->setStyleSheet(QString("QPushButton { background:rgb(%1, %2, %3) }").arg(aColor.red()).arg(aColor.green()).arg(aColor.blue()));

}

void     MCCADGUI_SetPropertyDialog::onSelectChg()
{
    Refresh();
}




MCCADGUI_ImportObjDialog::MCCADGUI_ImportObjDialog(QWidget*parent, MCCADGUI_DataModel * dm ): QDialog(parent)
{
    setupUi(this);
    this->dm  = dm; //assign the data model
//    isOK = false;
    connect(Btn_ok, SIGNAL(clicked()), this, SLOT(onOK())); //qiu
    connect(Btn_cancel, SIGNAL(clicked()), this, SLOT(onCancel()));
    connect(dm->getMCCADGUImodule(),SIGNAL(selectChg()),this,SLOT(onSelectChg()) );
}

void     MCCADGUI_ImportObjDialog::SetSourceObjName(const QStringList & aNameList)
{
    if (aNameList.isEmpty()) return;
    QString aNameStr;
    for (int i=0; i<aNameList.size(); i++) {
        aNameStr += aNameList.at(i) + QString("\n");
    }
    te_geom_name->setText(aNameStr);
}

void     MCCADGUI_ImportObjDialog::SetMCCADObjName(const QStringList &aNameList)
{

    if (aNameList.isEmpty()) return;
    QString aNameStr;
    for (int i=0; i<aNameList.size(); i++) {
        aNameStr += aNameList.at(i) + QString("\n");
    }
    te_mccad_name->setText(aNameStr);

}

/*!
 * \brief invoke the change of showing the name
 */
void     MCCADGUI_ImportObjDialog::onSelectChg()
{
    MCCADGUI * theGUI = dm->getMCCADGUImodule();
    QStringList aSelectedList;
//    theGUI->selected(aSelectedList, /*multiple objects*/ false);
    theGUI->selected(aSelectedList, /*multiple objects*/ true);
//    if (aSelectedList.isEmpty()) {
//        isOK = false;
//        return;
//    }

    QStringList aNameList;
    for (int i=0; i<aSelectedList.size(); i++) {
        MCCADGUI_DataObject * aObj = dm->findObject(aSelectedList[i]);
        QString aTmpName;
        if (aObj->isPart())
            aTmpName = QString("Part  ") + QString(aObj->name());
        else if (aObj->isGroup())
            aTmpName = QString("Group  ") + QString(aObj->name());
        else if (aObj->isComponent())
            aTmpName = QString("Component  ") + QString(aObj->name());
        else
            aTmpName = QString("MCCAD");

        aNameList.push_back(aTmpName);
    }

//    if (!aObj) {
//        isOK = false;
//        return;
//    }

    //set the name for mccad object label
    SetMCCADObjName(aNameList);
//    isOK = true;
}

void     MCCADGUI_ImportObjDialog::onOK()
{
//    if (!isOK) {
//        SUIT_MessageBox::warning( this, QString("Warning"),
//                                  QString( "Please selecte the correct object!"));
//        return ;
//    }
//    else {
//        this->done(0);
//    }
    this->done(0);
}

void     MCCADGUI_ImportObjDialog::onCancel()
{
   this->done(1);
}



//###########MCMEHSTRANGUI_TransparencyDlg###############//
MCCADGUI_TransparencyDlg::MCCADGUI_TransparencyDlg( QWidget* parent, const QStringList& entries )
  :QDialog( parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint )
{
  myEntries = entries;

  resize(152, 107);
  setWindowTitle(tr("Transparency"));
  setSizeGripEnabled(TRUE);
  QGridLayout* lay = new QGridLayout(this);
  lay->setSpacing(6);
  lay->setMargin(11);

  /*************************************************************************/
  QGroupBox* GroupButtons = new QGroupBox( this );
  QGridLayout* GroupButtonsLayout = new QGridLayout( GroupButtons );
  GroupButtonsLayout->setAlignment( Qt::AlignTop );
  GroupButtonsLayout->setSpacing( 6 );
  GroupButtonsLayout->setMargin( 11 );

  QPushButton* buttonApply = new QPushButton( GroupButtons );
  buttonApply->setText( tr( "Apply" ) );
  buttonApply->setAutoDefault( TRUE );
  buttonApply->setDefault( TRUE );

  QPushButton* buttonCancel = new QPushButton( GroupButtons );
  buttonCancel->setText( tr( "Cancel" ) );
  buttonCancel->setAutoDefault( TRUE );
  buttonCancel->setDefault( TRUE );

  GroupButtonsLayout->addWidget( buttonApply, 0, 0 );
  GroupButtonsLayout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 1 );
  GroupButtonsLayout->addWidget( buttonCancel, 0, 2 );

  /*************************************************************************/
  QGroupBox* GroupC1 = new QGroupBox( this );
  QGridLayout* GroupC1Layout = new QGridLayout( GroupC1 );
  GroupC1Layout->setAlignment( Qt::AlignTop );
  GroupC1Layout->setSpacing( 6 );
  GroupC1Layout->setMargin( 11 );
  GroupC1Layout->setColumnMinimumWidth(1, 100);

  QLabel* TextLabelOpaque = new QLabel( GroupC1 );
  TextLabelOpaque->setText( tr( "Transparent"  ) );
  TextLabelOpaque->setAlignment( Qt::AlignLeft );
  GroupC1Layout->addWidget( TextLabelOpaque, 0, 0 );
  GroupC1Layout->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ), 0, 1 );

  QLabel* TextLabelTransparent = new QLabel( GroupC1 );
  TextLabelTransparent->setText( tr( "Opaque"  ) );
  TextLabelTransparent->setAlignment( Qt::AlignRight );
  GroupC1Layout->addWidget( TextLabelTransparent, 0, 2 );

  mySlider = new QSlider( Qt::Horizontal, GroupC1 );
  mySlider->setMinimum( 0 );
  mySlider->setMaximum( 10 );
  GroupC1Layout->addWidget( mySlider, 0, 1 );
  /*************************************************************************/

  lay->addWidget(GroupC1, 0,  0);
  lay->addWidget(GroupButtons, 1, 0);

  if ( myEntries.count() == 1 ) {
    int value = (int)(MCCADGUI_Displayer().getTransparency( myEntries[ 0 ] ) * 10.);
    if ( value > 0 )
      mySlider->setValue( value );
    ValueHasChanged(mySlider->value());
  }

  // signals and slots connections : after ValueHasChanged()
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(accept()));
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(ClickOnCancel()));
//  connect(mySlider, SIGNAL(valueChanged(int)), this, SLOT(ValueHasChanged(int)));
}

//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void MCCADGUI_TransparencyDlg::ClickOnCancel()
{
    done(0);
}

//=================================================================================
// function : ValueHasChanged()
// purpose  : Called when value of slider change
//          : or the first time as initilisation
//=================================================================================
void MCCADGUI_TransparencyDlg::ValueHasChanged( int newValue )
{
  MCCADGUI_Displayer().setTransparency( myEntries, (double)mySlider->value() * 0.1 );
}

void MCCADGUI_TransparencyDlg::accept()
{
    MCCADGUI_Displayer().setTransparency( myEntries, (double)mySlider->value() * 0.1 );
}






//###########MCMEHSTRANGUI_TransformDialog###############//

MCCADGUI_TransformDialog::MCCADGUI_TransformDialog(QWidget*parent, MCCADGUI_DataModel * dm )
{
    setupUi(this);
    QDoubleValidator * vFactor= new QDoubleValidator(1e-9, 1e15,10,this  );
    le_scl_ft->setValidator(vFactor);

    QDoubleValidator * vDouble= new QDoubleValidator(-1e99, 1e99,14,this  );
    le_rt_ang->setValidator(vDouble);
    le_rt_dir_x->setValidator(vDouble);
    le_rt_dir_y->setValidator(vDouble);
    le_rt_dir_z->setValidator(vDouble);
    le_rt_org_x->setValidator(vDouble);
    le_rt_org_y->setValidator(vDouble);
    le_rt_org_z->setValidator(vDouble);
    le_tsl_x->setValidator(vDouble);
    le_tsl_y->setValidator(vDouble);
    le_tsl_z->setValidator(vDouble);

    this->dm  = dm; //assign the data model

    connect(Btn_apply, SIGNAL(clicked()), this, SLOT(onApply()));
    connect(Btn_close, SIGNAL(clicked()), this, SLOT(onClose()));

}

void     MCCADGUI_TransformDialog::onApply()
{
    MCCADGUI * theGUI = dm->getMCCADGUImodule();
    QStringList aSelectedList;
    theGUI->selected(aSelectedList, /*multiple objects*/ true);

    if(gb_tsl->isChecked()) {
         MCCAD_ORB::FixArray aVec ;
         //default 0,0,0
         aVec[0] = le_tsl_x->text().trimmed().isEmpty() ? 0 : le_tsl_x->text().trimmed().toDouble();
         aVec[1] = le_tsl_y->text().trimmed().isEmpty() ? 0 : le_tsl_y->text().trimmed().toDouble();
         aVec[2] = le_tsl_z->text().trimmed().isEmpty() ? 0 : le_tsl_z->text().trimmed().toDouble();
         dm->translateShapes(aSelectedList, aVec);
    }
    if (gb_rt->isChecked()){
        MCCAD_ORB::FixArray aOrigin ;
        MCCAD_ORB::FixArray aDir ;
        double aAngle = 0.;
        aOrigin[0] = le_rt_org_x->text().trimmed().isEmpty() ? 0 : le_rt_org_x->text().trimmed().toDouble();
        aOrigin[1] = le_rt_org_y->text().trimmed().isEmpty() ? 0 : le_rt_org_y->text().trimmed().toDouble();
        aOrigin[2] = le_rt_org_z->text().trimmed().isEmpty() ? 0 : le_rt_org_z->text().trimmed().toDouble();
        aDir[0] = le_rt_dir_x->text().trimmed().isEmpty() ? 0 : le_rt_dir_x->text().trimmed().toDouble();
        aDir[1] = le_rt_dir_y->text().trimmed().isEmpty() ? 0 : le_rt_dir_y->text().trimmed().toDouble();
        aDir[2] = le_rt_dir_z->text().trimmed().isEmpty() ? 0 : le_rt_dir_z->text().trimmed().toDouble();
        aAngle = le_rt_ang->text().trimmed().isEmpty() ? 0 : le_rt_ang->text().trimmed().toDouble();
        dm->rotateShapes(aSelectedList, aOrigin, aDir, aAngle);
    }
    if (gb_scl->isChecked()){
        double aFactor = 1.;
        aFactor = le_scl_ft->text().trimmed().isEmpty() ? 1. : le_scl_ft->text().trimmed().toDouble();
        dm->scaleShapes(aSelectedList, aFactor);
    }
    MCCADGUI_Displayer * d = dynamic_cast <MCCADGUI_Displayer * > (theGUI->displayer());
    theGUI->redisplay(d);
}

void    MCCADGUI_TransformDialog:: onClose()
{
    this->done(0);
}


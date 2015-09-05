#include "McCadMatManageDlg.hxx"
#include <assert.h>

McCadMatManageDlg::McCadMatManageDlg(QWidget* theParent) : QDialog(theParent)
{
    m_MatDialog.setupUi(this);
    connect(m_MatDialog.btnAddMat, SIGNAL(clicked()), this, SLOT(AddMaterial()));
    connect(m_MatDialog.btnDelMat, SIGNAL(clicked()), this, SLOT(DelMaterial()));
    //connect(m_MatDialog.btnMdfMat, SIGNAL(clicked()), this, SLOT(ModifyMaterial()));
    connect(m_MatDialog.okButton, SIGNAL(clicked()), this, SLOT(SaveMaterial()));
    //connect(m_MatDialog.saveXMLButton, SIGNAL(clicked()), this, SLOT(SaveXMLFile()));
    //connect(m_MatDialog.loadXMLButton, SIGNAL(clicked()), this, SLOT(LoadXMLFile()));
    connect(m_MatDialog.mMatTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(ShowSelectedMaterial(QTreeWidgetItem*,int)));
}


/** ********************************************************************
* @brief Load Materials and display them
*
* @param
* @return void
*
* @date 28/10/2013
* @author  Lei Lu
************************************************************************/
void McCadMatManageDlg::LoadMaterials(QString strSelectedGrp)
{
    m_MatDialog.mMatTree->clear();

    QString strInfo = QString("Material Group \"%1\" is being edited").arg(m_EditGroupName);
    m_MatDialog.label_1->setText(strInfo);

    int iMatNum = m_Editor->GetMatManager()->GetMatNum(strSelectedGrp); // The material number of selected group

    vector<int> MatIDList = m_Editor->GetMatManager()->GetMatIDList();
    for(int i = 0; i< MatIDList.size(); i++)
    {
        QTreeWidgetItem* itemAdd = new QTreeWidgetItem();
        QString itemText = "M " + QString::number(MatIDList.at(i));
        itemAdd->setText(0,itemText);
        itemAdd->setText(1,QString::number(MatIDList.at(i)));
        m_MatDialog.mMatTree->addTopLevelItem(itemAdd);

        if (iMatNum!=0 && iMatNum == MatIDList.at(i))
        {
            m_MatDialog.mMatTree->setItemSelected(itemAdd,true); // Select the material number of being edited group
            DisplayMaterial(iMatNum);
        }
    }
}



/** ********************************************************************
* @brief Update and refresh the displayed material information
*
* @param
* @return void
*
* @date 14/11/2013
* @author  Lei Lu
************************************************************************/
void McCadMatManageDlg::Refresh()
{
    vector<int> MatIDList = m_Editor->GetMatManager()->GetMatIDList();
    for(int i = 0; i< MatIDList.size(); i++)
    {
        QTreeWidgetItem* itemAdd = new QTreeWidgetItem();
        QString itemText = "M " + QString::number(MatIDList.at(i));
        itemAdd->setText(0,itemText);
        itemAdd->setText(1,QString::number(MatIDList.at(i)));
        m_MatDialog.mMatTree->addTopLevelItem(itemAdd);
    }

    if (MatIDList.size() != 0 )
    {
        DisplayMaterial(MatIDList.at(0));   // Load the detail information of the first material
    }
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
void McCadMatManageDlg::AddMaterial()
{
    QString strMessage;
    if(!CheckInput(strMessage))             // Check the input data and their formats
    {
        QMessageBox msg;
        msg.setText(strMessage);
        msg.exec();
        return;
    }

    int iOrgID;  // Record the selected material id and compared with the new one
    QTreeWidgetItem* itemCur = m_MatDialog.mMatTree->currentItem();
    if (NULL != itemCur)
    {
        iOrgID = itemCur->text(1).toInt();  // Get the material ID
    }
    else
    {
        iOrgID = 0;
    }

    if(m_ID == iOrgID)  // if the new material id is same as selected material, then modify it.
    {
        ModifyMaterial();
    }                   // else add a new material
    else if(m_Editor->GetMatManager()->InsertNewMaterial(m_ID,m_Density,m_Name,m_Card))
    {
        QTreeWidgetItem* itemAdd = new QTreeWidgetItem();
        QString itemText = "M " + m_MatDialog.IDEdit->text();
        itemAdd->setText(0,itemText);
        itemAdd->setText(1,m_MatDialog.IDEdit->text());
        m_MatDialog.mMatTree->addTopLevelItem(itemAdd);
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
bool McCadMatManageDlg::CheckInput(QString &strErrorMsg)
{
    if ( m_MatDialog.IDEdit->text().isEmpty() )
    {
        strErrorMsg = "Material ID is missing";
        return false;
    }

    if ( m_MatDialog.densityEdit->text().isEmpty() )
    {
        strErrorMsg = "Density is missing";
        return false;
    }

    if ( m_MatDialog.nameEdit->text().isEmpty() )
    {
        strErrorMsg = "Material name is missing";
        return false;
    }

    if ( m_MatDialog.nameEdit->text().length() > 50 )
    {
        strErrorMsg = "Material name is too long";
        return false;
    }

    m_Name = m_MatDialog.nameEdit->text();
    m_ID = m_MatDialog.IDEdit->text().toInt();
    m_Density = m_MatDialog.densityEdit->text().toDouble();
    m_Card = m_MatDialog.mCardText->toPlainText();

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
void McCadMatManageDlg::DelMaterial()
{
    QTreeWidgetItem* itemCur = m_MatDialog.mMatTree->currentItem();
    if (NULL == itemCur)
    {
        return;
    }

    int index = m_MatDialog.mMatTree->indexOfTopLevelItem(itemCur);
    int iDelItem = itemCur->text(1).toInt();            // Get the material ID

    if(m_Editor->GetMatManager()->DeleteMaterial(iDelItem))
    {
        m_MatDialog.mMatTree->takeTopLevelItem(index);  // Delete the item on the tree

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
void McCadMatManageDlg::ModifyMaterial()
{
    QTreeWidgetItem* itemCur = m_MatDialog.mMatTree->currentItem();
    if (NULL == itemCur)
    {
        return;
    }

    m_Card = m_MatDialog.mCardText->toPlainText();
    m_Name = m_MatDialog.nameEdit->text();
    m_ID = m_MatDialog.IDEdit->text().toInt();
    m_Density = m_MatDialog.densityEdit->text().toDouble();

    int index = itemCur->text(1).toInt();            // Get the material ID
    if (m_Editor->GetMatManager()->UpdateMaterial(index,m_ID,m_Density,m_Name,m_Card))
    {
        QString itemText = "M " + m_MatDialog.IDEdit->text();
        itemCur->setText(0,itemText);
        itemCur->setText(1,m_MatDialog.IDEdit->text());
    }
    else
    {
        return;
    }
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
void McCadMatManageDlg::ShowSelectedMaterial(QTreeWidgetItem* item,int colume)
{
    QTreeWidgetItem* itemCur = m_MatDialog.mMatTree->currentItem();
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
void McCadMatManageDlg::DisplayMaterial(int &iItem)
{
    Material *pMaterial = NULL;
    pMaterial = m_Editor->GetMatManager()->GetMaterial(iItem);

    if (NULL != pMaterial)
    {
        QString strID = QString::number(pMaterial->GetMatID());
        QString strDensity = QString::number(pMaterial->GetMatDensity());
        QString strName = pMaterial->GetMatName();
        QString strMatCard = pMaterial->GetMatCard();

        m_MatDialog.IDEdit->setText(strID);
        m_MatDialog.densityEdit->setText(strDensity);
        m_MatDialog.nameEdit->setText(strName);
        m_MatDialog.mCardText->setText(strMatCard);
    }
    return;
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
void McCadMatManageDlg::SaveMaterial()
{
    QTreeWidgetItem* itemCur = m_MatDialog.mMatTree->currentItem();
    if (NULL == itemCur)
    {
        return;
    }

    int iItem = itemCur->text(1).toInt();            // Get the material ID
    m_Editor->GetMatManager()->InsertMatGroup(m_EditGroupName,iItem);
    //m_Editor->GetMatManager()->SaveXMLfile();
}



/** ********************************************************************
* @brief Clear the edit box
*
* @param
* @return void
*
* @date 23/10/2013
* @author  Lei Lu
************************************************************************/
void McCadMatManageDlg::ClearData()
{
    m_MatDialog.mCardText->clear();
    m_MatDialog.nameEdit->clear();
    m_MatDialog.IDEdit->clear();
    m_MatDialog.densityEdit->clear();
}


/** ********************************************************************
* @brief Set the material group now being edited
*
* @param
* @return void
*
* @date 28/10/2013
* @author  Lei Lu
************************************************************************/
void McCadMatManageDlg::SetEditGroup(QString theGroupName)
{
    m_EditGroupName = theGroupName;
}


/** ********************************************************************
* @brief Set the editor, connect the editor with material manager
*
* @param
* @return void
*
* @date 28/10/2013
* @author  Lei Lu
************************************************************************/
void McCadMatManageDlg::SetCurrentEditor(QMcCad_Editor* theEditor)
{
    m_Editor = theEditor;
}


/** ********************************************************************
* @brief Save the material information as a xml file
*
* @param
* @return void
*
* @date 28/10/2013
* @author  Lei Lu
************************************************************************/
void McCadMatManageDlg::SaveXMLFile(QString &strFileName)
{
    QString fileFilters("*.xml");
    QString theFileName = QFileDialog::getSaveFileName(this, tr("Save As"),	".", fileFilters );

    if (!theFileName.isEmpty())
    {
        if(!theFileName.endsWith(".xml"))
        {
            theFileName.append(".xml");
        }
        assert(m_Editor);
        m_Editor->GetMatManager()->SaveXMLfile(theFileName);
        strFileName = theFileName;
        //QString strInfo = QString("Material xml file \"%1\"has been saved").arg(theFileName);
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
void McCadMatManageDlg::LoadXMLFile(QString &strFileName)
{
    QString fileFilters("*.xml");
    QString theFileName = QFileDialog::getOpenFileName(this, tr("Load"),	".", fileFilters );

    if (!theFileName.isEmpty())
    {
        if(!theFileName.endsWith(".xml"))
        {
            theFileName.append(".xml");
        }
        assert(m_Editor);
        m_Editor->GetMatManager()->LoadXMLfile(theFileName,false);
        Refresh(); // Display the loaded materials
        strFileName = theFileName;
    }
}




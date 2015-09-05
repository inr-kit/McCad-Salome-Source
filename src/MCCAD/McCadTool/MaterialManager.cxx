#include "MaterialManager.hxx"
#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>

#include <qmessagebox.h>

MaterialManager::MaterialManager()
{

}

MaterialManager::~MaterialManager()
{
     // Delete the mapIDMat
     map<int,Material *>::iterator iter;
     iter = m_mapMaterial.begin();

     while(iter != m_mapMaterial.end())
     {
         delete iter->second;
         iter->second = NULL;

         //m_mapMaterial.erase(iter);
         iter++;
     }
     m_mapMaterial.clear();

     MaterialGroup *pGroup = NULL;
     for(int i = 0; i<m_Group.size(); i++)
     {
         pGroup = m_Group.at(i);
         delete pGroup;
         pGroup = NULL;
     }
     m_Group.clear();
}




/** ********************************************************************
* @brief Insert a new material into the map record the
*        material number and material information
*
* @param int theID               // Materia ID
*        double  theDensity      // Material density
*        QString theName,        // Material name
*        QString theMatCard      // Material composition
* @return Standard_Boolean
*
* @date 23/10/2013
* @author  Lei Lu
************************************************************************/
Standard_Boolean MaterialManager::InsertNewMaterial(int theID,
                                        double  theDensity,
                                        QString theName,
                                        QString theMatCard)
{
    Material *pMaterial = new Material(theID,theName,theDensity,theMatCard);
    pair<map<int,Material*>::iterator,bool> result;
    result = m_mapMaterial.insert(pair<int,Material *>(theID,pMaterial));

    if(result.second)
    {
        return Standard_True;
    }
    else
    {
        delete pMaterial; // Delete the created Material class
        pMaterial = NULL;
        return Standard_False;
    }
}




/** ********************************************************************
* @brief  Delete the material from the map, with the input key
*
* @param    int index       // The key of the material being deleted
* @return   Material*
*
* @date 23/10/2013
* @author  Lei Lu
************************************************************************/
Standard_Boolean MaterialManager::DeleteMaterial(int index)
{
    map<int,Material *>::iterator iter;
    iter = m_mapMaterial.find(index);
    if (iter != m_mapMaterial.end())
    {
        delete iter->second;
        iter->second = NULL;
        m_mapMaterial.erase(iter);                         // Remove the material from map

        return Standard_True;
    }
    else
    {
        return Standard_False;
    }
}




/** ********************************************************************
* @brief  Get the material with key index
*
* @param    int index               // The key of the material map
* @return   Material*
*
* @date 23/10/2013
* @author  Lei Lu
************************************************************************/
Material* MaterialManager::GetMaterial(const int index)
{
    map<int,Material *>::iterator iter;
    iter = m_mapMaterial.find(index);
    if (iter != m_mapMaterial.end())
    {
        return iter->second;
    }
    else
    {
        return NULL;
    }
}




/** ********************************************************************
* @brief  Get the material list
*
* @param    int index               // The key of the material map
* @return   Material*
*
* @date 23/10/2013
* @author  Lei Lu
************************************************************************/
vector<Material *> MaterialManager::GetMatList()
{
    vector<Material *> MatList;
    map<int,Material *>::iterator iter;
    iter = m_mapMaterial.begin();
    while (iter != m_mapMaterial.end())
    {
        MatList.push_back(iter->second);
        iter++;
    }
    return MatList;
}



/** ********************************************************************
* @brief Get the detail material information
*
* @param const int index,           // solid ID
         int &iMatNum,              // material ID
         QString &groupName,        // group name
         double &fDensity,          // density
         QString &materialName      // material name
* @return
*
* @date 23/10/2013
* @author  Lei Lu
************************************************************************/
void MaterialManager::GetMatInfo(const int index,
                                 int &iMatNum,
                                 QString &groupName,
                                 double &fDensity,
                                 QString &materialName)
{
    map<int,QString>::iterator iterGroup;
    iterGroup = m_mapSolidGroup.find(index);
    if (iterGroup != m_mapSolidGroup.end())
    {
        groupName = iterGroup->second;
    }

    map<QString,int>::iterator iterMat;
    iterMat = m_mapGroupMaterial.find(groupName);
    if (iterMat != m_mapGroupMaterial.end())
    {
        iMatNum = iterMat->second;
    }

    Material *pMat = NULL;
    pMat = GetMaterial(iMatNum);

    fDensity = pMat->GetMatDensity();
    materialName = pMat->GetMatName();
}


/** ********************************************************************
* @brief Get the detail material information
*
* @param const int index,           // solid ID
         int &iMatNum,              // material ID
         QString &groupName,        // group name
         double &fDensity,          // density
         QString &materialName      // material name
* @return
*
* @date 23/10/2013
* @author  Lei Lu
************************************************************************/
QString MaterialManager::GetGroupName(const int index)
{
    map<int,QString>::iterator iter;
    iter = m_mapSolidGroup.find(index);
    if (iter != m_mapSolidGroup.end())
    {
        return iter->second;
    }
    else
    {
        return "";
    }
}


/** ********************************************************************
* @brief Get the material id of one group
*
* @param QString &groupName,        // group name

* @return int                       // material id of this group
*
* @date 23/10/2013
* @author  Lei Lu
************************************************************************/
int MaterialManager::GetMatNum(const QString groupName)
{
    map<QString,int>::iterator iter;
    iter = m_mapGroupMaterial.find(groupName);
    if (iter != m_mapGroupMaterial.end())
    {
        return iter->second;
    }
    else
    {
        return 0;
    }
}


/** ********************************************************************
* @brief Get the material density of a solid
*
* @param const int index,           // solid ID

* @return float                     // material density
*
* @date 23/10/2013
* @author  Lei Lu
************************************************************************/
float MaterialManager::GetMatDensity(const int index)
{
    Material *pMat = NULL;
    pMat = GetMaterial(index);
    if (pMat != NULL)
    {
        return pMat->GetMatDensity();
    }
    else
    {
        return 0.0;
    }
}



/** ********************************************************************
* @brief Input the solid id and get the material name
*
* @param const int index,           // solid ID

* @return (QString)                 // material name
*
* @date 23/10/2013
* @author  Lei Lu
************************************************************************/
QString MaterialManager::GetMatName(const int index)
{
    Material *pMat = NULL;
    pMat = GetMaterial(index);    
    if (pMat != NULL)
    {
        return pMat->GetMatName();
    }
    else
    {
        return "";
    }
}


/** ********************************************************************
* @brief Insert a new material into the map record the
*        material number and material information
*
* @param    int index               // The key of the item being changed
*           int theID               // New materia ID
*           double  theDensity      // New material density
*           QString theName,        // New material name
*           QString theMatCard      // New material composition
* @return Standard_Boolean
*
* @date 23/10/2013
* @author  Lei Lu
************************************************************************/
Standard_Boolean MaterialManager::UpdateMaterial(int    index,
                                                 int    theID,
                                                 double theDensity,
                                                 QString    theName,
                                                 QString    theMatCard)
{
    map<int,Material *>::iterator iter;
    iter = m_mapMaterial.find(index);
    if (iter != m_mapMaterial.end())
    {
        m_mapMaterial.erase(iter);                                         // Delete the original one
        m_mapMaterial.insert(pair<int,Material *>(theID,iter->second));    // update the map

        iter->second->SetMaterial(theID,theName,theDensity,theMatCard);      //replace with the new material data
        return Standard_True;
    }
    else
    {
        return Standard_False;
    }
}



/** ********************************************************************
* @brief Insert a material group into material group and material map
*
* @param
* @return
*
* @date
* @author  Lei Lu
************************************************************************/
Standard_Boolean MaterialManager::InsertMatGroup(QString &GroupName, int &MateriaID)
{
    //map<QString,int>::iterator iter;
    map<QString,int>::iterator iter;
    iter = m_mapGroupMaterial.find(GroupName);
    if (iter != m_mapGroupMaterial.end())
    {
        m_mapGroupMaterial.erase(iter);                                       // Delete the original one
        m_mapGroupMaterial.insert(pair<QString,int>(GroupName,MateriaID));    // update the map
        return Standard_True;
    }
    else
    {
        m_mapGroupMaterial.insert(pair<QString,int>(GroupName,MateriaID));
        return Standard_False;
    }
}


/** ********************************************************************
* @brief return the all the material number stored
*
* @param
* @return
*
* @date 27/10/2013
* @author  Lei Lu
************************************************************************/
vector<int> MaterialManager::GetMatIDList()
{
    vector<int> MatIDList;
    map<int,Material *>::iterator iter;
    iter = m_mapMaterial.begin();
    while (iter != m_mapMaterial.end())
    {
        MatIDList.push_back(iter->first);
        iter++;
    }
    return MatIDList;
}




/** ********************************************************************
* @brief Generate a map to connect the solid id and material group
*
* @param int iSolidIDBegin,
*        int iSolidIDEnd,
*        QString strMatGroup
* @return
*
* @date 27/10/2013
* @author  Lei Lu
************************************************************************/
Standard_Boolean MaterialManager::GenSolidGroupMap(int iSolidIDBegin, int iSolidIDEnd,
                                                   QString strMatGroup)
{
    if (0 == iSolidIDEnd)
    {
        return Standard_False;
    }

    for (int i = iSolidIDBegin; i <= iSolidIDEnd; i++)
    {
        pair<map<int,QString>::iterator,bool> result;
        result = m_mapSolidGroup.insert(pair<int,QString>(i,strMatGroup));

        if(!result.second)
        {
            return Standard_False;
        }
    }
}




/** ********************************************************************
* @brief Save the material information into a XML file
*
* @param QString theFileName(the XML file)
* @return
*
* @date 27/10/2013
* @author  Lei Lu
************************************************************************/
void MaterialManager::SaveXMLfile(QString theFileName)
{
    //theFileName.replace("/","\/");
    QFile *file = new QFile(theFileName);
    if(!file->open(QFile::WriteOnly|QFile::Truncate))
    {
        return;
    }

    QDomDocument doc;
    QDomProcessingInstruction instruction;
    instruction = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);

    QDomElement root=doc.createElement(("Material"));
    doc.appendChild(root);

    if(!m_mapSolidGroup.empty())
    {
        SaveSolidGroupMap(root, doc);   // Save solidID-materialGroup map
    }

    if(!m_mapGroupMaterial.empty())
    {
        SaveMaterialGroupMap(root, doc);
    }

    if(!m_mapMaterial.empty())
    {
        SaveMaterialMap(root, doc);
    }

    QTextStream out(file);
    doc.save(out,4);
    file->close();
}




/** ********************************************************************
* @brief Save the map connect the solid id and group name
*
* @param
* @return
*
* @date 04/11/2013
* @author  Lei Lu
************************************************************************/
void MaterialManager::SaveSolidGroupMap(QDomElement &prntItem, QDomDocument &domDoc)
{
    QDomElement solidGroupItem = domDoc.createElement(("SolidGroupMap"));

    map<int,QString>::iterator iter;
    iter = m_mapSolidGroup.begin();

    int iBegin = iter->first;
    int iEnd = iBegin;
    QString strGroup = iter->second;
    int iGroup = 0;

    while(iter != m_mapSolidGroup.end())
    {
        int iSolidID = iter->first;
        QString strNextGroup = iter->second;

        iter++;

        if(iter == m_mapSolidGroup.end())
        {
            iGroup ++;
            iEnd = iSolidID;
        }
        else if (strNextGroup != strGroup)
        {
            iGroup ++;
            iEnd = iSolidID - 1;
        }
        else
        {
            continue;
        }

        QDomElement groupNode = domDoc.createElement("Group");
        groupNode.setAttribute("number",QString::number(iGroup));

        QDomElement idBeginNode =  domDoc.createElement("begin");
        QDomElement idEndNode =  domDoc.createElement("end");
        QDomElement matNameNode =  domDoc.createElement("name");

        QDomText txtBegin = domDoc.createTextNode(QString::number(iBegin));
        QDomText txtEnd = domDoc.createTextNode(QString::number(iEnd));
        QDomText txtMaterial = domDoc.createTextNode(strGroup);

        idBeginNode.appendChild(txtBegin);
        idEndNode.appendChild(txtEnd);
        matNameNode.appendChild(txtMaterial);

        groupNode.appendChild(idBeginNode);
        groupNode.appendChild(idEndNode);
        groupNode.appendChild(matNameNode);

        solidGroupItem.appendChild(groupNode);

        strGroup = strNextGroup;
        iBegin = iSolidID;
    }
    prntItem.appendChild(solidGroupItem);
}


/** ********************************************************************
* @brief Save the material map
*
* @param
* @return
*
* @date 04/11/2013
* @author  Lei Lu
************************************************************************/
void MaterialManager::SaveMaterialMap(QDomElement &prntItem, QDomDocument &domDoc)
{
    QDomElement materialItem = domDoc.createElement(("MaterialMap"));

    map<int,Material *>::iterator iter;
    iter = m_mapMaterial.begin();

    while(iter != m_mapMaterial.end())
    {
        int iMatID = iter->first;              // Material number
        Material * pMat = iter->second;        // Get the material

        QDomElement materialIDNode = domDoc.createElement("MaterialID");
        materialIDNode.setAttribute("number",QString::number(iMatID));

        QDomElement idNode =  domDoc.createElement("id");
        QDomElement nameNode =  domDoc.createElement("name");
        QDomElement densityNode =  domDoc.createElement("density");
        QDomElement mcardNode =  domDoc.createElement("card");

        QDomText txtID = domDoc.createTextNode(QString::number(pMat->GetMatID()));
        QDomText txtName = domDoc.createTextNode(pMat->GetMatName());
        QDomText txtDensity = domDoc.createTextNode(QString::number(pMat->GetMatDensity()));
        QDomText txtMatcard = domDoc.createTextNode(pMat->GetMatCard());

        idNode.appendChild(txtID);
        nameNode.appendChild(txtName);
        densityNode.appendChild(txtDensity);
        mcardNode.appendChild(txtMatcard);

        materialIDNode.appendChild(idNode);
        materialIDNode.appendChild(nameNode);
        materialIDNode.appendChild(densityNode);
        materialIDNode.appendChild(mcardNode);

        materialItem.appendChild(materialIDNode);
        iter++;
    }
    prntItem.appendChild(materialItem);
}


/** ********************************************************************
* @brief Save the map connect groups and materials
*
* @param
* @return
*
* @date 04/11/2013
* @author  Lei Lu
************************************************************************/
void MaterialManager::SaveMaterialGroupMap(QDomElement &prntItem, QDomDocument &domDoc)
{
    QDomElement materialGroupItem = domDoc.createElement(("MaterialGroupMap"));

    map<QString,int>::iterator iter;
    iter = m_mapGroupMaterial.begin();

    do{
        QString strGroupNam = iter->first;
        int iMaterialID = iter->second;

        QDomElement groupNode = domDoc.createElement("MatGroup");
        groupNode.setAttribute("Name",strGroupNam);
        QDomText materialID = domDoc.createTextNode(QString::number(iMaterialID));
        groupNode.appendChild(materialID);
        materialGroupItem.appendChild(groupNode);

        iter++;
    }while(iter != m_mapGroupMaterial.end()) ;

    prntItem.appendChild(materialGroupItem);
}


/** ********************************************************************
* @brief Read the XML file, load the material information
*
* @param QString theFileName  (the input XML file which saves
*                              the material information)
* @return
*
* @date 04/11/2013
* @author  Lei Lu
************************************************************************/
Standard_Boolean MaterialManager::LoadXMLfile(QString theFileName, bool bRdSldGrpMap)
{    
    // QString fileName = QString(theFileName.ToCString());
    QFile file(theFileName);
    if(!file.open(QFile::ReadOnly))
    {
        return Standard_False;
    }

    QDomDocument doc;
    if(!doc.setContent(&file))
    {
        file.close();
        return Standard_False;
    }

    // Store the mateirals map
    QDomNodeList materialList = doc.elementsByTagName("MaterialMap");
    if (materialList.size() != 0)
    {
        LoadMaterialMap(materialList);
    }

    if(bRdSldGrpMap)
    {
        // Store the group-material map
        QDomNodeList materialGroupList = doc.elementsByTagName("MaterialGroupMap");
        if (materialGroupList.size() != 0)
        {
            LoadMaterialGroupMap(materialGroupList);
        }

        // Store the solid-group map
        QDomNodeList solidGroupList = doc.elementsByTagName("SolidGroupMap");
        if (solidGroupList.size() != 0)
        {
            LoadSolidGroupMap(solidGroupList);
        }
    }

    file.close();
    GenGroupData();
    return Standard_True;
}



/** ********************************************************************
* @brief Load the map connect groups and materials
*
* @param
* @return
*
* @date 04/11/2013
* @author  Lei Lu
************************************************************************/
void MaterialManager::LoadSolidGroupMap(QDomNodeList & theList)
{
    m_mapSolidGroup.clear();
    QDomNodeList groupList = theList.at(0).childNodes();
    for(int i = 0; i < groupList.size(); i++)
    {
        QDomNode groupNode = groupList.at(i).firstChild();
        int iBegin = 0;
        int iEnd = 0;
        QString strGroupName = "";

        while(!groupNode.isNull())
        {
            QDomElement groupElem = groupNode.toElement(); // convert the node to an element.
            if(!groupElem.isNull())
            {
                if (groupElem.tagName() == "begin")
                {
                    iBegin = groupElem.text().toInt();
                }
                else if(groupElem.tagName() == "end")
                {
                    iEnd = groupElem.text().toInt();
                }
                else if(groupElem.tagName() == "name")
                {
                    strGroupName = groupElem.text();
                }
            }
            groupNode = groupNode.nextSibling();
        }
        GenSolidGroupMap(iBegin,iEnd,strGroupName);// Create the solid id and material group map.        

        // Add the group into the group list.
        Material *pMat = GetMaterial(GetMatNum(strGroupName));
        MaterialGroup *pGroup = new MaterialGroup(iBegin,iEnd,strGroupName,*pMat);
        m_Group.push_back(pGroup);
    }
}



/** ********************************************************************
* @brief Load the map connect materials and groups
*
* @param
* @return
*
* @date 04/11/2013
* @author  Lei Lu
************************************************************************/
void MaterialManager::LoadMaterialGroupMap(QDomNodeList & theList)
{
    m_mapGroupMaterial.clear();
    QDomNodeList materialList = theList.at(0).childNodes();
    for(int i = 0; i < materialList.size(); i++)
    {
        QDomNode materialNode = materialList.at(i);
        QString strGrpName("");
        int iMat;

        QDomElement materialElem = materialNode.toElement(); // convert the node to an element.
        if(!materialElem.isNull())
        {
            strGrpName = materialElem.attribute("Name");
            iMat = materialElem.text().toInt();
        }
        InsertMatGroup(strGrpName,iMat);// Create the material and correspond material group map.
    }
}



/** ********************************************************************
* @brief Load the map connect materials and their detail information
*
* @param
* @return
*
* @date 05/11/2013
* @author  Lei Lu
************************************************************************/
void MaterialManager::LoadMaterialMap(QDomNodeList & theList)
{
    m_mapMaterial.clear();
    QDomNodeList materialList = theList.at(0).childNodes();
    for(int i = 0; i < materialList.size(); i++)
    {
        QDomNode materialNode = materialList.at(i).firstChild();

        int iID = 0;                // Serienumer of aterial
        double fDensity = 0;        // Density of material
        QString strName = "";       // Material name
        QString strMatCard = "";    // Material card includes the composition

        while(!materialNode.isNull())
        {
            QDomElement materialElem = materialNode.toElement(); // convert the node to an element.
            if(!materialElem.isNull())
            {
                if (materialElem.tagName() == "id")
                {
                    iID = materialElem.text().toInt();
                }
                else if(materialElem.tagName() == "name")
                {
                    strName = materialElem.text();
                }
                else if(materialElem.tagName() == "density")
                {
                    fDensity = materialElem.text().toDouble();
                }
                else if(materialElem.tagName() == "card")
                {
                    strMatCard = materialElem.text();
                }
            }
            materialNode = materialNode.nextSibling();
        }
        InsertNewMaterial(iID,fDensity,strName,strMatCard);

    }
}



/** ********************************************************************
* @brief Input the group number, and get the corresponding group
*
* @param int theGroup, the number of group.
* @return
*
* @date 12/11/2013
* @author  Lei Lu
************************************************************************/
MaterialGroup* MaterialManager::GetMaterialGroup(const int index)
{
    if( index >= m_Group.size() )
    {
        return NULL;
    }

    MaterialGroup * pGroup = m_Group.at(index);
    return pGroup;
}



/** ********************************************************************
* @brief  Generate the material information, e.g. each material group
*         information
*
* @param
* @return void
*
* @date 05/11/2013
* @author  Lei Lu
***********************************************************************/
void MaterialManager::GenGroupData()
{
    MaterialGroup *pGroup = NULL;
    QString strCompare = "";            // record the group name for comparing
    int iGroup = 0;                     // the serial number of group

    map<int,QString>::iterator iter;
    iter = m_mapSolidGroup.begin();

    while(iter != m_mapSolidGroup.end())
    {
        int iSolid = iter->first;                // Get the id of solid
        QString groupName = iter->second;        // Get the material

        /** Generate a map to store the material group information.
            It will be printed before each one material group*/
        if (groupName != strCompare)         // A new group starts
        {
            pGroup = GetMaterialGroup(iGroup);
            iGroup++;

            // insert a new group information in group information map
            pair<map<int,MaterialGroup*>::iterator,bool> result;
            result = m_groupInfo.insert(pair<int,MaterialGroup*>(iSolid,pGroup));
            if(result.second)
            {
                strCompare = groupName;
            }
        }
        iter++;
    }

//    map<int,Material *>::iterator iter;
//    iter = m_mapMaterial.begin();
//    while (iter != m_mapMaterial.end())
//    {
//        Material * pMat = GetMaterial(iter->first);
//        m_material.push_back(pMat);
//        iter++;
//    }
}


MaterialGroup * MaterialManager::GetGroupData(const int index)
{
    map<int,MaterialGroup*>::iterator iter;
    iter = m_groupInfo.find(index);
    if(iter != m_groupInfo.end())
    {
        return iter->second;
    }
    else
    {
        return NULL;
    }
}

























/* TCollection_AsciiString theFileName = "test.txt";

 OSD_Path thePath(theFileName);
 OSD_File theFile(thePath);
 theFile.Build(OSD_ReadWrite , OSD_Protection());

 TCollection_AsciiString FileName = thePath.Name() + thePath.Extension();
 const char* strName = FileName.ToCString();
 ofstream theStream(strName);

 theStream.setf(ios::left);

 int iNumOfMat = m_mapSolidGroup.size();

 map<int,QString>::iterator iter;
 iter = m_mapSolidGroup.begin();

 for(int i = 1; i <= iNumOfMat; i++ )
 {
     int iSolidID = iter->first;
     QString strGroup = iter->second;

     map<QString,int>::iterator material_iter;
     material_iter = m_mapMatGroup.find(strGroup);
     if (material_iter != m_mapMatGroup.end())
     {
         int iMat = m_mapMatGroup.at(strGroup);
         double fdensity = GetMaterial(iMat)->GetMatDensity();
         theStream<<iSolidID<<"         "<<strGroup.toStdString()<<"       "<<iMat<<"         "<<fdensity<<endl;
         iter ++;
     }
     else
     {
         iter ++;
         continue;
     }
 }

 theFile.Close();*/


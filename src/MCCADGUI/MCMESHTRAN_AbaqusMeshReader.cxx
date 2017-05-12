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
#include "MCMESHTRAN_AbaqusMeshReader.hxx"
#include <stdio.h>

#include <QStringList>
#include <QChar>

#include "utilities.h"


MCMESHTRAN_AbaqusMeshReader::MCMESHTRAN_AbaqusMeshReader()
{
    myAbqFileName = "";
    currentLine.clear();
    lineNum = 0;
    isSkipError = false;

    KeyWord["HEADING"]          = Key_heading;
    KeyWord["PART"]             = Key_part;
    KeyWord["END PART"]         = Key_part_end;
    KeyWord["ASSEMBLY"]         = Key_assembly;
    KeyWord["END ASSEMBLY"]     = Key_assembly_end;
    KeyWord["NODE"]             = Key_node;
    KeyWord["ELEMENT"]          = Key_element;
    KeyWord["INSTANCE"]         = Key_instance;
    KeyWord["END INSTANCE"]     = Key_instance_end;
    //    KeyWord["NSET"]             = ;
    //    KeyWord["ELSET"]            = ;
    //    KeyWord["SOLID SECTION"]    = ;

    ElementType["C3D4"]         = INTERP_KERNEL::NORM_TETRA4;
    ElementType["C3D10"]        = INTERP_KERNEL::NORM_TETRA10;
//    ElementType[""]           = INTERP_KERNEL::NORM_PYRA5;
//    ElementType[""]           = INTERP_KERNEL::NORM_PYRA13;
    ElementType["C3D6"]         = INTERP_KERNEL::NORM_PENTA6;
    ElementType["C3D15"]        = INTERP_KERNEL::NORM_PENTA15;
    ElementType["C3D8"]         = INTERP_KERNEL::NORM_HEXA8;
    ElementType["C3D20"]        = INTERP_KERNEL::NORM_HEXA20;
    ElementType["C3D27"]        = INTERP_KERNEL::NORM_HEXA27;
}

MCMESHTRAN_AbaqusMeshReader::~MCMESHTRAN_AbaqusMeshReader()
{
    //release the references
    for (int i=0; i<myParts.size(); i++) {
        myParts[i]->decrRef();
    }
    for (int i=0; i<myMeshList.size(); i++) {
        myMeshList[i]->decrRef();
    }
}

/*!
 * \brief return a list of UMesh, these mesh should decrRef() after used.
 * \return list of UMesh, these mesh should decrRef() after used.
 */
vector <const MEDCouplingUMesh*> MCMESHTRAN_AbaqusMeshReader::getMeshList ()     //return the mesh list
{
    vector <const MEDCouplingUMesh*> aTmpList ;
    for (int i=0; i<myMeshList.size(); i++)    {
        myMeshList[i]->incrRef();    //increase reference because myMeshList will be destroy when the reader is deleted,
        aTmpList.push_back(myMeshList[i]);
    }
    return aTmpList;
}


/*!
 * \brief load a abaqus mesh File, only the parts are created as mesh,
 * and transform to instances, the Nsets and Elsets are not take cared.
 * at the end the meshes will be merge into a big mesh of the assembly
 * \param AbqFileName Abaqus File Name
 * \return the reading status
 */
AbqLoadStatus MCMESHTRAN_AbaqusMeshReader::loadFile (const QString &AbqFileName, const bool &skipError )  //load the abaqus file
{
    myAbqFileName = AbqFileName;
    myAbaqusFile.open(myAbqFileName.toLatin1());
    if(myAbaqusFile.is_open())
        cout << "File " << myAbqFileName.toStdString() << " is open.\n";
    else    {
        cout << "Error opening " << myAbqFileName.toStdString()  << ".\n";
        return Load_failed;
    }

    isSkipError = skipError;
    AbqLineType theType = getLineAndCheck();
    AbqKeyType theKey;  //should initialized here
    bool isOk = true;
    bool Ok = true;
    while(theType != Line_eof )
    {
        switch (theType)
        {
        case Line_keyword:
            theKey = getKeyType();
            switch (theKey)
            {
            case Key_heading :
                Ok = read_heading();
                break;
            case Key_part:
                Ok = read_part();
                theType = getLineAndCheck();
                break;
            case Key_assembly:
                Ok = read_assembly();
                theType = getLineAndCheck();
                break;
            default:  //skip other key type
                Ok = skipUnknown();
                theType = checkLineType(); // check the type, otherwise the type is unknown to next loop
            }
            if (!Ok) {
                if (!isSkipError){
                    MESSAGE("Error in reading file!" );
                    return Load_failed;
                }
                MESSAGE("Something wrong in reading file!" );
                isOk = Ok;
            }
            break;
        case Line_comment :
            theType = getLineAndCheck();
            break;
        case Line_data:
            MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                    << lineNum << ": should not be data line" );
            if (!isSkipError)
                return Load_failed;
            isOk = false;//return warning at the end
            theType = getLineAndCheck();
            break;
        case Line_blank:
            MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                    << lineNum << ": should not be blank" );
            if (!isSkipError)
                return Load_failed;
            isOk = false; //return warning at the end
            theType = getLineAndCheck();
            break;
        default:
            MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                    << lineNum << ": unknown error." );
            return Load_failed;
        }
    }
    if (!isOk)
        return Load_warning;
    return Load_ok;
}

/*!
 * \brief read the heading of the file, right now just skip the heading
 * \return \c true or \c false
 */
bool    MCMESHTRAN_AbaqusMeshReader::read_heading()  //currently for skip the heading
{
    AbqLineType theType = getLineAndCheck();
    if (theType == Line_blank && !isSkipError)    {
        MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                << lineNum << ": found blank line!" );
        return false;
    }
    while (theType == Line_comment || theType == Line_data || theType == Line_blank)
        theType = getLineAndCheck();
    return true;
}

/*!
 * \brief read a part, construct a UMesh with the mesh name as part name,
 *
 * \return \c true or \c false
 */
bool    MCMESHTRAN_AbaqusMeshReader::read_part()  //read a part and return as a mesh
{
    QString aTmpStr;
    aTmpStr = QString::fromStdString(currentLine).trimmed();
    QMap <QString, QString> aMap = getParaValue(aTmpStr);
    QString aPartName = aMap.value("NAME","unknown");
    if (aPartName == "unknown")
    {
        MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                      << lineNum << ": part is not named!" );
        return false;
    }

    bool isEndofPart = false;
    vector <double > aCorVec;   // vector for storing coordinates
    vector <int> aIndexVec ; //indice for the nodes, because they might not in sequential order
    vector <int> Connectivity ; //nodal connectivity
    vector <int> NodalConnIndex;

    AbqLineType theType = getLineAndCheck();
    AbqKeyType theKey;
    bool Ok = true;// to see if read nodes and elements correct
    int aInitNodeIdx = 1;
    while (!isEndofPart)
    {
        switch (theType)
        {
        case Line_keyword:
            theKey = getKeyType();
            switch (theKey)
            {
            case Key_node :
                Ok = read_nodes(aCorVec, aIndexVec);
                break;
            case Key_element:
                Ok = read_elements(Connectivity, NodalConnIndex);
                break;
            case Key_part_end:
                isEndofPart = true;
                break;
            default:  //skip other keyword and contents
                Ok = skipUnknown();
                theType = checkLineType(); // check the type, otherwise the type is unknown to next loop
                break;
            }
            if (!Ok)
                return false;
//            theType = getLineAndCheck();
            break;
        case Line_comment :
            theType = getLineAndCheck();
            break;
        case Line_data:
            MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                    << lineNum << ": should not be data line" );
            if (!isSkipError)
                return false;
            theType = getLineAndCheck();
            break;
        case Line_blank:
            MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                    << lineNum << ": should not be blank" );
            if (!isSkipError)
                return false;
            theType = getLineAndCheck();
            break;
        case Line_eof:
            MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                    << lineNum << ": should have a *end part key word at the end" );
            if (!isSkipError)
                return false;
            else
                isEndofPart = true;  // consider the part is end
            break;
        default:
            MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                    << lineNum << ": unknown error." );
            return false;
        }
    }

    if (aCorVec.size() == 0 || Connectivity.size() == 0 || NodalConnIndex.size() == 0) {
        MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                      << lineNum << ": Found end of part but nodes or elements not read!" );
        return false;
    }

    //process the data
    //check if nodes are sequential
    bool isSequential = true;
    for (int i=0; i<aIndexVec.size()-1; i++) {
        if (aIndexVec[i] +1  != aIndexVec[i+1] ) {
            isSequential = false;
            break;
        }
    }

    //process the nodal connectivities
//    int cnt = 0; //looping the NodalConnIndex
    if (isSequential) {
        for (int i=0, cnt=0; i<Connectivity.size(); i++) {
            if (i == NodalConnIndex[cnt]) { //skip the element type
                cnt ++;
                continue;
            }
            Connectivity[i] = Connectivity[i] - aIndexVec[0];//IMPORTANT!! MED nodal index start from 0, but abaqus from 1!
        }
    }
    else { //if the node indices are not sequentially orderred.
        for (int i=0, cnt=0; i<Connectivity.size(); i++) {
            if (i == NodalConnIndex[cnt]) { //skip the element type
                cnt ++;
                continue;
            }
            //search for the node index
            int j;
            for (j=0; j<aIndexVec.size(); j++) {
                if (aIndexVec[j] == Connectivity[i] ) {
                    Connectivity[i] = j ; //point to the position of the node in aIndexVec
                    break;
                }
            }
            if (j == aIndexVec.size()) {
                MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                        << lineNum << ": Cannot find the node "  << Connectivity[i]<< " from node list!" );
                return false;
            }
        }
    }

    //copy data
    DataArrayDouble * theNodes = DataArrayDouble::New(); // the node list
    DataArrayInt    * theElmConn = DataArrayInt::New(); // the element connnectitly
    DataArrayInt    * theElmConnIndex = DataArrayInt::New(); // the element connnectitly index
    theNodes->alloc(aCorVec.size()/3 , 3);  //a data array with 3 components
    copy(aCorVec.begin(), aCorVec.end(), theNodes->getPointer());
    theElmConn->alloc(Connectivity.size(),1);
    copy(Connectivity.begin(), Connectivity.end(), theElmConn->getPointer());
    theElmConnIndex->alloc(NodalConnIndex.size(),1);
    copy(NodalConnIndex.begin(), NodalConnIndex.end(), theElmConnIndex->getPointer());

    MEDCouplingUMesh *aMesh=ParaMEDMEM::MEDCouplingUMesh::New(aPartName.toStdString(), 3);
    aMesh->setConnectivity(theElmConn, theElmConnIndex, false);
    aMesh->setCoords(theNodes);
    theNodes->decrRef(); theElmConn->decrRef(); theElmConnIndex->decrRef();  //decrease the reference, other wise array is still there even mesh is deleted
    myParts.push_back(aMesh);
    return true;
}

/*!
 * \brief read the node list into a MED array, the array include 3 components: x, y, z
 * \param the MED array to output data
 * \return \c true if success, \c false if error
 */
bool  MCMESHTRAN_AbaqusMeshReader::read_nodes (vector <double > & aCorVec, vector<int> &aIndexVec)  //read node and return the node list
{
    AbqLineType theType = getLineAndCheck();
    QString aTmpStr;
    QStringList aStringList;
    bool isOk;
//    aIndexVec = 1e8; //just a big number
    while (theType != Line_eof && theType != Line_keyword)
    {
        switch (theType)
        {
        case Line_data:
            aTmpStr.clear();
            aTmpStr = QString::fromStdString(currentLine).trimmed();
            aTmpStr = aTmpStr.replace(","," "); // replace "," with " "
            aStringList = aTmpStr.trimmed().split(" ", QString::SkipEmptyParts);
            if (aStringList.size() == 4) { // a line should include : index, x, y, z
//                int aIndex = aStringList[0].toInt(&isOk);
//                if (aIndexVec > aIndex)  aIndexVec = aIndex ;  //a study way to get the initial index, but does not matter
                aIndexVec.push_back(aStringList[0].toInt(&isOk));
                for (int i=1; i<4; i++) {
                    aCorVec.push_back(aStringList[i].toDouble(&isOk)); //x,y,z
                    if (!isOk) {
                        MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                                      << lineNum << ": unknown error in this line!" );
                        return false;
                    }
                }
            }
            else {
                MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                        << lineNum << ": data might be demaged!" );
                return false;
            }
            theType = getLineAndCheck();
            break;
        case Line_comment :
            theType = getLineAndCheck();
            break;
        case Line_blank:
            MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                    << lineNum << ": should not be blank" );
            if (!isSkipError)
                return false;
            theType = getLineAndCheck();
            break;
        default:
            return false;
        }
    }
    return true;
}

/*!
 * \brief read the element connectivities
 * the *elements key word might appear several times in mixed element part,
 * thus the element connectivities read later will be appended
 * \param theConnectivities the return connectivities,
 * \param theConnIndex the connectivity index, indexing the position of the beginning of every element
 * \return  \c true if success, \c false if error
 */
bool    MCMESHTRAN_AbaqusMeshReader::read_elements(vector <int> & Connectivity, vector <int> & NodalConnIndex)  //read the element connectivities and return the list
{

    //get the abaqus element type
    QString aTmpStr = QString::fromStdString(currentLine).trimmed();
    QMap <QString, QString> aMap = getParaValue(aTmpStr);
    QString aElmType = aMap.value("TYPE","unknown");

    //remove usless char in element type, e.g.  DCC3D8D -> C3D8
    while (aElmType.at(0) != 'C' && !aElmType.at(1).isNumber() ) // catch the C3
        aElmType.remove(0,1);  //remove 1 char from beginning
    while (!(aElmType.at(aElmType.size()-1).isNumber())) //if the ending char is not number, chop the end char
        aElmType.chop(1);

    int aMEDElmType = ElementType.value(aElmType, 0); //get the element tyep else get 0

    if (aElmType == "unknown" || aMEDElmType == 0) {
        MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                      << lineNum << ": element type is not recognized!" );
        return false;
    }

    //get the node number
    while (aElmType.at(0) != 'D') // e.g. C3D8->D8
        aElmType.remove(0,1);
    aElmType.remove(0,1); // e.g. D8->8
    bool isOk;
    int theNodeNb = aElmType.toInt(&isOk);
    if (!isOk) {
        MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                      << lineNum << ": get node number failed!" );
        return false;
    }

    //read the element connectivities
    QStringList aStringList;
    AbqLineType theType = getLineAndCheck();
    while (theType != Line_eof && theType != Line_keyword)
    {
        switch (theType)
        {
        case Line_data:
            aTmpStr.clear();
            aTmpStr = QString::fromStdString(currentLine).trimmed();
            aTmpStr = aTmpStr.replace(","," "); // replace "," with " "
            aStringList = aTmpStr.trimmed().split(" ", QString::SkipEmptyParts);
            //save the conn
            Connectivity.push_back(aMEDElmType); // for each cell, first put the cell type
            NodalConnIndex.push_back(Connectivity.size() - 1); // put the index of this cell in Connectivity array into index array
            if (aStringList.size() == theNodeNb + 1) { // a line should include : index, conns...
                for (int i=1; i<=theNodeNb; i++) { //start from 1 to skip the element index,
//                    Connectivity.push_back(aStringList[i].toInt(&isOk) -1); //IMPORTANT!! MED nodal index start from 0, but abaqus from 1!
                   Connectivity.push_back(aStringList[i].toInt(&isOk) );     //The connectivities will be correct in read_parts()
                   if (!isOk) {
                       MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                               << lineNum << ": unknown error in this line!" );
                       return false;
                   }
                }
            }
            else { //if using two line to specify the connectivities
                //append the first line
                int i; //to keep i
                for (i=1; i<aStringList.size(); i++) { //start from 1 to skip the element index,
//                    Connectivity.push_back(aStringList[i].toInt(&isOk) -1);
                    Connectivity.push_back(aStringList[i].toInt(&isOk) ); //The connectivities will be correct in read_parts()
                    if (!isOk) {
                        MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                                << lineNum << ": unknown error in this line!" );
                        return false;
                    }
                }
                //append the second line
                theType = getLineAndCheck();
                aTmpStr.clear();
                aTmpStr = QString::fromStdString(currentLine).trimmed();
                aTmpStr = aTmpStr.replace(","," "); // replace "," with " "
                aStringList = aTmpStr.trimmed().split(" ", QString::SkipEmptyParts);
                if (aStringList.size() != theNodeNb - (i-1)) { //i= previous aStringList.size()
                    MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                            << lineNum << ": data might be demaged!" );
                    return false;
                }
                for (i=0; i<aStringList.size(); i++) {
//                    Connectivity.push_back(aStringList[i].toInt(&isOk) -1);
                      Connectivity.push_back(aStringList[i].toInt(&isOk) );  //The connectivities will be correct in read_parts()
                    if (!isOk) {
                        MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                                << lineNum << ": unknown error in this line!" );
                        return false;
                    }
                }
            }
            theType = getLineAndCheck();
            break;
        case Line_comment :
            theType = getLineAndCheck();
            break;
        case Line_blank:
            MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                    << lineNum << ": should not be blank" );
            if (!isSkipError)
                return false;
            theType = getLineAndCheck();
            break;
        default:
            return false;
        }//end of switch(theType)
    } //end of while
    NodalConnIndex.push_back(Connectivity.size());//pushed the last index. IMPORTRANT!!
    return true;
}

/*!
 * \brief skip the unknown data, use for nset, elset, and other info
 * \return
 */
bool    MCMESHTRAN_AbaqusMeshReader::skipUnknown()
{
    AbqLineType theType = getLineAndCheck();
    if (theType == Line_blank && !isSkipError)    {
        MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                << lineNum << ": found blank line!" );
        return false;
    }
    while (theType == Line_comment || theType == Line_data || theType == Line_blank)
        theType = getLineAndCheck();
    return true;
}


/*!
 * \brief read the assembly.
 * the assembly mainly contains instances, other data is not supported.
 * at the end the whole assembly is created as a big mesh by merging all the mesh in the instances
 * \return \c true if success
 */
bool    MCMESHTRAN_AbaqusMeshReader::read_assembly() //read the assembly and return a list of mesh
{
    QString aTmpStr;
    aTmpStr = QString::fromStdString(currentLine).trimmed();
    QMap <QString, QString> aMap = getParaValue(aTmpStr);
    QString aAssemblyName = aMap.value("NAME","unknown");
    if (aAssemblyName == "unknown")    {
        MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                      << lineNum << ": Assembly is not named!" );
        return false;
    }

    bool isEndofAssembly = false;
    AbqLineType theType = getLineAndCheck();
    AbqKeyType theKey;
    bool Ok = true;
    while (!isEndofAssembly)
    {
        switch (theType)
        {
        case Line_keyword:
            theKey = getKeyType();
            switch (theKey)
            {
            case Key_instance :
                Ok = read_instance();
                theType = getLineAndCheck();
                break;
            case Key_assembly_end:
                isEndofAssembly = true;
                break;
            default:  //skip the unknown info
                Ok = skipUnknown();
                theType = checkLineType(); //get the type again
            }
            if (!Ok && !isSkipError)
                return false;
            break;
        case Line_comment :
            theType = getLineAndCheck();
            break;
        case Line_data:
            MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                    << lineNum << ": Data for assembly is not supported" );
            if (!isSkipError)
                return false;
            theType = getLineAndCheck();
            break;
        case Line_blank:
            MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                    << lineNum << ": should not be blank" );
            if (!isSkipError)
                return false;
            theType = getLineAndCheck();
            break;
        case Line_eof:
            MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                    << lineNum << ": have a *end assembly key word at the end" );
            if (!isSkipError)
                return false;
            else
                isEndofAssembly = true;  // consider it is end
            break;
        default:
            MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                    << lineNum << ": unknown error." );
            return false;
        }
    }//end of while

    //merge the mesh into one for the assembly// comment out because it failed in some cases
//     MEDCouplingUMesh *aMesh;
//     try {
//         aMesh = MEDCouplingUMesh::MergeUMeshes(myMeshList);
//     }
//     catch(INTERP_KERNEL::Exception &ex) {
//         MESSAGE("Error when merging mesh: " << ex.what()  );
//         return false;
//     }
//    myMeshList.push_back(aMesh);
    return true;
}

/*!
 * \brief read instances
 * the instances are created based on parts, then makes spatial transform
 * at the parts to create instances. create instances front-end is not supported
 * \return \c true if success
 */
bool    MCMESHTRAN_AbaqusMeshReader::read_instance() //read a instance and return as a mesh
{
    //get the instance name and the associated part name
    QString aTmpStr;
    aTmpStr = QString::fromStdString(currentLine).trimmed();
    QMap <QString, QString> aMap = getParaValue(aTmpStr);
    QString aInstanceName = aMap.value("NAME","unknown");
    QString theInstPartName = aMap.value("PART","unknown");
    if (aInstanceName == "unknown" || theInstPartName == "unknown")    {
        MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                      << lineNum << ": Instance name not found, or associated part not found!" );
        return false;
    }

    //check if the part are inside part list
    if (myParts.size() == 0) {
        MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                      << lineNum << ": No part is read yet!" );
        return false;
    }
    int thePartIndex = -1;
    for (int i=0; i<myParts.size(); i++) {
        if (strcmp(myParts.at(i)->getName().c_str(), theInstPartName.toStdString().c_str()) == 0) {
            thePartIndex = i;
            break;
        }
    }
    if (thePartIndex == -1) {
        MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                      << lineNum << ": Part cannot found in the read list!" );
        return false;
    }

    //get the translation and rotation parameter
    QStringList aStringList;
    bool isEndofInstance = false;
    AbqLineType theType = getLineAndCheck();
    AbqKeyType theKey;
    vector <double > theTranslation ;
    vector <double > theRotation ;
    bool isOk;

    while (!isEndofInstance)
    {
        switch (theType)
        {
        case Line_data:
            aTmpStr.clear();
            aTmpStr = QString::fromStdString(currentLine).trimmed();
            aTmpStr = aTmpStr.replace(","," "); // replace "," with " "
            aStringList = aTmpStr.trimmed().split(" ", QString::SkipEmptyParts);
            if (aStringList.size() == 3) {
                for (int i=0; i<3; i++) {
                    theTranslation.push_back(aStringList[i].toDouble(&isOk));
                    if (!isOk) {
                        MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                                << lineNum << ": unknown error in this line!" );
                        return false;
                    }
                }
            }
            else if (aStringList.size() == 7) {
                for (int i=0; i<7; i++) {
                    theRotation.push_back(aStringList[i].toDouble(&isOk));
                    if (!isOk) {
                        MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                                << lineNum << ": unknown error in this line!" );
                        return false;
                    }
                }
            }
            else {
                MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                        << lineNum << ": data might be demaged!" );
                return false;
            }
            theType = getLineAndCheck();
            break;
        case Line_keyword:
            theKey = getKeyType();
            switch (theKey)
            {
            case Key_instance_end:
                isEndofInstance = true;
                break;
            default:  //any other keyword inside instance is not supported
                MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                        << lineNum << ": Only those instances with translation and rotation is supported! " );
                if (!isSkipError)
                    return false;
                else {
                    skipUnknown();
                    theType = checkLineType();
                    break;
                }
            }
//            theType = getLineAndCheck();
            break;
        case Line_comment :
            theType = getLineAndCheck();
            break;
        case Line_blank:
            MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                    << lineNum << ": should not be blank" );
            if (!isSkipError)
                return false;
            theType = getLineAndCheck();
            break;
        case Line_eof:
            MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                    << lineNum << ": have a *end part key word at the end" );
            if (!isSkipError)
                return false;
            else
                isEndofInstance = true;  // consider the part is end
            break;
        default:
            MESSAGE("Error in File "<<myAbqFileName.toStdString() <<endl<<"line "
                    << lineNum << ": unknown error." );
            return false;
        } //end of switch (theType)
    }//end of while

    MEDCouplingUMesh *aMesh = dynamic_cast <MEDCouplingUMesh *> (myParts[thePartIndex]->deepCpy());  // copy the mesh from part
    aMesh->setName(aInstanceName.toStdString());
    if (theTranslation.size() == 3)  //if make translation
    {
        const double vec [3] = {theTranslation[0], theTranslation[1], theTranslation[2]};
        try {
            aMesh->translate(vec); //
        }
        catch(INTERP_KERNEL::Exception &ex) {
            MESSAGE("Error when translating mesh: " << ex.what()  );
            aMesh->decrRef();
            return false;
        }
    }
    if (theRotation.size() == 7)
    {
        const double cntr [3] = {theRotation[0], theRotation[1], theRotation[2]};  // the center point
        const double vec [3] =  {theRotation[3]-theRotation[0], theRotation[4]-theRotation[1], theRotation[5]-theRotation[2]}; // the vector, which form by two points
        double AngleInRadian = theRotation[6] * 3.1415926 / 180; // convert from degree to Radian: radian = degree * PI/ 180
        try {
            aMesh->rotate(cntr, vec, AngleInRadian);
        }
        catch(INTERP_KERNEL::Exception &ex) {
            MESSAGE("Error when rotating mesh: " << ex.what()  );
            aMesh->decrRef();
            return false;
        }
    }
    myMeshList.push_back(aMesh);
    return true;
}


AbqLineType MCMESHTRAN_AbaqusMeshReader::getLineAndCheck()
{
    currentLine.clear();
    getline(myAbaqusFile, currentLine);
//    cout <<currentLine<<endl;//for testing
    ++lineNum;
    return checkLineType();
}


AbqLineType  MCMESHTRAN_AbaqusMeshReader::checkLineType()    //check the line type
{
    //eof
    if (myAbaqusFile.eof())
        return Line_eof;
    //blanck
    QString aTmpStr = QString::fromStdString(currentLine).trimmed();
    if (aTmpStr.trimmed() == "")
        return Line_blank;
    //comment,keyword,data
    if (aTmpStr.at(0) == '*')
        if(aTmpStr.at(1) == '*')
            return Line_comment;
        else return Line_keyword;
    else return Line_data;
}


AbqKeyType MCMESHTRAN_AbaqusMeshReader::getKeyType ()
{
    QString aTmpStr = QString::fromStdString(currentLine).trimmed(); //trimmed to get rid of dos ASCII
    aTmpStr.replace(","," "); //replace "," with " "
    QStringList aStringList = aTmpStr.split(" ", QString::SkipEmptyParts);
    QString bTmpStr = aStringList[0].remove(0,1);; //get the first part, and remove '*'
    if (bTmpStr.toUpper() == "END") //if key=END, append the 2nd word (case "End part" ...)
        bTmpStr += QString(" ") + aStringList[1];

    return KeyWord.value(bTmpStr.toUpper(), Key_unknow);
}

QString  MCMESHTRAN_AbaqusMeshReader::chopKeyWord(const QString &aLine)
{
    QString aTmpStr = aLine;
    aTmpStr.replace(","," "); //replace "," with " "
    return aTmpStr.remove(0, aTmpStr.indexOf(" "));
}

QMap <QString, QString > MCMESHTRAN_AbaqusMeshReader::getParaValue(const QString &aLine)
{
    QMap <QString, QString > aMap;
    QString aTmpStr = aLine;
    aTmpStr.replace(","," ");
    QStringList aStringList = aTmpStr.split(" ", QString::SkipEmptyParts);
    if (aStringList.size() == 0 )
        return aMap;  //return a empty map
    for (int i=1; i<aStringList.size(); i++) //1-skip the first keyword section
    {
        QString bTmpStr = aStringList[i];
        QStringList bStringList = bTmpStr.split("=");//split the key and value
        aMap.insert(bStringList[0].toUpper(), bStringList[1]);
    }
    return aMap;
}

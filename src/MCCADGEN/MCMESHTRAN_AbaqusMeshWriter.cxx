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
#include "MCMESHTRAN_AbaqusMeshWriter.hxx"
#include "utilities.h"
#include <iostream>

MCMESHTRAN_AbaqusMeshWriter::MCMESHTRAN_AbaqusMeshWriter()
{
    myOutFileName = "";
    ElementType[INTERP_KERNEL::NORM_TETRA4]         = "C3D4";
    ElementType[INTERP_KERNEL::NORM_TETRA10]        = "C3D10";
    ElementType[INTERP_KERNEL::NORM_PENTA6]         = "C3D6";
    ElementType[INTERP_KERNEL::NORM_PENTA15]        = "C3D15";
    ElementType[INTERP_KERNEL::NORM_HEXA8]          = "C3D8";
    ElementType[INTERP_KERNEL::NORM_HEXA20]         = "C3D20";
    ElementType[INTERP_KERNEL::NORM_HEXA27]         = "C3D27";
}

MCMESHTRAN_AbaqusMeshWriter::~MCMESHTRAN_AbaqusMeshWriter()
{
    // don't need to release the MEDCouplingUmesh Pointers
    //they are still in use outside
}

/*!
 * \brief set the mesh list into the class
 * \param myMeshList list of MEDCouplingUMesh to be set
 */
void    MCMESHTRAN_AbaqusMeshWriter::setMeshes(vector <const MEDCouplingUMesh *> & MeshList)
{
    myMeshList = MeshList;
}

/*!
 * \brief export the meshes into abaqus file
 * \param OutFileName file path and name to be output
 * \return false if error
 */
bool     MCMESHTRAN_AbaqusMeshWriter::exportFile (const QString & OutFileName)
{
    //#######open the file
    myOutFileName = OutFileName;
    myOutFile.open( myOutFileName.toLatin1(), ios_base::trunc); //to overide the
    if(myOutFile.is_open())
        cout << "File " << myOutFileName.toStdString() << " is open to export Abaqus mesh.\n";
    else    {
        cout << "Error opening " << myOutFileName.toStdString()  << ".\n";
        return false;
    }

    //#######write the file heading ########
    myOutFile << "*Heading" <<endl;
    myOutFile <<"** Created by McMeshTran_AbaqusMeshWriter"<<endl;
    myOutFile <<"** further infomation contact yuefeng.qiu@kit.edu"<<endl;
    myOutFile <<"*Preprint, echo=NO, model=NO, history=NO, contact=NO"<<endl;

    //#######write mesh into parts ########
    if (myMeshList.size() == 0) {
        MESSAGE("No mesh to be export!");
        return false;
    }
    //loop all the meshes
    vector <int > outputedMeshes; //to append the index which meshes are outputed as a part, in case some mesh failed
    for (int i=0; i<myMeshList.size(); i++)
    {
        MEDCouplingUMesh * aMesh = const_cast <MEDCouplingUMesh * >( myMeshList[i]);  //don't decrRef()
        //check mesh first
        if (aMesh->getNumberOfCells() == 0) {
            MESSAGE("Mesh " <<aMesh->getName() <<" has no cell!");
            continue;  //next mesh
        }
        //MCNP6 allow mesh only using tetra, hexa, penta or combination of hexa and penta
        //here does not consider situations such as HEXA8 and HEXA20 exist in a same mesh
//        set <INTERP_KERNEL::NormalizedCellType> ElmTypes = aMesh->getAllTypes();
        set <INTERP_KERNEL::NormalizedCellType> ElmTypes = aMesh->getAllGeoTypes(); //change to salome 7.4.0
        if (ElmTypes.size() > 2) {
            MESSAGE("Mesh contain more than two element types!");
            continue;
        }
        else if (ElmTypes.size() == 2) {
            for (set<INTERP_KERNEL::NormalizedCellType>::iterator it=ElmTypes.begin(); it != ElmTypes.end(); ++it) {
                if (*it != INTERP_KERNEL::NORM_HEXA8 &&
                        *it != INTERP_KERNEL::NORM_HEXA20 &&
                        *it != INTERP_KERNEL::NORM_PENTA6 &&
                        *it != INTERP_KERNEL::NORM_PENTA15) {
                    MESSAGE("Only hexa and penta can exist in a hybrid mesh!");
                    continue;
                }
            }
        }
        else if (ElmTypes.size() == 1) {
            set<INTERP_KERNEL::NormalizedCellType>::const_iterator it = ElmTypes.begin();
            if (*it != INTERP_KERNEL::NORM_HEXA8 &&
                    *it != INTERP_KERNEL::NORM_HEXA20 &&
                    *it != INTERP_KERNEL::NORM_PENTA6 &&
                    *it != INTERP_KERNEL::NORM_PENTA15 &&
                    *it != INTERP_KERNEL::NORM_TETRA4 &&
                    *it != INTERP_KERNEL::NORM_TETRA10) {
                MESSAGE("should contain only one of hexa, penta or tetra element!");
                continue;
            }
        }
        else {
            MESSAGE("Error in getting element type of this mesh!");
            continue;
        }

        //print the part and its name
        QString aTmpName;
        if (aMesh->getName().c_str() == 0) //if no name
//        if (aMesh->getName().empty()) //if no name
            aTmpName = QString("Mesh-") + QString("%1").arg(i+1);
        else
            aTmpName = aMesh->getName().c_str();
//            aTmpName = aMesh->getName().c_str();
        myOutFile <<"*Part, name=Part-" <<aTmpName.toStdString() <<endl;

        //output the mesh coordinates
        myOutFile <<"*Node"<<endl;
        DataArrayDouble * aCoord = aMesh->getCoordinatesAndOwner();  //need to call decrRef()
        if (aCoord->getNumberOfComponents() != 3)        {
            MESSAGE("Coordinate array should contain x, y, z!");
            continue;
        }
        myOutFile.setf( std::ios::fixed, std:: ios::floatfield );
//        myOutFile.precision(5);  //set the precision
        myOutFile.precision(12);  //set the precision to be higher
        for (int j= 0; j< aCoord->getNumberOfTuples(); j++) {
            myOutFile << j+1 << ", "                //+1 because the node id starts with 1 in abaqus
                      <<*(aCoord->getPointer()+j*3) <<", " //X  be careful about the pointer
                     <<*(aCoord->getPointer()+j*3+1) <<", " //Y
                    <<*(aCoord->getPointer()+j*3+2) <<endl; //Z
        }
        myOutFile.unsetf ( std::ios::floatfield );

        //output element
        if (!aMesh->checkConsecutiveCellTypes()) { //check if the mesh is sorted by type
            DataArrayInt * aTmpArr = aMesh->sortCellsInMEDFileFrmt(); //sort the mesh element by type
            aTmpArr->decrRef();
        }
        DataArrayInt * aConn = aMesh->getNodalConnectivity(); //no need to decrRef()
        DataArrayInt * aConnIdx = aMesh->getNodalConnectivityIndex(); //no need to decrRef()
        int ElmType = 0; //for catch element type change
        for (int j =0; j<aConnIdx->getNumberOfTuples()-1; j++) { //-1: aConnIdx size = Nb of cells +1;
            int begin = *(aConnIdx->getPointer()+j);
            int end = *(aConnIdx->getPointer()+j+1);
            if (*(aConn->getPointer()+begin) != ElmType)   //if the element type is changed
                myOutFile << "*Element, type="<< ElementType.value(*(aConn->getPointer()+begin), "Unknown").toStdString() <<endl;
            ElmType = *(aConn->getPointer()+begin);
            myOutFile << j+1  ; //+1: element id start with 1;
            for (int k = begin+1; k < end; k++) {
                myOutFile << ", "<< *(aConn->getPointer()+k) +1 ;  //+1 : node id in abaqus start with 1
            }
            myOutFile <<endl;

        }

        //output the nset and elset
        myOutFile <<"*Nset, nset=Set-material_"<<aTmpName.toStdString() << "_"<<i+1<< ", generate" <<endl;
        myOutFile <<"   1,  " <<aCoord->getNumberOfTuples()<< ",    1" <<endl;
        myOutFile <<"*Elset, elset=Set-material_"<<aTmpName.toStdString() << "_"<<i+1<< ", generate" <<endl;
        myOutFile <<"   1,  " <<aMesh->getNumberOfCells()<< ",    1" <<endl;
        myOutFile <<"*Nset, nset=Set-statistic_"<<aTmpName.toStdString() << "_"<<i+1<< ", generate" <<endl;
        myOutFile <<"   1,  " <<aCoord->getNumberOfTuples()<< ",    1" <<endl;
        myOutFile <<"*Elset, elset=Set-statistic_"<<aTmpName.toStdString() << "_"<<i+1<< ", generate" <<endl;
        myOutFile <<"   1,  " <<aMesh->getNumberOfCells()<< ",    1" <<endl;

        //end of the part
        myOutFile << "*End Part" <<endl;
        outputedMeshes.push_back(i); //remember this mesh is outputed
        aCoord->decrRef();
    }

    //#######write assembly ########
    if (outputedMeshes.size() == 0) {
        MESSAGE("No mesh has been ouptuted!");
        return false;
    }
    myOutFile << "*Assembly, name=Assembly" <<endl;
    //write instances
    for (int i=0; i<outputedMeshes.size(); i++) {
        MEDCouplingUMesh * aMesh = const_cast <MEDCouplingUMesh * >( myMeshList[outputedMeshes[i]]);  //pick the outputed mesh
        QString aTmpName;
//        if (aMesh->getName().empty()) //if no name
        if (aMesh->getName().c_str() == 0) //if no name
     aTmpName = QString("Mesh-") + QString("%1").arg(i+1);
        else
//            aTmpName = aMesh->getName().c_str();
        aTmpName = aMesh->getName().c_str();
        QString bTmpName = aTmpName + "-1"; //instance name

        myOutFile <<"*Instance, name=Part-" <<bTmpName.toStdString() <<", part=Part-"<<aTmpName.toStdString()<<endl;
        //no need to do anything here
        myOutFile <<"*End Instance"<<endl;
    }
    myOutFile <<"*End Assembly" <<endl;

    //#######write material ########
    for (int i=0; i<outputedMeshes.size(); i++) {
        MEDCouplingUMesh * aMesh = const_cast <MEDCouplingUMesh * >( myMeshList[outputedMeshes[i]]); //pick the outputed mesh
        QString aTmpName;
//        if (aMesh->getName().empty()) //if no name
        if (aMesh->getName().c_str() == 0) //if no name
    aTmpName = QString("Mesh-") + QString("%1").arg(i+1);
        else
            aTmpName = aMesh->getName().c_str();
        //            aTmpName = aMesh->getName().c_str();
        myOutFile << "*Material, name=material_" << aTmpName.toStdString() <<"_"<<outputedMeshes[i]+1 <<endl;
    }

    return true;
}





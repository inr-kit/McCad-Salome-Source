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
#ifndef MCMESHTRAN_ABAQUSMESHREADER_HXX
#define MCMESHTRAN_ABAQUSMESHREADER_HXX

#include <fstream>
#include <QString>
#include <vector>
#include <QMap>
#include <MEDCouplingUMesh.hxx>
#include <MEDCouplingMemArray.hxx>

using namespace std;
using namespace ParaMEDMEM;



enum AbqLoadStatus {
    Load_ok ,
    Load_warning,
    Load_failed
};

enum AbqLineType {
    Line_keyword,
    Line_comment,
    Line_data,
    Line_blank,
    Line_eof
};

enum AbqKeyType {
    Key_heading,
    Key_part,
    Key_part_end,
    Key_assembly,
    Key_assembly_end,
    Key_instance,
    Key_instance_end,
    Key_node,
    Key_element,
    Key_unknow
};

class MCMESHTRAN_AbaqusMeshReader
{
public:
    MCMESHTRAN_AbaqusMeshReader();
    ~MCMESHTRAN_AbaqusMeshReader();
    AbqLoadStatus loadFile (const QString & AbqFileName , const bool &skipError = false);  //load the abaqus file
    vector <const MEDCouplingUMesh*> getMeshList () ;    //return the mesh list
private:
    bool    read_heading();  //currently for skip the heading
    bool    read_part();  //read a part and return as a mesh
    bool    read_instance();//read a instance and return as a mesh
    bool    read_assembly(); //read the assembly and return a list of mesh
//    bool    read_nodes (vector <double > & aCorVec, int &aInitNodeIdx);  //read node and return the node list
    bool    read_nodes (vector <double > & aCorVec, vector <int > & aIndexVec);  //read node and return the node list, update to read un-ordered nodes
//    bool    read_elements(vector <int> & Connectivity, vector <int> & NodalConnIndex, const int & aInitNodeIdx );  //read the element connectivities and return the list
    bool    read_elements(vector <int> & Connectivity, vector <int> & NodalConnIndex);  //read the element connectivities and return the list, update to read un-ordered nodes
    bool    skipUnknown();

    AbqLineType         getLineAndCheck();  // get a line to currentLine and check the line type
    AbqLineType         checkLineType();    //check the line type
    AbqKeyType          getKeyType ();  //get and judge the key word
    QString             chopKeyWord(const QString &aLine);  //chop the key word
    QMap <QString, QString > getParaValue(const QString & aLine); //get the parameters in this line, assume "key1=value2 key2=value2"

private:
    QString myAbqFileName;
    ifstream myAbaqusFile;        // abaqus file
    string currentLine;
    int lineNum;
    QMap <QString, AbqKeyType> KeyWord;
    QMap <QString, int> ElementType;
    vector <const MEDCouplingUMesh*> myParts; //for saving UMesh construct from parts
    vector <const MEDCouplingUMesh*> myMeshList;//for saving UMesh construct from instances, the merge field will append at the end
    bool isSkipError;

};

#endif // MCMESHTRAN_ABAQUSMESHREADER_HXX

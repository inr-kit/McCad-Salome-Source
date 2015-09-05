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
#ifndef MCMESHTRAN_ABAQUSMESHWRITER_HXX
#define MCMESHTRAN_ABAQUSMESHWRITER_HXX

#include <fstream>
#include <QString>
#include <vector>
#include <QMap>
#include <MEDCouplingUMesh.hxx>
#include <MEDCouplingMemArray.hxx>

using namespace std;
using namespace ParaMEDMEM;


class MCMESHTRAN_AbaqusMeshWriter
{
public:
    MCMESHTRAN_AbaqusMeshWriter();
    ~MCMESHTRAN_AbaqusMeshWriter();

    void    setMeshes(vector <const MEDCouplingUMesh *> & MeshList) ;
    bool    exportFile (const QString & OutFileName);


private:
    vector <const MEDCouplingUMesh *> myMeshList;
    QString myOutFileName;
    ofstream myOutFile;
    QMap <int, QString > ElementType;



};

#endif // MCMESHTRAN_ABAQUSMESHWRITER_HXX

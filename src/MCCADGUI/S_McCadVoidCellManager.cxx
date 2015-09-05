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
#include "S_McCadVoidCellManager.hxx"
#include "../MCCAD/McCadMcVoid/McCadGeomData.hxx"
//#include "../MCCAD/McCadMcVoid/McCadMcnpWriter.hxx"
//#include "../MCCAD/McCadMcVoid/McCadTripoliWriter.hxx"
#include "S_McCadMcnpWriter.hxx"
#include "S_McCadTripoliWriter.hxx"
#include "S_McCadGeomData.hxx"
#include "S_McCadGDMLWriter.hxx"

#include <assert.h>
#include <TCollection_AsciiString.hxx>
//#include <OSD_Path.hxx>
//#include <OSD_Protection.hxx>
#include <QStringList>

#include "../MCCAD/McCadTool/McCadMathTool.hxx"
#include "../MCCAD/McCadTool/McCadConvertConfig.hxx"
#include "../MCCAD/McCadTool/McCadGeneTool.hxx"
//SALOME
#include <SUIT_Session.h>
#include <SUIT_Study.h>
#include "MCCADGUI.h"
#include "utilities.h"

S_McCadVoidCellManager::S_McCadVoidCellManager()
{
    m_bHaveMaterial = Standard_False;

}


void S_McCadVoidCellManager::S_ReadGeomData(const int & studyID)
{
    //change SolidList to studyID, compared with original McCad
    McCadGeomData *pData = static_cast <McCadGeomData *>(new S_McCadGeomData(studyID,m_bGenerateVoid));
    m_pGeomData.reset(pData);
    m_pGeomData->SetManager(this);
}


void S_McCadVoidCellManager::S_Process()
{
    // Add by Lei 12/11/2013 Load the material XML file.
    //qiu skip because we load the material already inside S_McCadGeomData and S_McCad***Writer
//    TCollection_AsciiString material_file = "material.xml";
//    ReadMatData(material_file);

    //get the studyID and engine
    int studyID = SUIT_Session::session()->activeApplication()->activeStudy()->id();
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    if ( !studyID || CORBA::is_nil( engine ) )    {
        MESSAGE("Errors in study or in MCCAD engine!");
        return ;
    }
    if (engine->getMaterialList(studyID)->length() > 0)
        m_bHaveMaterial = Standard_True; //make sure if have materials

    McCadVoidGenerator *pVoidGen = new McCadVoidGenerator(this);
    pVoidGen->Process();

    m_pGeomData->SortSurface();
    m_pGeomData->UpdateFaceNum();

    IMcCadWriter * pWriter = NULL;
    if (m_Convertor == "TRIPOLI")
    {
        pWriter = new S_McCadTripoliWriter();
        ((S_McCadTripoliWriter *)pWriter)->SetVirtCellNum(10000);
    }
    else if (m_Convertor == "MCNP")
    {
        pWriter = new S_McCadMcnpWriter();
    }
    else if (m_Convertor == "GDML")
    {
        pWriter = new S_McCadGDMLWriter();
    }

    if (pWriter != NULL)
    {
        pWriter->SetMaterial(m_bHaveMaterial); //no need any more
        pWriter->SetVoid(m_bGenerateVoid);
        pWriter->SetManager(this);
        pWriter->SetFileName(m_OutFileName);
        pWriter->PrintFile();
    }

    cout<<endl;
    cout<<"o =============== Finish conversion =============== o"<<endl;
}

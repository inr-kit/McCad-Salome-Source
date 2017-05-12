#include "McCadVoidGenerator.hxx"

#include <assert.h>
#include <BRepBndLib.hxx>
#include <BRepPrimAPI_MakeBox.hxx>

#include "../McCadTool/McCadMathTool.hxx"

McCadVoidGenerator::McCadVoidGenerator()
{
}

McCadVoidGenerator::~McCadVoidGenerator()
{
}

McCadVoidGenerator::McCadVoidGenerator(const McCadVoidCellManager * pManager)
{
//qiu    m_pManager = pManager;
     m_pManager = const_cast<McCadVoidCellManager *> (pManager);
}



/** **********************************************************************
* @brief Generate the void boxes based on the dimension of material solids
*
* @param const vector<McCadExtFace*> & theExtFaceList
* @return void
*
* @date 31/8/2012
* @modify  10/12/2013
* @author  Lei Lu
**************************************************************************/
void McCadVoidGenerator::GenVoidCells()
{
    Bnd_Box bnd_box;
    McCadGeomData * pData = m_pManager->GetGeomData();

    for (unsigned int i = 0; i < pData->m_ConvexSolidList.size(); i++)
    {
        cout<<".";
        McCadConvexSolid  *pSolid = pData->m_ConvexSolidList[i];
        assert(pSolid);
        BRepBndLib::Add((TopoDS_Shape)(*pSolid),bnd_box); // Add each convex solid into bundary box
    }
    cout<<endl;

    bnd_box.SetGap(100.0); // Set the gap between the boundary box and material solids
    Standard_Real dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
    bnd_box.Get(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);

    // Interger the dimension parameters of boundary box.
    McCadMathTool::Integer(dXmin);
    McCadMathTool::Integer(dYmin);
    McCadMathTool::Integer(dZmin);
    McCadMathTool::Integer(dXmax);
    McCadMathTool::Integer(dYmax);
    McCadMathTool::Integer(dZmax);

    gp_Pnt bnd_max_pnt(dXmax,dYmax,dZmax);
    gp_Pnt bnd_min_pnt(dXmin,dYmin,dZmin);

    // Create the boundary box.
    McCadVoidCell *pVoid = new McCadVoidCell(BRepPrimAPI_MakeBox(bnd_min_pnt,bnd_max_pnt).Solid());
    pVoid->SetBntBox(dXmin,dYmin,dZmin,dXmax,dYmax,dZmax);
    pData->m_pOutVoid = pVoid;

    /* Split the boundar box into several parts */
    int iSplit = 2;
    for (int iX = 0 ; iX < iSplit ; iX++)
    {
        Standard_Real xMin = dXmin+(dXmax-dXmin)*iX/iSplit;
        Standard_Real xMax = dXmin+(dXmax-dXmin)*(iX+1)/iSplit;
        for (int iY = 0; iY < iSplit; iY++ )
        {
           Standard_Real yMin = dYmin+(dYmax-dYmin)*iY/iSplit;
           Standard_Real yMax = dYmin+(dYmax-dYmin)*(iY+1)/iSplit;
           for (int iZ = 0; iZ < iSplit; iZ++ )
           {
              Standard_Real zMin = dZmin +(dZmax-dZmin)*iZ/iSplit;
              Standard_Real zMax = dZmin +(dZmax-dZmin)*(iZ+1)/iSplit;

              McCadMathTool::Integer(xMin);
              McCadMathTool::Integer(yMin);
              McCadMathTool::Integer(zMin);
              McCadMathTool::Integer(xMax);
              McCadMathTool::Integer(yMax);
              McCadMathTool::Integer(zMax);

              gp_Pnt max_pnt(xMax,yMax,zMax);
              gp_Pnt min_pnt(xMin,yMin,zMin);

              pData->m_listVoidCell->Append(BRepPrimAPI_MakeBox(min_pnt,max_pnt).Shape());

              McCadVoidCell * pVoid = new McCadVoidCell(BRepPrimAPI_MakeBox(min_pnt,max_pnt).Solid());
              pVoid->SetBntBox(xMin,yMin,zMin,xMax,yMax,zMax);
              pData->m_VoidCellList.push_back(pVoid);

              cout<<".";
           }
        }
    }
    cout<<endl;
}


/** ********************************************************************
* @brief Calculate the collision between faces of solid and void cell
*
* @param const vector<McCadExtFace*> & theExtFaceList
* @return void
*
* @date 31/8/2012
* @modify  10/12/2013
* @author  Lei Lu
***********************************************************************/
void McCadVoidGenerator::VoidSolidCollision()
{
    McCadGeomData * pData = m_pManager->GetGeomData();

    try
    {
        for (unsigned int i = 0; i < pData->m_VoidCellList.size(); i++)
        {
            McCadVoidCell *pVoid = pData->m_VoidCellList.at(i);
            assert(pVoid);

            cout<<"Process No."<<i+1<<" void cell:";
            for (unsigned int j = 0; j < pData->m_ConvexSolidList.size(); j++)
            {
                McCadConvexSolid  *pSolid = pData->m_ConvexSolidList.at(j);
                assert(pSolid);

                Bnd_Box bbox_solid = pSolid->GetBntBox();
                Bnd_Box bbox_void = pVoid->GetBntBox();

                if(bbox_void.IsOut(bbox_solid))     // Detect the boundary boxes are collied or not
                {
                    continue;
                }
                else
                {
                    // Add the collided material solid's number into the void cell's list.
                    pVoid->AddColliedSolidNum(j);
                }
            }
            cout<<endl;
        }

    }
    catch(...)
    {
        cout << "#McCadVoidCellManage_Void & Face Collision Error" << endl;
    }
}




/** ********************************************************************
* @brief Calculate the collision between faces of solid and void cell
*
* @param const vector<McCadExtFace*> & theExtFaceList
* @return void
*
* @date 31/8/2012
* @modify  10/12/2013
* @author  Lei Lu
***********************************************************************/
void McCadVoidGenerator::VoidFaceCollision()
{
    McCadGeomData * pData = m_pManager->GetGeomData();

    try
    {
        for (unsigned int i = 0; i < pData->m_VoidCellList.size(); i++)
        {
            McCadVoidCell *pVoid = pData->m_VoidCellList.at(i);
            assert(pVoid);
            //cout<<"Void cell " << i+1 << " is collided with:";
            pVoid->CalColliedFaces(m_pManager->GetGeomData()); // Calculate the collision between the void cell and boundary surface.
        }
        cout<<endl<<endl;
    }
    catch(...)
    {
        cout << "#McCadVoidCellManage_Void & Face Collision Error" << endl;
    }
}


/** ********************************************************************
* @brief Split the void cell if the length of description is beyond the
*        limitation.
*
* @param const vector<McCadExtFace*> & theExtFaceList
* @return void
*
* @date 31/8/2012
* @modify  10/12/2013
* @author  Lei Lu
***********************************************************************/
void McCadVoidGenerator::SplitVoidCell()
{
    try
    {
        McCadGeomData * pData = m_pManager->GetGeomData();
        vector <McCadVoidCell *> void_list; // Create a temporary void cell list to store the void cells after splitting.
        for (int i = 0; i < pData->m_VoidCellList.size(); i++)
        {
            McCadVoidCell * pVoid = pData->m_VoidCellList.at(i);
            assert(pVoid);
            pVoid->SetSplitDepth(1);                            // Set the initial split depth.

            cout<<"Processing the "<<i+1<<" void cell:"<<endl;
            if( pVoid->SplitVoidCell(void_list, m_pManager->GetGeomData()))
            {
                pData->m_VoidCellList.erase(pData->m_VoidCellList.begin()+i); // Delete the original void cell
                i--;                                            // Move the point to the former position
            }
            cout<<endl;
        }

        for (unsigned int j = 0; j < void_list.size(); j++)
        {
            pData->m_VoidCellList.push_back(void_list.at(j));          // Add the new void cell generated into void cell list.
        }
        void_list.clear();
    }
    catch(...)
    {
        cout << "#McCadVoidCellManage_Void & Splitting Void Cell Error" << endl;
    }

}


/** ********************************************************************
* @brief Generate the surface list of void cells.
*
* @param
* @return void
*
* @date 31/8/2012
* @modify  10/12/2013
* @author  Lei Lu
***********************************************************************/
void McCadVoidGenerator::GenVoidSurfList()
{
    McCadGeomData * pData = m_pManager->GetGeomData();

    for (unsigned int i = 0; i < pData->m_VoidCellList.size(); i++)
    {
        McCadVoidCell * pVoid = pData->m_VoidCellList.at(i);
        assert(pVoid);
        vector <McCadExtBndFace *> surf_list = pVoid->GetGeomFaceList();
        pData->AddGeomSurfList(surf_list);     // Merge the sufaces with the surface list of material solids
    }

    vector <McCadExtBndFace *> out_surf_list = pData->m_pOutVoid->GetGeomFaceList();
    pData->AddGeomSurfList(out_surf_list);     // Add the surfaces of outer space into the surface list.
}




/** ********************************************************************
* @brief The main process control function
* @param null
* @return void
*
* @date 31/10/2012
* @modify  10/12/2013
* @author  Lei Lu
***********************************************************************/
void McCadVoidGenerator::Process()
{ 
    //qiu I don't know why this judgement is removed. Here I add it again
    if(m_pManager->GenVoid() == Standard_True)// If the switch of void generation is on
    {
    cout<<"=============== Step.3  Generate the void cells ==============="<<endl<<endl;
    GenVoidCells();                 // Step 1.  Generate the void cells

    cout<<"=============== Step.4  Detect the collisions =============== "<<endl<<endl;
    VoidSolidCollision();           // Step 2.  Detect the collision between material solids and void cells
    VoidFaceCollision();            // Step 3.  Detect the collision between void cells and each boundary faces

    cout<<"=============== Step.5  Split the void cell recursively"<<endl<<endl;
    SplitVoidCell();                // Step 4.  If the discription is long, split the void into small pieces

    cout<<"=============== Step.7  Add the surfaces of void cells into surface list ==============="<<endl<<endl;
    GenVoidSurfList();              // Step 5.  Generate the surface list, abstract the parameters
    }
}




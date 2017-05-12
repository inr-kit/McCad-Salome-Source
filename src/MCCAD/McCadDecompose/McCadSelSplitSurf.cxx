#include "McCadSelSplitSurf.hxx"
#include "McCadBndSurfCylinder.hxx"

McCadSelSplitSurf::McCadSelSplitSurf()
{
}

McCadSelSplitSurf::~McCadSelSplitSurf()
{    
}


/** ***************************************************************************
* @brief  Merge the splitting surfaces, and record the repeated times for
*         sorting the splitting surfaces
* @param
* @return Void
*
* @date 14/03/2016
* @modify
* @author  Lei Lu
******************************************************************************/
void McCadSelSplitSurf::MergeSplitSurfaces(vector<McCadSurface*> & face_list)
{
    for(int i = 0; i < face_list.size()-1; i++ )
    {
        Standard_Integer iRepeatNum = 0;
        McCadSurface *pFirFace = face_list.at(i);

        for(int j = i+1; j < face_list.size(); j++ )
        {
            McCadSurface *pSecFace = face_list.at(j);

            if(pFirFace->IsSameSurface(pSecFace))
            {
                iRepeatNum++;
                face_list.erase(face_list.begin()+j);

                //delete pSecFace;
                //pSecFace = NULL;

                j--;
                continue;
            }
        }

        pFirFace->SetRepeatSurfNum(iRepeatNum); // Set how many repeat surfaces in the surfacelist

        if (face_list.size() == 0)
        {
            break;
        }
    }
}




/** ***************************************************************************
* @brief  Select the splitting surfaces from the splitting surface list.
* @param
* @return Void
*
* @date 14/03/2016
* @modify
* @author  Lei Lu
******************************************************************************/
void McCadSelSplitSurf::GenSplitSurfList(vector<McCadSurface*> & face_list,
                                         vector<McCadSurface*> & selface_list)
{
    /** The 1st Step, select the splitting surface through 0 boundary surfaces **/
    for(int i = 0; i < face_list.size(); i++)
    {
        McCadSurface * pSurf = face_list.at(i);
        if (pSurf->GetSplitSurfNum() == 0)
        {
            selface_list.push_back(pSurf);
        }
    }

    /** The splitting surface does not collised with other boundary surfaces, it
     *  is unnecessary to sort and change the sequence */
    if(!selface_list.empty())
    {
        return;
    }

    /** The 2ed Step, select the splitting surfaces which are planes and through
     *  0 curved surfaces **/
    for(int i = 0; i < face_list.size(); i++)
    {
        McCadSurface * pSurf = face_list.at(i);
        if (pSurf->SplitCurvSurfNum() == 0 && pSurf->GetSurfType() == Plane)
        {
            selface_list.push_back(pSurf);
        }
    }

    if(!selface_list.empty())
    {
        if (selface_list.size() > 1)
        {
            SortSurfaces(selface_list);
        }
        return;
    }

    /** The 3rd Step, select the splitting surfaces which through only planes **/
    for(int i = 0; i < face_list.size(); i++)
    {
        McCadSurface * pSurf = face_list.at(i);
        if(pSurf->GetSurfType() == Cylinder)
        {
            McCadBndSurfCylinder *pCyln = (McCadBndSurfCylinder*)pSurf;
            if(!pCyln->HasAstSplitSurfaces())   // The cylinder has no edge for adding assisted splitting surface
            {
                selface_list.push_back(pSurf);
            }
        }
//        if(pSurf->GetSurfType() == Plane)
//        {
//            selface_list.push_back(pSurf);
//        }

    }
    if(!selface_list.empty())
    {
        if (selface_list.size() > 1)
        {
            SortSurfaces(selface_list);
        }
        return;
    }

    /** The 4th Step, select the other surfaces **/
    for(int i = 0; i < face_list.size(); i++)
    {
        McCadSurface * pSurf = face_list.at(i);
        if (pSurf->GetSurfType() == Plane)
        {
            selface_list.push_back(pSurf);
        }
    }

    SortSurfaces(selface_list);
    return;
}




/** ***************************************************************************
* @brief  Judge the solid has splitting surfaces which have the priorities,
*         e.g., the planes which through only planes. It is used before the
*         assisted surface adding function, if there are such surfaces,
*         assisted splitting surfaces is unneccessary to be added.
* @param
* @return Void
*
* @date 14/03/2016
* @modify
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadSelSplitSurf::HasPlnSplitOnlyPln(vector<McCadSurface*> & face_list)
{
    if (face_list.empty())
    {
         return Standard_False;
    }

    /** Select the splitting surfaces which are planes and through
        0 curved surfaces */
    for(unsigned int i = 0; i < face_list.size(); i++)
    {
        McCadSurface * pSurf = face_list.at(i);
        if (pSurf->SplitCurvSurfNum() == 0 && pSurf->GetSurfType() == Plane)
        {
            return Standard_True;
        }
    }

    return Standard_False;
}




/** ***************************************************************************
* @brief  Judge the solid has splitting surfaces which does not through any
*         boundary surfaces.
* @param
* @return Void
*
* @date 14/03/2016
* @modify
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadSelSplitSurf::HasSurfThroughNoBndSurf(vector<McCadSurface*> & face_list)
{
    if (face_list.empty())
    {
         return Standard_False;
    }

    /** Select the splitting surface through 0 boundary surfaces */
    for(unsigned int i = 0; i < face_list.size(); i++)
    {
        McCadSurface * pSurf = face_list.at(i);
        if ((pSurf->GetSplitSurfNum() == 0))
        {
            return Standard_True;
        }
    }

    return Standard_False;
}





/** ***************************************************************************
* @brief  Sort the surfaces by the number of surfaces have collision, number of
*         internal loops, repeated times of splitting surfaces and if the splitting
*         surfaces are assisted splitting surfaces or not.
* @param  Standard_Integer iSwitch:
*         If iSwitch equals to 0, switch off the number of collied surfaces
*         the splitting surface that throught 0 boundary surface do not need
*         to sort with collied surfaces and are not assisted splitting surfaces.
* @return Void
*
* @date 14/03/2016
* @modify
* @author  Lei Lu
******************************************************************************/
void McCadSelSplitSurf::SortSurfaces(vector<McCadSurface *> &face_list)
{
    /** The splitting surface through more concave edges has priority */
    for(unsigned int i = 0; i < face_list.size() - 1; i++)
    {
        for (unsigned int j = 0; j < face_list.size() - 1 - i; j++)
        {
            McCadSurface * pSurfA = face_list.at(j);
            McCadSurface * pSurfB = face_list.at(j+1);

            if (pSurfA->GetThroughConcaveEdges() < pSurfB->GetThroughConcaveEdges() ) // Compare the number of splitted sufaces
            {
                swap(face_list.at(j),face_list.at(j+1));
            }
        }
    }

    /** The splitting surface through less boundary surfaces has priority */
    for(unsigned int i = 0; i < face_list.size() - 1; i++)
    {
        for (unsigned int j = 0; j < face_list.size() - 1 - i; j++)
        {
            McCadSurface * pSurfA = face_list.at(j);
            McCadSurface * pSurfB = face_list.at(j+1);

            if (pSurfA->GetThroughConcaveEdges() > pSurfB->GetThroughConcaveEdges() ) // Compare the number of splitted sufaces
            {
                break;
            }

            if (pSurfA->GetSplitSurfNum() > pSurfB->GetSplitSurfNum()) // Compare the number of splitted sufaces
            {
                swap(face_list.at(j),face_list.at(j+1));
            }
        }
    }

//    /** The surface has internal loop, which will be used as splitting surface firstly */
//    for(unsigned int i = 0; i < face_list.size() - 1; i++)
//    {
//        for (unsigned int j = 0; j < face_list.size() - 1 - i; j++)
//        {
//            McCadSurface * pSurfA = face_list.at(j);
//            McCadSurface * pSurfB = face_list.at(j+1);

//            if (pSurfA->GetLoopsNum() < pSurfB->GetLoopsNum()) // Compare the number of internal loops
//            {
//                swap(face_list.at(j),face_list.at(j+1));
//            }
//        }
//    }


//    /** The surface repeated more in the solid has priority. */
//    for(unsigned int i = 0; i < face_list.size() - 1; i++)
//    {
//        for (unsigned int j = 0; j < face_list.size() - 1 - i; j++)
//        {
//            McCadSurface * pSurfA = face_list.at(j);
//            McCadSurface * pSurfB = face_list.at(j+1);

//            if (pSurfA->GetRepeatSurfNum() < pSurfB->GetRepeatSurfNum()) // Compare the repeated times
//            {
//                swap(face_list.at(j),face_list.at(j+1));
//            }
//        }
//    }


//    /** The surface is assisted surface, which will be used as splitting surface firstly */
//    if (iSwitch == 2)
//    {
//        for(unsigned int i = 0; i < face_list.size() - 1; i++)
//        {
//            for (unsigned int j = 0; j < face_list.size() - 1 - i; j++)
//            {
//                McCadSurface * pSurfA = face_list.at(j);
//                McCadSurface * pSurfB = face_list.at(j+1);

//                // The surface is assited splitting surface has priority
//                if (!pSurfA->IsAstSurf() && pSurfB->IsAstSurf())
//                {
//                    swap(face_list.at(j),face_list.at(j+1));
//                }
//            }
//        }
//    }
}

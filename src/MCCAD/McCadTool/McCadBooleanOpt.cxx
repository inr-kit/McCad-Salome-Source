#include "McCadBooleanOpt.hxx"

#include <BRepBndLib.hxx>
#include <BRepPrimAPI_MakeHalfSpace.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepTools.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepTools_ReShape.hxx>
//qiu #include <BOPTools_DSFiller.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepCheck_Analyzer.hxx>

#include <STEPControl_Writer.hxx>
#include <ShapeUpgrade_RemoveInternalWires.hxx>
#include <ShapeFix_Solid.hxx>
#include <ShapeFix_FixSmallFace.hxx>

#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_HSequenceOfShape.hxx>

#include <gp_Pln.hxx>
#include <gp_Cylinder.hxx>
#include <GeomAbs_SurfaceType.hxx>

#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>

#include "../McCadTool/McCadGeomTool.hxx"

McCadBooleanOpt::McCadBooleanOpt()
{
}

McCadBooleanOpt::McCadBooleanOpt(const TopoDS_Solid &theSolid, const TopoDS_Face &theFace)
{
    InputData(theSolid, theFace);
    m_ResultSolids = new TopTools_HSequenceOfShape;
}

McCadBooleanOpt::~McCadBooleanOpt()
{
    //m_ResultSolids->Clear();
    FreeSolidList(m_ResultSolids);
}

/** ********************************************************************
* @brief Input the solid and splittin surface
*
* @param
* @return void
*
* @date 04/08/2015
* @author  Lei Lu
***********************************************************************/
void McCadBooleanOpt::InputData(const TopoDS_Solid &theSolid, const TopoDS_Face &theFace)
{
    m_InputSolid = theSolid;
    m_SplitFace = theFace;
}




/** ********************************************************************
* @brief Calculate the boundary box of input solid
*
* @param
* @return void
*
* @date 04/08/2015
* @author  Lei Lu
***********************************************************************/
void McCadBooleanOpt::CalBndBox()
{
    Bnd_Box bndBox;
    BRepBndLib::Add(m_InputSolid, bndBox);

    bndBox.SetGap(10.0);
    Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    bndBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

    m_BndBox = BRepPrimAPI_MakeBox(gp_Pnt(aXmin, aYmin, aZmin), gp_Pnt(aXmax, aYmax, aZmax)).Shape();
    m_DglLength = sqrt(bndBox.SquareExtent());
}




/** ********************************************************************
* @brief Perform the Boolean operation
*
* @param
* @return Standard_Boolean
*
* @date 04/08/2015
* @author  Lei Lu
***********************************************************************/
Standard_Boolean McCadBooleanOpt::Perform()
{
     CalBndBox(); 
     TopoDS_Face split_face = CrtExtSplitFace();

     BRepAdaptor_Surface BS(split_face, Standard_True);
     GeomAdaptor_Surface AdpSurf = BS.Surface();

//     BRepBuilderAPI_Sewing builder(0.01,true,true,true,true);

//     TopExp_Explorer exf;
//     for(exf.Init(m_InputSolid,TopAbs_FACE);exf.More();exf.Next())
//     {
//         TopoDS_Face face = TopoDS::Face(exf.Current());
//         builder.Add(face);
//     }

//     builder.Perform();
//     TopoDS_Shape tmpShape = builder.SewedShape();
//     builder.Dump();

//     TopoDS_Shape shape;
//     TopExp_Explorer ex_shell;
//     for(ex_shell.Init(tmpShape,TopAbs_SHELL);ex_shell.More();ex_shell.Next())
//     {
//         try
//         {
//              TopoDS_Shell shell = TopoDS::Shell(ex_shell.Current());

//              ShapeFix_Solid solid;
//              solid.LimitTolerance(0.01);
//              shape = solid.SolidFromShell(shell);
//         }
//         catch(...){}
//      }

//     m_InputSolid = TopoDS::Solid(shape); 

     if( AdpSurf.GetType() == GeomAbs_Plane)
     {
         return Split(split_face);
     }
     else if(AdpSurf.GetType() == GeomAbs_Cylinder)
     {
         return SplitCyln(split_face);
     }
}


/** ********************************************************************
* @brief Split the solid with given cylinder surface, a cylinder will be
*        created for splitting
*
* @param TopoDS_Face & theFace
* @return Standard_Boolean
*
* @date 04/08/2015
* @author  Lei Lu
***********************************************************************/
Standard_Boolean McCadBooleanOpt::SplitCyln(TopoDS_Face & theFace)
{
    gp_Pnt posPnt, negPnt;              // Calculate the positive and negative points of the splitting face
    CalPoints(theFace,posPnt,negPnt);   // Calculate the negative point of cylinder

    Standard_Real UMin,UMax,VMin,VMax;
    BRepTools::UVBounds(theFace,UMin,UMax,VMin,VMax);

    UMin = 0;
    UMax = 2*M_PI;

    TopLoc_Location loc;
    const Handle(Geom_Surface)& aS1 = BRep_Tool::Surface(theFace,loc);
    TopoDS_Face face  = BRepBuilderAPI_MakeFace(aS1,UMin,UMax,VMin,VMax,1.e-7).Face();
    TopoDS_Solid NegCylnSld = BRepPrimAPI_MakeHalfSpace(face,negPnt).Solid();

    TopoDS_Shape PosBox;
    try
    {
        BRepAlgoAPI_Common PosCommon(NegCylnSld,m_BndBox);
        if(PosCommon.IsDone())
        {
            PosBox = PosCommon.Shape();
        }
        PosCommon.Destroy();       
    }
    catch(Standard_DomainError)
    {
        Standard_Failure::Caught()->Print(cout);
        cout<<endl;
    }

    try
    {
//qiu comment this to use    BRepAlgoAPI_Common without    BOPTools_DSFiller
//qiu        BOPTools_DSFiller aDF;
//qiu        aDF.SetShapes(m_InputSolid, NegCylnSld);
//qiu        aDF.Perform();

//qiu        BRepAlgoAPI_Common Common(PosBox,m_InputSolid,aDF);
        BRepAlgoAPI_Common Common(PosBox,m_InputSolid);

        if(Common.IsDone())
        {
            TopoDS_Shape ComSolid = Common.Shape();
            //TopoDS_Shape solid = RemoveInternalWires(ComSolid);
            m_ResultSolids->Append(ComSolid);

        }
        Common.Destroy();

//qiu        BRepAlgoAPI_Cut Cutter(m_InputSolid,PosBox, aDF);
        BRepAlgoAPI_Cut Cutter(m_InputSolid,PosBox);
        if(Cutter.IsDone())
        {
            TopoDS_Shape CutSolid = Cutter.Shape();
            m_ResultSolids->Append(CutSolid);
        }
        Cutter.Destroy();
    }
    catch(Standard_DomainError)
    {
        Standard_Failure::Caught()->Print(cout);
        cout<<endl;
    }

    return Standard_True;

}



/** ********************************************************************
* @brief Split the solid with given splitting surface
*
* @param TopoDS_Face & theFace
* @return Standard_Boolean
*
* @date 04/08/2015
* @author  Lei Lu
***********************************************************************/
Standard_Boolean McCadBooleanOpt::Split(TopoDS_Face & theFace)
{   
    gp_Pnt posPnt, negPnt;  // Calculate the positive and negative points of the splitting face

    CalPoints(theFace,posPnt,negPnt);

    TopoDS_Solid PosHalfSld = BRepPrimAPI_MakeHalfSpace(theFace,posPnt).Solid();
    TopoDS_Solid NegHalfSld = BRepPrimAPI_MakeHalfSpace(theFace,negPnt).Solid();

    //Handle_TopTools_HSequenceOfShape box_list = new TopTools_HSequenceOfShape;
    TopoDS_Shape PosBox;
    TopoDS_Shape NegBox;

    try
    {
        BRepAlgoAPI_Common PosCommon(PosHalfSld,m_BndBox);
        if(PosCommon.IsDone())
        {
            PosBox = PosCommon.Shape();
            //box_list->Append(PosBox);
        }
        PosCommon.Destroy();

        BRepAlgoAPI_Common NegCommon(NegHalfSld,m_BndBox);
        if(NegCommon.IsDone())
        {
            NegBox = NegCommon.Shape();
            //box_list->Append(NegBox);
        }
        NegCommon.Destroy();
    }
    catch(Standard_DomainError)
    {
        Standard_Failure::Caught()->Print(cout); cout << endl;
        return Standard_False;
    }

    if(SplitWithBoxes(PosBox,NegBox) && SplitWithBoxes(NegBox,PosBox))
    {
        PosBox.Free();
        NegBox.Free();
        return CheckAndRepair(); // Check the geometry errors of output solids
    }
    else
    {
        //FreeSolidList(m_ResultSolids);
        PosBox.Free();
        NegBox.Free();

        m_ResultSolids->Append(m_InputSolid);
        return Standard_False;
    }
}




/** ********************************************************************
* @brief Split the solid with given half box, if the common doesnot
*        work, use subtract.
*
* @param TopoDS_Shape & theBoxA, TopoDS_Shape & theBoxB
* @return Standard_Boolean
*
* @date 08/06/2016
* @author  Lei Lu
***********************************************************************/
Standard_Boolean McCadBooleanOpt::SplitWithBoxes(TopoDS_Shape &theBoxA,
                                                 TopoDS_Shape &theBoxB)
{
    try
    {
        BRepAlgoAPI_Common Common(theBoxA,m_InputSolid);
        if(Common.IsDone())
        {
            TopoDS_Shape ComSolid = Common.Shape();
            m_ResultSolids->Append(ComSolid);
        }
        else
        {
            try
            {
                BRepAlgoAPI_Cut Cutter(m_InputSolid,theBoxB);
                if(Cutter.IsDone())
                {
                    TopoDS_Shape CutSolid = Cutter.Shape();
                    m_ResultSolids->Append(CutSolid);
                }
                Cutter.Destroy();
            }
            catch(Standard_DomainError)
            {
                Standard_Failure::Caught()->Print(cout); cout << endl;
                return Standard_False;
            }
        }
        Common.Destroy();
    }
    catch(Standard_DomainError)
    {
        Standard_Failure::Caught()->Print(cout); cout << endl;
        return Standard_False;
    }

    return Standard_True;
}



    //    Handle(ShapeFix_Solid) genericFix = new ShapeFix_Solid;
    //    genericFix->Init(m_InputSolid);

    //    genericFix->SetPrecision(1.0e-7);
    //    genericFix->SetMaxTolerance(1.0e-3);

    //    genericFix->FixShellTool()->FixRemoveCurveMode() = 1;
    //    genericFix->FixShellTool()->FixConnected();

    //    genericFix->Perform();
    //    TopoDS_Solid theSolid =TopoDS::Solid(genericFix->Solid());


//    for(int i = 1; i <= box_list->Length(); i++)
//    {
//        TopoDS_Shape box = box_list->Value(i);
//        try
//        {
//            BRepAlgoAPI_Common Common(box,m_InputSolid);
//            if(Common.IsDone())
//            {
//                TopoDS_Shape ComSolid = Common.Shape();
//                wrt.Transfer(ComSolid, STEPControl_AsIs);
//                m_ResultSolids->Append(ComSolid);
//            }
//            else
//            {
//                try
//                {
//                    BRepAlgoAPI_Cut Cutter(m_InputSolid,box);
//                    if(Cutter.IsDone())
//                    {
//                        TopoDS_Shape CutSolid = Cutter.Shape();
//                        wrt.Transfer(CutSolid, STEPControl_AsIs);
//                        m_ResultSolids->Append(CutSolid);
//                    }
//                    Cutter.Destroy();
//                }
//                catch(Standard_DomainError)
//                {
//                    Standard_Failure::Caught()->Print(cout); cout << endl;
//                }
//            }
//            Common.Destroy();
//        }
//        catch(Standard_DomainError)
//        {
//            Standard_Failure::Caught()->Print(cout); cout << endl;
//            return Standard_False;
//        }


 /*   for(int i = 1; i <= box_list->Length(); i++)
    {
        TopoDS_Shape box = box_list->Value(i);
        try
        {
            BRepAlgoAPI_Cut Cutter(m_InputSolid,box);
            if(Cutter.IsDone())
            {
                TopoDS_Shape ComSolid = Cutter.Shape();
                TopoDS_Shape solid = RemoveInternalWires(ComSolid);

                m_ResultSolids->Append(solid);
                wrt.Transfer(solid, STEPControl_AsIs);
            }
            Cutter.Destroy();
        }
        catch(Standard_DomainError)
        {
            Standard_Failure::Caught()->Print(cout); cout << endl;
        }
    }*/






/** ********************************************************************
* @brief Check the geometry errors of output solids
*
* @param
* @return Standard_Boolean
*
* @date 01/06/2016
* @author  Lei Lu
***********************************************************************/
Standard_Boolean McCadBooleanOpt::CheckAndRepair()
{
    TopExp_Explorer exp;

    /** Remove the invalid solids */
    Handle_TopTools_HSequenceOfShape NewSolidList = new TopTools_HSequenceOfShape;    

    for(int i = 1; i <= m_ResultSolids->Length(); i++ )
    {
        TopoDS_Shape theShape = m_ResultSolids->Value(i);

        for(exp.Init(theShape,TopAbs_SOLID); exp.More(); exp.Next())
        {
            TopoDS_Solid tmpSolid = TopoDS::Solid(exp.Current());

            if(tmpSolid.IsNull())
            {
                cout<<"Error"<<endl;
                continue;
            }

            GProp_GProps GPsolid;
            BRepGProp::VolumeProperties(tmpSolid, GPsolid);

            if(GPsolid.Mass() <= 0.0001)
            {
                cout<<"#Solid with small volume--------------------"<<GPsolid.Mass()<<endl;
                continue;
            }
            NewSolidList->Append(tmpSolid);
        }
    }

    FreeSolidList(m_ResultSolids);

    for(int i = 1; i <= NewSolidList->Length(); i++ )
    {
        TopoDS_Solid theSolid = TopoDS::Solid(NewSolidList->Value(i));

        BRepCheck_Analyzer BrepAnalyzer(theSolid, Standard_True);

        if(!BrepAnalyzer.IsValid())
        {
            if (!RebuildSolidFromShell( theSolid ))   // Rebuild the geometry
            {
                FreeSolidList(m_ResultSolids);
                m_ResultSolids->Append(m_InputSolid);
                return Standard_False;
            }
        }
        else
        {
            m_ResultSolids->Append(theSolid);
        }
    }

    FreeSolidList(NewSolidList);
    return Standard_True;
}





/** ********************************************************************
* @brief Rebuild the geometry by sewing the faces of shell with errors
*
* @param
* @return Standard_Boolean
*
* @date 01/06/2016
* @author  Lei Lu
***********************************************************************/
Standard_Boolean McCadBooleanOpt::RebuildSolidFromShell(TopoDS_Solid & theSolid)
{
    cout<<"#-----------------------------------------------------------"<<endl;
    cout<<"#          -> A solid with geometry error is found          "<<endl;
    cout<<"#          -> Healing the error solid...                    "<<endl;

    // Check the geometry errors in the result solids
    Standard_Boolean bHasError = Standard_False;

    BRepBuilderAPI_Sewing builder(0.01,true,true,true,true);

    TopExp_Explorer exf;
    for(exf.Init(theSolid,TopAbs_FACE);exf.More();exf.Next())
    {
        TopoDS_Face face = TopoDS::Face(exf.Current());

        GProp_GProps GPface;
        BRepGProp::SurfaceProperties(face, GPface);

        if(GPface.Mass() <= 0.0001)
        {
            continue;
        }
        builder.Add(face);
    }

    builder.Perform();
    TopoDS_Shape tmpShape = builder.SewedShape();
    //builder.Dump();

    TopoDS_Shape shape;
    TopExp_Explorer ex_shell;

    for(ex_shell.Init(tmpShape,TopAbs_SHELL);ex_shell.More();ex_shell.Next())
    {
        try
        {
             TopoDS_Shell shell = TopoDS::Shell(ex_shell.Current());

             ShapeFix_Solid solid;
             solid.LimitTolerance(0.01);
             shape = solid.SolidFromShell(shell);
        }
        catch(...)
        {
             Standard_Failure::Caught()->Print(cout); cout << endl;
             bHasError = Standard_False;
             break;
        }
    }

    BRepCheck_Analyzer BA(shape, Standard_True);
    if(BA.IsValid())
    {
        cout<<"#          -> OK! The solid has been healed                 "<<endl;
        cout<<"#-----------------------------------------------------------"<<endl;
        bHasError = Standard_True;
        m_ResultSolids->Append(shape);
    }
    else
    {
        bHasError = Standard_False;
    }

    return bHasError;
}





/** ********************************************************************
* @brief Free the solid list
*
* @param
* @return void
*
* @date 07/06/2016
* @author  Lei Lu
***********************************************************************/
void McCadBooleanOpt::FreeSolidList(Handle_TopTools_HSequenceOfShape &theList)
{
    for(int i = 1; i <= theList->Length(); i++ )
    {
        TopoDS_Shape theShape = theList->Value(i);
        theShape.Free();
    }
    theList->Clear();
}




/** ********************************************************************
* @brief Remove the internal wire of surfaces and solid
*        Because after boolean operation, there are redundant surfaces
*        might be generated.
*
* @param TopoDS_Shape & theSolid
* @return TopoDS_Shape
*
* @date 04/08/2015
* @author  Lei Lu
***********************************************************************/
TopoDS_Shape McCadBooleanOpt::RemoveInternalWires(TopoDS_Shape & theSolid)
{
    ShapeUpgrade_RemoveInternalWires shapeUp;
    shapeUp.Init(theSolid);
    shapeUp.MinArea() = 1.0e5;
    shapeUp.Perform();

    TopoDS_Shape tmpShape = shapeUp.GetResult();
    return tmpShape;
}



//    BRepTools_ReShape reShape;
//    TopoDS_Shape tmpShape;
//    TopExp_Explorer exF;
//    int j =0;
//    for (exF.Init(theSolid,TopAbs_FACE); exF.More(); exF.Next())
//    {
//        j++;
//        TopoDS_Face face = TopoDS::Face(exF.Current());
//        if(j == 7)
//        {
//            reShape.Remove(face);
//            theSolid = reShape.Apply(theSolid);
//        }

//        TopExp_Explorer exW;
//        int i = 0;
//        for (exW.Init(face,TopAbs_WIRE); exW.More(); exW.Next())
//        {
//            i++;
//            TopoDS_Wire wire = TopoDS::Wire(exW.Current());
//            cout<<wire.Orientation()<<endl;
//            if( wire.Orientation() == TopAbs_REVERSED)
//            {
//                cout<<wire.Orientation()<<endl;
//                reShape.Remove(wire);
//                theSolid = reShape.Apply(theSolid);
//            }
//        }
//    }



/** ********************************************************************
* @brief  Get the result solid list after Boolean operation
*
* @param  Handle_TopTools_HSequenceOfShape & solid_list
* @return void
*         Handle_TopTools_HSequenceOfShape & solid_list
*
* @date 04/08/2015
* @author  Lei Lu
***********************************************************************/
void McCadBooleanOpt::GetResultSolids(Handle_TopTools_HSequenceOfShape & solid_list)
{
    TopExp_Explorer exp;

    for(int i = 1; i <= m_ResultSolids->Length(); i++ )
    {
        TopoDS_Shape theShape = m_ResultSolids->Value(i);

        for(exp.Init(theShape,TopAbs_SOLID); exp.More(); exp.Next())
        {
            TopoDS_Solid tmpSolid = TopoDS::Solid(exp.Current());
            solid_list->Append(tmpSolid);
        }
    }
}




/** ********************************************************************
* @brief  Create the extended splitting surface according to the size
*         of input solid
*
* @param
* @return TopoDS_Face
*
* @date 04/08/2015
* @author  Lei Lu
***********************************************************************/
TopoDS_Face McCadBooleanOpt::CrtExtSplitFace()
{    
    TopExp_Explorer exp;            // Trace the face of input solid

    exp.Init(m_SplitFace,TopAbs_WIRE);
    Standard_Real UMin,UMax, VMin, VMax;

    //TopoDS_Face face = BRepBuilderAPI_MakeFace(wire,Standard_True);
    BRepTools::UVBounds(m_SplitFace,UMin,UMax, VMin, VMax);   

    TopLoc_Location loc;
    const Handle(Geom_Surface)& aS1 = BRep_Tool::Surface(m_SplitFace,loc);

    // Update the U and V values for creating the splitting face ////
    Standard_Real UMid = UMin + (UMax - UMin)/2.0;
    Standard_Real VMid = VMin + (VMax - VMin)/2.0;

    UMax = UMid + m_DglLength;
    UMin = UMid - m_DglLength;
    VMax = VMid + m_DglLength;
    VMin = VMid - m_DglLength;

    // ////////////////////////////////////////////////////////////////
#ifdef OCC650
    TopoDS_Face face = BRepBuilderAPI_MakeFace(aS1, UMin,UMax,VMin,VMax);
#else
    TopoDS_Face face = BRepBuilderAPI_MakeFace(aS1, UMin,UMax,VMin,VMax, 1.e-7);
#endif

    return face;
}


/** ********************************************************************
* @brief  Calculate positive point and negative points
*
* @param  TopoDS_Face & theFace
*         gp_Pnt &posPnt
*         gp_Pnt &negPnt
* @return void
*
* @date 04/08/2015
* @author  Lei Lu
***********************************************************************/
void McCadBooleanOpt::CalPoints(TopoDS_Face & theFace, gp_Pnt &posPnt, gp_Pnt &negPnt)
{
    BRepAdaptor_Surface BS(theFace, Standard_True);
    GeomAdaptor_Surface AdpSurf = BS.Surface();

    if( AdpSurf.GetType() == GeomAbs_Plane)
    {
        gp_Pln plane  = AdpSurf.Plane();      // Get the geometry plane
        gp_Ax3 pos = plane.Position();
        gp_Dir dir = pos.Direction();
        gp_Pnt ctr = pos.Location();

        posPnt.SetX((ctr.X()+100*dir.X()));
        posPnt.SetY((ctr.Y()+100*dir.Y()));
        posPnt.SetZ((ctr.Z()+100*dir.Z()));

        negPnt.SetX((ctr.X()-100*dir.X()));
        negPnt.SetY((ctr.Y()-100*dir.Y()));
        negPnt.SetZ((ctr.Z()-100*dir.Z()));
    }
    else if ( AdpSurf.GetType() == GeomAbs_Cylinder)
    {
        gp_Cylinder cyln = AdpSurf.Cylinder();

        gp_Ax3 axis = cyln.Position();
        negPnt = axis.Location();
    }
}

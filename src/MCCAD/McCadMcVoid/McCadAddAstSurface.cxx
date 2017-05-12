#include "McCadAddAstSurface.hxx"

#include <TopLoc_Location.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cone.hxx>
#include <gp_Sphere.hxx>
#include <gp_Pln.hxx>
#include <gp_Torus.hxx>
#include <Geom_Plane.hxx>

#include <Geom_SurfaceOfRevolution.hxx>

#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

#include "../McCadTool/McCadMathTool.hxx"
#include "../McCadTool/McCadConvertConfig.hxx"
#include "../McCadTool/McCadEvaluator.hxx"

McCadAddAstSurface::McCadAddAstSurface()
{
}

McCadAddAstSurface::~McCadAddAstSurface()
{
}


/** ********************************************************************
* @brief Give a curved concave surface, and generate the assisted
*        surfaces, prevent the generation of ghost cell when write it
*        into MCNP file
*
* @param McCadExtBndFace *& pFace
*        Handle_TopTools_HSequenceOfShape &seqResultant
* @return Standard_Boolean
*
* @date 10/08/2016
* @author  Lei Lu
************************************************************************/
Standard_Boolean McCadAddAstSurface::AddAstSurf(const McCadExtBndFace *& pFace,
                                                Handle_TopTools_HSequenceOfShape &seqResultant)
{
    //qiu add const_cast, and replace all pFace->GetFaceType()
    GeomAbs_SurfaceType theFaceType = const_cast <McCadExtBndFace *&> (pFace)->GetFaceType();

    if( theFaceType == GeomAbs_Cylinder
        || theFaceType == GeomAbs_Cone
        || theFaceType == GeomAbs_Sphere)
    {
        return GenericAstFace(seqResultant,pFace);
    }
    else if( theFaceType == GeomAbs_Torus)
    {
        return GenAstFaceOfTorus(seqResultant,pFace);
    }
    else if( theFaceType == GeomAbs_SurfaceOfRevolution)
    {
        return GenAstFaceOfRevolution(seqResultant,pFace);
    }
    else
    {
        return Standard_False;
    }
}



/** ********************************************************************
* @brief Generic assisted surface adding function for adding the assisted
*        surfaces for cylinder, cone and sphere.
*
* @param McCadExtBndFace *& pFace
*        Handle_TopTools_HSequenceOfShape &seqResultant
* @return Standard_Boolean
*
* @date 10/08/2016
* @author  Lei Lu
************************************************************************/
Standard_Boolean McCadAddAstSurface::GenericAstFace(Handle_TopTools_HSequenceOfShape & seqResultant,
                                                    const McCadExtBndFace *&pFace )
{
    if (pFace->GetOrientation() == TopAbs_FORWARD) /// Convex surface does not need add assisted surface
    {
       return Standard_False;
    }

    TopLoc_Location loc;
    Handle_Geom_Surface HndGeomFace = BRep_Tool::Surface(*pFace,loc);
//qiu    GeomAdaptor_Surface adpFace = pFace->GetAdpFace();
    GeomAdaptor_Surface adpFace = const_cast<McCadExtBndFace *&>(pFace)->GetAdpFace();

    Standard_Real UMin,UMax, VMin, VMax;
    BRepTools::UVBounds(*pFace, UMin,UMax, VMin, VMax);

    McCadMathTool::ZeroValue(UMin,1.e-10);
    McCadMathTool::ZeroValue(UMax,1.e-10);
    McCadMathTool::ZeroValue(VMin,1.e-10);
    McCadMathTool::ZeroValue(VMin,1.e-10);

    BRepAdaptor_Surface BSF(*pFace,Standard_True);
    gp_Trsf T = BSF.Trsf(); /// the surfaces coordinate system

    if ( fabs(UMax - UMin)<= gp::Resolution() || fabs(VMax - VMin)<= gp::Resolution())
    {
        return Standard_False;
    }

    if( adpFace.IsUClosed() )
    {
       return Standard_False;
    }

    /// get extremal points in the surfaces coord sys.
    gp_Pnt P1 = (HndGeomFace->Value(UMin,VMin)).Transformed(T);
    gp_Pnt P2 = (HndGeomFace->Value(UMin,VMax)).Transformed(T);
    gp_Pnt P3 = (HndGeomFace->Value(UMax,VMin)).Transformed(T);
    gp_Pnt P4 = (HndGeomFace->Value(UMax,VMax)).Transformed(T);

    /// The middle point.
    gp_Pnt pnt_mid = (HndGeomFace->Value(UMin+(UMax-UMin)/2.0,VMin+(VMax-VMin)/2.0)).Transformed(T);
    gp_Pnt pnt_center;   // The point on the assisted surface

    /// handle closed faces, calculate the normal of assisted surface
    gp_Vec v1, v2, v3;
//qiu    if( pFace->GetFaceType() == GeomAbs_Cylinder)
    if( const_cast<McCadExtBndFace *&>(pFace)->GetFaceType() == GeomAbs_Cylinder)
    {
        v1 = gp_Vec::gp_Vec(P2,P1);
        v2 = gp_Vec::gp_Vec(P3,P1);
        v3 = v2 ^ v1;

        gp_Cylinder cyln = adpFace.Cylinder();
        gp_Ax3 axis = cyln.Position();
        pnt_center = axis.Location();    // point on the axis of cylinder
    }
//qiu    else if( pFace->GetFaceType() == GeomAbs_Cone )
    else if( const_cast<McCadExtBndFace *&>(pFace)->GetFaceType() == GeomAbs_Cone )
    {
        v1 = gp_Vec::gp_Vec(P2,P1);
        v2 = gp_Vec::gp_Vec(P3,P1);
        v3 = v2 ^ v1;

        gp_Cone cone = adpFace.Cone();
        gp_Ax3 axis = cone.Position();
        pnt_center = axis.Location();    // point on the peak point of cone
    }
//qiu    else if( pFace->GetFaceType() == GeomAbs_Sphere)
    else if( const_cast<McCadExtBndFace *&>(pFace)->GetFaceType() == GeomAbs_Sphere)
    {
        P3 = (HndGeomFace->Value(UMin,VMin+((VMax - VMin)/2.0)) ).Transformed(T);
        P4 = (HndGeomFace->Value(UMax,VMin+((VMax - VMin)/2.0)) ).Transformed(T);

        v1 = gp_Vec::gp_Vec(P2,P1);
        v2 = gp_Vec::gp_Vec(P3,P4);
        v3 = v2 ^ v1;

        gp_Sphere sphere = adpFace.Sphere();
        pnt_center = sphere.Location();    // point on the center point of sphere
    }

    v3.Normalize();
    gp_Dir plane_dir(v3);
    gp_Pnt pnt_ast;

    /* qiu (Abs(UMax - UMin) <= M_PI)? pnt_ast = pnt_center:*/ pnt_ast = P1;

    Handle_Geom_Plane plane = new Geom_Plane(pnt_ast,plane_dir);
    TopoDS_Face face = BRepBuilderAPI_MakeFace(plane->Pln());

    /// Calculate the center point is on the plus side or minus side
    Standard_Real orientVal = McCadEvaluator::Evaluate(plane->Pln(),pnt_mid);
    TopAbs_Orientation ori_plane;

    if(orientVal < 0.0 )
    {
        ori_plane = TopAbs_FORWARD;
    }
    else if (orientVal > 0.0 )
    {
        ori_plane = TopAbs_REVERSED;
    }

    face.Orientation(ori_plane);
    seqResultant->Append(face);

    return Standard_True;
}



/** ********************************************************************
* @brief Generate the assisted surface of a tori, normaly, if the tori
*        is concave, it needs add two assisted surfaces in two directions.
*        One is rotated direction, and one is cross section.
*        If the tori is convex, it need only one assisted surface at
*        the rotated direction.
*
* @param McCadExtBndFace *& pFace
*        Handle_TopTools_HSequenceOfShape &seqResultant
* @return Standard_Boolean
*
* @date 10/08/2016
* @author  Lei Lu
************************************************************************/
Standard_Boolean McCadAddAstSurface::GenAstFaceOfTorus(Handle_TopTools_HSequenceOfShape &seqResultant,
                                                       const McCadExtBndFace *&pFace)
{
    TopLoc_Location loc;
//qiu    GeomAdaptor_Surface adpFace = pFace->GetAdpFace();
    GeomAdaptor_Surface adpFace = const_cast<McCadExtBndFace *&>(pFace)->GetAdpFace();
    Handle_Geom_Surface HndGeomFace = BRep_Tool::Surface(*pFace,loc);

    Standard_Real UMin,UMax, VMin, VMax;
    BRepTools::UVBounds(*pFace,UMin,UMax,VMin,VMax);
    BRepAdaptor_Surface BSF(*pFace,Standard_True);
    gp_Trsf T = BSF.Trsf(); // the surfaces coordinate system

    if ( fabs(UMax - UMin)<= gp::Resolution() || fabs(VMax - VMin)<= gp::Resolution())
    {
        return Standard_False;
    }

    McCadMathTool::ZeroValue(UMin,1.e-10);
    McCadMathTool::ZeroValue(UMax,1.e-10);
    McCadMathTool::ZeroValue(VMin,1.e-10);
    McCadMathTool::ZeroValue(VMin,1.e-10);

    Standard_Real orientVal;

    TopAbs_Orientation orient = pFace->GetOrientation();

    // handle closed faces
    if(!adpFace.IsUClosed()) // closed cylinder doesn't need resultant
    {
        gp_Pnt PA = (HndGeomFace->Value(UMin, M_PI)).Transformed(T);
        gp_Pnt PB = (HndGeomFace->Value(UMax, M_PI)).Transformed(T);

        gp_Torus torus = adpFace.Torus();    // Get the geometry of torus
        gp_Ax3 Axis = torus.Position();                 // The coordinator of torus
        gp_Dir Dir = Axis.Direction();                  // The direction of torus axis

        gp_Vec vecA(Dir);
        gp_Vec vecB(PA, PB);

        gp_Vec vecPln =  vecA ^ vecB;
        vecPln.Normalize();
        gp_Dir dirPln(vecPln);

        //gp_Pnt PC(PA.X()+vecPln.X()*0.0001,PA.Y()+vecPln.Y()*0.0001,PA.Z()+vecPln.Z()*0.0001);
        Handle(Geom_Plane) plane = new Geom_Plane(PA,dirPln);

        TopoDS_Face face = BRepBuilderAPI_MakeFace(plane->Pln());

        gp_Pnt pnt_mid = (HndGeomFace->Value(UMin+(UMax-UMin)/2.0,VMin+(VMax-VMin)/2.0)).Transformed(T);
        orientVal = McCadEvaluator::Evaluate(plane->Pln(),pnt_mid);

        TopAbs_Orientation ori_new;

        if(orientVal < 0.0 )
        {
            ori_new = TopAbs_FORWARD;
        }
        else if (orientVal > 0.0 )
        {
            ori_new = TopAbs_REVERSED;
        }

        face.Orientation(ori_new); // this orientation will be used !!!
        seqResultant->Append(face);
    }

    if(orient == TopAbs_REVERSED)
    {
        gp_Pnt PA = (HndGeomFace->Value(0, VMin)).Transformed(T);
        gp_Pnt PB = (HndGeomFace->Value(0, VMax)).Transformed(T);

        gp_Vec vecA(PA,PB);

        gp_Torus torus = adpFace.Torus();    // Get the geometry of torus
        gp_Ax3 axis = torus.Position();      // The coordinator of torus
        gp_Pnt center = axis.Location();     // The center of torus
        gp_Dir dir = axis.Direction();       // The direction of torus axis
        gp_Vec vecB(dir);

        Standard_Real majRadius = torus.MajorRadius();

        Standard_Real semiAngle = vecA.Angle(vecB);
        gp_Pnt pnt_mid = (HndGeomFace->Value(UMin+(UMax-UMin)/2.0,VMin+(VMax-VMin)/2.0)).Transformed(T);

        TopoDS_Face face;

        if (Abs(semiAngle - M_PI/2.0) < 1.0e-3)
        {
            gp_Pln plane(center,dir);
            face = BRepBuilderAPI_MakeFace(plane);
            orientVal = McCadEvaluator::Evaluate(plane,pnt_mid);

        }
        else if (Abs(semiAngle - 0.0) < 1.0e-3
                 || Abs(semiAngle - M_PI) < 1.0e-3)
        {
            gp_Cylinder cyln(axis,majRadius);
            face = BRepBuilderAPI_MakeFace(cyln);
            orientVal = McCadEvaluator::Evaluate(cyln,pnt_mid);
        }
        else
        {
            if (semiAngle > M_PI/2.0)
            {
                semiAngle = M_PI - semiAngle;

                vecA.SetX(-1.0*vecA.X());
                vecA.SetY(-1.0*vecA.Y());
                vecA.SetZ(-1.0*vecA.Z());

                orientVal = -1;
            }
            else
            {
                orientVal = 1;
            }

            gp_Pnt pnt_cnt = (HndGeomFace->Value(0.0,0.0)).Transformed(T);
            gp_Vec vecC(center,pnt_cnt);

            Standard_Real angle = vecA.Angle(vecC);

            if(angle > M_PI/2.0)
            {
                gp_Dir new_dir(-1.0*dir.X(), -1.0*dir.Y(), -1.0*dir.Z());
                axis.SetDirection(new_dir);
            }

            gp_Cone cone;

            gp_Pnt newcenter(0,0,PB.Z());
            Standard_Real radiusnew;
            radiusnew = sqrt(PB.X()*PB.X()+PB.Y()*PB.Y());

            if(orientVal < 0.0)
            {
                radiusnew += 1;
            }
            else if (orientVal > 0.0 )
            {
                radiusnew -= 1;
            }

            cone.SetSemiAngle(semiAngle);
            cone.SetPosition(axis);
            cone.SetLocation(newcenter);
            cone.SetRadius(radiusnew);

            face = BRepBuilderAPI_MakeFace(cone);

            TopAbs_Orientation ori,ori_new;
            ori = face.Orientation();

            if(orientVal < 0.0 )
            {
                ori_new = TopAbs_FORWARD;
            }
            else if (orientVal > 0.0 )
            {
                ori_new = TopAbs_REVERSED;
            }

            face.Orientation(ori_new); // this orientation will be used !!!
            seqResultant->Append(face);
        }
    }

    if(seqResultant->IsEmpty())
    {
        return Standard_True;
    }
    else
    {
        return Standard_False;
    }
}




/** ********************************************************************
* @brief Generate the assisted surface of a Revolution, normally, they
*        are torus or ellipse torus, so it use the same algorithm to
*        generate the assisted surface.
*        If it is concave, it needs add two assisted surfaces in two
*        directions.One is rotated direction, and one is cross section.
*        If the tori is convex, it need only one assisted surface at
*        the rotated direction.
*
* @param McCadExtBndFace *& pFace
*        Handle_TopTools_HSequenceOfShape &seqResultant
* @return Standard_Boolean
*
* @date 10/08/2016
* @author  Lei Lu
************************************************************************/
Standard_Boolean McCadAddAstSurface::GenAstFaceOfRevolution( Handle_TopTools_HSequenceOfShape & seqResultant,
                                                             const McCadExtBndFace *& pFace)
{
    TopLoc_Location loc;
//qiu    GeomAdaptor_Surface adpFace = pFace->GetAdpFace();
    GeomAdaptor_Surface adpFace = const_cast<McCadExtBndFace *&>(pFace)->GetAdpFace();
    Handle_Geom_Surface HndGeomFace = BRep_Tool::Surface(*pFace,loc);

    Standard_Real UMin,UMax, VMin, VMax;
    BRepTools::UVBounds(*pFace,UMin,UMax, VMin, VMax);
    BRepAdaptor_Surface BSF(*pFace,Standard_True);
    gp_Trsf T = BSF.Trsf(); //the surfaces coordinate system

    Standard_Real orientVal;
    TopoDS_Face face;

    McCadMathTool::ZeroValue(UMin,1.e-10);
    McCadMathTool::ZeroValue(UMax,1.e-10);
    McCadMathTool::ZeroValue(VMin,1.e-10);
    McCadMathTool::ZeroValue(VMin,1.e-10);

    TopAbs_Orientation orient = pFace->GetOrientation();

    Handle_Geom_Surface surf = BRep_Tool::Surface(*pFace,loc);
    Handle_Geom_SurfaceOfRevolution rev = Handle(Geom_SurfaceOfRevolution)::DownCast(surf);

    // handle closed faces
    if(!adpFace.IsUClosed()) // closed cylinder doesn't need resultant
    {
        gp_Ax1 revAx = rev->Axis();
        gp_Ax3 Axis;
        Axis.SetAxis(revAx);

        gp_Dir Dir = Axis.Direction();                  // The direction of torus axis

        gp_Pnt PA = (HndGeomFace->Value(UMin, 0)).Transformed(T);
        gp_Pnt PB = (HndGeomFace->Value(UMax, 0)).Transformed(T);

        gp_Vec vecA(Dir);
        gp_Vec vecB(PA, PB);

        gp_Vec vecPln =  vecA ^ vecB;
        vecPln.Normalize();
        gp_Dir dirPln(vecPln);

        Handle(Geom_Plane) plane = new Geom_Plane(PA,dirPln);

        TopoDS_Face face = BRepBuilderAPI_MakeFace(plane->Pln());

        gp_Pnt pnt_mid = (HndGeomFace->Value(UMin+(UMax-UMin)/2.0,VMin+(VMax-VMin)/2.0)).Transformed(T);
        orientVal = McCadEvaluator::Evaluate(plane->Pln(),pnt_mid);
        return Standard_False;

        TopAbs_Orientation ori_new;

        if(orientVal < 0.0 )
        {
            ori_new = TopAbs_FORWARD;
        }
        else if (orientVal > 0.0 )
        {
            ori_new = TopAbs_REVERSED;
        }

        face.Orientation(ori_new); // this orientation will be used !!!
        seqResultant->Append(face);
    }

    if (orient == TopAbs_REVERSED)
    {
        gp_Pnt PA = (HndGeomFace->Value(0, VMin)).Transformed(T);
        gp_Pnt PB = (HndGeomFace->Value(0, VMax)).Transformed(T);

        gp_Ax1 revAx = rev->Axis();
        gp_Pnt center = revAx.Location();               // The center of revolution axis
        gp_Ax3 axis;
        axis.SetAxis(revAx);
        gp_Dir dir = axis.Direction();                  // The direction of torus axis

        gp_Vec vecA(PA,PB);
        gp_Vec vecB(dir);

        //McCadGeomRevolution *pGeomFaceA = new McCadGeomRevolution(adaptedFaceSurface);
        //Standard_Real radius = pGeomFaceA->GetRadius()*10;

        Standard_Real semiAngle = vecA.Angle(vecB);
        gp_Pnt pnt_mid = (HndGeomFace->Value(UMin+(UMax-UMin)/2.0,VMin+(VMax-VMin)/2.0)).Transformed(T);

        Standard_Real dis_tol = McCadConvertConfig::GetTolerence();
        Standard_Real angle_tol = 1.0e-3; //McCadConvertConfig::GetTolerence();

        gp_Ax3 Ax3_X(center, gp::DX());
        gp_Ax3 Ax3_Y(center, gp::DY());
        gp_Ax3 Ax3_Z(center, gp::DZ());

        gp_Vec vecC;
        gp_Pnt newCenter;
        Standard_Real newRadius;

        if(axis.IsCoplanar(Ax3_Z,dis_tol,angle_tol))
        {
            vecC.SetX(PA.X());
            vecC.SetY(PA.Y());
            vecC.SetZ(0);

            newCenter.SetX(0);
            newCenter.SetY(0);
            newCenter.SetZ(PB.Z());

            newRadius = sqrt(PB.X()*PB.X()+PB.Y()*PB.Y());
        }
        else if(axis.IsCoplanar(Ax3_X,dis_tol,angle_tol))
        {
            vecC.SetX(0);
            vecC.SetY(PA.Y());
            vecC.SetZ(PA.Z());

            newCenter.SetX(PB.X());
            newCenter.SetY(0);
            newCenter.SetZ(0);

            newRadius = sqrt(PB.Y()*PB.Y()+PB.Z()*PB.Z());
        }
        else if(axis.IsCoplanar(Ax3_Y,dis_tol,angle_tol))
        {
            vecC.SetX(PA.X());
            vecC.SetY(0);
            vecC.SetZ(PA.Z());

            newCenter.SetX(0);
            newCenter.SetY(PB.Y());
            newCenter.SetZ(0);

            newRadius = sqrt(PB.X()*PB.X()+PB.Z()*PB.Z());
        }

        if (Abs(semiAngle - M_PI/2.0) < McCadConvertConfig::GetTolerence())
        {
            gp_Pln plane(center,dir);
            face = BRepBuilderAPI_MakeFace(plane);
            orientVal = McCadEvaluator::Evaluate(plane,pnt_mid);
        }
        else if (Abs(semiAngle - 0.0) < McCadConvertConfig::GetTolerence()
                 || Abs(semiAngle - M_PI) < McCadConvertConfig::GetTolerence())
        {
            gp_Cylinder cyln(axis,newRadius);
            face = BRepBuilderAPI_MakeFace(cyln);
            orientVal = McCadEvaluator::Evaluate(cyln,pnt_mid);
        }
        else
        {
            if (semiAngle > M_PI/2.0)
            {
                semiAngle = M_PI - semiAngle;
                vecA.SetX(-1.0*vecA.X());
                vecA.SetY(-1.0*vecA.Y());
                vecA.SetZ(-1.0*vecA.Z());

                orientVal = -1;
            }
            else
            {
                orientVal = 1;
            }

            Standard_Real angle = vecA.Angle(vecC);
            if(angle > M_PI/2.0)
            {
                gp_Dir new_dir(-1.0*dir.X(), -1.0*dir.Y(), -2.0*dir.Z());
                axis.SetDirection(new_dir);
            }

            gp_Cone cone;

            /// Move the assisted cone a little bit far from the cornor of tori.
            if(orientVal == -1)
            {
                newRadius += 1;
            }
            else
            {
                newRadius -= 1;
            }

            cone.SetSemiAngle(semiAngle);
            cone.SetPosition(axis);
            cone.SetLocation(newCenter);
            cone.SetRadius(newRadius);

            TopoDS_Face face = BRepBuilderAPI_MakeFace(cone);

            TopAbs_Orientation ori_new;
            if(orientVal < 0.0 )
            {
                ori_new = TopAbs_FORWARD;
            }
            else if (orientVal > 0.0 )
            {
                ori_new = TopAbs_REVERSED;
            }

            face.Orientation(ori_new); // this orientation will be used !!!
            seqResultant->Append(face);
        }
    }

    if(seqResultant->IsEmpty())
    {
        return Standard_True;
    }
    else
    {
        return Standard_False;
    }
}


#include <McCadDiscDs_DiscSolid.ixx>

#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <BRepBndLib.hxx>

//#include <PColStd_HSequenceOfReal.hxx>

McCadDiscDs_DiscSolid::McCadDiscDs_DiscSolid()
{
    Init();
}

McCadDiscDs_DiscSolid::McCadDiscDs_DiscSolid(const TopoDS_Shape& theSolid)
{
    Init();
    SetSolid(theSolid);
}


void McCadDiscDs_DiscSolid::Init()
{
    myComment="c     McCad: ";
    myName="Nobody";
    myVoxelSeq = new TColgp_HSequenceOfPnt();
    myOwnVFaces = new McCadDiscDs_HSequenceOfDiscFace();
    myOtherVSolidsIndexList = new TColStd_HSequenceOfInteger();
    myIndex = 0;
    myIsDiscret = false;
    myIsVoid = Standard_True;
    myIsOuterVoid = Standard_False;
    myHaveMaterial = Standard_False;
    myHaveDensity = Standard_False;
    myMaterialNumber = 0;
    myDensity = 0;
}


Standard_Boolean McCadDiscDs_DiscSolid::HasDensity() const
{
    return myHaveDensity;
}


Standard_Boolean McCadDiscDs_DiscSolid::HasMaterial() const
{
    return myHaveMaterial;
}


void McCadDiscDs_DiscSolid::UnsetMaterial()
{
    myMaterialNumber = 0;
    myHaveMaterial = Standard_False;
}


void McCadDiscDs_DiscSolid::UnsetDensity()
{
    myDensity = 0;
    myHaveDensity = Standard_False;
}


Standard_Real McCadDiscDs_DiscSolid::GetDensity() const
{
    return myDensity;
}

Standard_Integer McCadDiscDs_DiscSolid::GetMaterialNumber() const
{
    return myMaterialNumber;
}


void McCadDiscDs_DiscSolid::SetMaterialNumber(const Standard_Integer theMaterialNumber)
{
    myMaterialNumber = theMaterialNumber;
}

void McCadDiscDs_DiscSolid::SetDensity(const Standard_Real theDensity)
{
    myDensity = theDensity;
}


void McCadDiscDs_DiscSolid::SetIsDiscret(const Standard_Boolean theState)
{
    myIsDiscret = theState;
}

Standard_Boolean McCadDiscDs_DiscSolid::IsDiscret() const
{
    return myIsDiscret;
}

Standard_Boolean McCadDiscDs_DiscSolid::IsVoid() const
{
    return myIsVoid;
}

void McCadDiscDs_DiscSolid::SetIsVoid(const Standard_Boolean theVoidStatus)
{
    myIsVoid = theVoidStatus;
}

Standard_Boolean McCadDiscDs_DiscSolid::IsOuterVoid() const
{
    return myIsOuterVoid;
}

void McCadDiscDs_DiscSolid::SetIsOuterVoid(const Standard_Boolean theStatus)
{
    myIsOuterVoid = theStatus;
}


void McCadDiscDs_DiscSolid::MakeOwnVFaces()
{
//	Handle(PColStd_HSequenceOfReal) areaSeq = new PColStd_HSequenceOfReal;
//	Handle(McCadDiscDs_HSequenceOfDiscFace) sortedDiscFaces = new McCadDiscDs_HSequenceOfDiscFace;

    for (TopExp_Explorer exF(myShape, TopAbs_FACE); exF.More(); exF.Next()) // sort descending by surface area
    {
        TopoDS_Face aFace = TopoDS::Face(exF.Current());
        /*GProp_GProps GP1;
        BRepGProp::SurfaceProperties(aFace,GP1);
        Standard_Real area = GP1.Mass();*/
        McCadDiscDs_DiscFace theDFace(aFace);
        AppendOwnVFace(theDFace);

        /*if(areaSeq->Length() < 1) // first element
        {
            areaSeq->Append(area);
            sortedDiscFaces->Append(theDFace);
        }
        else
        {
            Standard_Boolean appended(Standard_False);
            for(Standard_Integer j=1; j <= areaSeq->Length(); j++)
            {
                if(area > areaSeq->Value(j))
                {
                    areaSeq->InsertBefore(j, area);
                    sortedDiscFaces->InsertBefore(j, theDFace);
                    appended = Standard_True;
                    break;
                }
            }
        }*/
    }

    /*for(Standard_Integer i=1; i<=sortedDiscFaces->Length(); i++)
        AppendOwnVFace(sortedDiscFaces->Value(i));*/
}


void McCadDiscDs_DiscSolid::SetIndex(const Standard_Integer theIndex)
{
    myIndex = theIndex;
}


Standard_Integer McCadDiscDs_DiscSolid::GetIndex() const
{
    return myIndex;
}


void McCadDiscDs_DiscSolid::SetSolid(const TopoDS_Shape& theShape)
{
    myShape = theShape;
    myOwnVFaces->Clear();
    MakeOwnVFaces();
}


TopoDS_Shape McCadDiscDs_DiscSolid::GetSolid() const
{
    return myShape;
}


void McCadDiscDs_DiscSolid::SetVoxel(const Handle(TColgp_HSequenceOfPnt)& theVoxelSeq)
{
    myVoxelSeq->Append(theVoxelSeq);
}


Handle(TColgp_HSequenceOfPnt) McCadDiscDs_DiscSolid::GetVoxel() const
{
    if(myVoxelSeq->Length() > 0)
        return myVoxelSeq; // is full

    for (Standard_Integer i=1; i <= myOwnVFaces->Length();i++)
        myVoxelSeq->Append((myOwnVFaces->Value(i)).GetPoints());

    return myVoxelSeq;
}


void McCadDiscDs_DiscSolid::SetName(const TCollection_AsciiString& aName)
{
    myName = aName;
    myComment = myComment + aName;
}


void McCadDiscDs_DiscSolid::SetName(const TCollection_AsciiString& aName, const Standard_Integer i)
{
    TCollection_AsciiString thePath = aName;
    myFileName = aName;

    TCollection_AsciiString tmpName;
    if(thePath.SearchFromEnd("/")>0)
        tmpName = thePath.Split(thePath.SearchFromEnd("/"));

    tmpName.Remove(tmpName.SearchFromEnd("."), tmpName.Length() - tmpName.SearchFromEnd(".")+1); //cut extension

    if(tmpName.Search("ExOut") > 0)
    {
        TCollection_AsciiString theName = tmpName;
        theName.Remove(1, tmpName.Search("ExOut") + 4);
        tmpName = theName;
    }
    if(tmpName.Search("converted") > 0)
    {
        TCollection_AsciiString theName = tmpName;
        theName.Remove(1,tmpName.Search("converted") + 8);
        tmpName = theName;
    }
    myName = tmpName;

    myComment += thePath + TCollection_AsciiString(" :    ") + myName;
}


TCollection_AsciiString McCadDiscDs_DiscSolid::GetComment() const
{
    return myComment;
}


TCollection_AsciiString McCadDiscDs_DiscSolid::GetFileName() const
{
    if( myFileName.IsEmpty() ) {
        if( myName.IsEmpty() )
            return "unnamed";
        else
            return myName;
    }

    return myFileName;
}


TCollection_AsciiString McCadDiscDs_DiscSolid::GetName() const
{
    return myName;
}


void McCadDiscDs_DiscSolid::SetBBox(const Bnd_Box& theBBox)
{
    myBBox = theBBox;
}

Bnd_Box McCadDiscDs_DiscSolid::GetBBox()
{
    if(myBBox.IsVoid())
    {
        // calculate Bounding Box
        Bnd_Box aBB;
        BRepBndLib::Add(myShape, aBB);
        aBB.SetGap(0);

                myBBox.Add(aBB);
        }

    return myBBox;
}


void McCadDiscDs_DiscSolid::SetOwnVFaces(const Handle(McCadDiscDs_HSequenceOfDiscFace)& vFaces)
{
    myOwnVFaces->Clear();
    myOwnVFaces->Append(vFaces);
}


void McCadDiscDs_DiscSolid::AppendOwnVFace(const McCadDiscDs_DiscFace& vFace)
{
    myOwnVFaces->Append(vFace);
}


Handle(McCadDiscDs_HSequenceOfDiscFace) McCadDiscDs_DiscSolid::GetOwnVFaces() const
{
    return myOwnVFaces;
}


Standard_Integer McCadDiscDs_DiscSolid::NbOfOwnVFace() const
{
    return myOwnVFaces->Length();
}


void McCadDiscDs_DiscSolid::SetOtherVSolids(const Handle(TColStd_HSequenceOfInteger)& vSolidsIndexList)
{
    myOtherVSolidsIndexList->Clear();
    myOtherVSolidsIndexList->Append(vSolidsIndexList);
}


void McCadDiscDs_DiscSolid::AppendOtherVSolid(Standard_Integer& theIndex)
{
    myOtherVSolidsIndexList->Append(theIndex);
}


Handle(TColStd_HSequenceOfInteger) McCadDiscDs_DiscSolid::GetOtherVSolids() const
{
    return myOtherVSolidsIndexList;
}


Standard_Integer McCadDiscDs_DiscSolid::NbOfOtherVSolids() const
{
    return myOtherVSolidsIndexList->Length();
}


Standard_Integer McCadDiscDs_DiscSolid::NbOfPoints() const
{
    return myVoxelSeq->Length();
}


Standard_Real McCadDiscDs_DiscSolid::Volume() const
{
    GProp_GProps GP;
    BRepGProp::VolumeProperties(myShape, GP);
    return GP.Mass();
}


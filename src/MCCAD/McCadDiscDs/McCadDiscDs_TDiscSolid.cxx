/*
 * McCadDiscDs_TDiscSolid.cxx
 *
 * This is a persistence class for McCadDiscDs_DiscSolid, to allow to save it in an xml file
 * after the conversion, before the file is exported to MCNP or Tripoli syntax, this is the raw
 * file format. From this raw file format it nearly costs no time to switch from Tripoli to MCNP
 * and vice versa.
 */

#include <McCadDiscDs_TDiscSolid.hxx>


const Standard_GUID& McCadDiscDs_TDiscSolid::GetID ()
{
	  static Standard_GUID TDataStd_GeometryID ("17a7c949-e0fb-49e3-98eb-6b5346ff2784");
	  return TDataStd_GeometryID;
}

const Standard_GUID& McCadDiscDs_TDiscSolid::ID() const
{
	return GetID();
}


void McCadDiscDs_TDiscSolid::Paste (const Handle(TDF_Attribute)& theInto,
                              const Handle(TDF_RelocationTable)& RT ) const
{
  Handle(McCadDiscDs_TDiscSolid) tDS = Handle(McCadDiscDs_TDiscSolid)::DownCast(theInto);
  tDS->Set(myDiscSolid);
}


Handle(TDF_Attribute) McCadDiscDs_TDiscSolid::NewEmpty () const
{
  return new McCadDiscDs_TDiscSolid();
}


Standard_OStream& McCadDiscDs_TDiscSolid::Dump(Standard_OStream& theOS) const
{
  Standard_OStream& anOS = TDF_Attribute::Dump( theOS );
  anOS << myString;
  return anOS;
}

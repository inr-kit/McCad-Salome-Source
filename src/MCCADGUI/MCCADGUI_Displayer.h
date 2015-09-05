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

#ifndef MCCADGUI_DISPLAYER_HEADER
#define MCCADGUI_DISPLAYER_HEADER

#include <LightApp_Displayer.h>
#include <LightApp_Application.h>
#include "MCCADGUI_DataModel.h"
#include <GEOM_Displayer.h>
//#include <SALOME_Actor.h>
#include <SALOME_InteractiveObject.hxx>

#include <qstringlist.h>
#include <qcolor.h>

#include "MCCAD_AISShape.hxx"

class SALOME_View;
class SALOME_Prs;
class SALOME_OCCPrs;
class SALOME_OCCViewType;
class SALOME_ListIO;


class MCCADGUI_Displayer : public LightApp_Displayer
{
public:
  MCCADGUI_Displayer(/*SalomeApp_Study* st*/);
  virtual ~MCCADGUI_Displayer();

  virtual bool canBeDisplayed( const QString& /*entry*/, const QString& /*viewer_type*/ ) const;
//  void updateActor( SALOME_Actor* );
//  GEOM_Displayer *  getGEOMDispalyer();

  /* Display/Erase object methods */
  void          Display   ( const QString& aEntry,
                            const bool updateViewer = true,
                            SALOME_View* theViewFrame = 0 );
  void          Erase     ( const QString& aEntry,
                            const bool forced = false,
                            const bool updateViewer = true,
                            SALOME_View* theViewFrame = 0 );
  void          Redisplay ( const QString& aEntry,
                            const bool updateViewer = true );



  /* Reimplemented from SALOME_Displayer */
  virtual void  Update( SALOME_OCCPrs* );
  SalomeApp_Study* getStudy() const;



  /* Set color for shape displaying. If it is equal -1 then default color is used.
     Available values are from Quantity_NameOfColor enumeration */
  void          SetColor  ( const int );
  void          UnsetColor();
  int           GetColor  () const;
  bool          HasColor  () const;

  /* Set width for shape displaying. If it is equal -1 then default width is used. */
  void          SetWidth  ( const double );
  void          UnsetWidth();
  double        GetWidth  () const;
  bool          HasWidth  () const;

  /* Set width for iso-lines displaying. If it is equal -1 then default width is used. */
  void          SetIsosWidth  ( const int );
  int           GetIsosWidth  () const;
  bool          HasIsosWidth  () const;

  /* Set display mode shape displaying. If it is equal -1 then display mode is used. */
  int           SetDisplayMode( const int );
  int           GetDisplayMode() const;
  int           UnsetDisplayMode();
  bool          HasDisplayMode() const;

//  /* Set Transparency for shape displaying. If it is equal -1 then default Transparency is used. */
//  void          SetTransparency  ( const double );
//  void          UnsetTransparency();
//  double        GetTransparency  () const;
//  bool          HasTransparency  () const;

  void setTransparency(const QStringList& entries, const double transparency );
  double getTransparency( const QString& entry );


  /* Sets name - for temporary objects only */
  void          SetName( const char* theName );
  void          UnsetName();

  /* This methos is used for activisation/deactivisation of objects to be displayed*/
  void          SetToActivate( const bool );
  bool          ToActivate() const;

  virtual void  BeforeDisplay( SALOME_View*, const SALOME_OCCPrs* );
  virtual void  AfterDisplay ( SALOME_View*, const SALOME_OCCPrs* );

  bool          isVisible (const QString & aEntry);


protected:

  virtual SALOME_Prs* buildPresentation(const QString& entry, SALOME_View* theViewFrame = 0 );

  /* Resets internal data */
  void        internalReset();

  void        clearTemporary( LightApp_SelectionMgr* theSelMgr );

  /* Sets interactive object */
  void        setIO( const Handle(SALOME_InteractiveObject)& theIO );
  /* Sets shape */
  void        setShape( const TopoDS_Shape& theShape );
  void           updateShapeProperties( const Handle(MCCAD_AISShape)&, bool );
  PropMap getObjectProperties( SalomeApp_Study*, const QString&, SALOME_View* = 0 );
  PropMap getDefaultPropertyMap();
  QColor         colorFromResources( const QString&, const QColor& );
//  QString   propertyName( Property );

protected:

  Handle(SALOME_InteractiveObject) myIO;
  TopoDS_Shape                     myShape;
  std::string                      myName;
//  std::string                      myTexture;
//  int                              myType;
  SALOME_View*                     myViewFrame;

  // Attributes
  Quantity_Color                   myShadingColor;
  int                              myColor;
  double                           myWidth;
  int                              myIsosWidth;
  bool                             myToActivate;
  int                              myDisplayMode;
  bool                             myHasDisplayMode;
  double                           myTransparency;
//  Aspect_TypeOfMarker              myTypeOfMarker;
//  double                           myScaleOfMarker;

private:
//    SalomeApp_Application* myApp;

//  GEOM_Displayer *          myGEOM_Displayer;
  LightApp_Application *    app() const;
  MCCADGUI_DataModel * datamodel() const;

};

#endif

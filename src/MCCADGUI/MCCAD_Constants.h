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


#ifndef MCCAD_CONSTANTS_H
#define MCCAD_CONSTANTS_H

#include <QString>

//#include "GEOM_OBJECT_defs.hxx"

namespace MCCAD
{

enum Property {
    Visibility,
    Transparency,
    DisplayMode,
    NbIsos,
    Color,
    EdgesDirection,
    Deflection,
    //    PointMarker,
    //    Material,
    LineWidth,
    IsosWidth,
    TopLevel,
    LastStudyProperty = TopLevel,
    Opacity,
    ShadingColor,
    WireframeColor,
    LineColor,
    FreeBndColor,
    PointColor,
    IsosColor,
    OutlineColor,
    LastProperty = OutlineColor,
};

double minDeflection();

QString sectionSeparator();
QString subSectionSeparator();

QString propertyName( Property );

} //namespace
  
#endif //

//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __FIGUREAPI_LINEGEOMETRY_H_
#define __FIGUREAPI_LINEGEOMETRY_H_

#include <omnetpp.h>
#include "TestBase.h"

using namespace omnetpp;

class LineGeometry : public TestBase
{
    cLineFigure *lines[2];
    cArcFigure *arcs[4];
    cPolylineFigure *polylines[4];

  protected:
    virtual std::string init() override;
    virtual std::string onTick(int tick) override;
};

#endif

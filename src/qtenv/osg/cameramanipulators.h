//==========================================================================
//  CAMERAMANIPULATORS.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_QTENV_OSG_CAMERAMANIPULATORS_H
#define __OMNETPP_QTENV_OSG_CAMERAMANIPULATORS_H

#include "qtenv/qtenvdefs.h"

#include <osgGA/OrbitManipulator>

namespace omnetpp {
namespace qtenv {

// derived from TerrainManipulator, but will not let the eye
// move below the center, the center can be moved with
// the right mouse button, and this will never roll (up is always +z)
class OverviewManipulator : public osgGA::OrbitManipulator
{
    typedef OrbitManipulator inherited;

public:
    OverviewManipulator(int flags = DEFAULT_SETTINGS);
    OverviewManipulator(const OverviewManipulator& tm,
                        const osg::CopyOp& copyOp = osg::CopyOp::SHALLOW_COPY);

    virtual void setByMatrix(const osg::Matrixd& matrix);

    virtual void setTransformation(const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up);

protected:
    virtual bool performMovementLeftMouseButton(double eventTimeDelta, double dx, double dy);
    virtual bool performMovementMiddleMouseButton(double eventTimeDelta, double dx, double dy);
    virtual bool performMovementRightMouseButton(double eventTimeDelta, double dx, double dy);

    osg::Vec3d _previousUp;
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_OSG_CAMERAMANIPULATORS_H

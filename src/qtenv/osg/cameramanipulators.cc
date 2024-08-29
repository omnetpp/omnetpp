//==========================================================================
//  CAMERAMANIPULATORS.CC - part of
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

#include "cameramanipulators.h"

namespace omnetpp {
namespace qtenv {

using namespace osg;
using namespace osgGA;

OverviewManipulator::OverviewManipulator(int flags)
    : inherited(flags)
{
}

OverviewManipulator::OverviewManipulator(const OverviewManipulator& tm, const osg::CopyOp& copyOp)
    : osg::Object(tm, copyOp),
    inherited(tm, copyOp),
    _previousUp(tm._previousUp)
{
    setVerticalAxisFixed(true);
}

// taken from TerrainManipulator, just ripped off the non-fixed-vertical-axis bits
void OverviewManipulator::setByMatrix(const Matrixd& matrix)
{
    Vec3d lookVector(-matrix(2, 0), -matrix(2, 1), -matrix(2, 2));
    Vec3d eye(matrix(3, 0), matrix(3, 1), matrix(3, 2));

    if (!_node) {
        _center = eye + lookVector;
        _distance = lookVector.length();
        _rotation = matrix.getRotate();
        return;
    }

    _previousUp = getUpVector(getCoordinateFrame(_center));
}

// taken from TerrainManipulator, just ripped off the non-fixed-vertical-axis bits
void OverviewManipulator::setTransformation(const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up)
{
    if (!_node)
        return;

    // compute rotation matrix
    Vec3d lv(center-eye);
    _distance = lv.length();
    _center = center;

    // note LookAt = inv(CF)*inv(RM)*inv(T) which is equivalent to:
    // inv(R) = CF*LookAt.

    _rotation = Matrixd::lookAt(eye, center, up).getRotate().inverse();
    _previousUp = getUpVector(getCoordinateFrame(_center));
}

bool OverviewManipulator::performMovementLeftMouseButton(double eventTimeDelta, double dx, double dy)
{
    // saving the original distance
    double distance = getDistance();

    // performing the rotation normally
    bool result = OrbitManipulator::performMovementLeftMouseButton(eventTimeDelta, dx, dy);

    // then if the eye got below the center, snapping it back a bit
    auto matrix = getMatrix();
    // from the function above
    Vec3d eye(matrix(3, 0), matrix(3, 1), matrix(3, 2));
    // not that far low!
    eye.z() = std::max(getCenter().z(), eye.z());

    // correcting to keep the distance
    Vec3d toEye = eye - getCenter();
    toEye.normalize();
    eye = getCenter() + toEye * distance;

    // and we're done!
    setTransformation(eye, getCenter(), Vec3d(0, 0, 1));

    return result;
}

// same as in TerrainManipulator
bool OverviewManipulator::performMovementMiddleMouseButton(double eventTimeDelta, double dx, double dy)
{
    // pan model.
    double scale = -0.3f * _distance * getThrowScale(eventTimeDelta);

    Matrixd rotation_matrix;
    rotation_matrix.makeRotate(_rotation);

    // compute look vector.
    Vec3d sideVector = getSideVector(rotation_matrix);

    // CoordinateFrame coordinateFrame = getCoordinateFrame(_center);
    // Vec3d localUp = getUpVector(coordinateFrame);
    Vec3d localUp = _previousUp;

    Vec3d forwardVector = localUp^sideVector;
    sideVector = forwardVector^localUp;

    forwardVector.normalize();
    sideVector.normalize();

    Vec3d dv = forwardVector * (dy*scale) + sideVector * (dx*scale);

    _center += dv;

    if (_node.valid()) {
        // now reorientate the coordinate frame to the frame coords.
        Vec3d new_localUp = getUpVector(getCoordinateFrame(_center));

        Quat pan_rotation;
        pan_rotation.makeRotate(localUp, new_localUp);

        if (!pan_rotation.zeroRotation()) {
            _rotation = _rotation * pan_rotation;
            _previousUp = new_localUp;
        }
        else {
            OSG_INFO<<"New up orientation nearly inline - no need to rotate"<<std::endl;
        }
    }

    return true;
}

bool OverviewManipulator::performMovementRightMouseButton(double eventTimeDelta, double dx, double dy)
{
    // same as for the middle mouse panning above (just positive)
    double scale = 0.3f * _distance * getThrowScale(eventTimeDelta);

    auto matrix = getMatrix();
    // from the setByMatrix function
    Vec3d lookVector(-matrix(2, 0), -matrix(2, 1), -matrix(2, 2));

    Vec3d side = osg::Vec3d(0, 0, 1) ^ lookVector;
    side.normalize();

    // only moving perpendicular to the look vector horizontally, and vertically
    setCenter(getCenter() + (side * dx - Vec3d(0, 0, dy)) * scale);

    return true;
}

}  // namespace omnetpp
}  // namespace omnetpp


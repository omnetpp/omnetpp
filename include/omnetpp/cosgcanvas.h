//==========================================================================
//   COSGCANVAS.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COSGCANVAS_H
#define __OMNETPP_COSGCANVAS_H

#include "cownedobject.h"

#ifdef WITH_OSG

// don't include OSG headers yet
namespace osg { class Node; }

NAMESPACE_BEGIN

#define OMNETPP_OSGCANVAS_VERSION  0x20150721  //XXX identifies canvas code version until API stabilizes

/**
 * Wraps an OpenSceneGraph scene, allowing 3D visualization in graphical user
 * interfaces that support it (currently Qtenv). This class only wraps an OSG
 * scene (as an osg::Node* pointer) and some visualization hints, other
 * tasks like setting up a 3D viewer window are taken care of by the OMNeT++
 * user interface code (Qtenv). The scene graph can be assembled using the OSG
 * API, e.g. using osgDB::readNodeFile() or creating and adding nodes directly.
 *
 * Since OpenSceneGraph is not a mandatory part of OMNeT++, it is recommended
 * that you surround your OSG code with #ifdef HAVE_OSG (or if you use osgEarth,
 * #ifdef HAVE_OSGEARTH) directives.
 *
 * @ingroup Canvas
 */
class SIM_API cOsgCanvas : public cOwnedObject
{
    private:
        osg::Node *scene;  // *not* owned!

    private:
        void copy(const cOsgCanvas& other);

    public:
        /** @name Constructors, destructor, assignment */
        //@{
        cOsgCanvas(const char *name=nullptr, osg::Node *scene=nullptr);
        cOsgCanvas(const cOsgCanvas& other) : cOwnedObject(other) {copy(other);}
        virtual ~cOsgCanvas();
        cOsgCanvas& operator=(const cOsgCanvas& other);
        //@}

        /** @name Redefined cObject member functions. */
        //@{
        virtual cOsgCanvas *dup() const override {return new cOsgCanvas(*this);}
        virtual std::string info() const override;
        //@}

        /** @name OSG scene. */
        //@{
        virtual void setScene(osg::Node *scene);
        virtual osg::Node *getScene() const {return scene;}
        //@}
};

/**
 * cOsgCanvas specialized for osgEarth.
 */
class SIM_API cOsgEarthCanvas : public cOsgCanvas
{
    private:
        void copy(const cOsgEarthCanvas& other) {}

    public:
        /** @name Constructors, destructor, assignment */
        //@{
        cOsgEarthCanvas(const char *name=nullptr, osg::Node *scene=nullptr) : cOsgCanvas(name,scene) {}
        cOsgEarthCanvas(const cOsgEarthCanvas& other) : cOsgCanvas(other) {copy(other);}
        virtual ~cOsgEarthCanvas() {}
        cOsgEarthCanvas& operator=(const cOsgEarthCanvas& other);
        //@}

        /** @name Redefined cObject member functions. */
        //@{
        virtual cOsgCanvas *dup() const override {return new cOsgEarthCanvas(*this);}
        virtual std::string info() const override;
        //@}

        //TODO stuff (osgEarth-related hints)
};

NAMESPACE_END

#endif // WITH_OSG


#endif

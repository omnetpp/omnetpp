//==========================================================================
//  OSGVIEWER.CC - part of
//
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

#ifdef WITH_OSG

#include <QDebug>
#include <QToolTip>
#include <QResizeEvent>
#include <osgEarth/Version>
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2, 6, 0)
    #include <osgEarthUtil/Sky>
#else
    #include <osgEarthUtil/SkyNode>
#endif
#include <osgEarth/MapNode>
#include <osgEarthUtil/EarthManipulator>
#include <osgGA/GUIEventAdapter>
#include <osgGA/TerrainManipulator>
#include <osgGA/TrackballManipulator>
#include <osg/TexGenNode>
#include "omnetpp/osgutil.h"
#include "inspectorutil.h"
#include "osgviewer.h"
#include "cameramanipulators.h"

namespace omnetpp {
namespace qtenv {

#define emit

// XXX is this still necessary? can be replaced by Qt events?
class PickHandler : public osgGA::GUIEventHandler
{
  private:
    OsgViewer *viewer;

  public:
    PickHandler(OsgViewer *viewer) : viewer(viewer) {}
    virtual ~PickHandler();
    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    virtual std::vector<cObject *> pick(osgViewer::View *view, const osgGA::GUIEventAdapter& ea);
};

PickHandler::~PickHandler()
{
}

bool PickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
    osgViewer::View *view = dynamic_cast<osgViewer::View *>(&aa);
    if (!view)
        return false;

    auto eventType = ea.getEventType();

    // keeping track of where the right button has been pressed down so we can
    // show the context menu upon release only if it hasn't been moved much
    // - otherwise it was probably a zooming or panning gesture, no need for a menu
    static float rightClickX;
    static float rightClickY;
    // only showing the context menu if the drag was shorter than this many pixels
    const int rightDragThreshold = 3;

    // this second little hack is necessary because if we setObject while handling the DOUBLECLICK
    // event, and the next object is shown in 2D mode, OSG will not get the release event, and
    // upon coming back to the module in which the doubleclick happened, OSG will think the button
    // is still held down while it is not.
    static bool lastEventWasDoubleClick = false;  // ignoring FRAME events

    switch (eventType) {
        case osgGA::GUIEventAdapter::PUSH:
            if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
                auto objects = pick(view, ea);
                if (!objects.empty()) {
                    emit viewer->objectsPicked(objects);
                    return true;
                }
            }
            if (ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON) {
                rightClickX = ea.getX();
                rightClickY = ea.getY();
            }
            break;

        // context menus can't be opened on push, because then only the QMenu would receive
        // the release event, osg wouldn't, and it would think it's stuck in the "down" position
        case osgGA::GUIEventAdapter::RELEASE:
            if (ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON
                    && osg::Vec2d(ea.getX() - rightClickX, ea.getY() - rightClickY).length() < rightDragThreshold) {
                auto objects = pick(view, ea);
                Inspector *insp = InspectorUtil::getContainingInspector(viewer);
                QMenu *menu;
                if (!objects.empty() && insp && insp->supportsObject(objects.front())) {
                    menu = InspectorUtil::createInspectorContextMenu(QVector<cObject *>::fromStdVector(objects), insp);
                    menu->addSeparator();
                }
                else {
                    menu = new QMenu(viewer);
                }

                menu->addMenu(viewer->createCameraManipulatorMenu());
                menu->exec(viewer->mapToGlobal(QPoint(ea.getX(), viewer->childrenRect().height() - ea.getY())));
                return true;
            }
            if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON && lastEventWasDoubleClick) {
                auto objects = pick(view, ea);
                Inspector *insp = InspectorUtil::getContainingInspector(viewer);
                if (!objects.empty() && insp && insp->supportsObject(objects.front())) {
                    insp->setObject(objects.front());
                    return true;
                }
            }
            break;

        case osgGA::GUIEventAdapter::KEYDOWN:
            if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Space || ea.getKey() == osgGA::GUIEventAdapter::KEY_Return) {
                osg::ref_ptr<osgGA::GUIEventAdapter> event = new osgGA::GUIEventAdapter(ea);
                event->setX((ea.getXmin()+ea.getXmax())*0.5);
                event->setY((ea.getYmin()+ea.getYmax())*0.5);
                auto objects = pick(view, *event);
                if (!objects.empty()) {
                    emit viewer->objectsPicked(objects);
                    return true;
                }
            }
            break;

        default:
            ;
    }

    lastEventWasDoubleClick = eventType == osgGA::GUIEventAdapter::DOUBLECLICK
            // not letting the frame events reset this flag
            || (lastEventWasDoubleClick && eventType == osgGA::GUIEventAdapter::FRAME);

    return false;
}

std::vector<cObject *> PickHandler::pick(osgViewer::View *view, const osgGA::GUIEventAdapter& ea)
{
    return viewer->objectsAt(QPoint(ea.getX()-ea.getWindowX(), // to widget local, then invert y
                                    ea.getWindowHeight() - (ea.getY() - ea.getWindowY())));
}

//---

OsgViewer::OsgViewer(QWidget *parent) : QWidget(parent)
{
    setThreadingModel(osgViewer::ViewerBase::SingleThreaded);  // XXX crashes with Multithreaded
    setContextMenuPolicy(Qt::NoContextMenu);  // to prevent the default Qt context handling, we will do it manually
    // disable the default setting of viewer.done() by pressing Escape.
    setKeyEventSetsDone(0);

    glWidget = addViewWidget();

    QGridLayout *grid = new QGridLayout;
    grid->addWidget(glWidget, 0, 0);
    grid->setMargin(1);  // WARNING: don't set 0 or the 3D scene won't appear!
    setLayout(grid);

    // set up periodic update of 3D view
    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
    timer.start(33);  // 60Hz -- TODO make configurable

    QActionGroup *cameraManipulatorActionGroup = new QActionGroup(this);  // will provide radiobutton functionality
    connect(cameraManipulatorActionGroup, SIGNAL(triggered(QAction *)), this, SLOT(setCameraManipulator(QAction *)));

    toTerrainManipulatorAction = new QAction("Terrain", this);
    toTerrainManipulatorAction->setData(cOsgCanvas::CAM_TERRAIN);
    toTerrainManipulatorAction->setActionGroup(cameraManipulatorActionGroup);
    toTerrainManipulatorAction->setCheckable(true);

    toOverviewManipulatorAction = new QAction("Overview", this);
    toOverviewManipulatorAction->setData(cOsgCanvas::CAM_OVERVIEW);
    toOverviewManipulatorAction->setActionGroup(cameraManipulatorActionGroup);
    toOverviewManipulatorAction->setCheckable(true);

    toTrackballManipulatorAction = new QAction("Trackball", this);
    toTrackballManipulatorAction->setData(cOsgCanvas::CAM_TRACKBALL);
    toTrackballManipulatorAction->setActionGroup(cameraManipulatorActionGroup);
    toTrackballManipulatorAction->setCheckable(true);

    toEarthManipulatorAction = new QAction("Earth", this);
    toEarthManipulatorAction->setData(cOsgCanvas::CAM_EARTH);
    toEarthManipulatorAction->setActionGroup(cameraManipulatorActionGroup);
    toEarthManipulatorAction->setCheckable(true);
}

OsgViewer::~OsgViewer()
{
    removeView(view);
}

QWidget *OsgViewer::addViewWidget()
{
    // create an OSG viewer (NOT part of the Qt widget tree!)
    view = new osgViewer::View();
    addView(view);

    // configure viewer and its camera with some default settings
    // (they will be overwritten from inspector hints later)
    osg::Camera *camera = view->getCamera();
    camera->setClearColor(osg::Vec4(0.9, 0.9, 0.9, 1.0));
    camera->setViewport(new osg::Viewport(0, 0, 100, 100));
    camera->setProjectionResizePolicy(osg::Camera::HORIZONTAL);
    defaultComputeNearFarMode = camera->getComputeNearFarMode();

    view->addEventHandler(new PickHandler(this));
    view->addEventHandler(new osgViewer::StatsHandler);
    view->setCameraManipulator(new osgGA::TrackballManipulator);

    // create an OSG "graphics window" where camera will render the scene;
    // the graphics window wraps a QGLWidget, and we'll add THAT to the Qt widget tree
    osg::DisplaySettings *ds = osg::DisplaySettings::instance().get();
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->windowName = "";
    traits->windowDecoration = false;
    traits->x = 0;
    traits->y = 0;
    traits->width = 100;
    traits->height = 100;
    traits->doubleBuffer = true;
    traits->alpha = ds->getMinimumNumAlphaBits();
    traits->stencil = ds->getMinimumNumStencilBits();
    traits->samples = 4;  // TODO should come from a hint

    osgQt::GraphicsWindowQt *graphicsWindow = new osgQt::GraphicsWindowQt(traits.get());
    camera->setGraphicsContext(graphicsWindow);
    return graphicsWindow->getGLWidget();
}

void OsgViewer::paintEvent(QPaintEvent *event)
{
    frame();
}

bool OsgViewer::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {
        QHelpEvent *helpEvent = (QHelpEvent *)event;

        cComponent *comp = nullptr;
        auto objects = objectsAt(helpEvent->pos());
        for (auto o : objects) {
            if ((comp = dynamic_cast<cComponent *>(o)))
                break;
        }

        QPoint pos = helpEvent->pos();
        auto offs = QPoint(10, 10);
        QRect rect(pos-offs, pos+offs);

        if (comp)
            QToolTip::showText(helpEvent->globalPos(),
                               makeComponentTooltip(comp),
                               this, rect);
        else
            QToolTip::hideText();
    }
    return QWidget::event(event);
}

void OsgViewer::setOsgCanvas(cOsgCanvas *canvas)
{
    if (osgCanvas != canvas) {
        osgCanvas = canvas;
        refresh();
        if (osgCanvas != nullptr) {
            applyViewerHints();
            toEarthManipulatorAction->setEnabled(osgCanvas->getViewerStyle() == cOsgCanvas::STYLE_EARTH);
        }
        else
            resetViewer();
    }
}

void OsgViewer::refresh()
{
    osg::Node *scene = osgCanvas ? osgCanvas->getScene() : nullptr;
    if (scene != view->getSceneData()) {
        view->setSceneData(scene);

        auto sky = osgEarth::findTopMostNodeOfType<osgEarth::Util::SkyNode>(scene);
        if (sky)
            sky->attach(view);
    }
}

void OsgViewer::applyViewerHints()
{
    ASSERT(osgCanvas != nullptr);

    cOsgCanvas::Color color = osgCanvas->getClearColor();
    setClearColor(color.red/255.0, color.green/255.0, color.blue/255.0, 1.0);

    setCameraManipulator(osgCanvas->getCameraManipulatorType());

    osg::Camera *camera = view->getCamera();
    camera->setProjectionMatrixAsPerspective(30, 1, 1, 1000);  // reset to sensible values to avoid later calls bump into a singular state

    double widgetAspect = glWidget->geometry().width() / (double)glWidget->geometry().height();
    setAspectRatio(widgetAspect);
    setFieldOfViewAngle(osgCanvas->getFieldOfViewAngle());

    if (osgCanvas->hasZLimits()) {
        camera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
        setZNearFar(osgCanvas->getZNear(), osgCanvas->getZFar());
    }
    else {
        camera->setComputeNearFarMode(defaultComputeNearFarMode);
    }
}

void OsgViewer::resetViewer()
{
    // called when there is no cOsgCanvas to display
    ASSERT(osgCanvas == nullptr);
    setClearColor(0.9, 0.9, 0.9, 1.0);
    osg::Camera *camera = view->getCamera();
    camera->setProjectionMatrixAsPerspective(30, 1, 1, 1000);
}

std::vector<cObject *> OsgViewer::objectsAt(const QPoint& pos)
{
    osgUtil::LineSegmentIntersector::Intersections intersections;
    view->computeIntersections(view->getCamera(), osgUtil::Intersector::WINDOW, pos.x(), height() - pos.y(), intersections);

    std::vector<cObject *> objects;
    for (osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin(); hitr != intersections.end(); ++hitr) {
        const osg::NodePath& nodePath = hitr->nodePath;
        if (!nodePath.empty()) {
            // find cObject pointer node (OmnetppObjectNode) in nodepath, back to front
            for (int i = nodePath.size()-1; i >= 0; --i) {
                if (cObjectOsgNode *objNode = dynamic_cast<cObjectOsgNode *>(nodePath[i])) {
                    if (cObject *obj = const_cast<cObject *>(objNode->getObject())) {
                        // qDebug() << "hit omnetpp object" << obj->getClassName() << obj->getFullPath().c_str();
                        if (std::find(objects.begin(), objects.end(), obj) == objects.end())  // filter out duplicates
                            objects.push_back(obj);
                    }
                }
            }
        }
    }
    return objects;
}

void OsgViewer::setClearColor(float r, float g, float b, float alpha)
{
    osg::Camera *camera = view->getCamera();
    camera->setClearColor(osg::Vec4(r, g, b, alpha));
}

void OsgViewer::setCameraManipulator(cOsgCanvas::CameraManipulatorType type, bool keepView)
{
    // saving the current viewpoint

    // all of these are in world coordinates, so expect large-values when using osgEarth
    osg::Vec3d eye, center, up;
    float distance = 1;  // how far the center is from the eye.

    // all of the generic manipulators are OrbitManipulators
    if (auto orbitManip = dynamic_cast<osgGA::OrbitManipulator *>(view->getCameraManipulator()))
        distance = orbitManip->getDistance();

    // EarthManipulator just happens to have a similar behaviour and a method with the same name
    if (auto earthManip = dynamic_cast<osgEarth::Util::EarthManipulator *>(view->getCameraManipulator()))
        distance = earthManip->getDistance();

    view->getCamera()->getViewMatrixAsLookAt(eye, center, up, distance);

    // setting up the new manipulator

    osgGA::CameraManipulator *manipulator = nullptr;

    if (type == cOsgCanvas::CAM_AUTO)
        type = osgCanvas->getViewerStyle() == cOsgCanvas::STYLE_GENERIC ? cOsgCanvas::CAM_OVERVIEW : cOsgCanvas::CAM_EARTH;

    switch (type) {
        case cOsgCanvas::CAM_TERRAIN:
            manipulator = new osgGA::TerrainManipulator;
            toTerrainManipulatorAction->setChecked(true);
            break;

        case cOsgCanvas::CAM_OVERVIEW:
            manipulator = new OverviewManipulator;
            toOverviewManipulatorAction->setChecked(true);
            break;

        case cOsgCanvas::CAM_TRACKBALL:
            manipulator = new osgGA::TrackballManipulator;
            toTrackballManipulatorAction->setChecked(true);
            break;

        case cOsgCanvas::CAM_EARTH:
            manipulator = new osgEarth::Util::EarthManipulator;
            toEarthManipulatorAction->setChecked(true);
            break;

        case cOsgCanvas::CAM_AUTO:  /* Impossible, look at the if above, just silencing a warning. */
            break;
    }

    view->setCameraManipulator(manipulator);

    // restoring the viewpoint into the new manipulator

    if (keepView) {
        if (auto orbitManip = dynamic_cast<osgGA::OrbitManipulator *>(manipulator)) {
            // this was the simplest way to force all kinds of manipulators to the viewpoint
            orbitManip->setHomePosition(eye, center, up);
            orbitManip->home(0);
            orbitManip->setDistance(distance);
        }

        // and EarthManipulator is a different story as always
        if (auto earthManip = dynamic_cast<osgEarth::Util::EarthManipulator *>(manipulator)) {
            auto srs = osgEarth::MapNode::findMapNode(osgCanvas->getScene())->getMap()->getSRS();

            osg::Vec3d geoCenter;  // should be longitude, latitude and absolute altitude
            srs->transformFromWorld(center, geoCenter);

            auto gc = osgEarth::GeoPoint(srs, geoCenter, osgEarth::ALTMODE_ABSOLUTE);
            // this transforms into a little local Descartian space around the point the camera is looking at
            osg::Matrixd worldToLocal;
            gc.createWorldToLocal(worldToLocal);

            // localCenter would be zero (even if imprecisely)
            osg::Vec3d localEye = eye * worldToLocal;
            osg::Vec3d localUp = (eye + up) * worldToLocal - localEye;

            double heading = -atan2(localEye.y(), localEye.x()) * 180.0 / M_PI - 90;
            double pitch = atan2(localUp.z(), osg::Vec2d(localUp.x(), localUp.y()).length()) * 180.0 / M_PI - 90;

            osgEarth::Viewpoint vp("viewpoint", geoCenter.x(), geoCenter.y(), geoCenter.z(), heading, pitch, distance);
            earthManip->setViewpoint(vp);
        }
    }

    // setting the home viewpoint if found

    if (type == cOsgCanvas::CAM_EARTH) {
        const osgEarth::Viewpoint& homeViewpoint = osgCanvas->getEarthViewpoint();
        if (homeViewpoint.isValid())
            ((osgEarth::Util::EarthManipulator *)manipulator)->setHomeViewpoint(homeViewpoint);
    }
    else {
        const cOsgCanvas::Viewpoint& homeViewpoint = osgCanvas->getGenericViewpoint();
        if (homeViewpoint.valid)
            manipulator->setHomePosition(homeViewpoint.eye, homeViewpoint.center, homeViewpoint.up);
    }

    // and going home if needed
    if (!keepView)
        manipulator->home(0);
}

void OsgViewer::setFieldOfViewAngle(double fovy)
{
    osg::Camera *camera = view->getCamera();
    double origFovy, origAspect, origZNear, origZFar;
    camera->getProjectionMatrixAsPerspective(origFovy, origAspect, origZNear, origZFar);
    camera->setProjectionMatrixAsPerspective(fovy, origAspect, origZNear, origZFar);
}

void OsgViewer::setAspectRatio(double aspectRatio)
{
    osg::Camera *camera = view->getCamera();
    double origFovy, origAspect, origZNear, origZFar;
    camera->getProjectionMatrixAsPerspective(origFovy, origAspect, origZNear, origZFar);
    camera->setProjectionMatrixAsPerspective(origFovy, aspectRatio, origZNear, origZFar);
}

void OsgViewer::setZNearFar(double zNear, double zFar)
{
    osg::Camera *camera = view->getCamera();
    double origFovy, origAspect, origZNear, origZFar;
    camera->getProjectionMatrixAsPerspective(origFovy, origAspect, origZNear, origZFar);
    camera->setProjectionMatrixAsPerspective(origFovy, origAspect, zNear, zFar);
}

QMenu *OsgViewer::createCameraManipulatorMenu()
{
    QMenu *menu = new QMenu("Camera manipulator", this);
    menu->addAction(toTerrainManipulatorAction);
    menu->addAction(toOverviewManipulatorAction);
    menu->addAction(toTrackballManipulatorAction);
    menu->addAction(toEarthManipulatorAction);
    return menu;
}

void OsgViewer::setCameraManipulator(QAction *sender)
{
    setCameraManipulator((cOsgCanvas::CameraManipulatorType)sender->data().value<int>(), true);
}
}  // qtenv
}  // omnetpp

#endif  // WITH_OSG


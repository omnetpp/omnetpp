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
#include <QResizeEvent>
#include <osgEarth/Version>
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2, 6, 0)
    #include <osgEarthUtil/Sky>
#else
    #include <osgEarthUtil/SkyNode>
#endif
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

class PickHandler : public osgGA::GUIEventHandler
{
private:
    OsgViewer *viewer;
public:
    PickHandler(OsgViewer *viewer) : viewer(viewer) {}
    virtual ~PickHandler();
    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    virtual std::vector<cObject*> pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);
};

PickHandler::~PickHandler()
{
}

bool PickHandler::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    osgViewer::View *view = dynamic_cast<osgViewer::View*>(&aa);

    // keeping track of where the right button has been pressed down so we can
    // show the context menu upon release only if it hasn't been moved much
    // - otherwise it was probably a zooming or panning gesture, no need for a menu
    static float rightClickX;
    static float rightClickY;
    // only showing the context menu if the drag was shorter than this many pixels
    const int rightDragThreshold = 3;

    switch(ea.getEventType())
    {
        case osgGA::GUIEventAdapter::PUSH:
            if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
                emit viewer->objectsPicked(pick(view, ea));
            if (ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON) {
                rightClickX = ea.getX();
                rightClickY = ea.getY();
            }
            break;
        // context menus can't be opened on push, because then only the QMenu would receive
        // the release event, osg wouldn't, and it would think it's stuck in the "down" position
        case osgGA::GUIEventAdapter::RELEASE:
            if (view && ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON
                    && osg::Vec2d(ea.getX() - rightClickX, ea.getY() - rightClickY).length() < rightDragThreshold) {
                auto objects = pick(view, ea);
                Inspector *insp = dynamic_cast<Inspector*>(viewer->parentWidget()->parentWidget());
                QMenu *menu;
                if (!objects.empty()) {
                    menu = InspectorUtil::createInspectorContextMenu(QVector<cObject*>::fromStdVector(objects), insp);
                    menu->addSeparator();
                } else {
                    menu = new QMenu(viewer);
                }

                menu->addMenu(viewer->createCameraManipulatorMenu());
                menu->exec(viewer->mapToGlobal(QPoint(ea.getX(), viewer->childrenRect().height() - ea.getY())));
            }
            break;
        case osgGA::GUIEventAdapter::KEYDOWN:
            if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Space || ea.getKey() == osgGA::GUIEventAdapter::KEY_Return) {
                osg::ref_ptr<osgGA::GUIEventAdapter> event = new osgGA::GUIEventAdapter(ea);
                event->setX((ea.getXmin()+ea.getXmax())*0.5);
                event->setY((ea.getYmin()+ea.getYmax())*0.5);
                if (view)
                    emit viewer->objectsPicked(pick(view, *event));
            }
            break;
        default:;
    }
    return false;
}

std::vector<cObject*> PickHandler::pick(osgViewer::View *view, const osgGA::GUIEventAdapter &ea)
{
    // printf("pick()\n");

    osgUtil::LineSegmentIntersector::Intersections intersections;
    std::vector<cObject*> objects;

    if (view->computeIntersections(ea, intersections)) {
        // looks like the list goes in increasing distance from camera, so we can just pick the first one
        for (osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin(); hitr != intersections.end(); ++hitr) {
            const osg::NodePath& nodePath = hitr->nodePath;
            if (!nodePath.empty()) {
                // find cObject pointer node (OmnetppObjectNode) in nodepath, back to front
                for (int i = nodePath.size()-1; i >= 0; --i) {
                    if (OmnetppObjectNode *objNode = dynamic_cast<OmnetppObjectNode*>(nodePath[i])) {
                        if (cObject *obj = objNode->getObject()) {
                            // qDebug() << "hit omnetpp object" << obj->getClassName() << obj->getFullPath().c_str();
                            if (std::find(objects.begin(), objects.end(), obj) == objects.end()) // filter out duplicates
                                objects.push_back(obj);
                        }
                    }
                }
            }
        }
    }

    return objects;
}


//---

OsgViewer::OsgViewer(QWidget *parent) : QWidget(parent)
{
    setThreadingModel(osgViewer::ViewerBase::SingleThreaded);  //XXX crashes with Multithreaded
    setContextMenuPolicy(Qt::NoContextMenu); // to prevent the default Qt context handling, we will do it manually
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

    QActionGroup *cameraManipulatorActionGroup = new QActionGroup(this); // will provide radiobutton functionality
    connect(cameraManipulatorActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(setCameraManipulator(QAction*)));

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
    camera->setProjectionResizePolicy(osg::Camera::FIXED);
    camera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

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

void OsgViewer::resizeEvent(QResizeEvent *event)
{
    if (osgCanvas)
        setPerspective(osgCanvas->getFieldOfViewAngle(), osgCanvas->getZNear(), osgCanvas->getZFar());
    QWidget::resizeEvent(event);
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
    // printf("applyViewerHints()\n");
    ASSERT(osgCanvas != nullptr);

    cOsgCanvas::Color color = osgCanvas->getClearColor();
    setClearColor(color.red/255.0, color.green/255.0, color.blue/255.0, 1.0);

    setCameraManipulator(osgCanvas->getCameraManipulatorType());

    setPerspective(osgCanvas->getFieldOfViewAngle(), osgCanvas->getZNear(), osgCanvas->getZFar());

    if (osgCanvas->getViewerStyle() == cOsgCanvas::STYLE_EARTH)
        setEarthViewpoint(osgCanvas->getEarthViewpoint());
}

void OsgViewer::resetViewer()
{
    // printf("resetViewer()\n");
    setClearColor(0.9, 0.9, 0.9, 1.0);
    //setCameraManipulator(nullptr);
    setPerspective(30, 1, 1000);
}

void OsgViewer::setClearColor(float r, float g, float b, float alpha)
{
    osg::Camera *camera = view->getCamera();
    camera->setClearColor(osg::Vec4(r, g, b, alpha));
}

void OsgViewer::setCameraManipulator(cOsgCanvas::CameraManipulatorType type)
{
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
        case cOsgCanvas::CAM_AUTO: /* Impossible, look at the if above, just silencing a warning. */ break;
    }

    const cOsgCanvas::Viewpoint &viewpoint = osgCanvas->getGenericViewpoint();

    if (viewpoint.valid) {
        osg::Vec3d eye(viewpoint.eye.x, viewpoint.eye.y, viewpoint.eye.z);
        osg::Vec3d center(viewpoint.center.x, viewpoint.center.y, viewpoint.center.z);
        osg::Vec3d up(viewpoint.up.x, viewpoint.up.y, viewpoint.up.z);

        manipulator->setHomePosition(eye, center, up);
        manipulator->home(0);
    }

    view->setCameraManipulator(manipulator);
}

void OsgViewer::setPerspective(double fieldOfViewAngle, double zNear, double zFar)
{
    osg::Camera *camera = view->getCamera();
    double widgetAspect = glWidget->geometry().width() / (double) glWidget->geometry().height();
    camera->setProjectionMatrixAsPerspective(fieldOfViewAngle, widgetAspect, zNear, zFar);
}

void OsgViewer::setEarthViewpoint(const osgEarth::Viewpoint& viewpoint)
{
    if (viewpoint.isValid()) {
        osgGA::CameraManipulator *manip = view->getCameraManipulator();
        if (osgEarth::Util::EarthManipulator *earthManip = dynamic_cast<osgEarth::Util::EarthManipulator*>(manip)) {
            double duration = 0.0; //XXX make configurable (or viewer hint in cOsgCanvas
            earthManip->setViewpoint(viewpoint, duration);
        }
    }
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
    setCameraManipulator((cOsgCanvas::CameraManipulatorType)sender->data().value<int>());
}

} // qtenv
} // omnetpp

#endif // WITH_OSG

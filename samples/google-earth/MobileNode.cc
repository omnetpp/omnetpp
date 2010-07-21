//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2010 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include <omnetpp.h>
#include "KmlHttpServer.h"
#include "KmlUtil.h"
#include "ChannelController.h"

/**
 * A mobile node.
 */
class MobileNode : public cSimpleModule, public IKmlFragmentProvider, public IMobileNode
{
  protected:
    // configuration
    double playgroundLat,playgroundLon;  // NW corner of playground, in degrees
    double playgroundHeight,playgroundWidth;  // in meters
    double timeStep;
    unsigned int trailLength;
    std::string color;
    std::string modelURL;
    double modelScale;
    bool showTxRange;
    double txRange;
    double speed;

    // node position and heading (speed is constant in this model)
    double heading; // in degrees
    double x, y; // in meters, relative to playground origin

    std::vector<KmlUtil::Pt2D> path; // for visualization

    // utility function: converts playground-relative y coordinate (meters) to latitude
    double y2lat(double y) { return KmlUtil::y2lat(playgroundLat, y); }

    // utility function: converts playground-relative x coordinate (meters) to longitude
    double x2lon(double x) { return KmlUtil::x2lon(playgroundLat, playgroundLon, x); }

  public:
     MobileNode();
     virtual ~MobileNode();

     double getX() { return x; }
     double getY() { return y; }
     double getTxRange() { return txRange; }

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual std::string getKmlFragment();
};

Define_Module(MobileNode);

MobileNode::MobileNode()
{
}

MobileNode::~MobileNode()
{
}

void MobileNode::initialize()
{
    x = par("startX");
    y = par("startY");
    heading = 360*dblrand();
    timeStep = par("timeStep");

    getDisplayString().setTagArg("p", 0, x);
    getDisplayString().setTagArg("p", 1, y);

    playgroundLat = simulation.getSystemModule()->par("playgroundLatitude");
    playgroundLon = simulation.getSystemModule()->par("playgroundLongitude");
    playgroundHeight = simulation.getSystemModule()->par("playgroundHeight");
    playgroundWidth = simulation.getSystemModule()->par("playgroundWidth");

    trailLength = par("trailLength");
    modelURL = par("modelURL").stringValue();
    modelScale = par("modelScale");
    showTxRange = par("showTxRange");
    speed = par("speed");
    txRange = par("txRange");

    color = par("color").stringValue();
    if (color.empty())
    {
        // pick a color with a random hue
        char buf[16];
        double red,green,blue;
        KmlUtil::hsbToRgb(dblrand(), 1.0, 1.0, red, green, blue);
        sprintf(buf, "%2.2x%2.2x%2.2x", int(blue*255), int(green*255), int(red*255));
        color = buf;
    }

    KmlHttpServer::getInstance()->addKmlFragmentProvider(this);
    ChannelController::getInstance()->addMobileNode(this);

    // schedule first move
    cMessage *timer = new cMessage("move");
    scheduleAt(simTime(), timer);
}

void MobileNode::handleMessage(cMessage *msg)
{
    // update speed vector
    heading += 120*(dblrand()-0.5) * timeStep;
    heading = fmod(heading + 360, 360);

    // update position
    double vx = sin(heading*M_PI/180) * speed;
    double vy = -cos(heading*M_PI/180) * speed;
    x += vx*timeStep;
    y += vy*timeStep;

    // keep the node inside the playground
    if (x < 0) {x=0; heading = 360 - heading; }
    if (x > playgroundWidth) {x=playgroundWidth; heading = 360-heading;}
    if (y < 0) {y=0; heading = 180 - heading;}
    if (y > playgroundHeight) {y=playgroundHeight; heading = 180 - heading;}

    // store the position to be able to create a trail
    if (trailLength > 0)
        path.push_back(KmlUtil::Pt2D(x2lon(x),y2lat(y)));

    // Trail is at max length. Remove the oldest point to keep it at "trailLength"
    // note: this is not very efficient because entire vector is shifted down; should use circular buffer
    if (path.size () > trailLength)
        path.erase(path.begin());

    getDisplayString().setTagArg("p", 0, x);
    getDisplayString().setTagArg("p", 1, y);

    // schedule next move
    scheduleAt(simTime()+timeStep, msg);
}

std::string MobileNode::getKmlFragment()
{
    double longitude = x2lon(x);
    double latitude = y2lat(y);
    char buf[16];
    sprintf(buf, "%d", getIndex());
    std::string fragment;
    fragment += KmlUtil::folderHeader((std::string("folder_")+buf).c_str(), getFullName());

#ifdef USE_TRACK
    fragment += KmlUtil::track((std::string("track_")+buf).c_str(), path, timeStep, modelScale, modelURL.c_str(), "movement trail", NULL, (std::string("ff")+color).c_str());
#else
    fragment += KmlUtil::placemark((std::string("placemark_")+buf).c_str(), longitude, latitude, 2*modelScale, getFullName(), NULL);
    if (trailLength > 0)
        fragment += KmlUtil::lineString((std::string("trail_")+buf).c_str(), path, "movement trail", NULL, (std::string("ff")+color).c_str());
    if (showTxRange)
        fragment += KmlUtil::disk((std::string("disk_")+buf).c_str(), longitude, latitude, txRange, "transmission range", NULL, (std::string("40")+color).c_str());
    if (!modelURL.empty()) {
        double modelheading = fmod((360 + 90 + heading), 360);
        fragment += KmlUtil::model((std::string("model_")+buf).c_str(), longitude, latitude, modelheading, modelScale, modelURL.c_str(), "3D model", NULL);
    }
#endif

    fragment += "</Folder>\n";
    return fragment;
}


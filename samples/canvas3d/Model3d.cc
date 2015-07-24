//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <osgDB/ReadFile>
#include <omnetpp.h>

using namespace omnetpp;

class Model3d : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Model3d);

void Model3d::initialize()
{
    osg::Node* gliderScene = osgDB::readNodeFile("glider.osgb");

    cOsgCanvas *osgCanvas = new cOsgCanvas("3d model");
    osgCanvas->setScene(gliderScene);

    cOsgCanvas *osgCanvas2 = new cOsgCanvas("3d model with hints");
    osgCanvas2->setScene(gliderScene);
    osgCanvas2->setClearColor(cOsgCanvas::Color(0,64,0));
    osgCanvas2->setFieldOfViewAngle(120);
}

void Model3d::handleMessage(cMessage *msg)
{
}

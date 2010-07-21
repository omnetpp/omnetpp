//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2010 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __KMLHTTPSERVER_H__
#define __KMLHTTPSERVER_H__

#include <omnetpp.h>

class cSocketRTScheduler;

/**
 * Interface for objects that want to provide KML markup
 * fragments into the document returned by KmlHttpServer.
 */
class IKmlFragmentProvider
{
  public:
    virtual std::string getKmlFragment() = 0;
};


/**
 * Answers HTTP GET requests that arrive to the built-in web server
 * of the simulation, and returns KML or HTML documents.
 *
 * KML is assembled from markup fragments provided by the
 * registered IKmlFragmentProviders.
 */
class KmlHttpServer : public cSimpleModule
{
  protected:
    static KmlHttpServer *instance;

    cMessage *rtEvent;
    cSocketRTScheduler *rtScheduler;
    char recvBuffer[4000];
    int numRecvBytes;

    std::vector<IKmlFragmentProvider*> providerList;

  protected:
    int findKmlFragmentProvider(IKmlFragmentProvider* p);

  public:
    KmlHttpServer();
    virtual ~KmlHttpServer();

    static KmlHttpServer *getInstance();
    virtual void addKmlFragmentProvider(IKmlFragmentProvider* p);
    virtual void removeKmlFragmentProvider(IKmlFragmentProvider* p);

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void handleSocketEvent();
    virtual std::string processHttpRequest(const char *request);
    virtual std::string getReplyFor(const char *uri);
    virtual std::string addHttpHeader(const char *content, const char *mimetype);
};

#endif


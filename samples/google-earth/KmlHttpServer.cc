//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2010 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include "SocketRTScheduler.h"
#include "KmlHttpServer.h"


Define_Module(KmlHttpServer);

KmlHttpServer *KmlHttpServer::instance = NULL;

KmlHttpServer::KmlHttpServer()
{
    rtEvent = NULL;
    if (instance)
        error("There can be only one KmlHttpServer instance in the network");
    instance = this;
}

KmlHttpServer::~KmlHttpServer()
{
    cancelAndDelete(rtEvent);
    instance = NULL;
}

void KmlHttpServer::initialize()
{
    rtEvent = new cMessage("rtEvent");
    rtScheduler = check_and_cast<cSocketRTScheduler *>(simulation.getScheduler());
    rtScheduler->setInterfaceModule(this, rtEvent, recvBuffer, 4000, &numRecvBytes);
}

KmlHttpServer *KmlHttpServer::getInstance()
{
    if (!instance)
        throw cRuntimeError("KmlHttpServer::getInstance(): there is no KmlHttpServer module in the network");
    return instance;
}

int KmlHttpServer::findKmlFragmentProvider(IKmlFragmentProvider* p)
{
    for (int i=0; i<(int)providerList.size(); i++)
        if (providerList[i] == p)
            return i;
    return -1;
}

void KmlHttpServer::addKmlFragmentProvider(IKmlFragmentProvider* p)
{
    if (findKmlFragmentProvider(p) == -1)
        providerList.push_back(p);
}

void KmlHttpServer::removeKmlFragmentProvider(IKmlFragmentProvider* p)
{
    int k = findKmlFragmentProvider(p);
    if (k != -1)
        providerList.erase(providerList.begin()+k);
}

void KmlHttpServer::handleMessage(cMessage *msg)
{
    if (msg != rtEvent)
        error("This module does not handle messages"); // only those from the SocketRTScheduler

    handleSocketEvent();
}

void KmlHttpServer::handleSocketEvent()
{
    // try to find a double line feed in the input -- that's the end of the HTTP header.
    char *endHeader = NULL;
    for (char *s=recvBuffer; s<=recvBuffer+numRecvBytes-4; s++)
        if (*s=='\r' && *(s+1)=='\n' && *(s+2)=='\r' && *(s+3)=='\n')
            {endHeader = s+4; break;}

    // we don't have a complete header yet -- keep on waiting
    if (!endHeader)
        return;
    std::string header = std::string(recvBuffer, endHeader-recvBuffer);
    //EV << header;

    // remove HTTP header from buffer
    if (endHeader == recvBuffer+numRecvBytes)
        numRecvBytes = 0;
    else {
        int bytesLeft = recvBuffer+numRecvBytes-endHeader;
        memmove(endHeader, recvBuffer, bytesLeft);
        numRecvBytes = bytesLeft;
    }

    // process HTTP request
    std::string reply = processHttpRequest(header.c_str());

    // send back reply
    rtScheduler->sendBytes(reply.c_str(), reply.size());
    rtScheduler->close();
}

std::string KmlHttpServer::processHttpRequest(const char *httpReqHeader)
{
    // parse header. first line should be: GET uri HTTP/1.1
    std::string header(httpReqHeader);
    std::string::size_type pos = header.find("\r\n");
    if (pos==std::string::npos)
    {
        EV << "Bad HTTP request\n";
        return std::string("HTTP/1.0 400 Bad Request\r\n");
    }

    std::string cmd(header,0,pos);
    EV << "Received: " << cmd << "\n";

    // we only accept GET
    if (cmd.length()<4 || cmd.compare(0,4,"GET "))
    {
        EV << "Wrong HTTP verb, only GET is supported\n";
        return std::string("HTTP/1.0 501 Not Implemented\r\n");
    }

    // parse URI and get corresponding content
    pos = cmd.find(" ",4);
    std::string uri(cmd,4,pos-4);

    // create and return reply
    std::string reply = getReplyFor(uri.c_str());
    return reply;
}

const char *INDEX_HTML =
    "<html>\n"
    "<head>\n"
    "   <title>OMNeT++ Google Earth Demo</title>\n"
    "   <script src=\"http://www.google.com/jsapi?key=ABQIAAAAhaIVVNK7shon8s61HoJP3hTWQ61sd-CgFDCq8tRqXSWpKVHs8RQiqzV8RfPWPm7pTTJyU_gk6LVxAg\"></script>\n" // ID registered for omnetpp.org
    "   <script type=\"text/javascript\">\n"
    "      var ge;\n"
    "      var networkLink;  // for setTimeout()\n"
    "\n"
    "      if (typeof google == \"undefined\") {\n"
    "         alert(\"Error: Google API not available, connection to google.com failed?\");\n"
    "         throw \"we're done\";\n"
    "      }\n"
    "\n"
    "      try {\n"
    "         google.load(\"earth\", \"1\");\n"
    "      } catch (e) {\n"
    "         alert(\"Error loading Google Earth: \" + e.message);\n"
    "      }\n"
    "\n"
    "      function init() {\n"
    "         google.earth.createInstance('map3d', initCB, failureCB);\n"
    "      }\n"
    "\n"
    "      function initCB(instance) {\n"
    "         ge = instance;\n"
    "         ge.getWindow().setVisibility(true);\n"
    "         ge.getNavigationControl().setVisibility(true);\n"
    "\n"
    "         // add NetworkLink to snapshot.kml\n"
    "         var link = ge.createLink('');\n"
    "         var href = window.location.href.replace(/[^/]*$/, \"snapshot.kml\");\n"
    "         link.setHref(href);\n"
    "         link.setRefreshMode(ge.REFRESH_ON_INTERVAL);\n"
    "         link.setRefreshInterval(0.1);  // reload every 0.1s, or as frequently as possible\n"
    "         networkLink = ge.createNetworkLink('');\n"
    "         networkLink.set(link, false, true); // sets link, refreshVisibility, and flyToView\n"
    "         ge.getFeatures().appendChild(networkLink);\n"
    "\n"
    "         setTimeout(\"networkLink.setFlyToView(false)\", 2000);\n"
    "      }\n"
    "\n"
    "      function failureCB(errorCode) {\n"
    "         alert(\"Google Earth reported an error: \" + errorCode);\n"
    "      }\n"
    "\n"
    "      google.setOnLoadCallback(init);\n"
    "   </script>\n"
    "\n"
    "</head>\n"
    "<body>\n"
    "   <div id=\"map3d\" style=\"height: 660px; width: 1000px;\"></div>\n"
    "</body>\n"
    "</html>\n";

std::string KmlHttpServer::getReplyFor(const char *uri)
{
    if (strcmp(uri, "/")==0 || strcmp(uri, "/index.html")==0)
    {
        return addHttpHeader(INDEX_HTML, "text/html");
    }
    if (strcmp(uri, "/snapshot.kml")==0)
    {
        std::stringstream out;
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        out << "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n";
        out << "<Document>\n";
        for (int i=0; i<(int)providerList.size(); i++)
            out << providerList[i]->getKmlFragment();
        out << "</Document>\n";
        out << "</kml>\n";

        return addHttpHeader(out.str().c_str(), "application/vnd.google-earth.kml+xml");
    }
    return "HTTP/1.0 404 Not Found\r\n\r\n";
}

std::string KmlHttpServer::addHttpHeader(const char *content, const char *mimetype)
{
    // assemble reply
    std::stringstream out;
    out << "HTTP/1.0 200 OK\r\n";
    out << "Content-Type: " << mimetype << "\r\n";
    //out << "Content-Length: " << strlen(content) << "\r\n";
    out << "\r\n";
    out << content;
    return out.str();
}


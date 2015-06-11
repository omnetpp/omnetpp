//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <fstream>
#include "HttpServer.h"
#include "HttpMsg_m.h"

Define_Module(HTTPServer);

void HTTPServer::initialize()
{
    QueueBase::initialize();
}

simtime_t HTTPServer::startService(cMessage *msg)
{
    EV << "Starting service of " << msg->getName() << endl;
    return par("serviceTime").doubleValue();
}

void HTTPServer::endService(cMessage *msg)
{
    EV << "Completed service of " << msg->getName() << endl;

    HTTPMsg *httpMsg = check_and_cast<HTTPMsg *>(msg);

    std::string reply = processHTTPCommand(httpMsg->getPayload());
    httpMsg->setPayload(reply.c_str());

    int clientAddr = httpMsg->getSrcAddress();
    int srvAddr = httpMsg->getDestAddress();
    httpMsg->setDestAddress(clientAddr);
    httpMsg->setSrcAddress(srvAddr);

    send(msg, "g$o");
}

std::string HTTPServer::processHTTPCommand(const char *httpReqHeader)
{
    // parse header. first line should be: GET uri HTTP/1.1
    std::string header(httpReqHeader);
    std::string::size_type pos = header.find("\r\n");
    if (pos == std::string::npos) {
        EV << "Bad HTTP request\n";
        return std::string("Bad request 400\r\n");
    }

    std::string cmd(header, 0, pos);
    EV << "Received: " << cmd << "\n";

    // we only accept GET
    if (cmd.length() < 4 || cmd.compare(0, 4, "GET ")) {
        EV << "Wrong HTTP verb, only GET is supported\n";
        return std::string("501 Not Implemented\r\n");
    }

    // parse URI and get corresponding content
    pos = cmd.find(" ", 4);
    std::string uri(cmd, 4, pos-4);

    std::string content = getContentFor(uri.c_str());

    // assemble reply
    char len[16];
    sprintf(len, "%d", (int)content.length());
    std::string reply = std::string("HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "Content-Length: ") + len + "\r\n"
                        "\r\n" +
                        content;
    return reply;
}

std::string HTTPServer::getContentFor(const char *uri)
{
    // try to find in cache
    StringMap::iterator it = htdocs.find(uri);
    if (it != htdocs.end())
        return it->second;

    // not in cache -- load and cache it
    std::string fname = std::string("htdocs/")+(strcmp(uri, "/") == 0 ? "index.html" : uri);
#ifdef _MSC_VER
    for (unsigned int i = 0; i < fname.length(); i++)
        if (fname.at(i) == '/')
            fname.at(i) = '\\';

#endif

    std::ifstream file(fname.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
    if (file.fail()) {
        std::string content = "<html><body><h3>404 Not found</h3></body></html>";
        htdocs[uri] = content;
        return content;
    }

    long size = file.tellg();
    file.seekg(0, std::ios::beg);
    char *buffer = new char[size];
    file.read(buffer, size);
    file.close();
    std::string content(buffer, size);
    delete[] buffer;

    EV << "URI=" << uri << " ---> " << content << "\n";
    htdocs[uri] = content;
    return content;
}


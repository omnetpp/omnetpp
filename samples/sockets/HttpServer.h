//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __HTTPSERVER_H
#define __HTTPSERVER_H

#include <map>
#include <string>
#include "QueueBase.h"

/**
 * Simple model of an HTTP server.
 */
class HTTPServer : public QueueBase
{
  private:
    typedef std::map<std::string, std::string> StringMap;
    StringMap htdocs;

  protected:
    virtual void initialize() override;
    virtual simtime_t startService(cMessage *msg) override;
    virtual void endService(cMessage *msg) override;
    std::string processHTTPCommand(const char *req);
    std::string getContentFor(const char *uri);
};

#endif


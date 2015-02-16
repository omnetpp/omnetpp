//==========================================================================
//  MONGOOSEHTTPSERVER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __ENVIR_MONGOOSEHTTPSERVER_H
#define __ENVIR_MONGOOSEHTTPSERVER_H

#include "envirdefs.h"
#include "httpserver.h"
#include "mongoose.h"

NAMESPACE_BEGIN

/**
 * TODO docu
 */
class ENVIR_API cMongooseHttpServer : public cHttpServer
{
  protected:
    struct mg_context *ctx;
    std::vector<cHttpRequestHandler*> handlers;

  protected:
    int findHttpRequestHandler(cHttpRequestHandler *p);
    static void uri_callback(struct mg_connection *conn, const struct mg_request_info *info, void *user_data);

  public:
    cMongooseHttpServer();
    virtual void start(const char *portSpec);
    virtual void stop();
    virtual void addHttpRequestHandler(cHttpRequestHandler *p);
    virtual void removeHttpRequestHandler(cHttpRequestHandler *p);
    virtual bool handleOneRequest(bool blocking);
};

/**
 * TODO docu
 */
class ENVIR_API cMongooseHttpRequest : public cHttpRequest
{
  protected:
    struct mg_connection *conn;
    const struct mg_request_info *req;

  public:
    cMongooseHttpRequest(struct mg_connection *conn, const struct mg_request_info *req);
    virtual const char *getRequestMethod() const {return req->request_method;}
    virtual const char *getUri() const {return req->uri;}
    virtual const char *getQueryString() const {return req->query_string;}
    virtual const char *getPostDataBuffer() const {return req->post_data;}
    virtual int getPostDataLength() const {return req->post_data_len;}
    virtual int getNumHeaders() const {return req->num_headers;}
    virtual const char *getHeaderName(int k) const {return req->http_headers[k].name;}
    virtual const char *getHeaderValue(int k) const {return req->http_headers[k].value;}

    virtual int print(const char *s);
    virtual int printf(const char *format, ...);
    virtual int write(void *buffer, size_t bytes);
};

NAMESPACE_END

#endif

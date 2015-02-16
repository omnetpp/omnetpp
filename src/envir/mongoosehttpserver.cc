//==========================================================================
//  MONGOOSEHTTPSERVER.CC - part of
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

#include <ctime>
#include "mongoosehttpserver.h"
#include "cexception.h"

NAMESPACE_BEGIN

cMongooseHttpServer::cMongooseHttpServer()
{
    ctx = NULL;
}

void cMongooseHttpServer::start(const char *portSpec)
{
    if ((ctx = mg_start()) == NULL)
        throw cRuntimeError("Cannot initialize embedded HTTP server");

    if (!mg_set_option(ctx, "ports", portSpec))
        throw cRuntimeError("Embedded HTTP server could not bind to port %s", portSpec);

    mg_set_uri_callback(ctx, "*", uri_callback, this);

    // turn off worker threads; instead we'll poll the dispatch queue and serve
    // requests from the main thread. The reason is that the simulation library
    // is single-threaded so we'd have to protect it with a mutex anyway; and
    // certain operations like "process one event" or "run" must be done in the
    // main thread anyway.
    mg_set_option(ctx, "max_threads", "0");

    ::printf("Embedded web server started on port %d\n", mg_get_last_port()); // note: mg_get_option(ctx, "ports") would return "80+" even if mongoose actually listens on, say, port 83
    ::fflush(stdout);
}

void cMongooseHttpServer::stop()
{
    if (ctx)
    {
        mg_stop(ctx);
        ctx = NULL;
    }
}

void cMongooseHttpServer::addHttpRequestHandler(cHttpRequestHandler *p)
{
    if (findHttpRequestHandler(p) == -1)
        handlers.push_back(p);
}

void cMongooseHttpServer::removeHttpRequestHandler(cHttpRequestHandler *p)
{
    int k = findHttpRequestHandler(p);
    if (k != -1)
        handlers.erase(handlers.begin()+k);
}

bool cMongooseHttpServer::handleOneRequest(bool blocking)
{
    return mg_process_socket(ctx, blocking);
}

int cMongooseHttpServer::findHttpRequestHandler(cHttpRequestHandler *p)
{
    for (int i=0; i<(int)handlers.size(); i++)
        if (handlers[i] == p)
            return i;
    return -1;
}

void cMongooseHttpServer::uri_callback(struct mg_connection *conn, const struct mg_request_info *info, void *user_data)
{
    cMongooseHttpRequest req(conn, info);
    cMongooseHttpServer *self = (cMongooseHttpServer *)user_data;

    bool handled = false;
    for (int i=0; i<(int)self->handlers.size(); i++)
        if ((handled = self->handlers[i]->handleHttpRequest(&req)))
            break;

    if (!handled)
    {
        char body[128];
        sprintf(body, "No handler registered for %.100s", info->uri);
        mg_printf(conn,
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%s", strlen(body), body);
    }
}

//----

cMongooseHttpRequest::cMongooseHttpRequest(struct mg_connection *conn, const struct mg_request_info *req)
{
    this->conn = conn;
    this->req = req;
}

int cMongooseHttpRequest::print(const char *s)
{
    int len = strlen(s);
    if (len >= 16384)
        ::printf("[http] producing large response: writing %dKb\n", len/1024);
    return mg_write(conn, s, len);
}

int cMongooseHttpRequest::printf(const char *fmt, ...)
{
    char buf[MAX_REQUEST_SIZE];

    va_list ap;
    va_start(ap, fmt);
    int len = mg_vsnprintf(conn, buf, sizeof(buf), fmt, ap);
    va_end(ap);

    if (len >= 16384)
        ::printf("[http] large response: writing %dK into it\n", len/1024);
    return mg_write(conn, buf, len);
}

int cMongooseHttpRequest::write(void *buffer, size_t bytes)
{
    return mg_write(conn, buffer, bytes);
}

NAMESPACE_END

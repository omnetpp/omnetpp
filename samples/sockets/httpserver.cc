//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include "httpserver.h"
#include "httpmsg_m.h"


Define_Module( HTTPServer );

simtime_t HTTPServer::startService(cMessage *msg)
{
    ev << "Starting service of " << msg->name() << endl;
    return par("serviceTime");
}

void HTTPServer::endService(cMessage *msg)
{
    ev << "Completed service of " << msg->name() << endl;

    HTTPMsg *httpMsg = check_and_cast<HTTPMsg *>(msg);

    std::string reply = processHTTPCommand(httpMsg->getPayload());
    httpMsg->setPayload(reply.c_str());

    int clientAddr = httpMsg->getSrcAddress();
    int srvAddr = httpMsg->getDestAddress();
    httpMsg->setDestAddress(clientAddr);
    httpMsg->setSrcAddress(srvAddr);

    send(msg, "out");
}


std::string HTTPServer::processHTTPCommand(const char *httpReqHeader)
{
    // parse header. first line should be: GET uri HTTP/1.1
    std::string header(httpReqHeader);
    std::string::size_type pos = header.find("\r\n");
    if (pos==std::string::npos)
    {
        ev << "Bad HTTP request\n";
        return std::string("Bad request 400\r\n");
    }

    std::string cmd(header,0,pos);
    ev << "Received: " << cmd << "\n";

    // we only accept GET
    if (cmd.length()<4 || cmd.compare(0,4,"GET "))
    {
        ev << "Wrong HTTP verb, only GET is supported\n";
        return std::string("501 Not Implemented\r\n");
    }

    // parse URI and get corresponding content
    pos = cmd.find(" ",4);
    std::string uri(cmd,4,pos);

    std::string content = getContentFor(uri.c_str());

    // assemble reply
    char len[16];
    sprintf(len, "%d", content.length());
    std::string reply = std::string("HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "Content-Length: ") + len + "\r\n"
                        "\r\n" +
                        content;
    return reply;
}

std::string HTTPServer::getContentFor(const char *uri)
{
    std::string content = std::string("<html><body>"
                          "<h3>This is page ") + uri + "</h3>"
                          "<a href='foo'>foo</a> "
                          "<a href='bar'>bar</a> "
                          "<a href='foo'>baz</a> "
                          "</body></html>\r\n";
    return content;
}

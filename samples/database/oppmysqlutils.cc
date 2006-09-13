//==========================================================================
//  OPPMYSQLUTILS.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <assert.h>
#include <omnetpp.h>
#include "oppmysqlutils.h"


void opp_mysql_connectToDB(MYSQL *mysql, cConfiguration *cfg, const char *prefix)
{
    // read config file
    if (!prefix) prefix = "";
    std::string pfx = prefix;
    if (prefix[0] && prefix[strlen(prefix)-1]!='-') pfx += "-";
    const char *host = cfg->getAsString("General", (pfx+"mysql-host").c_str(), NULL);
    const char *user = cfg->getAsString("General", (pfx+"mysql-user").c_str(), NULL);
    const char *passwd = cfg->getAsString("General", (pfx+"mysql-password").c_str(), NULL);
    const char *db = cfg->getAsString("General", (pfx+"mysql-database").c_str(), NULL);
    unsigned int port = (unsigned int) cfg->getAsInt("General", (pfx+"mysql-port").c_str(), 0);
    const char *socket = cfg->getAsString("General", (pfx+"mysql-socket").c_str(), NULL);
    unsigned int clientflag = (unsigned int) cfg->getAsInt("General", (pfx+"mysql-clientflag").c_str(), 0);
    bool usepipe = cfg->getAsBool("General", (pfx+"mysql-use-pipe").c_str(), false);
    std::string optionsfile = cfg->getAsFilename("General", (pfx+"mysql-options-file").c_str(), NULL);

    // set options, then connect
    if (!optionsfile.empty())
        mysql_options(mysql, MYSQL_READ_DEFAULT_FILE, optionsfile.c_str());
    if (usepipe)
        mysql_options(mysql, MYSQL_OPT_NAMED_PIPE, 0);
    if (!mysql_real_connect(mysql, host, user, passwd, db, port, socket, clientflag))
        throw new cRuntimeError("MySQL error: Failed to connect to database: %s", mysql_error(mysql));
}

void opp_mysql_bindSTRING(MYSQL_BIND& bind, const char *buf, size_t buflen, unsigned long& len)
{
    memset(&bind, 0, sizeof(MYSQL_BIND));
    bind.buffer_type = MYSQL_TYPE_STRING;
    bind.buffer = (char *)buf;
    bind.buffer_length = buflen;
    bind.is_null = 0;
    bind.length = &len;
    ASSERT(buflen != sizeof(char *));  // catch trivial usage error: sizeof(bufferPtr)
}

void opp_mysql_bindLONG(MYSQL_BIND& bind, int& d)
{
    memset(&bind, 0, sizeof(MYSQL_BIND));
    bind.buffer_type = MYSQL_TYPE_LONG;
    bind.buffer = (void *)&d;
}

void opp_mysql_bindDOUBLE(MYSQL_BIND& bind, double& d)
{
    memset(&bind, 0, sizeof(MYSQL_BIND));
    bind.buffer_type = MYSQL_TYPE_DOUBLE;
    bind.buffer = (void *)&d;
}

void opp_mysql_assignSTRING(MYSQL_BIND& bind, char *buf, const char *s)
{
    ASSERT(buf==bind.buffer);

    if (!s) s = "";
    size_t len = strlen(s);
    if (len > bind.buffer_length) len = bind.buffer_length;
    strncpy(buf, s, len);
    (*bind.length) = len;
}

int opp_mysql_substitute(std::string& query, const char *substring, const char *value, MYSQL *mysql)
{
    if (!substring || !substring[0])
        throw new cRuntimeError("opp_mysql_substitute(): substring cannot be NULL or ''");
    if (!value)
        value = "";

    char *quotedvalue = new char[2*strlen(substring)+1];
    mysql_real_escape_string(mysql, quotedvalue, value, strlen(value));

    int count = 0;
    int pos = 0;
    while ((pos = query.find(substring,pos))!=-1)
    {
        query.replace(pos, strlen(substring), quotedvalue);
        pos += strlen(quotedvalue);
        count++;
    }
    delete [] quotedvalue;

    if (count==0)
        throw new cRuntimeError("opp_mysql_substitute(): substring '%s' not found in '%s'", substring, query.c_str());

    return count;
}

int opp_mysql_substitute(std::string& query, const char *substring, long value, MYSQL *mysql)
{
    char buf[32];
    sprintf(buf, "%ld", value);
    return opp_mysql_substitute(query, substring, buf, mysql);
}


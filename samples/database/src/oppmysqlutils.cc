//==========================================================================
//  OPPMYSQLUTILS.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2011 Zoltan Bojthe

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <assert.h>

#include "oppmysqlutils.h"

Register_PerObjectConfigOption(CFGID_MYSQL_HOST,       "mysql-host",         KIND_OTHER, CFG_STRING,   "\"\"",  "Hostname of mysql server");
Register_PerObjectConfigOption(CFGID_MYSQL_USER,       "mysql-user",         KIND_OTHER, CFG_STRING,   "\"\"",  "User name for mysql server");
Register_PerObjectConfigOption(CFGID_MYSQL_PASSWD,     "mysql-password",     KIND_OTHER, CFG_STRING,   "\"\"",  "Password for mysql server");
Register_PerObjectConfigOption(CFGID_MYSQL_DB,         "mysql-database",     KIND_OTHER, CFG_STRING,   "\"\"",  "Database name");
Register_PerObjectConfigOption(CFGID_MYSQL_PORT,       "mysql-port",         KIND_OTHER, CFG_INT,      "0",     "Port of mysql server");
Register_PerObjectConfigOption(CFGID_MYSQL_SOCKET,     "mysql-socket",       KIND_OTHER, CFG_STRING,   "\"\"",  "Socket of mysql server");
Register_PerObjectConfigOption(CFGID_MYSQL_CLIENTFLAG, "mysql-clientflag",   KIND_OTHER, CFG_INT,      "0",     "??? for mysql server");
Register_PerObjectConfigOption(CFGID_MYSQL_USE_PIPE,   "mysql-use-pipe",     KIND_OTHER, CFG_BOOL,     "0",     "??? for mysql server");
Register_PerObjectConfigOption(CFGID_MYSQL_OPT_FILE,   "mysql-options-file", KIND_OTHER, CFG_FILENAME, "",      "Options file for mysql server");


void opp_mysql_connectToDB(MYSQL *mysql, cConfiguration *cfg, const char *cfgobj)
{
    // read config file
    if (!cfgobj || !*cfgobj)
        cfgobj = "mysql";
    std::string host = cfg->getAsString(cfgobj, CFGID_MYSQL_HOST, nullptr);
    std::string user = cfg->getAsString(cfgobj, CFGID_MYSQL_USER, nullptr);
    std::string passwd = cfg->getAsString(cfgobj, CFGID_MYSQL_PASSWD, nullptr);
    std::string db = cfg->getAsString(cfgobj, CFGID_MYSQL_DB, nullptr);
    unsigned int port = (unsigned int)cfg->getAsInt(cfgobj, CFGID_MYSQL_PORT, 0);
    std::string socket = cfg->getAsString(cfgobj, CFGID_MYSQL_SOCKET, nullptr);
    unsigned int clientflag = (unsigned int)cfg->getAsInt(cfgobj, CFGID_MYSQL_CLIENTFLAG, 0);
    bool usepipe = cfg->getAsBool(cfgobj, CFGID_MYSQL_USE_PIPE, false);
    std::string optionsfile = cfg->getAsFilename(cfgobj, CFGID_MYSQL_OPT_FILE);

    // set options, then connect
    if (!optionsfile.empty())
        mysql_options(mysql, MYSQL_READ_DEFAULT_FILE, optionsfile.c_str());
    if (usepipe)
        mysql_options(mysql, MYSQL_OPT_NAMED_PIPE, 0);
    if (!mysql_real_connect(mysql, host.empty() ? nullptr : host.c_str(),
                user.c_str(), passwd.c_str(), db.c_str(), port,
                socket.empty() ? nullptr : socket.c_str(), clientflag))
        throw cRuntimeError("MySQL error: Failed to connect to database: %s", mysql_error(mysql));
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
    ASSERT(buf == bind.buffer);

    if (!s)
        s = "";
    size_t len = strlen(s);
    if (len > bind.buffer_length)
        len = bind.buffer_length;
    strncpy(buf, s, len);
    (*bind.length) = len;
}

int opp_mysql_substitute(std::string& query, const char *substring, const char *value, MYSQL *mysql)
{
    if (!substring || !substring[0])
        throw cRuntimeError("opp_mysql_substitute(): substring cannot be nullptr or ''");
    if (!value)
        value = "";

    char *quotedvalue = new char[2*strlen(value)+1];
    mysql_real_escape_string(mysql, quotedvalue, value, strlen(value));

    int count = 0;
    int pos = 0;
    while ((pos = query.find(substring, pos)) != -1) {
        query.replace(pos, strlen(substring), quotedvalue);
        pos += strlen(quotedvalue);
        count++;
    }
    delete[] quotedvalue;

    if (count == 0)
        throw cRuntimeError("opp_mysql_substitute(): substring '%s' not found in '%s'", substring, query.c_str());

    return count;
}

int opp_mysql_substitute(std::string& query, const char *substring, long value, MYSQL *mysql)
{
    char buf[32];
    sprintf(buf, "%ld", value);
    return opp_mysql_substitute(query, substring, buf, mysql);
}

std::string opp_mysql_escape(MYSQL *mysql, const char *str)
{
    size_t len = strlen(str);
    char *buf = new char[2*len+1];
    buf[0] = '\0';
    mysql_real_escape_string(mysql, buf, str, len);
    std::string ret = std::string(buf);
    delete[] buf;
    return ret;
}


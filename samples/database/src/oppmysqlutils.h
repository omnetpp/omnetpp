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

#ifndef __OPPMYSQLUTILS_H__
#define __OPPMYSQLUTILS_H__

#include <mysql.h>

#include <omnetpp.h>

using namespace omnetpp;

/**
 * The following options are read from the configuration (omnetpp.ini):
 *
 * <pre>
 * [General]
 * mysql.mysql-host = <hostname>
 * mysql.mysql-user = <username>
 * mysql.mysql-password = <password>
 * mysql.mysql-database = <database-name>
 * mysql.mysql-port = <TCP-port-number>
 * mysql.mysql-socket = <unix-socket-name>
 * mysql.mysql-clientflag = <int>
 * mysql.mysql-use-pipe = <true/false>  # Windows named pipes
 * mysql.mysql-options-file = <MySQL-options-filename>
 * </pre>
 *
 * Please refer to the MySQL documentation about their meanings.
 *
 * If a cfgobj is given (not nullptr or the empty string), then the above
 * config entries get from cfgobj instead 'mysql'. That is, with cfgobj="homepc",
 * the configuration is searched for the following entries:
 *
 * <pre>
 * [General]
 * homepc.mysql-host = <hostname>
 * homepc.mysql-user = <username>
 * homepc.mysql-password = <password>
 * ...
 * </pre>
 */
void opp_mysql_connectToDB(MYSQL *mysql, cConfiguration *cfg, const char *cfgobj);

/**
 * Fills in a MYSQL_BIND structure with the given data.
 */
void opp_mysql_bindSTRING(MYSQL_BIND& bind, const char *buf, size_t buflen, unsigned long& len);

/**
 * Fills in a MYSQL_BIND structure with the given data.
 */
void opp_mysql_bindLONG(MYSQL_BIND& bind, int& d);

/**
 * Fills in a MYSQL_BIND structure with the given data.
 */
void opp_mysql_bindDOUBLE(MYSQL_BIND& bind, double& d);

/**
 * Assigns the string in s to the given MYSQL_BIND structure. The buf pointer
 * should point to the bound buffer, and it only needs to be passed to
 * catch user errors (the function verifies that it's the same as the one inside
 * the MYSQL_BIND structure.)
 */
void opp_mysql_assignSTRING(MYSQL_BIND& bind, char *buf, const char *s);

/**
 * Substitutes all occurrences of substring with value in query, and returns
 * the number of replacements. The mysql arg should hold a valid database
 * connection -- it is needed because the escaping depends on the character
 * set in use by the server.
 *
 * Example:
 * <pre>
 * std::string query = "SELECT name, age FROM emp WHERE name=\"@empname@\"";
 * opp_mysql_substitute(query, "@empname@", "John Doe", mysql);
 * </pre>
 */
int opp_mysql_substitute(std::string& query, const char *substring, const char *value, MYSQL *mysql);

/**
 * Like the other opp_mysql_substitute(), but this one converts the long to string first.
 */
int opp_mysql_substitute(std::string& query, const char *substring, long value, MYSQL *mysql);

/**
 * This function escaping the input string str to a legal SQL string that you can use in an SQL statement.
 * See also mysql_real_escape_string() in MySQL documentation.
 */
std::string opp_mysql_escape(MYSQL *mysql, const char *str);

#endif


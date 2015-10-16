//==========================================================================
//  CMYSQLNETBUILDER.CC - part of
//
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

#include <string.h>

#include <mysql.h>

#include <omnetpp.h>

using namespace omnetpp;

#include "oppmysqlutils.h"

/**
 * Builds a network dynamically, with the topology coming from a
 * MySQL database.
 *
 * <pre>
 * CREATE TABLE network (
 *      id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
 *      name VARCHAR(80) NOT NULL
 * );

 * CREATE TABLE node (
 *      id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
 *      networkid INT NOT NULL,
 *      type VARCHAR(80) NOT NULL,
 *      name VARCHAR(80) NOT NULL,
 *      display VARCHAR(200),
 *      FOREIGN KEY (networkid) REFERENCES network(id)
 * );

 * CREATE TABLE nodeparameter (
 *      nodeid INT NOT NULL,
 *      name VARCHAR(200) NOT NULL,
 *      value VARCHAR(200) NOT NULL,
 *      FOREIGN KEY (nodeid) REFERENCES node(id)
 * );
 *
 * CREATE TABLE link (
 *      id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
 *      srcnodeid INT NOT NULL,
 *      srcgatevector VARCHAR(80) NOT NULL,
 *      destnodeid INT NOT NULL,
 *      destgatevector VARCHAR(80) NOT NULL,
 *      channeltype VARCHAR(80) NOT NULL,
 *      FOREIGN KEY (srcnodeid) REFERENCES node(id),
 *      FOREIGN KEY (destnodeid) REFERENCES node(id)
 * );
 *
 * CREATE TABLE linkparameter (
 *      linkid INT  NOT NULL,
 *      name VARCHAR(200) NOT NULL,
 *      value VARCHAR(200) NOT NULL,
 *      FOREIGN KEY (linkid) REFERENCES link(id)
 * );
 * </pre>
 *
 * For database configuration parameters, please refer to the documentation
 * of opp_mysql_connectToDB(). mysqlConnectionName can be given in a module parameter.
 */
class cMySQLNetBuilder : public cSimpleModule
{
  public:
    cMySQLNetBuilder() : cSimpleModule() {};

  protected:
    void readAndBuild(const char *networkName);
    cChannel *createLink(const char *channelTypeName, int linkId, cGate *srcGate, cGate *destGate);

    virtual void initialize();
    virtual int numInitStages() const { return 2; }
    virtual void handleMessage(cMessage *msg);
};

Define_Module(cMySQLNetBuilder);

void cMySQLNetBuilder::initialize()
{
#if 0
    cMessage *timer = new cMessage("cMySQLNetBuilder build");
    timer->setSchedulingPriority(-32768);
    scheduleAt(SIMTIME_ZERO, timer);
#else
    readAndBuild(par("networkName").stringValue());
#endif
}

void cMySQLNetBuilder::readAndBuild(const char *networkName)
{
    bool systemModuleInitialized = getSystemModule()->initialized();

    // connect
    EV << getClassName() << " connecting to MySQL database...";
    const char *cfgobj = par("mysqlConnectionName").stringValue();
    if (*cfgobj == '\0')
        cfgobj = "mysql";
    MYSQL *mysql = mysql_init(nullptr);
    opp_mysql_connectToDB(mysql, getEnvir()->getConfig(), cfgobj);
    EV << " OK\n";

    // do the job, and close the database
    const char *modName = par("targetModule");
    cModule *parentmod = (!modName || !modName[0]) ? getParentModule() : getModuleByPath(modName);
    // read and create nodes
    std::string stmt = "SELECT n.id, n.name, n.type, n.display FROM node n, network t "
                       "WHERE n.networkid=t.id AND t.name='@networkname@' "
                       "ORDER BY n.id";
    opp_mysql_substitute(stmt, "@networkname@", networkName, mysql);
    if (mysql_query(mysql, stmt.c_str()))
        throw cRuntimeError("MySQL error: SELECT failed: %s", mysql_error(mysql));

    typedef std::map<long, cModule *> NodeId2ModuleMap;
    NodeId2ModuleMap nodeId2Mod;
    typedef std::map<long, std::string> NodeId2StringMap;
    NodeId2StringMap nodeId2DispStr;

    MYSQL_RES *res = mysql_store_result(mysql);
    if (res == nullptr)
        throw cRuntimeError("MySQL error: mysql_store_result() failed: %s", mysql_error(mysql));
    if (mysql_num_rows(res) == 0)
        throw cRuntimeError("networkName='%s': no such network in the database, or it has no nodes", networkName);
    ASSERT(mysql_num_fields(res) == 4);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != nullptr) {
        // get fields from row
        long nodeid = atol(row[0]);
        const char *name = row[1];
        const char *modtypename = row[2];
        const char *dispstr = row[3];
        EV << "NODE id=" << nodeid << " name=" << name << " type=" << modtypename << "\n";

        // create module
        cModuleType *modtype = cModuleType::get(modtypename);
        cModule *mod = modtype->create(name, parentmod);
        nodeId2Mod[nodeid] = mod;

        // set display string later
        nodeId2DispStr[nodeid] = dispstr;
    }
    mysql_free_result(res);

    // set node parameters
    stmt = "SELECT p.nodeid, p.name, p.value FROM nodeparameter p, node n, network t "
           "WHERE p.nodeid=n.id and n.networkid=t.id AND t.name='@networkname@' "
           "ORDER BY p.nodeid";
    opp_mysql_substitute(stmt, "@networkname@", networkName, mysql);
    if (mysql_query(mysql, stmt.c_str()))
        throw cRuntimeError("MySQL error: SELECT failed: %s", mysql_error(mysql));
    res = mysql_store_result(mysql);
    if (res == nullptr)
        throw cRuntimeError("MySQL error: mysql_store_result() failed: %s", mysql_error(mysql));
    ASSERT(mysql_num_fields(res) == 3);
    while ((row = mysql_fetch_row(res)) != nullptr) {
        // get fields from row
        long nodeid = atol(row[0]);
        const char *parname = row[1];
        const char *value = row[2];

        // look up module and set its parameter
        NodeId2ModuleMap::iterator it = nodeId2Mod.find(nodeid);
        // WHERE clause should guarantee this
        ASSERT(it != nodeId2Mod.end());
        cPar& par = it->second->par(parname);  // will throw an error if no such param
        par.parse(value);
    }

    // read and create connections
    typedef std::pair<cChannel *, cChannel *> ChannelPair;
    typedef std::map<long, ChannelPair> LinkId2ChannelPair;
    LinkId2ChannelPair linkId2ChannelPair;
    stmt = "SELECT l.id, srcnodeid, srcgatevector, destnodeid, destgatevector, channeltype "
           "FROM link l, node sn, node dn, network t "
           "WHERE (l.srcnodeid=sn.id and sn.networkid=t.id)"
           "  AND (l.srcnodeid=dn.id and dn.networkid=t.id)"
           "  AND t.name='@networkname@' ";
    opp_mysql_substitute(stmt, "@networkname@", networkName, mysql);
    if (mysql_query(mysql, stmt.c_str()))
        throw cRuntimeError("MySQL error: SELECT failed: %s", mysql_error(mysql));
    res = mysql_store_result(mysql);
    if (res == nullptr)
        throw cRuntimeError("MySQL error: mysql_store_result() failed: %s", mysql_error(mysql));
    ASSERT(mysql_num_fields(res) == 6);
    while ((row = mysql_fetch_row(res)) != nullptr) {
        // get fields from row
        long linkId = atol(row[0]);
        long srcnodeid = atol(row[1]);
        const char *srcgatevector = row[2];
        long destnodeid = atol(row[3]);
        const char *destgatevector = row[4];
        const char *channeltype = row[5];

        NodeId2ModuleMap::iterator srcIt = nodeId2Mod.find(srcnodeid);
        NodeId2ModuleMap::iterator destIt = nodeId2Mod.find(destnodeid);
        // the SQL query's WHERE clause guarantees the following
        ASSERT(srcIt != nodeId2Mod.end());
        ASSERT(destIt != nodeId2Mod.end());
        cModule *srcmod = srcIt->second;
        cModule *destmod = destIt->second;

        if (!srcmod->isGateVector(srcgatevector) || !destmod->isGateVector(destgatevector))
            throw cRuntimeError("Gates must be vectors");

        cGate::Type srcGateType = srcmod->gateType(srcgatevector);
        cGate::Type destGateType = destmod->gateType(destgatevector);
        if (srcGateType == cGate::INOUT && destGateType == cGate::INOUT) {
            // bidirectional
            cGate *srcIn, *srcOut, *destIn, *destOut;
            srcmod->getOrCreateFirstUnconnectedGatePair(srcgatevector, false, true, srcIn, srcOut);
            destmod->getOrCreateFirstUnconnectedGatePair(destgatevector, false, true, destIn, destOut);
            linkId2ChannelPair[linkId] = ChannelPair(
                        createLink(channeltype, linkId, srcOut, destIn),
                        createLink(channeltype, linkId, destOut, srcIn));
        }
        else if (srcGateType == cGate::OUTPUT && destGateType == cGate::INPUT) {
            // simple
            cGate *srcOut = srcmod->getOrCreateFirstUnconnectedGate(srcgatevector, 0, false, true);
            cGate *destIn = destmod->getOrCreateFirstUnconnectedGate(destgatevector, 0, false, true);
            linkId2ChannelPair[linkId] = ChannelPair(
                        createLink(channeltype, linkId, srcOut, destIn),
                        nullptr);
        }
        else
            throw cRuntimeError("Invalid gate type combination (%c --> %c) at linkId=%ld",
                    (char)srcGateType, (char)destGateType, linkId);
    }
    mysql_free_result(res);

    // set parameters of connections
    stmt = "SELECT p.linkid, p.name, p.value FROM linkparameter p, link l, node n, network t "
           "WHERE p.linkid=l.id and l.srcnodeid=n.id and n.networkid=t.id AND t.name=\"@networkname@\" ";
    opp_mysql_substitute(stmt, "@networkname@", networkName, mysql);
    if (mysql_query(mysql, stmt.c_str()))
        throw cRuntimeError("MySQL error: SELECT failed: %s", mysql_error(mysql));
    res = mysql_store_result(mysql);
    if (res == nullptr)
        throw cRuntimeError("MySQL error: mysql_store_result() failed: %s", mysql_error(mysql));
    ASSERT(mysql_num_fields(res) == 3);
    while ((row = mysql_fetch_row(res)) != nullptr) {
        // get fields from row
        long linkid = atol(row[0]);
        const char *parname = row[1];
        const char *value = row[2];

        // WHERE clause should guarantee this
        LinkId2ChannelPair::iterator it = linkId2ChannelPair.find(linkid);
        ASSERT(it != linkId2ChannelPair.end());

        // look up module and set its parameter
        if (it->second.first)
            it->second.first->par(parname).parse(value);
        if (it->second.second)
            it->second.second->par(parname).parse(value);
    }
    mysql_free_result(res);
    mysql_close(mysql);

    // initialize channels when need.
    if (!systemModuleInitialized) {
        // We are in the stage 0, the channel->callInitialize(0) already called on all channels generated by NED.
        // We must call the channel->callInitialize(0) for our dynamically generated channels.
        // The cMysqlNetBuilder has init stage 1, it's guarantee the calling of channel->callInitialize(1) for all channels.
        for (LinkId2ChannelPair::iterator it = linkId2ChannelPair.begin(); it != linkId2ChannelPair.end(); ++it) {
            if (it->second.first)
                it->second.first->callInitialize(0);
            if (it->second.second)
                it->second.second->callInitialize(0);
        }
    }

    // final touches: buildinside, initialize()
    std::map<long, cModule *>::iterator it;
    for (it = nodeId2Mod.begin(); it != nodeId2Mod.end(); ++it) {
        cModule *mod = it->second;
        long nodeId = it->first;
        mod->finalizeParameters();
        mod->setDisplayString(nodeId2DispStr[nodeId].c_str());
        mod->buildInside();
    }

    for (it = nodeId2Mod.begin(); it != nodeId2Mod.end(); ++it) {
        cModule *mod = it->second;
        if (systemModuleInitialized)
            mod->callInitialize();
        else {
            // we are in system initialize stage 0
#if 0
            // TODO call callInitialize(0) when need
            if (mod->getParentModule()->stage0Initialized()) {  // TODO mod->stage0Initialized() not existing, need implementing it
                // parent module already called callInitialize(0) on its other static submodules, we should call it on dynamic submodules
                if (mod->callInitialize(0)) {
                    // mod has more stages, reset FL_INITIALIZED on parents of mod
                    for ( ; ; ) {
                        mod = mod->getParentModule();
                        if (!mod)
                            break;
                        mod->setFlag(FL_INITIALIZED, false);  // TODO setFlag() is protected, FL_INITIALIZED is private
                    }
                }
            }
#endif
        }
    }
}

cChannel *cMySQLNetBuilder::createLink(const char *channelTypeName, int linkId, cGate *srcGate, cGate *destGate)
{
    cChannelType *channelType = channelTypeName ? cChannelType::get(channelTypeName) : nullptr;
    cChannel *channel = channelType ? channelType->create("channel") : nullptr;

    // create connection
    srcGate->connectTo(destGate, channel);
    return channel;
}

void cMySQLNetBuilder::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        delete msg;
        readAndBuild(par("networkName").stringValue());
    }
    else
        error("This modules does not process messages.");
}


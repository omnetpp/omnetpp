//==========================================================================
//  OMNETAPP.CC - part of
//                             OMNeT++
//             Discrete System Simulation in C++
//
//  Implementation of
//    TOmnetApp
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <fstream>

#include "args.h"
#include "omnetapp.h"
#include "patmatch.h"
#include "fsutils.h"

#include "ctypes.h"
#include "ccoroutine.h"
#include "csimul.h"
#include "cscheduler.h"
#include "cpar.h"
#include "random.h"
#include "cmodule.h"
#include "cxmlelement.h"
#include "cxmldoccache.h"

#ifdef WITH_PARSIM
#include "cparsimcomm.h"
#include "parsim/cparsimpartition.h"
#include "parsim/cparsimsynchr.h"
#include "parsim/creceivedexception.h"
#endif


#ifdef USE_PORTABLE_COROUTINES /* coroutine stacks reside in main stack area */

# define TOTAL_STACK_KB     2048
# define MAIN_STACK_KB       128  // for MSVC+Tkenv, 64K is not enough

#else /* nonportable coroutines, stacks are allocated on heap */

# define TOTAL_STACK_KB        0  // dummy value
# define MAIN_STACK_KB         0  // dummy value

#endif


using std::ostream;


// This variable could really be a local var inside the functions where it is
// used; it was only made a static to reduce per-module stack size with activity().
static char buffer[1024];


#define CREATE_BY_CLASSNAME(var,classname,baseclass,description) \
     baseclass *var ## _tmp = (baseclass *) createOne(classname); \
     var = dynamic_cast<baseclass *>(var ## _tmp); \
     if (!var) \
         throw new cException("Class \"%s\" is not subclassed from " #baseclass, (const char *)classname);


//-------------------------------------------------------------

TOmnetApp::TOmnetApp(ArgList *arglist, cConfiguration *conf)
{
    args = arglist;
    config = conf;
    xmlcache = NULL;
    opt_genk_randomseed = new long[NUM_RANDOM_GENERATORS];
    next_startingseed = 0;

    outvectmgr = NULL;
    outscalarmgr = NULL;
    snapshotmgr = NULL;

    scheduler = NULL;
#ifdef WITH_PARSIM
    parsimcomm = NULL;
    parsimpartition = NULL;
    parsimsynchronizer = NULL;
#endif

    initialized = false;
}

TOmnetApp::~TOmnetApp()
{
    delete [] opt_genk_randomseed;
    delete args;
    delete config;
    delete xmlcache;

    delete outvectmgr;
    delete outscalarmgr;
    delete snapshotmgr;

    delete scheduler;
#ifdef WITH_PARSIM
    delete parsimcomm;
    delete parsimpartition;
    delete parsimsynchronizer;
#endif
}

void TOmnetApp::setup()
{
     try
     {
         // set opt_* variables from ini file(s)
         readOptions();

         // initialize coroutine library
         if (opt_total_stack_kb<=MAIN_STACK_KB)
         {
            ev.printf("Total stack size %dK increased to %dK\n", opt_total_stack_kb, MAIN_STACK_KB+4);
            opt_total_stack_kb = MAIN_STACK_KB+4;
         }
         cCoroutine::init(1024*opt_total_stack_kb, 1024*MAIN_STACK_KB);

         // install XML document cache
         xmlcache = new cXMLDocCache();

         // install output vector manager
         CREATE_BY_CLASSNAME(outvectmgr, opt_outputvectormanager_class.c_str(), cOutputVectorManager, "output vector manager");

         // install output scalar manager
         CREATE_BY_CLASSNAME(outscalarmgr, opt_outputscalarmanager_class.c_str(), cOutputScalarManager, "output scalar manager");

         // install snapshot manager
         CREATE_BY_CLASSNAME(snapshotmgr, opt_snapshotmanager_class.c_str(), cSnapshotManager, "snapshot manager");

         // set up for distributed execution
         if (!opt_parsim)
         {
             CREATE_BY_CLASSNAME(scheduler, opt_scheduler_class.c_str(), cScheduler, "event scheduler");
             scheduler->setSimulation(&simulation);
             simulation.setScheduler(scheduler);
         }
         else
         {
#ifdef WITH_PARSIM
             // create components
             CREATE_BY_CLASSNAME(parsimcomm, opt_parsimcomm_class.c_str(), cParsimCommunications, "parallel simulation communications layer");
             parsimpartition = new cParsimPartition();
             CREATE_BY_CLASSNAME(parsimsynchronizer, opt_parsimsynch_class.c_str(), cParsimSynchronizer, "parallel simulation synchronization layer");

             // wire them together (note: 'parsimsynchronizer' is also the scheduler for 'simulation')
             parsimpartition->setContext(&simulation,parsimcomm,parsimsynchronizer);
             parsimsynchronizer->setContext(&simulation,parsimpartition,parsimcomm);
             simulation.setScheduler(parsimsynchronizer);

             // initialize them
             parsimcomm->init();
#else
             throw new cException("Parallel simulation is turned on in the ini file, but OMNeT++ was compiled without parallel simulation support (WITH_PARSIM=no)");
#endif
         }

         // preload NED files
         const char *nedfiles = getConfig()->getAsString("General", "preload-ned-files", NULL);
         if (nedfiles)
         {
             // iterate through file names
             ev.printf("\n");
             char *buf = opp_strdup(nedfiles);
             char *fname = strtok(buf, " ");
             while (fname!=NULL)
             {
                 if (fname[0]=='@')
                     globAndLoadListFile(fname+1);
                 else if (fname[0])
                     globAndLoadNedFile(fname);
                 fname = strtok(NULL, " ");
             }
             delete [] buf;
         }
     }
     catch (cException *e)
     {
         displayError(e);
         delete e;
         return;  // don't set initialized to true
     }
     initialized = true;
}

int TOmnetApp::getParsimProcId()
{
#ifdef WITH_PARSIM
    return parsimcomm ? parsimcomm->getProcId() : 0;
#else
    return 0;
#endif
}

int TOmnetApp::getParsimNumPartitions()
{
#ifdef WITH_PARSIM
    return parsimcomm ? parsimcomm->getNumPartitions() : 0;
#else
    return 0;
#endif
}


void TOmnetApp::shutdown()
{
    try
    {
        simulation.deleteNetwork();
#ifdef WITH_PARSIM
        if (opt_parsim && parsimpartition)
            parsimpartition->shutdown();
#endif
    }
    catch (cException *e)
    {
        displayError(e);
        delete e;
    }
}

void TOmnetApp::startRun()
{
    resetClock();
    outvectmgr->startRun();
    outscalarmgr->startRun();
    snapshotmgr->startRun();
    if (opt_parsim)
    {
#ifdef WITH_PARSIM
        parsimpartition->startRun();
        parsimsynchronizer->startRun();
#endif
    }
    simulation.startRun();
}

void TOmnetApp::endRun()
{
    // reverse order as startRun()
    simulation.endRun();
    if (opt_parsim)
    {
#ifdef WITH_PARSIM
        parsimsynchronizer->endRun();
        parsimpartition->endRun();
#endif
    }
    snapshotmgr->endRun();
    outscalarmgr->endRun();
    outvectmgr->endRun();
}

//-------------------------------------------------------------

const char *TOmnetApp::getParameter(int run_no, const char *parname)
{
    char section[16];
    sprintf(section,"Run %d",run_no);

    return getConfig()->getAsCustom2(section,"Parameters",parname,NULL);
}

bool TOmnetApp::isModuleLocal(cModule *parentmod, const char *modname, int index)
{
#ifdef WITH_PARSIM
    if (!opt_parsim)
       return true;

    // toplevel module is local everywhere
    if (!parentmod)
       return true;

    // find out if this module is (or has any submodules that are) on this partition
    char section[16];
    sprintf(section,"Run %d",1 /*run_no*/); // FIXME get run_no from somewhere!!!

    char parname[MAX_OBJECTFULLPATH];
    if (index<0)
        sprintf(parname,"%s.%s.partition-id", parentmod->fullPath(), modname);
    else
        sprintf(parname,"%s.%s[%d].partition-id", parentmod->fullPath(), modname, index);
    int procId = getConfig()->getAsInt2(section,"Partitioning",parname,-1);
    if (procId<0)
        throw new cException("incomplete or wrong partitioning: missing or invalid value for '%s'",parname);
    if (procId>=parsimcomm->getNumPartitions())
        throw new cException("wrong partitioning: value %d too large for '%s' (total partitions=%d)",
                             procId,parname,parsimcomm->getNumPartitions());
    // FIXME this solution isn't good!!! has to check if myProcId is CONTAINED
    // in the set of procIds defined for the children of this module
    int myProcId = parsimcomm->getProcId();
    return procId==myProcId;
#else
    return true;
#endif
}

void TOmnetApp::getOutVectorConfig(int run_no, const char *modname,const char *vecname, /*input*/
                                   bool& enabled, double& starttime, double& stoptime /*output*/ )
{
    // prepare section name and entry name
    char section[16];
    sprintf(section,"Run %d", run_no);

    sprintf(buffer, "%s.%s.", modname?modname:"", vecname?vecname:"");
    char *end = buffer+strlen(buffer);

    // get 'module.vector.disabled=' entry
    strcpy(end,"enabled");
    enabled = getConfig()->getAsBool2(section,"OutVectors",buffer,true);

    // get 'module.vector.interval=' entry
    strcpy(end,"interval");
    const char *s = getConfig()->getAsString2(section,"OutVectors",buffer,NULL);
    if (!s)
    {
       starttime = 0;
       stoptime = 0;
       return;
    }

    // parse interval string
    char *ellipsis = strstr(s,"..");
    if (!ellipsis)
        throw new cException("Error in output vector interval %s=%s -- contains no `..'",buffer,s);

    const char *startstr = s;
    const char *stopstr = ellipsis+2;
    starttime = strToSimtime0(startstr);
    stoptime = strToSimtime0(stopstr);

    if (startstr<ellipsis || *stopstr!='\0')
        throw new cException("Error in output vector interval %s=%s",buffer,s);
}

const char *TOmnetApp::getDisplayString(int run_no, const char *name)
{
    char section[16];
    sprintf(section,"Run %d",run_no);

    return getConfig()->getAsString2(section,"DisplayStrings",name,NULL);
}

cXMLElement *TOmnetApp::getXMLDocument(const char *filename, const char *path)
{
    cXMLElement *documentnode = xmlcache->getDocument(filename);
    assert(documentnode);
    if (path)
        return cXMLElement::getDocumentElementByPath(documentnode, path);
    else
        return documentnode->getFirstChild();
}

cConfiguration *TOmnetApp::getConfig()
{
    return config;
}

//-------------------------------------------------------------

void TOmnetApp::processFileName(opp_string& fname)
{
    if (opt_fname_append_host)
    {
        const char *hostname=getenv("HOST");
        if (!hostname)
            hostname=getenv("HOSTNAME");
        if (!hostname)
            hostname=getenv("COMPUTERNAME");
        if (!hostname)
            throw new cException("Cannot append hostname to file name `%s': no HOST, HOSTNAME "
                                 "or COMPUTERNAME (Windows) environment variable",
                                 fname.c_str());
        int pid = getProcessId();

        // add ".<hostname>.<pid>" to fname
        opp_string origfname = fname;
        fname.reserve(strlen(origfname.c_str())+1+strlen(hostname)+10+1);
        sprintf(fname.buffer(),"%s.%s.%d", origfname.buffer(), hostname, pid);
    }
}

void TOmnetApp::readOptions()
{
    cConfiguration *cfg = getConfig();

    // this must be the very first:
    opt_ini_warnings = cfg->getAsBool( "General", "ini-warnings", false );
    //FIXME cfg->setWarnings(opt_ini_warnings);

    opt_total_stack_kb = cfg->getAsInt( "General", "total-stack-kb", TOTAL_STACK_KB );
    if (cfg->getAsBool("General", "distributed", false))
         ev.printfmsg("Warning: ini file entry distributed= is obsolete (parallel simulation support was reimplemented in version 2.4)");
    opt_parsim = cfg->getAsBool("General", "parallel-simulation", false);
    if (!opt_parsim)
    {
        opt_scheduler_class = cfg->getAsString("General", "scheduler-class", "cSequentialScheduler");
    }
    else
    {
#ifdef WITH_PARSIM
        opt_parsimcomm_class = cfg->getAsString("General", "parsim-communications-class", "cFileCommunications");
        opt_parsimsynch_class = cfg->getAsString("General", "parsim-synchronization-class", "cNullMessageProtocol");
#else
        throw new cException("Parallel simulation is turned on in the ini file, but OMNeT++ was compiled without parallel simulation support (WITH_PARSIM=no)");
#endif
    }
    opt_load_libs = cfg->getAsString( "General", "load-libs", "" );

    opt_outputvectormanager_class = cfg->getAsString( "General", "outputvectormanager-class", "cFileOutputVectorManager" );
    opt_outputscalarmanager_class = cfg->getAsString( "General", "outputscalarmanager-class", "cFileOutputScalarManager" );
    opt_snapshotmanager_class = cfg->getAsString( "General", "snapshotmanager-class", "cFileSnapshotManager" );

    opt_fname_append_host = cfg->getAsBool("General","fname-append-host",false);

    // other options are read on per-run basis
}

void TOmnetApp::readPerRunOptions(int run_nr)
{
    cConfiguration *cfg = getConfig();

    char section[16];
    sprintf(section,"Run %d",run_nr);

    // get options from ini file
    opt_network_name = cfg->getAsString2( section, "General", "network", "default" );
    opt_pause_in_sendmsg = cfg->getAsBool2( section, "General", "pause-in-sendmsg", false );
    opt_warnings = cfg->getAsBool2( section, "General", "warnings", true );
    opt_simtimelimit = cfg->getAsTime2( section, "General", "sim-time-limit", 0.0 );
    opt_cputimelimit = (long)cfg->getAsTime2( section, "General", "cpu-time-limit", 0.0 );
    opt_netifcheckfreq = cfg->getAsInt2( section, "General", "netif-check-freq", 1);

    // temporarily disable warnings
    //bool w = cfg->warnings; cfg->warnings = false;
    int fromtable = 0;
    // seeds for random number generators
    opt_genk_randomseed[0] = cfg->getAsInt2( section, "General", "random-seed",
                                          starting_seeds[next_startingseed] );
    if (cfg->notFound())
    {
        opt_genk_randomseed[0] = cfg->getAsInt2( section, "General", "gen0-seed",
                                          starting_seeds[next_startingseed] );
        // if default value was used, increment next_startingseed
        if (cfg->notFound())
        {
             next_startingseed = (next_startingseed+1)%NUM_STARTINGSEEDS;
             fromtable++;
        }
    }

    for (int i=1;i<NUM_RANDOM_GENERATORS;i++)
    {
         char entry[16];
         sprintf(entry,"gen%d-seed",i);
         opt_genk_randomseed[i] = cfg->getAsInt2( section, "General", entry, starting_seeds[next_startingseed]);
         // if default value was used (=from table), increment next_startingseed
         if (cfg->notFound())
         {
             next_startingseed = (next_startingseed+1)%NUM_STARTINGSEEDS;
             fromtable++;
         }
    }
    // restore warning state
    //cfg->warnings = w;

    // report
    if (opt_ini_warnings)
         ev.printf("Note: %d random num generators initialized"
                   " from ini file, %d from internal table\n",
                   NUM_RANDOM_GENERATORS-fromtable, fromtable);
}

void TOmnetApp::makeOptionsEffective()
{
     cModule::pause_in_sendmsg = opt_pause_in_sendmsg;

     for(int i=0;i<NUM_RANDOM_GENERATORS;i++)
         genk_randseed( i, opt_genk_randomseed[i] );
}

void TOmnetApp::globAndLoadNedFile(const char *fnamepattern)
{
    Globber glob(fnamepattern);
    const char *fname;
    while ((fname=glob.getNext())!=NULL)
    {
        ev.printf("Loading NED file: %s\n", fname);
        simulation.loadNedFile(fname);
    }
}

void TOmnetApp::globAndLoadListFile(const char *fnamepattern)
{
    Globber glob(fnamepattern);
    const char *fname;
    while ((fname=glob.getNext())!=NULL)
    {
        processListFile(fname);
    }
}

void TOmnetApp::processListFile(const char *listfilename)
{
    // files should be relative to list file, so try cd into list file's directory
    opp_string dir, fnameonly;
    splitFileName(listfilename, dir, fnameonly);
    PushDir d(dir.c_str());

    // FIXME error handling is really poor here (what if can't cd, etc!)
    // FIXME put this code into main.cc too!

    std::ifstream in(fnameonly.c_str(), std::ios::in);
    if (in.fail())
        throw new cException("Cannot open list file '%s'",listfilename);

    ev.printf("Processing listfile: %s\n", listfilename);

    const int maxline=1024;
    char line[maxline];
    while (in.getline(line, maxline))
    {
        int len = in.gcount();
        if (line[len-1]=='\n')
            line[len-1] = '\0';
        const char *fname = line;

        if (fname[0]=='@')
            globAndLoadListFile(fname+1);
        else if (fname[0] && fname[0]!='#')
            globAndLoadNedFile(fname);
    }

    if (in.bad())
        throw new cException("Error reading list file '%s'",listfilename);
    in.close();
}

//-------------------------------------------------------------

void *TOmnetApp::registerOutputVector(const char *modulename, const char *vectorname, int tuple)
{
    assert(outvectmgr);
    return outvectmgr->registerVector(modulename, vectorname, tuple);
}

void TOmnetApp::deregisterOutputVector(void *vechandle)
{
    assert(outvectmgr);
    outvectmgr->deregisterVector(vechandle);
}

bool TOmnetApp::recordInOutputVector(void *vechandle, simtime_t t, double value)
{
    assert(outvectmgr);
    return outvectmgr->record(vechandle, t, value);
}

bool TOmnetApp::recordInOutputVector(void *vechandle, simtime_t t, double value1, double value2)
{
    assert(outvectmgr);
    return outvectmgr->record(vechandle, t, value1, value2);
}

//-------------------------------------------------------------

void TOmnetApp::recordScalar(cModule *module, const char *name, double value)
{
    assert(outscalarmgr);
    outscalarmgr->recordScalar(module, name, value);
}

//-------------------------------------------------------------

ostream *TOmnetApp::getStreamForSnapshot()
{
    return snapshotmgr->getStreamForSnapshot();
}

void TOmnetApp::releaseStreamForSnapshot(ostream *os)
{
    snapshotmgr->releaseStreamForSnapshot(os);
}

//-------------------------------------------------------------

bool TOmnetApp::memoryIsLow()
{
    // if the concrete user interface implementation has a memory manager,
    // it should override this function
    return false;
}

void TOmnetApp::displayError(cException *e)
{
    if (e->moduleID()==-1)
        ev.printfmsg("Error: %s.", e->message());
    else
        ev.printfmsg("Error in module (%s) %s: %s.", e->moduleClassName(), e->moduleFullPath(), e->message());
}

void TOmnetApp::displayMessage(cException *e)
{
    if (e->moduleID()==-1)
        ev.printfmsg("%s.", e->message());
    else
        ev.printfmsg("Module (%s) %s: %s.", e->moduleClassName(), e->moduleFullPath(), e->message());
}

bool TOmnetApp::idle()
{
    return false;
}

//-------------------------------------------------------------

void TOmnetApp::resetClock()
{
    struct timeb now;
    ftime(&now);
    laststarted = simendtime = simbegtime = now;
    elapsedtime.time = elapsedtime.millitm = 0;
}

void TOmnetApp::startClock()
{
    ftime(&laststarted);
}

void TOmnetApp::stopClock()
{
    ftime(&simendtime);
    elapsedtime = elapsedtime + simendtime - laststarted;
    simulatedtime = simulation.simTime();
}

struct timeb TOmnetApp::totalElapsed()
{
    struct timeb now;
    ftime(&now);
    return now - laststarted + elapsedtime;
}

void TOmnetApp::checkTimeLimits()
{
    if (opt_simtimelimit!=0 && simulation.simTime()>=opt_simtimelimit)
         throw new cTerminationException(eSIMTIME);
    if (opt_cputimelimit==0)
         return;
    struct timeb now;
    ftime(&now);
    long elapsedsecs = now.time - laststarted.time + elapsedtime.time;
    if (elapsedsecs>=opt_cputimelimit)
         throw new cTerminationException(eREALTIME);
}

void TOmnetApp::stoppedWithTerminationException(cTerminationException *e)
{
    // if we're running in parallel and this exception is NOT one we received
    // from other partitions, then notify other partitions
#ifdef WITH_PARSIM
    if (opt_parsim && !dynamic_cast<cReceivedTerminationException *>(e))
        parsimpartition->broadcastTerminationException(e);
#endif
}

void TOmnetApp::stoppedWithException(cException *e)
{
    // if we're running in parallel and this exception is NOT one we received
    // from other partitions, then notify other partitions
#ifdef WITH_PARSIM
    if (opt_parsim && !dynamic_cast<cReceivedException *>(e))
        parsimpartition->broadcastException(e);
#endif
}

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
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <ctype.h>
#include <stdio.h>
#include <assert.h>

#include "args.h"
#include "omnetapp.h"
#include "cinifile.h"
#include "patmatch.h"

#include "ctypes.h"
#include "ccoroutine.h"
#include "csimul.h"
#include "cscheduler.h"
#include "cpar.h"
#include "random.h"
#include "cmodule.h"

#ifdef WITH_PARSIM
#include "parsim/cparsimcomm.h"
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


// heuristic upper limits for various strings: obj->className(), obj->fullPath(), obj->info()
#define MAX_CLASSNAME       100
#define MAX_OBJECTFULLPATH  500
#define MAX_OBJECTINFO      500


// some platform dependency
#if defined(_WIN32) && !defined(__CYGWIN32__)
#include <process.h>
#define getpid() _getpid()
#else
#include <sys/types.h>
#include <unistd.h>  // getpid()
#endif

using std::ostream;

// This variable could really be a local var inside the functions where it is
// used; it was only made a static to reduce per-module stack size with activity().
static char buffer[1024];


// FIXME static_cast<> here?
#define CREATE_BY_CLASSNAME(var,classname,baseclass,description) \
     baseclass *var ## _tmp = (baseclass *) createOne(classname); \
     var = dynamic_cast<baseclass *>(var ## _tmp); \
     if (!var) \
         throw new cException("Class \"%s\" is not subclassed from " #baseclass, (const char *)classname);



TOmnetApp::TOmnetApp(ArgList *arglist, cIniFile *inifile)
{
    args = arglist;
    ini_file = inifile;
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
    delete ini_file;

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
         opt_inifile_name = ini_file->filename();

         // DEBUG code: print out ini file contents
         // for (cIniFileIterator i(ini_file); !i.end(); i++)
         //     printf("[%s] %s= %s\n", i.section(), i.entry(), i.value());

         // set opt_* variables from ini file(s)
         readOptions();

         // initialize coroutine library
         if (opt_total_stack_kb<=MAIN_STACK_KB)
         {
            ev.printf("Total stack size %dK increased to %dK\n", opt_total_stack_kb, MAIN_STACK_KB+4);
            opt_total_stack_kb = MAIN_STACK_KB+4;
         }
         cCoroutine::init( 1024*opt_total_stack_kb, 1024*MAIN_STACK_KB );
         simulation.init();

         // install output vector manager
         CREATE_BY_CLASSNAME(outvectmgr, opt_outputvectormanager_class, cOutputVectorManager, "output vector manager");

         // install output scalar manager
         CREATE_BY_CLASSNAME(outscalarmgr, opt_outputscalarmanager_class, cOutputScalarManager, "output scalar manager");

         // install snapshot manager
         CREATE_BY_CLASSNAME(snapshotmgr, opt_snapshotmanager_class, cSnapshotManager, "snapshot manager");

         // set up for distributed execution
         if (!opt_parsim)
         {
             CREATE_BY_CLASSNAME(scheduler, opt_scheduler_class, cScheduler, "event scheduler");
             scheduler->setSimulation(&simulation);
             simulation.setScheduler(scheduler);
         }
         else
         {
#ifdef WITH_PARSIM
             // create components
             CREATE_BY_CLASSNAME(parsimcomm, opt_parsimcomm_class, cParsimCommunications, "parallel simulation communications layer");
             parsimpartition = new cParsimPartition();
             CREATE_BY_CLASSNAME(parsimsynchronizer, opt_parsimsynch_class, cParsimSynchronizer, "parallel simulation synchronization layer");

             // wire them together (note: 'parsimsynchronizer' is also the scheduler for 'simulation')
             parsimpartition->setContext(&simulation,parsimcomm,parsimsynchronizer);
             parsimsynchronizer->setContext(&simulation,parsimpartition,parsimcomm);
             simulation.setScheduler(parsimsynchronizer);

             // initialize them
             parsimcomm->init();
#else
             throw new cException("Envir library compiled without parallel simulation support, check WITH_PARSIM option");
#endif
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

void TOmnetApp::shutdown()
{
    try
    {
        simulation.deleteNetwork();
#ifdef WITH_PARSIM
        if (opt_parsim && parsimcomm)
            parsimcomm->shutdown();
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

    ini_file->error(); // clear error flag
    return ini_file->getRaw2(section,"Parameters",parname,NULL);
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

    ini_file->error(); // clear error flag
    char parname[MAX_OBJECTFULLPATH];
    if (index<0)
        sprintf(parname,"%s.%s.partition-id", parentmod->fullPath(), modname);
    else
        sprintf(parname,"%s.%s[%d].partition-id", parentmod->fullPath(), modname, index);
    int procId = ini_file->getAsInt2(section,"Partitioning",parname,-1);
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
    enabled = ini_file->getAsBool2(section,"OutVectors",buffer,true);

    // get 'module.vector.interval=' entry
    strcpy(end,"interval");
    const char *s = ini_file->getAsString2(section,"OutVectors",buffer,NULL);
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

    ini_file->error(); // clear error flag
    return ini_file->getAsString2(section,"DisplayStrings",name,NULL);
}

cIniFile *TOmnetApp::getIniFile()
{
    return ini_file;
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
                                 (const char *)fname);
        int pid = getpid();

        // add ".<hostname>.<pid>" to fname
        opp_string origfname = fname;
        fname.allocate(strlen(origfname)+1+strlen(hostname)+10+1);
        sprintf(fname.buffer(),"%s.%s.%d", origfname.buffer(), hostname, pid);
    }
}

void TOmnetApp::readOptions()
{
    ini_file->error(); // clear error flag

    // this must be the very first:
    opt_ini_warnings = ini_file->getAsBool( "General", "ini-warnings", false );
    ini_file->warnings = opt_ini_warnings;

    opt_total_stack_kb = ini_file->getAsInt( "General", "total-stack-kb", TOTAL_STACK_KB );
    if (ini_file->getAsBool("General", "distributed", false))
         ev.printfmsg("Warning: ini file entry distributed= is obsolete (parallel simulation support was reimplemented in version 2.4)");
    opt_parsim = ini_file->getAsBool("General", "parallel-simulation", false);
    if (!opt_parsim)
    {
        opt_scheduler_class = ini_file->getAsString("General", "scheduler-class", "cSequentialScheduler");
    }
    else
    {
#ifdef WITH_PARSIM
        opt_parsimcomm_class = ini_file->getAsString("General", "parsim-communications-class", "cFileCommunications");
        opt_parsimsynch_class = ini_file->getAsString("General", "parsim-synchronization-class", "cNullMessageProtocol");
#else
        throw new cException("Envir library compiled without parallel simulation support, check WITH_PARSIM option");
#endif
    }
    opt_load_libs = ini_file->getAsString( "General", "load-libs", "" );

    opt_outputvectormanager_class = ini_file->getAsString( "General", "outputvectormanager-class", "cFileOutputVectorManager" );
    opt_outputscalarmanager_class = ini_file->getAsString( "General", "outputscalarmanager-class", "cFileOutputScalarManager" );
    opt_snapshotmanager_class = ini_file->getAsString( "General", "snapshotmanager-class", "cFileSnapshotManager" );

    opt_fname_append_host = ini_file->getAsBool("General","fname-append-host",false);

    // other options are read on per-run basis
}

void TOmnetApp::readPerRunOptions(int run_nr)
{
    char section[16];
    sprintf(section,"Run %d",run_nr);

    // get options from ini file
    opt_network_name = ini_file->getAsString2( section, "General", "network", "default" );
    opt_pause_in_sendmsg = ini_file->getAsBool2( section, "General", "pause-in-sendmsg", false );
    opt_warnings = ini_file->getAsBool2( section, "General", "warnings", true );
    opt_simtimelimit = ini_file->getAsTime2( section, "General", "sim-time-limit", 0.0 );
    opt_cputimelimit = (long)ini_file->getAsTime2( section, "General", "cpu-time-limit", 0.0 );
    opt_netifcheckfreq = ini_file->getAsInt2( section, "General", "netif-check-freq", 1);

    // temporarily disable warnings
    bool w = ini_file->warnings; ini_file->warnings = false;
    int fromtable = 0;
    // seeds for random number generators
    opt_genk_randomseed[0] = ini_file->getAsInt2( section, "General", "random-seed",
                                          starting_seeds[next_startingseed] );
    if (ini_file->error())
    {
        opt_genk_randomseed[0] = ini_file->getAsInt2( section, "General", "gen0-seed",
                                          starting_seeds[next_startingseed] );
        // if default value was used, increment next_startingseed
        if (ini_file->error())
        {
             next_startingseed = (next_startingseed+1)%NUM_STARTINGSEEDS;
             fromtable++;
        }
    }

    for(int i=1;i<NUM_RANDOM_GENERATORS;i++)
    {
         char entry[16];
         sprintf(entry,"gen%d-seed",i);
         opt_genk_randomseed[i] = ini_file->getAsInt2( section, "General", entry, starting_seeds[next_startingseed]);
         // if default value was used (=from table), increment next_startingseed
         if (ini_file->error())
         {
             next_startingseed = (next_startingseed+1)%NUM_STARTINGSEEDS;
             fromtable++;
         }
    }
    // restore warning state
    ini_file->warnings = w;

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

void TOmnetApp::recordScalar(cModule *module, const char *name, const char *text)
{
    assert(outscalarmgr);
    outscalarmgr->recordScalar(module, name, text);
}

void TOmnetApp::recordScalar(cModule *module, const char *name, cStatistic *stats)
{
    assert(outscalarmgr);
    outscalarmgr->recordScalar(module, name, stats);
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
        ev.printfmsg("Error in module %s: %s.", e->moduleFullPath(), e->message());
}

void TOmnetApp::displayMessage(cException *e)
{
    if (e->moduleID()==-1)
        ev.printfmsg("%s.", e->message());
    else
        ev.printfmsg("Module %s: %s.", e->moduleFullPath(), e->message());
}

bool TOmnetApp::idle()
{
    return false;
}

//-------------------------------------------------------------

void TOmnetApp::resetClock()
{
    laststarted = simendtime = simbegtime = time(0);
    elapsedtime = 0LU;
}

void TOmnetApp::startClock()
{
    laststarted = time(0);
}

void TOmnetApp::stopClock()
{
    simendtime = time(0);
    elapsedtime +=  simendtime - laststarted;
    simulatedtime = simulation.simTime();
}

time_t TOmnetApp::totalElapsed()
{
    return elapsedtime + time(0) - laststarted;
}

void TOmnetApp::checkTimeLimits()
{
    if (opt_simtimelimit!=0 && simulation.simTime()>=opt_simtimelimit)
         throw new cTerminationException(eSIMTIME);
    else if (opt_cputimelimit!=0 && elapsedtime+time(0)-laststarted>=opt_cputimelimit)
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

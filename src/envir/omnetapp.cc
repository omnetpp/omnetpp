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
#include "cpar.h"
#include "cnetmod.h"


#ifdef USE_PORTABLE_COROUTINES /* coroutine stacks reside in main stack area */

# define TOTAL_STACK_KB     2048
# define MAIN_STACK_KB       128  // for MSVC+Tkenv, 64K is not enough

#else /* nonportable coroutines, stacks are allocated on heap */

# define TOTAL_STACK_KB        0  // dummy value
# define MAIN_STACK_KB         0  // dummy value

#endif


// This variable could really be a local var inside the functions where it is
// used; it was only made a static to reduce per-module stack size with activity().
static char buffer[1024];


TOmnetApp::TOmnetApp(ArgList *arglist, cIniFile *inifile)
{
     args = arglist;
     ini_file = inifile;
     opt_genk_randomseed = new long[NUM_RANDOM_GENERATORS];
     next_startingseed = 0;

     outvectmgr = NULL;
     outscalarmgr = NULL;
     snapshotmgr = NULL;

     initialized = false;
}

TOmnetApp::~TOmnetApp()
{
     delete opt_genk_randomseed;
     delete args;
     delete ini_file;

     delete outvectmgr;
     delete outscalarmgr;
     delete snapshotmgr;
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
         cOutputVectorManager *ovm = (cOutputVectorManager *)createOne(opt_outputvectormanager_class);
         if (!ovm)
             throw new cException("Could not create output vector manager class \"%s\"", (const char *)opt_outputvectormanager_class);
         outvectmgr = ovm;

         // install output scalar manager
         cOutputScalarManager *osm = (cOutputScalarManager *)createOne(opt_outputscalarmanager_class);
         if (!osm)
             throw new cException("Could not create output scalar manager class \"%s\"", (const char *)opt_outputscalarmanager_class);
         outscalarmgr = osm;

         // install snapshot manager
         cSnapshotManager *snsm = (cSnapshotManager *)createOne(opt_snapshotmanager_class);
         if (!snsm)
             throw new cException("Could not create snapshot manager class \"%s\"", (const char *)opt_snapshotmanager_class);
         snapshotmgr = snsm;

         // set up for distributed execution
         if (opt_distributed)
         {
             if (ev.runningMode()==NONPARALLEL_MODE)
                 throw new cException("Support for parallel execution not linked");
             if (ev.runningMode()==STARTUPERROR_MODE)
                 throw new cException("There was an error at startup, unable to run in parallel");

             const char *libname = NULL;
             if (strcmp(opt_parallel_env, "PVM")==0)
                 libname = "cPvmMod";
             else if (strcmp(opt_parallel_env, "MPI")==0)
                 libname = "cMpiMod";
             else
                 throw new cException("Unknown parallel simulation library '%s', should be 'PVM' or 'MPI'", (const char *)opt_parallel_env);

             cNetMod *netmod = (cNetMod *)createOne(libname);
             if (!netmod)
                 throw new cException("Could not create parallel simulation library \"%s\"", (const char *)opt_parallel_env);
             try
             {
                 netmod->init();
             }
             catch (cException *e)
             {
                 throw new cException("Network interface did not initialize properly: %s", e->message());
             }
             simulation.setNetInterface( netmod );
         }
     }
     catch (cException *e)
     {
         displayError(e);
         delete e;
     }
     initialized = true;
}

void TOmnetApp::shutdown()
{
    try
    {
        simulation.deleteNetwork();
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
    simulation.startRun();
}

void TOmnetApp::endRun()
{
    simulation.endRun();
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

const char *TOmnetApp::getPhysicalMachineFor(const char *logical_mach)
{
    if (!opt_distributed)
    {
       return "default";
    }
    else
    {
       const char *mach = ini_file->getAsString("Machines", logical_mach, NULL);
       if (mach==NULL)
           throw new cException("No mapping for logical machine `%s'",logical_mach);
       return mach;
    }
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

void TOmnetApp::readOptions()
{
    ini_file->error(); // clear error flag

    // this must be the very first:
    opt_ini_warnings = ini_file->getAsBool( "General", "ini-warnings", false );
    ini_file->warnings = opt_ini_warnings;

    opt_total_stack_kb = ini_file->getAsInt( "General", "total-stack-kb", TOTAL_STACK_KB );
    opt_distributed = ini_file->getAsBool( "General", "distributed", false );
    if(opt_distributed)
        opt_parallel_env = ini_file->getAsString( "General", "parallel-system", "MPI" );
    opt_load_libs = ini_file->getAsString( "General", "load-libs", "" );

    opt_outputvectormanager_class = ini_file->getAsString( "General", "outputvectormanager-class", "cFileOutputVectorManager" );
    opt_outputscalarmanager_class = ini_file->getAsString( "General", "outputscalarmanager-class", "cFileOutputScalarManager" );
    opt_snapshotmanager_class = ini_file->getAsString( "General", "snapshotmanager-class", "cFileSnapshotManager" );
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
     simulation.setNetIfCheckFreq( opt_netifcheckfreq );

     for(int i=0;i<NUM_RANDOM_GENERATORS;i++)
         genk_randseed( i, opt_genk_randomseed[i] );
}

//--------------------------------------------------------------------
// default, stdio versions of input/output functions

void TOmnetApp::putmsg(const char *s)
{
     ::printf( "<!> %s\n", s);
     ::fflush(stdout);
}

void TOmnetApp::puts(const char *s)
{
     ::printf( "%s", s);
}

void TOmnetApp::flush()
{
     ::fflush(stdout);
}

bool TOmnetApp::gets(const char *promptstr, char *buf, int len)
{
    ::printf("%s", promptstr);
    if (buf[0]) ::printf("(default: %s) ",buf);

    ::fgets(buffer,512,stdin);
    buffer[strlen(buffer)-1] = '\0'; // chop LF

    if( buffer[0]=='\x1b' ) // ESC?
       return true;
    else
    {
       if( buffer[0] )
          strncpy(buf, buffer, len);
       return false;
    }
}

int TOmnetApp::askYesNo(const char *question )
{
    // should also return -1 (==CANCEL)
    for(;;)
    {
        ::printf("%s (y/n) ", question);
        ::fgets(buffer,512,stdin);
        buffer[strlen(buffer)-1] = '\0'; // chop LF
        if (toupper(buffer[0])=='Y' && !buffer[1])
           return 1;
        else if (toupper(buffer[0])=='N' && !buffer[1])
           return 0;
        else
           putmsg("Please type 'y' or 'n'!\n");
    }
}

void TOmnetApp::foreignPuts(const char *hostname, const char *mod, const char *str)
{
    if (!mod || !*mod)
        sprintf(buffer,"<host %s:>", hostname);
    else
        sprintf(buffer,"<%s on host %s:>", mod, hostname);
    puts(buffer);
    puts(str);
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


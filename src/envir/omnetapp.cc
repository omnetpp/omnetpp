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
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <ctype.h>
#include <stdio.h>

#if HAVE_DLOPEN
#include <dlfcn.h>
#endif

#include "args.h"
#include "omnetapp.h"
#include "cinifile.h"
#include "patmatch.h"

#include "ctypes.h"
#include "ccor.h"
#include "csimul.h"
#include "cpar.h"
#include "cnetmod.h"


#ifdef PORTABLE_COROUTINES /* coroutine stacks reside in main stack area */

# define TOTAL_STACK_KB     2048
# define MAIN_STACK_KB       128  // for MSVC+Tkenv, 64K is not enough

#else /* nonportable coroutines, stacks are allocated on heap */

# define TOTAL_STACK_KB        0  // dummy value
# define MAIN_STACK_KB         0  // dummy value

#endif

static char buffer[1024];


TOmnetApp::TOmnetApp(int,char *[])
{
     ini_file = NULL;
     opt_genk_randomseed = new long[NUM_RANDOM_GENERATORS];
     next_startingseed = 0;
}

TOmnetApp::~TOmnetApp()
{
     delete opt_genk_randomseed;
     delete ini_file;
}

void TOmnetApp::setup(int, char *[])
{
     // '-fsomething.ini' option(s) specify ini files to be read
     char *fname = argValue('f',0);
     if (!fname) fname="omnetpp.ini";
     opt_inifile_name = fname;
     ini_file = new cIniFile( fname );
     if (ini_file->error())
     {
        opp_error("Ini file processing failed");
        return;
     }

     int k;
     for (k=1; (fname=argValue('f',k))!=(char *)NULL; k++)
     {
        ini_file->readFile( fname );
        if (ini_file->error())
        {
            opp_error("Processing of additional ini file failed");
            return;
        }
     }

     // load shared libraries given with '-l' option(s)
     char *libname;
     for (k=0; simulation.ok() && (libname=argValue('l',k))!=NULL; k++)
        loadLibrary(libname);

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
     simulation.setup();

     // load dynamic libs given in [General]/load-libs=
     if (opt_load_libs)
     {
         char *buf = opp_strdup(opt_load_libs);
         char *lib, *s = buf;
         while (isspace(*s)) s++;
         while (*s)
         {
            lib = s;
            while (*s && !isspace(*s)) s++;
            if (*s) *s++ = 0;
            loadLibrary(lib);
         }
         delete buf;
     }

     if (opt_distributed)
     {
         if (ev.runningMode()==NONPARALLEL_MODE)
         {
             opp_error("Support for parallel execution not linked");
             return;
         }
         if (ev.runningMode()==STARTUPERROR_MODE)
         {
             opp_error("There was an error at startup, unable to run in parallel");
             return;
         }

         cNetMod *pvmmod = (cNetMod *)createOne( "cPvmMod");
         if (pvmmod==NULL)
         {
             opp_error("No network interface class");
             return;
         }
         if (!simulation.ok())
         {
             opp_error("Network interface did not initialize properly");
             delete pvmmod;
             return;
         }
         simulation.setNetInterface( pvmmod );
     }
}

void TOmnetApp::shutdown()
{
}

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
       if (mach==NULL) opp_error("No mapping for logical machine `%s'",logical_mach);
       return mach;
    }
}

void TOmnetApp::getOutVectorConfig(const char *modname,const char *vecname, /*input*/
                                   bool& enabled, /*output*/
                                   double& starttime, double& stoptime)
{
    // prepare section name and entry name
    char section[16];
    sprintf(section,"Run %d",simulation.runNumber());

    sprintf(buffer, "%s.%s.", modname,vecname);
    char *end = buffer+strlen(buffer);

    // get 'module.vector.disabled=' entry
    strcpy(end,"enabled");
    enabled = ini_file->getAsBool2(section,"OutVectors",buffer,TRUE);

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
    {
        opp_error("Error in output vector interval %s=%s"
                         " -- contains no `..'",buffer,s);
        return;
    }

    const char *startstr = s;
    const char *stopstr = ellipsis+2;
    starttime = strToSimtime0(startstr);
    stoptime = strToSimtime0(stopstr);

    //DEBUG: printf(" /%s/ /%s/\n",startstr,stopstr);
    if (startstr<ellipsis || *stopstr!='\0')
    {
        opp_error("Error in output vector interval %s=%s",
                         buffer,s);
        return;
    }

}

const char *TOmnetApp::getDisplayString(int run_no, const char *name)
{
    char section[16];
    sprintf(section,"Run %d",run_no);

    ini_file->error(); // clear error flag
    return ini_file->getAsString2(section,"DisplayStrings",name,NULL);
}

void TOmnetApp::readOptions()
{
    ini_file->error(); // clear error flag

    // this must be the very first:
    opt_ini_warnings = ini_file->getAsBool( "General", "ini-warnings", FALSE );
    ini_file->warnings = opt_ini_warnings;

    opt_total_stack_kb = ini_file->getAsInt( "General", "total-stack-kb", TOTAL_STACK_KB );
    opt_distributed = ini_file->getAsBool( "General", "distributed", FALSE );

    opt_load_libs = ini_file->getAsString( "General", "load-libs", "" );

    // other options are read on per-run basis
}

void TOmnetApp::readPerRunOptions(int run_nr)
{
    char section[16];
    sprintf(section,"Run %d",run_nr);

    // get options from ini file
    opt_network_name = ini_file->getAsString2( section, "General", "network", "default" );
    opt_snapshotfile_name = ini_file->getAsString2( section, "General", "snapshot-file", "omnetpp.sna" );
    opt_outvectfile_name = ini_file->getAsString2( section, "General", "output-vector-file", "omnetpp.vec" );
    opt_scalarfile_name = ini_file->getAsString2( section, "General", "output-scalar-file", "omnetpp.sca" );
    opt_logparchanges = ini_file->getAsBool2( section, "General", "log-parchanges", FALSE );
    opt_parchangefile_name = ini_file->getAsString2( section, "General", "parchange-file", "omnetpp.pch" );
    opt_pause_in_sendmsg = ini_file->getAsBool2( section, "General", "pause-in-sendmsg", FALSE );
    opt_warnings = ini_file->getAsBool2( section, "General", "warnings", TRUE );
    opt_simtimelimit = ini_file->getAsTime2( section, "General", "sim-time-limit", 0.0 );
    opt_cputimelimit = (long)ini_file->getAsTime2( section, "General", "cpu-time-limit", 0.0 );
    opt_netifcheckfreq = ini_file->getAsInt2( section, "General", "netif-check-freq", 1);

    // temporarily disable warnings
    bool w = ini_file->warnings; ini_file->warnings = FALSE;
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
     simulation.setWarnings( opt_warnings );
     cModule::pause_in_sendmsg = opt_pause_in_sendmsg;
     simulation.setSimTimeLimit( opt_simtimelimit );
     simulation.setTimeLimit( opt_cputimelimit );
     simulation.setNetIfCheckFreq( opt_netifcheckfreq );
     simulation.outvectfilemgr.setFileName( opt_outvectfile_name );
     simulation.scalarfilemgr.setFileName( opt_scalarfile_name );
     simulation.snapshotfilemgr.setFileName( opt_snapshotfile_name );
     simulation.logparchanges = opt_logparchanges;
     simulation.parchangefilemgr.setFileName( opt_parchangefile_name );
     for(int i=0;i<NUM_RANDOM_GENERATORS;i++)
         genk_randseed( i, opt_genk_randomseed[i] );
}

void TOmnetApp::loadLibrary(const char *libname)
{
#if HAVE_DLOPEN
     if (!dlopen(libname,RTLD_NOW))
         opp_error("Cannot load library: %s",dlerror());
#else
     opp_error("Cannot load '%s': dlopen() syscall not available",libname);
#endif
}

//--------------------------------------------------------------------
// default, stdio version of input/output function

void TOmnetApp::putmsg(const char *s)
{
     ::printf( "<!> %s\n", s);
}

void TOmnetApp::puts(const char *s)
{
     ::printf( "%s", s);
}

bool TOmnetApp::gets(const char *promptstr, char *buf, int len)
{
    ::printf("%s", promptstr);
    if (buf[0]) ::printf("(default: %s) ",buf);

    ::fgets(buffer,512,stdin);
    buffer[strlen(buffer)-1] = '\0'; // chop LF

    if( buffer[0]=='\x1b' ) // ESC?
       return TRUE;
    else
    {
       if( buffer[0] )
          strncpy(buf, buffer, len);
       return FALSE;
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
        sprintf(buffer,"<%s:>", hostname, str);
    else
        sprintf(buffer,"<%s on %s:>", mod, hostname, str);
    puts(buffer);
    puts(str);
}


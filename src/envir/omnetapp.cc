//==========================================================================
//  OMNETAPP.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  Implementation of
//    TOmnetApp
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

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
#include "patternmatcher.h"
#include "fsutils.h"

#include "ctypes.h"
#include "ccoroutine.h"
#include "csimul.h"
#include "cscheduler.h"
#include "cpar.h"
#include "random.h"
#include "crng.h"
#include "cmodule.h"
#include "cxmlelement.h"
#include "cxmldoccache.h"
#include "cstrtokenizer2.h"

#ifdef WITH_PARSIM
#include "cparsimcomm.h"
#include "parsim/cparsimpartition.h"
#include "parsim/cparsimsynchr.h"
#include "parsim/creceivedexception.h"
#endif

#include "platdep/time.h"
#include "platdep/misc.h"
#include "platdep/fileutil.h"  // splitFileName
#include "platdep/fileglobber.h"

#include "platdep/fileglobber.c" // pull in implementation


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
         throw new cRuntimeError("Class \"%s\" is not subclassed from " #baseclass, (const char *)classname);


//-------------------------------------------------------------

TOmnetApp::TOmnetApp(ArgList *arglist, cConfiguration *conf)
{
    args = arglist;
    config = conf;
    xmlcache = NULL;

    outvectmgr = NULL;
    outscalarmgr = NULL;
    snapshotmgr = NULL;

    num_rngs = 0;
    rngs = NULL;

    scheduler = NULL;
#ifdef WITH_PARSIM
    parsimcomm = NULL;
    parsimpartition = NULL;
#endif

    initialized = false;
}

TOmnetApp::~TOmnetApp()
{
    delete args;
    delete config;
    delete xmlcache;

    delete outvectmgr;
    delete outscalarmgr;
    delete snapshotmgr;

    for (int i = 0; i < num_rngs; i++)
         delete rngs[i];
    delete [] rngs;

    delete scheduler;
#ifdef WITH_PARSIM
    delete parsimcomm;
    delete parsimpartition;
#endif
}

void TOmnetApp::setup()
{
     // handle -h command-line option
     if (args->argGiven('h'))
     {
         printHelp();
         return;  // don't set initialized to true
     }

     try
     {
         // set opt_* variables from ini file(s)
         readOptions();

         // initialize coroutine library
         if (TOTAL_STACK_KB!=0 && opt_total_stack_kb<=MAIN_STACK_KB+4)
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

         // set up for sequential or distributed execution
         if (!opt_parsim)
         {
             // sequential
             CREATE_BY_CLASSNAME(scheduler, opt_scheduler_class.c_str(), cScheduler, "event scheduler");
             scheduler->setSimulation(&simulation);
             simulation.setScheduler(scheduler);
         }
         else
         {
#ifdef WITH_PARSIM
             // parsim: create components
             CREATE_BY_CLASSNAME(parsimcomm, opt_parsimcomm_class.c_str(), cParsimCommunications, "parallel simulation communications layer");
             parsimpartition = new cParsimPartition();
             cParsimSynchronizer *parsimsynchronizer;
             CREATE_BY_CLASSNAME(parsimsynchronizer, opt_parsimsynch_class.c_str(), cParsimSynchronizer, "parallel simulation synchronization layer");

             // wire them together (note: 'parsimsynchronizer' is also the scheduler for 'simulation')
             parsimpartition->setContext(&simulation, parsimcomm, parsimsynchronizer);
             parsimsynchronizer->setContext(&simulation, parsimpartition, parsimcomm);
             simulation.setScheduler(parsimsynchronizer);
             scheduler = parsimsynchronizer;

             // initialize them
             parsimcomm->init();
#else
             throw new cRuntimeError("Parallel simulation is turned on in the ini file, but OMNeT++ was compiled without parallel simulation support (WITH_PARSIM=no)");
#endif
         }

         // preload NED files
         std::string nedfiles = getConfig()->getAsFilenames("General", "preload-ned-files", NULL);
         if (!nedfiles.empty())
         {
             // iterate through file names
             ev.printf("\n");

             cStringTokenizer2 tokenizer(nedfiles.c_str());
             const char *fname;
             while ((fname = tokenizer.nextToken())!=NULL)
             {
                 if (fname[0]=='@' && fname[1]=='@')
                     globAndLoadListFile(fname+2, true);
                 else if (fname[0]=='@')
                     globAndLoadListFile(fname+1, false);
                 else if (fname[0])
                     globAndLoadNedFile(fname);
             }
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

void TOmnetApp::printHelp()
{
    ev << "\n";
    ev << "Command line options:\n";
    ev << "  -h            Print this help and exit.\n";
    ev << "  -f <inifile>  Use the given ini file instead of omnetpp.ini. Multiple\n";
    ev << "                -f options are accepted to load several ini files.\n";
    ev << "  -u <ui>       Selects the user interface. Standard choices are Cmdenv\n";
    ev << "                and Tkenv. To make a user interface available, you need\n";
    ev << "                to link the simulation executable with the cmdenv/tkenv\n";
    ev << "                library, or load it as shared library via the -l option.\n";
    ev << "  -l <library>  Load the specified shared library (.so or .dll) on startup.\n";
    ev << "                The file name should be given without the .so or .dll suffix\n";
    ev << "                (it will be appended automatically.) The loaded module may\n";
    ev << "                contain simple modules, plugins, etc. Multiple -l options\n";
    ev << "                can be present.\n";
    ev << "\n";
    printUISpecificHelp();

    ev << "The following components are available:\n";

    if (networks.instance()->items()>0)
    {
        ev << "  networks:\n";
        cArray::Iterator iter(*networks.instance());
        for (; iter(); iter++)
            ev << "    " << iter()->name() << '\n';
        ev << "\n";
    }

    if (modtypes.instance()->items()>0)
    {
        ev << "  module types:\n";
        cArray::Iterator iter2(*modtypes.instance());
        for (; iter2(); iter2++)
            ev << "    " << iter2()->name() << '\n';
        ev << "\n";
    }

    if (channeltypes.instance()->items()>0)
    {
        ev << "  channel types:\n";
        cArray::Iterator iter3(*channeltypes.instance());
        for (; iter3(); iter3++)
            ev << "    " << iter3()->name() << '\n';
        ev << "\n";
    }
}

const char *TOmnetApp::getRunSectionName(int runnumber)
{
    static int lastrunnumber = -1;
    static char section[16] = "";

    if (runnumber!=lastrunnumber)
    {
        sprintf(section, "Run %d", runnumber);
        lastrunnumber = runnumber;
    }
    return section;
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
    if (!initialized)
        return;

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
#endif
    }
    simulation.scheduler()->startRun();
    simulation.startRun();
    ev.flushlastline();
}

void TOmnetApp::endRun()
{
    // reverse order as startRun()
    simulation.endRun();
    simulation.scheduler()->endRun();
    if (opt_parsim)
    {
#ifdef WITH_PARSIM
        parsimpartition->endRun();
#endif
    }
    snapshotmgr->endRun();
    outscalarmgr->endRun();
    outvectmgr->endRun();
}

//-------------------------------------------------------------

std::string TOmnetApp::getParameter(int run_no, const char *parname)
{
    const char *str = getConfig()->getAsCustom2(getRunSectionName(run_no),"Parameters",parname,"");
    if (str[0]=='x' && !strncmp(str,"xmldoc",6) && !isalnum(str[6]))
    {
        // Make XML file location relative to the ini file in which it occurs.
        // Find substring between first two quotes (that is, the XML filename),
        // and prefix it with the directory.
        const char *begQuote = strchr(str+6,'"');
        if (!begQuote)
            return std::string(str);
        const char *endQuote = strchr(begQuote+1,'"');
        while (endQuote && *(endQuote-1)=='\\' && *(endQuote-2)!='\\')
            endQuote = strchr(endQuote+1,'"');
        if (!endQuote)
            return std::string(str);
        std::string fname(begQuote+1, endQuote-begQuote-1);
        const char *baseDir = getConfig()->getBaseDirectoryFor(getRunSectionName(run_no),"Parameters",parname);
        fname = tidyFilename(concatDirAndFile(baseDir, fname.c_str()).c_str());
        std::string ret = std::string(str, begQuote-str+1) + fname + endQuote;
        return ret;
    }
    else
    {
        return std::string(str);
    }
}

bool TOmnetApp::getParameterUseDefault(int run_no, const char *parname)
{
    std::string entry = parname;
    entry += ".use-default";
    return getConfig()->getAsBool2(getRunSectionName(run_no), "Parameters", entry.c_str(), false);
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
    char parname[MAX_OBJECTFULLPATH];
    if (index<0)
        sprintf(parname,"%s.%s.partition-id", parentmod->fullPath().c_str(), modname);
    else
        sprintf(parname,"%s.%s[%d].partition-id", parentmod->fullPath().c_str(), modname, index);
    int procId = getConfig()->getAsInt2(getRunSectionName(simulation.runNumber()),"Partitioning",parname,-1);
    if (procId<0)
        throw new cRuntimeError("incomplete or wrong partitioning: missing or invalid value for '%s'",parname);
    if (procId>=parsimcomm->getNumPartitions())
        throw new cRuntimeError("wrong partitioning: value %d too large for '%s' (total partitions=%d)",
                                procId,parname,parsimcomm->getNumPartitions());
    // FIXME This solution is not entirely correct. Rather, we'd have to check if
    // myProcId is CONTAINED in the set of procIds defined for the children of this
    // module.
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
    sprintf(buffer, "%s.%s.", modname?modname:"", vecname?vecname:"");
    char *end = buffer+strlen(buffer);

    // get 'module.vector.disabled=' entry
    strcpy(end,"enabled");
    enabled = getConfig()->getAsBool2(getRunSectionName(run_no),"OutVectors",buffer,true);

    // get 'module.vector.interval=' entry
    strcpy(end,"interval");
    const char *s = getConfig()->getAsString2(getRunSectionName(run_no),"OutVectors",buffer,NULL);
    if (!s)
    {
       starttime = 0;
       stoptime = 0;
       return;
    }

    // parse interval string
    const char *ellipsis = strstr(s,"..");
    if (!ellipsis)
        throw new cRuntimeError("Error in output vector interval %s=%s -- contains no `..'",buffer,s);

    const char *startstr = s;
    const char *stopstr = ellipsis+2;
    starttime = strToSimtime0(startstr);
    stoptime = strToSimtime0(stopstr);

    if (startstr<ellipsis || *stopstr!='\0')
        throw new cRuntimeError("Error in output vector interval %s=%s",buffer,s);
}

cXMLElement *TOmnetApp::getXMLDocument(const char *filename, const char *path)
{
    cXMLElement *documentnode = xmlcache->getDocument(filename);
    assert(documentnode);
    if (path)
    {
        ModNameParamResolver resolver(simulation.contextModule()); // resolves $MODULE_NAME etc in XPath expr.
        return cXMLElement::getDocumentElementByPath(documentnode, path, &resolver);
    }
    else
    {
        // returns the root element (child of the document node)
        return documentnode->getFirstChild();
    }
}

cConfiguration *TOmnetApp::getConfig()
{
    return config;
}

//-------------------------------------------------------------

void TOmnetApp::undisposedObject(cObject *obj)
{
    if (opt_print_undisposed)
        ::printf("undisposed: (%s) %s\n", obj->className(), obj->fullPath().c_str());
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
            throw new cRuntimeError("Cannot append hostname to file name `%s': no HOST, HOSTNAME "
                                    "or COMPUTERNAME (Windows) environment variable",
                                    fname.c_str());
        int pid = getpid();

        // add ".<hostname>.<pid>" to fname (note: parsimProcId cannot be appended
        // because of initialization order)
        opp_string origfname = fname;
        fname.reserve(strlen(origfname.c_str())+strlen(hostname)+30);
        sprintf(fname.buffer(),"%s.%s.%d", origfname.buffer(), hostname, pid);
    }
}

void TOmnetApp::readOptions()
{
    cConfiguration *cfg = getConfig();

    // this must be the very first:
    opt_ini_warnings = cfg->getAsBool( "General", "ini-warnings", false );
    //cfg->setWarnings(opt_ini_warnings);

    opt_total_stack_kb = cfg->getAsInt( "General", "total-stack-kb", TOTAL_STACK_KB);
    if (cfg->getAsBool("General", "distributed", false))
         ev.printfmsg("Warning: config entry distributed= is obsolete (parallel simulation support was reimplemented for version 3.0)");
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
        throw new cRuntimeError("Parallel simulation is turned on in the ini file, but OMNeT++ was compiled without parallel simulation support (WITH_PARSIM=no)");
#endif
    }
    opt_load_libs = cfg->getAsFilenames("General", "load-libs", "").c_str();

    opt_num_rngs = cfg->getAsInt("General", "num-rngs", 1);
    opt_rng_class = cfg->getAsString("General", "rng-class", "cMersenneTwister");

    opt_outputvectormanager_class = cfg->getAsString("General", "outputvectormanager-class", "cFileOutputVectorManager");
    opt_outputscalarmanager_class = cfg->getAsString("General", "outputscalarmanager-class", "cFileOutputScalarManager");
    opt_snapshotmanager_class = cfg->getAsString("General", "snapshotmanager-class", "cFileSnapshotManager");

    opt_fname_append_host = cfg->getAsBool("General","fname-append-host",false);

    // warn for obsolete RNG seed entries
    bool found = false;
    for (int k=0; k<cfg->getNumSections(); k++)
        if (cfg->exists(cfg->getSectionName(k), "random-seed") ||
            cfg->exists(cfg->getSectionName(k), "gen0-seed") ||
            cfg->exists(cfg->getSectionName(k), "gen1-seed") ||
            cfg->exists(cfg->getSectionName(k), "gen2-seed") ||
            cfg->exists(cfg->getSectionName(k), "gen3-seed"))
            found = true;
    if (found)
        ev.printfmsg("Warning: config entries random-seed= and genX-seed= are obsolete!\n"
                     "THE SEEDS YOU SPECIFIED ARE NOT USED. "
                     "Please update your ini file to the OMNeT++ 3.0 Random Number Architecture.");

    ev.debug_on_errors = cfg->getAsBool("General", "debug-on-errors", false);
    cDefaultList::doGC = cfg->getAsBool("General", "perform-gc", false);
    opt_print_undisposed = cfg->getAsBool("General", "print-undisposed", true);

    // other options are read on per-run basis
}

void TOmnetApp::readPerRunOptions(int run_no)
{
    cConfiguration *cfg = getConfig();
    const char *section = getRunSectionName(run_no);

    // get options from ini file
    opt_network_name = cfg->getAsString2(section, "General", "network", "default");
    opt_pause_in_sendmsg = cfg->getAsBool2(section, "General", "pause-in-sendmsg", false);
    opt_warnings = cfg->getAsBool2(section, "General", "warnings", true);
    opt_simtimelimit = cfg->getAsTime2(section, "General", "sim-time-limit", 0.0);
    opt_cputimelimit = (long)cfg->getAsTime2(section, "General", "cpu-time-limit", 0.0);
    opt_netifcheckfreq = cfg->getAsInt2(section, "General", "netif-check-freq", 1);

    cModule::pause_in_sendmsg = opt_pause_in_sendmsg;

    // run RNG self-test
    cRNG *testrng;
    CREATE_BY_CLASSNAME(testrng, opt_rng_class.c_str(), cRNG, "random number generator");
    testrng->selfTest();
    delete testrng;

    // set up RNGs
    int i;
    for (i = 0; i < num_rngs; i++)
         delete rngs[i];
    delete [] rngs;

    num_rngs = opt_num_rngs;
    rngs = new cRNG *[num_rngs];
    for (i = 0; i < num_rngs; i++)
    {
        cRNG *rng;
        CREATE_BY_CLASSNAME(rng, opt_rng_class.c_str(), cRNG, "random number generator");
        rngs[i] = rng;
        rngs[i]->initialize(run_no, i, num_rngs, getParsimProcId(), getParsimNumPartitions(), getConfig());
    }

    // init nextuniquenumber -- startRun() is too late because simple module ctors have run by then
    nextuniquenumber = 0;
#ifdef WITH_PARSIM
    if (opt_parsim)
        nextuniquenumber = (unsigned)parsimcomm->getProcId() * ((~0UL) / (unsigned)parsimcomm->getNumPartitions());
#endif

}

void TOmnetApp::globAndLoadNedFile(const char *fnamepattern)
{
    try {
        FileGlobber glob(fnamepattern);
        const char *fname;
        while ((fname=glob.getNext())!=NULL)
        {
            ev.printf("Loading NED file: %s\n", fname);
            simulation.loadNedFile(fname);
        }
    } catch (std::runtime_error e) {
        throw new cRuntimeError(e.what());
    }
}

void TOmnetApp::globAndLoadListFile(const char *fnamepattern, bool istemplistfile)
{
    try {
        FileGlobber glob(fnamepattern);
        const char *fname;
        while ((fname=glob.getNext())!=NULL)
        {
            processListFile(fname, istemplistfile);
        }
    } catch (std::runtime_error e) {
        throw new cRuntimeError(e.what());
    }
}

void TOmnetApp::processListFile(const char *listfilename, bool istemplistfile)
{
    std::ifstream in(listfilename, std::ios::in);
    if (in.fail())
        throw new cRuntimeError("Cannot open list file '%s'",listfilename);

    ev.printf("Processing listfile: %s\n", listfilename);

    // @listfile: files should be relative to list file, so try cd into list file's directory
    // @@listfile (temp=true): don't cd.
    PushDir d(istemplistfile ? NULL : directoryOf(listfilename).c_str());

    const int maxline=1024;
    char line[maxline];
    while (in.getline(line, maxline))
    {
        int len = in.gcount();
        if (line[len-1]=='\n')
            line[len-1] = '\0';
        const char *fname = line;

        if (fname[0]=='@' && fname[1]=='@')
            globAndLoadListFile(fname+2, true);
        else if (fname[0]=='@')
            globAndLoadListFile(fname+1, false);
        else if (fname[0] && fname[0]!='#')
            globAndLoadNedFile(fname);
    }

    if (in.bad())
        throw new cRuntimeError("Error reading list file '%s'",listfilename);
    in.close();
}

//-------------------------------------------------------------

int TOmnetApp::numRNGs()
{
    return num_rngs;
}

cRNG *TOmnetApp::rng(int k)
{
    if (k<0 || k>=num_rngs)
        throw new cRuntimeError("RNG index %d is out of range (num-rngs=%d, "
                                "check the configuration)", k, num_rngs);
    return rngs[k];
}

void TOmnetApp::getRNGMappingFor(cModule *mod)
{
    cConfiguration *cfg = getConfig();
    std::vector<opp_string> entries = cfg->getEntriesWithPrefix("General", mod->fullPath().c_str(), ".rng-");
    if (entries.size()==0)
        return;

    // extract into tmpmap[]
    int mapsize=0;
    int tmpmap[100];
    for (int i=0; i<(int)entries.size(); i+=2)
    {
        char *s1, *s2;
        int modRng = strtol(entries[i].c_str(), &s1, 10);
        int physRng = strtol(entries[i+1].c_str(), &s2, 10);
        if (*s1!='\0' || *s2!='\0')
            throw new cRuntimeError("Configuration error: rng-%s=%s of module %s: "
                                    "numeric RNG indices expected",
                                    entries[i].c_str(), entries[i+1].c_str(), mod->fullPath().c_str());

        if (physRng>numRNGs())
            throw new cRuntimeError("Configuration error: rng-%d=%d of module %s: "
                                    "RNG index out of range (num-rngs=%d)",
                                    modRng, physRng, mod->fullPath().c_str(), numRNGs());
        if (modRng>=mapsize)
        {
            if (modRng>=100)
                throw new cRuntimeError("Configuration error: rng-%d=... of module %s: "
                                        "local RNG index out of supported range 0..99",
                                        modRng, mod->fullPath().c_str());
            while (mapsize<=modRng)
            {
                tmpmap[mapsize] = mapsize;
                mapsize++;
            }
        }
        tmpmap[modRng] = physRng;
    }

    // install map into the module
    if (mapsize>0)
    {
        int *map = new int[mapsize];
        memcpy(map, tmpmap, mapsize*sizeof(int));
        mod->setRNGMap(mapsize, map);
    }
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

unsigned long TOmnetApp::getUniqueNumber()
{
    // TBD check for overflow
    return nextuniquenumber++;
}

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
    timeval now;
    gettimeofday(&now, NULL);
    laststarted = simendtime = simbegtime = now;
    elapsedtime.tv_sec = elapsedtime.tv_usec = 0;
}

void TOmnetApp::startClock()
{
    gettimeofday(&laststarted, NULL);
}

void TOmnetApp::stopClock()
{
    gettimeofday(&simendtime, NULL);
    elapsedtime = elapsedtime + simendtime - laststarted;
    simulatedtime = simulation.simTime();
}

timeval TOmnetApp::totalElapsed()
{
    timeval now;
    gettimeofday(&now, NULL);
    return now - laststarted + elapsedtime;
}

void TOmnetApp::checkTimeLimits()
{
    if (opt_simtimelimit!=0 && simulation.simTime()>=opt_simtimelimit)
         throw new cTerminationException(eSIMTIME);
    if (opt_cputimelimit==0) // no limit
         return;
    if (ev.disable_tracing && simulation.eventNumber()&0xFF!=0) // optimize: in Express mode, don't call gettimeofday() on every event
         return;
    timeval now;
    gettimeofday(&now, NULL);
    long elapsedsecs = now.tv_sec - laststarted.tv_sec + elapsedtime.tv_sec;
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

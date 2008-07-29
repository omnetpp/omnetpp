//=========================================================================
//  CENVIR.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cenvir.h"
#include "commonutil.h"
#include "ccomponent.h"  // for DummyEnv


/**
 * The std::streambuf class used by cEnvir's ostream. It redirects writes to
 * cEnvir::sputn(s,n). Flush is done at the end of each line, meanwhile
 * writes are buffered in a stringbuf.
 */
class evbuf : public std::basic_stringbuf<char>
{
  public:
    evbuf() {}
    // gcc>=3.4 needs either this-> or std::basic_stringbuf<E,T>:: in front of pptr()/pbase()
    // note: this method is needed because pptr() and pbase() are protected
    bool isempty() {return this->pptr()==this->pbase();}
  protected:
    virtual int sync()  {
        ev.sputn(this->pbase(), this->pptr()-this->pbase());
        setp(this->pbase(),this->epptr());
        return 0;
    }
    virtual std::streamsize xsputn(const char *s, std::streamsize n) {
        std::streamsize r = std::basic_stringbuf<char>::xsputn(s,n);
        for(;n>0;n--,s++)
            if (*s=='\n')
               {sync();break;}
        return r;
    }
};

//----

cEnvir::cEnvir() : out(new evbuf())
{
    disable_tracing = false;
    debug_on_errors = false;
    suppress_notifications = false; //FIXME set to true when not needed!
}

cEnvir::~cEnvir()
{
}

// note: exploits the fact that evbuf does sync() on "\n"'s
void cEnvir::flushLastLine()
{
    evbuf *buf = (evbuf *)out.rdbuf();
    if (!buf->isempty())
        buf->sputn("\n",1);
}

//
// Temp buffer for vararg functions below.
// Note: using a static buffer reduces stack usage of activity() modules;
// it also makes the following functions non-reentrant, but we don't need
// them to be reentrant anyway.
//
#define BUFLEN 1024
static char staticbuf[BUFLEN];

void cEnvir::printfmsg(const char *fmt,...)
{
    VSNPRINTF(staticbuf, BUFLEN, fmt);
    putsmsg(staticbuf);
}

int cEnvir::printf(const char *fmt,...)
{
    if (disable_tracing)
        return 0;

    va_list va;
    va_start(va, fmt);
    int len = vsnprintf(staticbuf, BUFLEN, fmt, va);
    staticbuf[BUFLEN-1] = '\0';
    va_end(va);

    // route it through streambuf to preserve ordering
    out.rdbuf()->sputn(staticbuf, len);
    return len;
}

bool cEnvir::askYesNo(const char *fmt,...)
{
    VSNPRINTF(staticbuf, BUFLEN, fmt);
    return askyesno(staticbuf);
}

//----

/**
 * A dummy implementation of cEnvir, only provided so that one
 * can use simulation library classes outside simulations, that is,
 * in programs that only link with the simulation library (<i>sim_std</i>)
 * and not with the <i>envir</i>, <i>cmdenv</i>, <i>tkenv</i>,
 * etc. libraries.
 *
 * Many simulation library classes make calls to <i>ev</i> methods,
 * which would crash if <tt>evPtr</tt> was NULL; one example is
 * cObject's destructor which contains an <tt>ev.objectDeleted()</tt>.
 * The solution provided here is that <tt>evPtr</tt> is initialized
 * to point to a DummyEnv instance, thus enabling library classes to work.
 *
 * DummyEnv methods either do nothing, or throw an "unsupported method"
 * exception, so DummyEnv is only useful for the most basic usage scenarios.
 * For anything more complicated, <tt>evPtr</tt> must be set in <tt>main()</tt>
 * to point to a proper cEnvir implementation, like the Cmdenv or
 * Tkenv classes. (The <i>envir</i> library provides a <tt>main()</tt>
 * which does exactly that.)
 *
 * @ingroup Envir
 */
class DummyEnv : public cEnvir
{
  protected:
    void unsupported() const {throw opp_runtime_error("DummyEnv: unsupported method called");}

    virtual void sputn(const char *s, int n) {::fwrite(s,1,n,stdout);}
    virtual void putsmsg(const char *msg) {::printf("\n<!> %s\n\n", msg);}
    virtual bool askyesno(const char *msg)  {unsupported(); return false;}

  public:
    // constructor, destructor
    DummyEnv() {}
    virtual ~DummyEnv() {}

    // life cycle
    virtual int run(int argc, char *argv[], cConfiguration *cfg) {unsupported();return 0;}

    // eventlog callback interface
    virtual void objectDeleted(cObject *object) {}
    virtual void simulationEvent(cMessage *msg)  {}
    virtual void messageSent_OBSOLETE(cMessage *msg, cGate *directToGate=NULL)  {}
    virtual void messageScheduled(cMessage *msg)  {}
    virtual void messageCancelled(cMessage *msg)  {}
    virtual void beginSend(cMessage *msg)  {}
    virtual void messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay)  {}
    virtual void messageSendHop(cMessage *msg, cGate *srcGate)  {}
    virtual void messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay)  {}
    virtual void endSend(cMessage *msg)  {}
    virtual void messageDeleted(cMessage *msg)  {}
    virtual void moduleReparented(cModule *module, cModule *oldparent)  {}
    virtual void componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va)  {}
    virtual void componentMethodEnd()  {}
    virtual void moduleCreated(cModule *newmodule)  {}
    virtual void moduleDeleted(cModule *module)  {}
    virtual void gateCreated(cGate *newgate)  {}
    virtual void gateDeleted(cGate *gate)  {}
    virtual void connectionCreated(cGate *srcgate)  {}
    virtual void connectionDeleted(cGate *srcgate)  {}
    virtual void displayStringChanged(cComponent *component)  {}
    virtual void undisposedObject(cObject *obj) {}

     // configuration, model parameters
    virtual void readParameter(cPar *parameter)  {unsupported();}
    virtual bool isModuleLocal(cModule *parentmod, const char *modname, int index)  {return true;}
    virtual cXMLElement *getXMLDocument(const char *filename, const char *path=NULL)  {unsupported(); return NULL;}
    virtual unsigned getExtraStackForEnvir() const  {return 0;}
    virtual cConfiguration *getConfig()  {unsupported(); return NULL;}
    virtual bool isGUI() const  {return false;}

    // UI functions (see also protected ones)
    virtual void bubble(cComponent *component, const char *text)  {}
    virtual std::string gets(const char *prompt, const char *defaultreply=NULL)  {unsupported(); return "";}
    virtual cEnvir& flush()  {::fflush(stdout); return *this;}

    // RNGs
    virtual int getNumRNGs() const {return 0;}
    virtual cRNG *getRNG(int k)  {unsupported(); return NULL;}
    virtual void getRNGMappingFor(cComponent *component)  {component->setRNGMap(0,NULL);}

    // output vectors
    virtual void *registerOutputVector(const char *modulename, const char *vectorname)  {return NULL;}
    virtual void deregisterOutputVector(void *vechandle)  {}
    virtual void setVectorAttribute(void *vechandle, const char *name, const char *value)  {}
    virtual bool recordInOutputVector(void *vechandle, simtime_t t, double value)  {return false;}

    // output scalars
    virtual void recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes=NULL)  {}
    virtual void recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes=NULL)  {}

    // snapshot file
    virtual std::ostream *getStreamForSnapshot()  {unsupported(); return NULL;}
    virtual void releaseStreamForSnapshot(std::ostream *os)  {unsupported();}

    // misc
    virtual int getArgCount() const  {unsupported(); return 0;}
    virtual char **getArgVector() const  {unsupported(); return NULL;}
    virtual int getParsimProcId() const {return 0;}
    virtual int getParsimNumPartitions() const {return 1;}
    virtual const char *getRunId() const  {unsupported(); return NULL;}
    virtual unsigned long getUniqueNumber()  {unsupported(); return 0;}
    virtual bool idle()  {return false;}
};

static DummyEnv dummyEnv;

/**
 * Pointer to the simulation's environment. Initially points to a DummyEnv,
 * which should be overwritten in main() for anything serious.
 */
cEnvir *evPtr = &dummyEnv;




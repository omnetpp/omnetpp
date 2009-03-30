#include <omnetpp.h>

#ifndef TMP_H
#define TMP_H

#define MAX_OBJECTFULLPATH 1024
#define MAX_CLASSNAME      100

#define LL  INT64_PRINTF_FORMAT


/**
 * Emulates an empty omnetpp.ini, causing config options' default values
 * to be used.
 */
class EmptyConfig : public cConfiguration
{
  protected:
    class NullKeyValue : public KeyValue {
      public:
        virtual const char *getKey() const {return NULL;}
        virtual const char *getValue() const {return NULL;}
        virtual const char *getBaseDirectory() const {return NULL;}
    };
    NullKeyValue nullKeyValue;

  protected:
    virtual const char *substituteVariables(const char *value) {return value;}

  public:
    virtual const char *getConfigValue(const char *key) const {return NULL;}
    virtual const KeyValue& getConfigEntry(const char *key) const {return nullKeyValue;}
    virtual const char *getPerObjectConfigValue(const char *objectFullPath, const char *keySuffix) const {return NULL;}
    virtual const KeyValue& getPerObjectConfigEntry(const char *objectFullPath, const char *keySuffix) const {return nullKeyValue;}
};

/**
 * Defines a minimal environment for the simulation. Module parameters get
 * initialized to their default values (causing an error if there's no
 * default); module log messages (ev<<) get printed on the standard output;
 * calls to record results will be ignored.
 */
class MinimalEnv : public cNullEnvir
{
  public:
    // constructor
    MinimalEnv(int ac, char **av, cConfiguration *c) : cNullEnvir(ac, av, c) {}

    // model parameters
    virtual void readParameter(cPar *par) {
        if (par->containsValue())
            par->acceptDefault();
        else
            throw cRuntimeError("no value for parameter %s", par->getFullPath().c_str());
    }

    // see module output
    virtual void sputn(const char *s, int n) {
        //XXX (void) ::fwrite(s,1,n,stdout);
    }

    virtual bool isGUI() const {return true;}

};

#include "logbuffer.h"
class NotSoMinimalEnv : public MinimalEnv
{
  private:
    LogBuffer logBuffer;

  public:
    // constructor
    NotSoMinimalEnv(int ac, char **av, cConfiguration *c) : MinimalEnv(ac, av, c) {}

    LogBuffer *getLogBuffer() {
        return &logBuffer;
    }

    virtual void simulationEvent(cMessage *msg) {
        cModule *module = simulation.getContextModule();
        printEventBanner(msg, module);
    }

    // method copied from 4.0 Tkenv
    void printEventBanner(cMessage *msg, cModule *module) {
        bool opt_short_banners = false; //FIXME

        // produce banner text
        char banner[2*MAX_OBJECTFULLPATH+2*MAX_CLASSNAME+60];
        if (opt_short_banners)
            sprintf(banner,"** Event #%"LL"d  T=%s  %s, on `%s'",
                    simulation.getEventNumber(),
                    SIMTIME_STR(simulation.getSimTime()),
                    module->getFullPath().c_str(),
                    msg->getFullName()
                  );
        else
            sprintf(banner,"** Event #%"LL"d  T=%s  %s (%s, id=%d), on %s`%s' (%s, id=%ld)",
                    simulation.getEventNumber(),
                    SIMTIME_STR(simulation.getSimTime()),
                    module->getFullPath().c_str(),
                    module->getComponentType()->getName(),
                    module->getId(),
                    (msg->isSelfMessage() ? "selfmsg " : ""),
                    msg->getFullName(),
                    msg->getClassName(),
                    msg->getId()
                  );

        // insert into log buffer
        logBuffer.addEvent(simulation.getEventNumber(), simulation.getSimTime(), module, banner);
    }

    virtual void sputn(const char *s, int n) {
        //XXX (void) ::fwrite(s,1,n,stdout);
        cModule *module = simulation.getContextModule();
        // note: we must strip the trailing "\n"
        if (module)
            logBuffer.addLogLine(std::string(s,n-1).c_str()); //FIXME too much copying! reuse original string if no quoting needed
        else
            logBuffer.addInfo(std::string(s,n-1).c_str()); //FIXME too much copying! reuse original string if no quoting needed
    }

};



#endif


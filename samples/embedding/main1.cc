#include <omnetpp.h>

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


class MinimalEnv : public cEnvir
{
  protected:
    int argc;
    char **argv;
    cConfiguration *cfg;
    cRNG *rng;

  protected:
    void unsupported() const {throw opp_runtime_error("MinimalEnv: unsupported method called");}

    virtual void sputn(const char *s, int n) {(void) ::fwrite(s,1,n,stdout);}
    virtual void putsmsg(const char *msg) {::printf("\n<!> %s\n\n", msg);}
    virtual bool askyesno(const char *msg)  {unsupported(); return false;}

  public:
    // constructor, destructor
    MinimalEnv(int ac, char **av, cConfiguration *c) {argc=ac; argv=av; cfg=c; rng=new cMersenneTwister();}
    virtual ~MinimalEnv() {delete cfg;}

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
    virtual void readParameter(cPar *par)  {
        if (par->containsValue())
            par->acceptDefault();
        else
            throw cRuntimeError("no value for parameter %s", par->getFullPath());
    }
    virtual bool isModuleLocal(cModule *parentmod, const char *modname, int index)  {return true;}
    virtual cXMLElement *getXMLDocument(const char *filename, const char *path=NULL)  {unsupported(); return NULL;}
    virtual unsigned getExtraStackForEnvir() const  {return 0;}
    virtual cConfiguration *getConfig()  {return cfg;}
    virtual bool isGUI() const  {return false;}

    // UI functions (see also protected ones)
    virtual void bubble(cComponent *component, const char *text)  {}
    virtual std::string gets(const char *prompt, const char *defaultreply=NULL)  {unsupported(); return "";}
    virtual cEnvir& flush()  {::fflush(stdout); return *this;}

    // RNGs
    virtual int getNumRNGs() const {return 1;}
    virtual cRNG *getRNG(int k)  {return rng;}
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
    virtual int getArgCount() const  {return argc}
    virtual char **getArgVector() const  {return argv;}
    virtual int getParsimProcId() const {return 0;}
    virtual int getParsimNumPartitions() const {return 1;}
    virtual unsigned long getUniqueNumber()  {unsupported(); return 0;}
    virtual bool idle()  {return false;}
};



void simulate(const char *networkName, simtime_t limit)
{
    // set up the network
    cModuleType *networkType = cModuleType::find(networkName);
    if (networkType == NULL) {
        printf("No such network: %s\n", networkName);
        return;
    }
    simulation.setupNetwork(networkType); //XXX may throw exception

    // prepare for running it
    simulation.startRun();

    // run the simulation
    bool ok = true;
    try {
        while (simulation.getSimTime() < limit) {
            cSimpleModule *mod = simulation.selectNextModule();
            if (!mod)
                break;  //XXX
            simulation.doOneEvent(mod);
        }
    }
    catch (cTerminationException& e) {
        printf("Finished: %s", e.what());
    }
    catch (std::exception& e) {
        ok = false;
        printf("ERROR: %s", e.what());
    }

    if (ok)
        simulation.callFinish();  //FIXME what if there's an exception during finish()?

    // finish the simulation and clean up the network
    simulation.endRun();
    simulation.deleteNetwork();
}

int main(int argc, char *argv[])
{
    // the following line MUST be at the top of main()
    cStaticFlag dummy;

    // initializations
    ExecuteOnStartup::executeAll();
    SimTime::setScaleExp(-12);

    // set up an environment for the simulation
    cEnvir *oldEv = evPtr;
    evPtr = new MinimalEnv(argc, argv, new EmptyConfig());

    // load NED files
    simulation.loadNedSourceFolder("./mysim-core");
    simulation.loadNedSourceFolder("./mysim-ext");
    simulation.doneLoadingNedFiles();

    // set up and run a simulation model
    simulate("FooNetwork", 1000);
    simulate("BarNetwork", 2000);

    // exit
    delete evPtr;
    evPtr = oldEv;
    return 0;
}

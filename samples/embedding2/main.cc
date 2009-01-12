#include <omnetpp.h>
#include <map>
#include <string>

// strings containing the NED sources to be loaded by the simulation
// See the nedfiles.cc file in the model directory.
extern const char *ALOHA_NED;
extern const char *SERVER_NED;
extern const char *HOST_NED;

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


class MinimalEnv : public cNullEnvir
{
  public:
    typedef std::map<std::string, double> StringDoubleMap;
    StringDoubleMap scalarResults;
    // constructor
    MinimalEnv(int ac, char **av, cConfiguration *c) : cNullEnvir(ac, av, c) {}

    // model parameters
    virtual void readParameter(cPar *par)  
    {
        if (par->containsValue())
            par->acceptDefault();
        else
            throw cRuntimeError("no value for parameter %s", par->getFullPath().c_str());
    }
    
    virtual void recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes=NULL)  
    {
       // store all reported scalar results into a map for later use
       scalarResults[component->getFullPath()+"."+name] = value;
    }
    
    // dump any scalar result gathered by the simulation model
    void dumpResults() 
    {
	      if (!scalarResults.empty())
      	{
            ::printf("Scalar statistics:\n");
            for (StringDoubleMap::const_iterator it = scalarResults.begin(); it!=scalarResults.end(); ++it)
                ::printf("  %s: %lf\n", it->first.c_str(), it->second);
	      }
    }
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
        printf("Finished.\n");
    }
    catch (cTerminationException& e) {
        printf("Finished: %s\n", e.what());
    }
    catch (std::exception& e) {
        ok = false;
        printf("ERROR: %s\n", e.what());
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
    simulation.loadNedSourceFolder("./model");
    simulation.doneLoadingNedFiles();

    // set up and run a simulation model
    printf("Running slotted aloha simulation.\n");
    simulate("Aloha", 1000);
    
    // display the simulation results
    ((MinimalEnv *)evPtr)->dumpResults();
                                            
    // exit
    delete evPtr;
    evPtr = oldEv;
    return 0;
}

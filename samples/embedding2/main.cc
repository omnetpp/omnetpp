#include <omnetpp.h>
#include <map>
#include <string>
#include <iostream>

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
    std::string iaTime;
    int numHosts;

    // constructor
    MinimalEnv(int ac, char **av, cConfiguration *c) : cNullEnvir(ac, av, c) {}

    void setParameters(int numHosts, std::string iaTime)
    {
    	this->iaTime = iaTime;
    	this->numHosts = numHosts;
    }

    double getStatistic(std::string name)
    {
    	return scalarResults[name];
    }

    void clearStatistics()
    {
    	scalarResults.clear();
    }

    // model parameters
    virtual void readParameter(cPar *par)
    {
    	if (strcmp(par->getName(), "iaTime") == 0)
    		par->parse(iaTime.c_str());
    	else if (strcmp(par->getName(), "numHosts") == 0)
    		par->setLongValue(numHosts);
    	else if (par->containsValue())
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

// run the simulation with the provided arguments and return the channel utilization
double simulate(std::string networkName, simtime_t limit, int numHosts, double iaMean)
{
	MinimalEnv *menv = (MinimalEnv *)evPtr;

	// set the simulation parameters in the environment
	std::ostringstream iaParam;
	iaParam << "exponential(" << iaMean << "s)";
    menv->setParameters(numHosts, iaParam.str());

	// set up the network
    cModuleType *networkType = cModuleType::find(networkName.c_str());
    if (networkType == NULL) {
        printf("No such network: %s\n", networkName.c_str());
        return 0;
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

    // get the channel utilization from the simulation
    double result = menv->getStatistic("Aloha.server.channel utilization");
    // clear all statistics (we do not need them anymore)
    menv->clearStatistics();
    return result;
}

int main(int argc, char *argv[])
{
    // the following line MUST be at the top of main()
    cStaticFlag dummy;
    int numHosts;
    double iaMean;
    std::string againQuestion;

    // initializations
    ExecuteOnStartup::executeAll();
    SimTime::setScaleExp(-12);

    // set up an environment for the simulation
    cEnvir *oldEv = evPtr;
    evPtr = new MinimalEnv(argc, argv, new EmptyConfig());

    // load NED files
    simulation.loadNedSourceFolder("./model");
    simulation.doneLoadingNedFiles();

    do
    {
        std::cout << "Slotted aloha simulation...\n";
        std::cout << "Specify the number of hosts: ";
        std::cin >> numHosts;
        std::cout << "Specify the mean of the inter-arrival time between packets (sec): ";
        std::cin >> iaMean;
        std::cout << "Runnig simulation...";

        // simulate the Aloha network for 1 hour
        double chanelUtil = simulate("Aloha", 3600, numHosts, iaMean);

        // display the simulation results
        std::cout << "Channel utilization: " << chanelUtil << endl;


        std::cout << "Do you want to run a simulation again? (y/n): ";
        std::cin >> againQuestion;
    } while (againQuestion == "y");


    // exit
    delete evPtr;
    evPtr = oldEv;
    return 0;
}

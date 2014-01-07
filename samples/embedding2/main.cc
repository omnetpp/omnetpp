//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

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
double simulateAloha(simtime_t limit, int numHosts, double iaMean)
{
    // set up an environment for the simulation
    MinimalEnv *menv = new MinimalEnv(0, NULL, new EmptyConfig());
    cSimulation *sim = new cSimulation("simulation", menv);
    cSimulation::setActiveSimulation(sim);

    // set the simulation parameters in the environment
    std::ostringstream iaParam;
    iaParam << "exponential(" << iaMean << "s)";
    menv->setParameters(numHosts, iaParam.str());

    // set up the network
    cModuleType *networkType = cModuleType::find("Aloha");
    if (!networkType)
        throw cRuntimeError("Aloha network not found");
    sim->setupNetwork(networkType); //XXX may throw exception

    // prepare for running it
    sim->startRun();

    // run the simulation
    bool ok = true;
    try {
        while (sim->getSimTime() < limit) {
            cSimpleModule *mod = sim->selectNextModule();
            if (!mod)
                break;  //XXX
            sim->doOneEvent(mod);
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
        sim->callFinish();  //XXX may throw exception

    // finish the simulation and clean up the network
    sim->endRun();
    sim->deleteNetwork();

    // get the channel utilization from the simulation
    double result = menv->getStatistic("Aloha.server.channel utilization");

    // delete simulation
    cSimulation::setActiveSimulation(NULL);
    delete sim;  // deletes menv as well

    return result;
}

int main(int argc, char *argv[])
{
    // the following line MUST be at the top of main()
    cStaticFlag dummy;

    // initializations
    CodeFragments::executeAll(CodeFragments::STARTUP);
    SimTime::setScaleExp(-12);

    // load NED definitions from string constants (this program doesn't need external NED files!)
    cSimulation::loadNedText("aloha", ALOHA_NED);
    cSimulation::loadNedText("server", SERVER_NED);
    cSimulation::loadNedText("host", HOST_NED);
    //cSimulation::loadNedSourceFolder("./model");
    cSimulation::doneLoadingNedFiles();

    // run simulations until user tells us to exit
    std::string againQuestion;
    do {
        int numHosts;
        double iaMean;

        std::cout << "\n";
        std::cout << "Slotted Aloha simulation\n";
        std::cout << "========================\n";
        std::cout << "\n";
        std::cout << "Specify the number of hosts: ";
        std::cin >> numHosts;
        std::cout << "Specify the mean inter-arrival time between packets (sec): ";
        std::cin >> iaMean;
        std::cout << "Running simulation...";

        // simulate the Aloha network for 1 hour
        double channelUtilization = simulateAloha(3600, numHosts, iaMean);

        // display the simulation results
        std::cout << "Channel utilization: " << channelUtilization << endl;

        std::cout << "Do you want to run a simulation again? (y/n): ";
        std::cin >> againQuestion;
    }
    while (againQuestion == "y");

    // deallocate registration lists, loaded NED files, etc.
    CodeFragments::executeAll(CodeFragments::SHUTDOWN);
    return 0;
}

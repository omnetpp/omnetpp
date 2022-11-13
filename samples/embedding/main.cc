//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>

using namespace omnetpp;

/**
 * Emulates an empty omnetpp.ini, causing config options' default values
 * to be used.
 */
class EmptyConfig : public cConfiguration
{
  protected:
    class NullKeyValue : public KeyValue {
      public:
        virtual const char *getKey() const override {return nullptr;}
        virtual const char *getValue() const override {return nullptr;}
        virtual const char *getBaseDirectory() const override {return nullptr;}
    };
    NullKeyValue nullKeyValue;

  protected:
    virtual const char *substituteVariables(const char *value) const override {return value;}

  public:
    virtual const char *getConfigValue(const char *key) const override {return nullptr;}
    virtual const KeyValue& getConfigEntry(const char *key) const override {return nullKeyValue;}
    virtual const char *getPerObjectConfigValue(const char *objectFullPath, const char *keySuffix) const override {return nullptr;}
    virtual const KeyValue& getPerObjectConfigEntry(const char *objectFullPath, const char *keySuffix) const override {return nullKeyValue;}
    virtual std::map<std::string,std::string> getPredefinedVariables() const override {return {};}
    virtual std::map<std::string,std::string> getIterationVariables() const override {return {};}
    virtual const char *getVariable(const char *varname) const override {return nullptr;}
    virtual std::vector<const char *> getMatchingConfigKeys(const char *pattern) const override {return {};}
    virtual const char *getParameterValue(const char *moduleFullPath, const char *paramName, bool hasDefaultValue) const override {return nullptr;}
    virtual const KeyValue& getParameterEntry(const char *moduleFullPath, const char *paramName, bool hasDefaultValue) const override {return nullKeyValue;}
    virtual std::vector<const char *> getKeyValuePairs(int flags=FILT_ALL) const override {return {};}
    virtual std::vector<const char *> getMatchingPerObjectConfigKeySuffixes(const char *objectFullPath, const char *keySuffixPattern) const override {return {};}
    virtual const char *getFileName() const override {return nullptr;}
};

/**
 * Defines a minimal environment for the simulation. Module parameters get
 * initialized to their default values (causing an error if there's no
 * default); module log messages (EV<<) get printed on the standard output;
 * calls to record results will be ignored.
 */
class MinimalEnv : public cNullEnvir
{
  public:
    // model parameters
    virtual void readParameter(cPar *par) override {
        if (par->containsValue())
            par->acceptDefault();
        else
            throw cRuntimeError("no value for parameter %s", par->getFullPath().c_str());
    }

    // see module output
    virtual void sputn(const char *s, int n) {
        (void) ::fwrite(s,1,n,stdout);
    }

};

void simulate(cSimulation *simulation, const char *networkName, simtime_t limit)
{
    // set up the network
    cModuleType *networkType = cModuleType::find(networkName);
    if (networkType == nullptr) {
        printf("No such network: %s\n", networkName);
        return;
    }
    simulation->setupNetwork(networkType); //XXX may throw exception
    simulation->setSimulationTimeLimit(limit);

    // prepare for running it
    simulation->callInitialize();

    // run the simulation
    bool ok = true;
    try {
        while (true) {
            cEvent *event = simulation->takeNextEvent();
            if (!event)
                break;  //XXX
            simulation->executeEvent(event);
        }
    }
    catch (cTerminationException& e) {
        printf("Finished: %s\n", e.what());
    }
    catch (std::exception& e) {
        ok = false;
        printf("ERROR: %s\n", e.what());
    }

    if (ok)
        simulation->callFinish();  //XXX may throw exception

    // finish the simulation and clean up the network
    simulation->deleteNetwork();
}

int main(int argc, char *argv[])
{
    // the following line MUST be at the top of main()
    cStaticFlag dummy;

    // initializations
    CodeFragments::executeAll(CodeFragments::EARLY_STARTUP);
    SimTime::setScaleExp(-12);

    // set up an environment for the simulation
    cEnvir *envir = new MinimalEnv();
    cSimulation *simulation = new cSimulation("simulation", envir);
    cSimulation::setActiveSimulation(simulation);

    cConfiguration *cfg = new EmptyConfig();
    simulation->configure(cfg);

    // load NED files
    simulation->loadNedFolder("model");

    CodeFragments::executeAll(CodeFragments::STARTUP);

    // set up and run a simulation model
    simulate(simulation, "Net", 1000);

    // exit
    cSimulation::setActiveSimulation(nullptr);
    delete simulation;
    delete cfg;

    // deallocate registration lists, loaded NED files, etc.
    CodeFragments::executeAll(CodeFragments::SHUTDOWN);
    return 0;
}

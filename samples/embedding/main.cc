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
    virtual const char *getVariable(const char *varname) const override {return nullptr;}
    virtual std::vector<const char *> getIterationVariableNames() const override {return {};}
    virtual std::vector<const char *> getPredefinedVariableNames() const override {return {};}
    virtual const char *getVariableDescription(const char *varname) const override {return nullptr;}
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
    // constructor
    MinimalEnv(cConfiguration *cfg) : cNullEnvir(cfg) {}

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

void simulate(const char *networkName, simtime_t limit)
{
    // set up the network
    cModuleType *networkType = cModuleType::find(networkName);
    if (networkType == nullptr) {
        printf("No such network: %s\n", networkName);
        return;
    }
    getSimulation()->setupNetwork(networkType); //XXX may throw exception
    getSimulation()->setSimulationTimeLimit(limit);

    // prepare for running it
    getSimulation()->callInitialize();

    // run the simulation
    bool ok = true;
    try {
        while (true) {
            cEvent *event = getSimulation()->takeNextEvent();
            if (!event)
                break;  //XXX
            getSimulation()->executeEvent(event);
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
        getSimulation()->callFinish();  //XXX may throw exception

    // finish the simulation and clean up the network
    getSimulation()->deleteNetwork();
}

int main(int argc, char *argv[])
{
    // the following line MUST be at the top of main()
    cStaticFlag dummy;

    // initializations
    CodeFragments::executeAll(CodeFragments::STARTUP);
    SimTime::setScaleExp(-12);

    // set up an environment for the simulation
    cEnvir *env = new MinimalEnv(new EmptyConfig());
    cSimulation *sim = new cSimulation("simulation", env);
    cSimulation::setActiveSimulation(sim);

    // load NED files
    sim->loadNedSourceFolder("model");
    sim->doneLoadingNedFiles();

    // set up and run a simulation model
    simulate("Net", 1000);

    // exit
    cSimulation::setActiveSimulation(nullptr);
    delete sim;

    // deallocate registration lists, loaded NED files, etc.
    CodeFragments::executeAll(CodeFragments::SHUTDOWN);
    return 0;
}

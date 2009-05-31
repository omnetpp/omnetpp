#include <omnetpp.h>

// swig cannot handle inner classes, so wrap the ones we need in global classes

class cModule_GateIterator
{
  private:
    cModule::GateIterator it;
  public:
    cModule_GateIterator(const cModule *m) : it(m) {}
    cGate *get() const {return it();}
    bool end() const {return it.end();}
    void next() {it++;}
    void advance(int k) {it+=k;}
};

class cModule_SubmoduleIterator
{
  private:
    cModule::SubmoduleIterator it;
  public:
    cModule_SubmoduleIterator(const cModule *m) : it(m) {}
    cModule *get() const {return it();}
    bool end() const {return it.end();}
    void next() {it++;}
};

class cModule_ChannelIterator
{
  private:
    cModule::ChannelIterator it;
  public:
    cModule_ChannelIterator(const cModule *m) : it(m) {}
    cChannel *get() const {return const_cast<cModule_ChannelIterator*>(this)->it();}
    bool end() const {return it.end();}
    void next() {it++;}
};




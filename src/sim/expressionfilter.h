//==========================================================================
//  EXPRESSIONFILTER.H - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __OMNETPP_EXPRESSIONFILTER_H
#define __OMNETPP_EXPRESSIONFILTER_H

#include "common/expression.h"
#include "common/exprnodes.h"
#include "omnetpp/simkerneldefs.h"
#include "omnetpp/cresultfilter.h"
#include "omnetpp/cresultrecorder.h"

namespace omnetpp {

struct SIM_API SignalSource
{
  public:
    cResultFilter *filter = nullptr;
    cComponent *component = nullptr;
    simsignal_t signalID = -1;

    SignalSource() {}
    SignalSource(cResultFilter *filter) : filter(filter) {}
    SignalSource(cComponent *component, const char *signalName);
    SignalSource(cComponent *component, simsignal_t signalID) : component(component), signalID(signalID) {}
    bool operator==(const SignalSource& other) const {return filter==other.filter && component==other.component && signalID==other.signalID;}
    bool isEmpty() const {return !filter && !component;}
    bool isSubscribed(cResultListener *listener) const;
    void subscribe(cResultListener *listener) const; // duplicate subscriptions will raise an error
    std::string str() const;
};


class SIM_API ExpressionFilter : public cResultFilter
{
    public:
        typedef omnetpp::common::Expression Expression;
        typedef omnetpp::common::expression::ExprValue ExprValue;
        typedef omnetpp::common::expression::ExprNode ExprNode;
        typedef omnetpp::common::expression::ValueNode ValueNode;
        typedef omnetpp::common::expression::LeafNode LeafNode;
        typedef omnetpp::common::expression::Context Context;
    public:
        class SIM_API FilterInputNode : public common::expression::ValueNode
        {
          private:
            ExpressionFilter *owner = nullptr;
            int index = -1;
          protected:
            virtual void print(std::ostream& out, int spaciousness) const override {out << getName();}
            virtual ExprValue evaluate(Context* context) const override {
                return owner->numInputs==1 ? owner->soleInput.lastValue : owner->inputs[index].lastValue;
            }
          public:
            FilterInputNode(ExpressionFilter *owner=nullptr, int index=-1) : owner(owner), index(index) {}
            void setOwner(ExpressionFilter *owner, int index) {this->owner = owner; this->index = index;}
            virtual FilterInputNode *dup() const override {return new FilterInputNode(owner, index);}
            virtual std::string getName() const override {return "$" + std::to_string(index);}
        };

        class WrappedSignalSource : public LeafNode {
        protected:
            SignalSource value;
        protected:
            virtual void print(std::ostream& out, int spaciousness) const override {out << value.str();}
            virtual ExprValue evaluate(Context *context) const override {throw cRuntimeError("Illegal call");}
        public:
            WrappedSignalSource(const SignalSource& value) : value(value) {}
            virtual ExprNode *dup() const override {return new WrappedSignalSource(value);}
            virtual std::string getName() const override {return value.str();}
            SignalSource& get() {return value;}
        };

    protected:
        Expression expr;

        struct FilterInput { // note: we don't store FilterInputNode*, as it's one-to-many relationship
            SignalSource source;
            ExprValue lastValue;
        };

        int numInputs = 0;
        FilterInput soleInput; // used when numInputs==1, unused otherwise (very rarely)
        FilterInput *inputs = nullptr; // used when numInputs > 1

        ExprValue lastOutput;
        simtime_t lastTimestamp = SIMTIME_ZERO;

    protected:
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details) override {find(prev)->lastValue = ExprValue(b); process(t, details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, intval_t l, cObject *details) override {find(prev)->lastValue = ExprValue(l); process(t, details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, uintval_t l, cObject *details) override {find(prev)->lastValue = ExprValue((intval_t)l); process(t, details);} //TODO signed<-->unsigned!
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, double d, cObject *details) override {find(prev)->lastValue = ExprValue(d); process(t, details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v, cObject *details) override {find(prev)->lastValue = ExprValue(SIMTIME_DBL(v)); process(t, details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s, cObject *details) override {ASSERT(s); find(prev)->lastValue = ExprValue(s); process(t, details);} // note: emitting nullptr is not allowed
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *obj, cObject *details) override {find(prev)->lastValue = ExprValue(obj); process(t, details);}

        virtual void receiveSignal(cComponent *source, simsignal_t signalID, bool b, cObject *details) override {find(source,signalID)->lastValue = ExprValue(b); process(now(), details);}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, intval_t l, cObject *details) override {find(source,signalID)->lastValue = ExprValue(l); process(now(), details);}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, uintval_t l, cObject *details) override {find(source,signalID)->lastValue = ExprValue((intval_t)l); process(now(), details);} //TODO signed<-->unsigned!
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d, cObject *details) override {find(source,signalID)->lastValue = ExprValue(d); process(now(), details);}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, const SimTime& v, cObject *details) override {find(source,signalID)->lastValue = ExprValue(SIMTIME_DBL(v)); process(now(), details);}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s, cObject *details) override {ASSERT(s); find(source,signalID)->lastValue = ExprValue(s); process(now(), details);} // note: emitting nullptr is not allowed
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) override {find(source,signalID)->lastValue = ExprValue(obj); process(now(), details);}

        virtual void subscribedTo(cResultFilter *prev) override {} // to allow subscription to multiple signals

        FilterInput *find(cResultFilter *prev);
        FilterInput *find(cComponent *source, simsignal_t signalID);
        simtime_t now() const;
        void process(simtime_t_cref t, cObject *details);
        int findInput(const SignalSource& source) const;

    public:
        ExpressionFilter() {}
        ~ExpressionFilter() {delete [] inputs;}
        const virtual char *getName() const override;
        Expression& getExpression() {return expr;}
        virtual std::string str() const override;
        int addInput(const SignalSource& source, FilterInputNode *filterInputNode);
        int getNumInputs() const {return numInputs;}
        SignalSource getInputSource(int k) const {ASSERT(k>=0 && k<numInputs); return numInputs==1 ? soleInput.source : inputs[k].source;}
        ExprValue getLastValue() const {return lastOutput;}
        simtime_t getLastTimestamp() const {return lastTimestamp;}
};

}  // namespace omnetpp


#endif

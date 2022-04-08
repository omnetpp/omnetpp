//==========================================================================
//  RESULTFILTERS.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_RESULTFILTERS_H
#define __OMNETPP_RESULTFILTERS_H

#include <cmath>  // INFINITY, NAN
#include "omnetpp/simkerneldefs.h"
#include "omnetpp/cresultfilter.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cstatistic.h"

namespace omnetpp {

/**
 * @addtogroup ResultFiltersRecorders
 * @{
 */

/**
 * @brief Result filter that absorbs input values during the configured warm-up
 * period (see warmup-period configuration option), and lets everything through
 * once the warm-up period is over.
 *
 * @see cSimulation::getWarmupPeriod()
 */
class SIM_API WarmupPeriodFilter : public cResultFilter
{
    protected:
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, intval_t, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, uintval_t, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, double d, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *obj, cObject *details) override;
    public:
        virtual simtime_t_cref getEndWarmupPeriod() const {return getSimulation()->getWarmupPeriod();}
        virtual std::string str() const override;
};

/**
 * @brief Result filter that demultiplexes its input to several outputs. The
 * selector (a.ka. demux label) is the name string of the details object emitted
 * with the value, <tt>details->getFullName()</tt>. Results recorded on the various
 * outputs are tagged with the demux label (i.e. the demux label appears as part
 * of their names).
 *
 * New outputs are created on demand, by cloning the result filter/recorder chain(s)
 * initially attached as output to the demux filter (i.e. before the first emitted signal).
 */
class SIM_API DemuxFilter : public cResultFilter
{
    private:
        int fanOut = -1; // number of initial outputs; #outputs is a multiple of fanOut
        std::map<std::string, int> labelToDelegateStartIndexMap;
    private:
        int getDelegateStartIndexByLabel(cObject *details);
        cResultListener *copyChain(cResultListener *templateChain, const char *label);
    protected:
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, intval_t, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, uintval_t, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, double d, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *obj, cObject *details) override;
    public:
        virtual std::string str() const override;
};

/**
 * @brief Result filter for counting the input values, including NaN and nullptr values.
 */
class SIM_API TotalCountFilter : public cResultFilter
{
    protected:
        intval_t count;
    protected:
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details) override {count++; fire(this,t,count,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, intval_t l, cObject *details) override {count++; fire(this,t,count,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, uintval_t l, cObject *details) override {count++; fire(this,t,count,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, double d, cObject *details) override {count++; fire(this,t,count,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v, cObject *details) override {count++; fire(this,t,count,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s, cObject *details) override {count++; fire(this,t,count,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *obj, cObject *details) override {count++; fire(this,t,count,details);}
    public:
        TotalCountFilter() {count = 0;}
        intval_t getCount() const {return count;}
        virtual double getInitialDoubleValue() const override {return getCount();}
        virtual std::string str() const override;
};

/**
 * @brief Result filter for counting signals. Signal values do not need
 * to be numeric to be counted. NaN and nullptr values are ignored.
 */
class SIM_API CountFilter : public TotalCountFilter
{
    protected:
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, double d, cObject *details) override {if (!std::isnan(d)) count++; fire(this,t,count,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s, cObject *details) override {if (s) count++; fire(this,t,count,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *obj, cObject *details) override {if (obj) count++; fire(this,t,count,details);}
    public:
        CountFilter() {}
};

/**
 * @brief Result filter that replaces every value with a constant. Signal values
 * do not need to be numeric.
 */
class SIM_API ConstantFilter : public cResultFilter
{
    protected:
        double c;
    protected:
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details) override {fire(this,t,c,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, intval_t l, cObject *details) override {fire(this,t,c,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, uintval_t l, cObject *details) override {fire(this,t,c,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, double d, cObject *details) override {fire(this,t,c,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v, cObject *details) override {fire(this,t,c,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s, cObject *details) override {fire(this,t,c,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *obj, cObject *details) override {fire(this,t,c,details);}
    public:
        ConstantFilter(double c) {this->c = c;}
        double getConstant() const {return c;}
        virtual std::string str() const override;
};

/**
 * @brief Result filter that replaces every value with zero.
 */
class SIM_API Constant0Filter : public ConstantFilter
{
    public:
        Constant0Filter() : ConstantFilter(0) {}
};

/**
 * @brief Result filter that replaces every value with 1.0.
 */
class SIM_API Constant1Filter : public ConstantFilter
{
    public:
        Constant1Filter() : ConstantFilter(1) {}
};

/**
 * @brief Result filter that yields the time of emitting the signal.
 *
 * Note: unlike CountFilter, this filter does not ignore NaN and nullptr
 * emitted values.
 */
class SIM_API TimeFilter : public cResultFilter
{
    protected:
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details) override {fire(this,t,t,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, intval_t l, cObject *details) override {fire(this,t,t,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, uintval_t l, cObject *details) override {fire(this,t,t,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, double d, cObject *details) override {fire(this,t,t,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v, cObject *details) override {fire(this,t,t,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s, cObject *details) override {fire(this,t,t,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *obj, cObject *details) override {fire(this,t,t,details);}
    public:
        TimeFilter() {}
};

/**
 * @brief This class is a no-op filter.
 */
class SIM_API IdentityFilter : public cResultFilter
{
    protected:
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details) override {fire(this,t,b,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, intval_t l, cObject *details) override {fire(this,t,l,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, uintval_t l, cObject *details) override {fire(this,t,l,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, double d, cObject *details) override {fire(this,t,d,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v, cObject *details) override {fire(this,t,v,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s, cObject *details) override {fire(this,t,s,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *obj, cObject *details) override {fire(this,t,obj,details);}
    public:
        IdentityFilter() {}
};

/**
 * @brief Filter that merges several inputs into one output.
 */
class SIM_API MergeFilter : public IdentityFilter
{
    protected:
        virtual void subscribedTo(cResultFilter *prev) override {} // to allow subscription to multiple signals
    public:
        MergeFilter() {}
};


/**
 * @brief Filter that removes (filters out) NaNs, and lets through all other values.
 */
class SIM_API SkipNanFilter : public cNumericResultFilter
{
    protected:
        virtual bool process(simtime_t& t, double& value, cObject *details) override {return !std::isnan(value);}
};

/**
 * @brief Filter that raises a runtime error if it sees a NaN in the input.
 */
class SIM_API ErrorNanFilter : public cNumericResultFilter
{
    protected:
        virtual bool process(simtime_t& t, double& value, cObject *details) override;
};

/**
 * @brief Filter that counts NaN values in the input.
 */
class SIM_API CountNanFilter : public cNumericResultFilter
{
    protected:
        intval_t count = 0;
    protected:
        virtual bool process(simtime_t& t, double& value, cObject *details) override;
    public:
        CountNanFilter() {}
        intval_t getCount() const {return count;}
        virtual double getInitialDoubleValue() const override {return getCount();}
        virtual std::string str() const override;
};

/**
 * @brief Filter that outputs the sum of signal values. NaN values in the input are ignored.
 */
class SIM_API SumFilter : public cNumericResultFilter
{
    protected:
        double sum;
    protected:
        virtual bool process(simtime_t& t, double& value, cObject *details) override;
    public:
        SumFilter() {sum = 0;}
        double getSum() const {return sum;}
        virtual double getInitialDoubleValue() const override {return getSum();}
        virtual std::string str() const override;
};

/**
 * @brief Result filter that computes the (time-weighted or unweighted) mean
 * of signal values. NaN values in the input are ignored, or in the time-weighted
 * case, they denote intervals to be ignored.
 */
class SIM_API MeanFilter : public cNumericResultFilter
{
    protected:
        bool timeWeighted = false;
        intval_t count = 0;
        double lastValue = NAN;
        simtime_t lastTime = SIMTIME_ZERO;
        double weightedSum = 0;
        simtime_t totalTime = SIMTIME_ZERO;
    protected:
        virtual bool process(simtime_t& t, double& value, cObject *details) override;
    public:
        MeanFilter() {}
        virtual void init(Context *ctx) override;
        double getMean() const;
        virtual std::string str() const override;
};

/**
 * @brief Result filter that computes the minimum of signal values.
 * NaN values in the input are ignored.
 */
class SIM_API MinFilter : public cNumericResultFilter
{
    protected:
        double min;
    protected:
        virtual bool process(simtime_t& t, double& value, cObject *details) override;
    public:
        MinFilter() {min = INFINITY;}
        double getMin() const {return min;}
        virtual std::string str() const override;
};

/**
 * @brief Result filter that computes the maximum of signal values.
 * NaN values in the input are ignored.
 */
class SIM_API MaxFilter : public cNumericResultFilter
{
    protected:
        double max;
    protected:
        virtual bool process(simtime_t& t, double& value, cObject *details) override;
    public:
        MaxFilter() {max = -INFINITY;}
        double getMax() const {return max;}
        virtual std::string str() const override;
};

/**
 * @brief Result filter that computes the arithmetic mean of signal values.
 * NaN values in the input are ignored.
 */
class SIM_API AverageFilter : public cNumericResultFilter
{
    protected:
        intval_t count;
        double sum;
    protected:
        virtual bool process(simtime_t& t, double& value, cObject *details) override;
    public:
        AverageFilter() {count = 0; sum = 0;}
        double getAverage() const {return sum/count;}
        virtual std::string str() const override;
};

/**
 * @brief Result filter that computes the time average of signal values.
 * NaN values in the input denote intervals to be ignored.
 */
class SIM_API TimeAverageFilter : public cNumericResultFilter
{
    protected:
        double lastValue = NAN;
        simtime_t lastTime = SIMTIME_ZERO;
        double weightedSum = 0;
        simtime_t totalTime = SIMTIME_ZERO;
    protected:
        virtual bool process(simtime_t& t, double& value, cObject *details) override;
    public:
        TimeAverageFilter() {}
        double getTimeAverage() const;
        virtual std::string str() const override;
};

/**
 * @brief Result filter that removes repeated values
 */
class SIM_API RemoveRepeatsFilter : public cNumericResultFilter
{
    protected:
        double prev;
    protected:
        virtual bool process(simtime_t& t, double& value, cObject *details) override;
    public:
        RemoveRepeatsFilter() {prev = NAN;}
        double getLastValue() const {return prev;}
        virtual std::string str() const override;
};

/**
 * @brief Filter that expects a cPacket and outputs its length in bytes (getByteLength()).
 * Null (nullptr) values are ignored; non-cPacket objects are an error.
 */
class SIM_API PacketBytesFilter : public cObjectResultFilter
{
    protected:
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *object, cObject *details) override;
    public:
        using cObjectResultFilter::receiveSignal;
};

/**
 * @brief Filter that expects a cPacket and outputs its length in bits (getBitLength()).
 * Null (nullptr) values are ignored; non-cPacket objects are an error.
 */
class SIM_API PacketBitsFilter : public cObjectResultFilter
{
    protected:
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *object, cObject *details) override;
    public:
        using cObjectResultFilter::receiveSignal;
};

/**
 * @brief Filter that outputs the sum of signal values divided by the measurement
 * interval (simtime minus warmupPeriod). NaN values in the input are ignored.
 */
class SIM_API SumPerDurationFilter : public cNumericResultFilter
{
    protected:
        double sum;
    protected:
        virtual bool process(simtime_t& t, double& value, cObject *details) override;
    public:
        SumPerDurationFilter() {sum = 0;}
        double getSumPerDuration() const;
        virtual double getInitialDoubleValue() const override {return getSumPerDuration();}
        virtual std::string str() const override;
};

/** @} */

}  // namespace omnetpp

#endif

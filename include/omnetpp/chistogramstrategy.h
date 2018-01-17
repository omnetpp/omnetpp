//==========================================================================
//  CHISTOGRAMSTRATEGY.H - part of
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

#ifndef __OMNETPP_CHISTOGRAMSTRATEGY_H
#define __OMNETPP_CHISTOGRAMSTRATEGY_H

#include <omnetpp/chistogram.h>

namespace omnetpp {

class SIM_API cIHistogramStrategy : public cObject
{
    friend class cHistogram;
  public:
    enum HistogramMode {MODE_AUTO, MODE_INTEGERS, MODE_REALS}; // for use by histogram setup strategy classes

  protected:
    cHistogram *hist = nullptr; // backreference to "owner"

  public:
    virtual void init(cHistogram *hist);
    virtual void collect(double value) = 0;
    virtual void collectWeighted(double value, double weight) = 0;

    virtual bool binsCreated() const = 0;
    virtual void createBins() = 0;
};

class SIM_API cFixedRangeHistogramStrategy : public cIHistogramStrategy
{
  protected:
    double lo;
    double hi;
    double binSize;
    HistogramMode mode;  // may not be AUTO

    virtual void setupBins();

  public:
    cFixedRangeHistogramStrategy(double lo, double hi, double binSize=1, HistogramMode mode=MODE_REALS) :
        lo(lo), hi(hi), binSize(binSize), mode(mode) {}

    double getHi() const {return hi;}
    void setHi(double hi) {this->hi = hi;}
    double getLo() const {return lo;}
    void setLo(double lo) {this->lo = lo;}
    HistogramMode getMode() const {return mode;}
    void setMode(HistogramMode mode) {this->mode = mode;}
    double getBinSize() const {return binSize;}
    void setBinSize(double binSize) {this->binSize = binSize;}

    virtual void collect(double value) override;
    virtual void collectWeighted(double value, double weight) override;

    virtual bool binsCreated() const override {return hist->getNumBins() > 0;}
    virtual void createBins() override {};
};

class SIM_API cPrecollectionBasedHistogramStrategy : public cIHistogramStrategy
{
  protected:
    size_t numToPrecollect = 100;

    std::vector<double> values;
    std::vector<double> weights; // if weighted

    bool inPrecollection = true;
    virtual void moveValuesIntoHistogram();

  public:

    int getNumToPrecollect() const {return numToPrecollect;}
    void setNumToPrecollect(int numToPrecollect) {this->numToPrecollect = numToPrecollect;}

    virtual bool binsCreated() const override;
    // should be called when the histogram has to be finalized (recorded) before precollection is over
    virtual void createBins() override;
};

class cGenericHistogramStrategy : public cPrecollectionBasedHistogramStrategy
{
  private:
    double rangeExtensionFactor = 1.5;
    double binSize = NAN;
    int desiredNumBins;

    HistogramMode mode = MODE_AUTO;

  protected:
    virtual void setupBins();
    double computeBinSize(double& rangeMin, double& rangeMax);
    double computeIntegerBinSize(double& rangeMin, double& rangeMax);

  public:
    cGenericHistogramStrategy(int desiredNumBins=30, HistogramMode mode=MODE_AUTO) : desiredNumBins(desiredNumBins), mode(mode) {}

    double getBinSize() const {return binSize;}
    HistogramMode getMode() const {return mode;}

    virtual void collect(double value) override;
    virtual void collectWeighted(double value, double weight) override;
};

// note: when constraints are overdetermined, "best effort" will be made to satisfy them:
// at least as numBins bins created; histogram range to include the [lo,hi) interval, etc.
class cAutoRangeHistogramStrategy : public cPrecollectionBasedHistogramStrategy
{
  private:
    double lo = NAN;  // set NaN for unspecified
    double hi = NAN;  // set NaN for unspecified
    double rangeExtensionFactor = 1.5;
    int numBins = -1;
    double binSize = NAN;
    HistogramMode mode = MODE_AUTO;
    bool binSizeRounding = true;
    bool autoExtend = true; //TODO implement
    bool mergeBins = true; //TODO implement
    // TODO maybe add maxNumBins? i.e. when #bins exceeds maxNumBins, call mergeBins() with appropriate "n"

  protected:
    virtual void setupBins();
    double computeDoubleBinSize(double& rangeMin, double& rangeMax);
    double computeIntegerBinSize(double& rangeMin, double& rangeMax);

  public:
    // lo, hi: specify NaN for either or both or neither!
    explicit cAutoRangeHistogramStrategy(double lo=NAN, double hi=NAN, int numBins=-1, HistogramMode mode=MODE_AUTO) :
        lo(lo), hi(hi), numBins(numBins), mode(mode) {}

    double getBinSize() const {return binSize;}
    void setBinSize(double binSize) {this->binSize = binSize;}
    bool getBinSizeRounding() const {return binSizeRounding;}
    void setBinSizeRounding(bool binSizeRounding) {this->binSizeRounding = binSizeRounding;}
    double getHi() const {return hi;}
    void setHi(double hi) {this->hi = hi;}
    double getLo() const {return lo;}
    void setLo(double lo) {this->lo = lo;}
    HistogramMode getMode() const {return mode;}
    void setMode(HistogramMode mode) {this->mode = mode;}
    int getNumBins() const {return numBins;}
    void setNumBins(int numBins) {this->numBins = numBins;}
    double getRangeExtensionFactor() const {return rangeExtensionFactor;}
    void setRangeExtensionFactor(double rangeExtensionFactor) {this->rangeExtensionFactor = rangeExtensionFactor;}

    virtual void collect(double value) override;
    virtual void collectWeighted(double value, double weight) override;
};

}  // namespace omnetpp


#endif




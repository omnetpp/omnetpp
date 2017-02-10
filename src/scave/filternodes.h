//=========================================================================
//  FILTERNODES.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_FILTERNODES_H
#define __OMNETPP_SCAVE_FILTERNODES_H

#include "commonnodes.h"
#include "enums.h"

namespace omnetpp {
namespace scave {


/**
 * Processing node which does nothing but copying its input to its output.
 */
class SCAVE_API NopNode : public FilterNode
{
    public:
        NopNode()  {}
        virtual ~NopNode() {}
        virtual bool isReady() const override;
        virtual void process() override;
};

class SCAVE_API NopNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "nop";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
};

//----

/**
 * Processing node which adds a constant to every value.
 */
class SCAVE_API AdderNode : public FilterNode
{
    protected:
        double c;
    public:
        AdderNode(double c)  {this->c = c;}
        virtual ~AdderNode() {}
        virtual bool isReady() const override;
        virtual void process() override;
};

class SCAVE_API AdderNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "add";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual void mapVectorAttributes(/*inout*/StringMap& attrs, /*out*/StringVector& warnings) const override;
};

//----

/**
 * Processing node which multiplies every value by a constant.
 */
class SCAVE_API MultiplierNode : public FilterNode
{
    protected:
        double a;
    public:
        MultiplierNode(double a)  {this->a = a;}
        virtual ~MultiplierNode() {}
        virtual bool isReady() const override;
        virtual void process() override;
};

class SCAVE_API MultiplierNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "multiply-by";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual void getAttrDefaults(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual void mapVectorAttributes(/*inout*/StringMap& attrs, /*out*/StringVector& warnings) const override;
};

//----

/**
 * Processing node which divides every value by a constant.
 */
class SCAVE_API DividerNode : public FilterNode
{
    protected:
        double a;
    public:
        DividerNode(double a)  {this->a = a;}
        virtual ~DividerNode() {}
        virtual bool isReady() const override;
        virtual void process() override;
};

class SCAVE_API DividerNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "divide-by";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual void getAttrDefaults(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual void mapVectorAttributes(/*inout*/StringMap& attrs, /*out*/StringVector& warnings) const override;
};

//----

/**
 * Processing node which computes value modulo a constant.
 */
class SCAVE_API ModuloNode : public FilterNode
{
    protected:
        double a;
    public:
        ModuloNode(double a)  {this->a = a;}
        virtual ~ModuloNode() {}
        virtual bool isReady() const override;
        virtual void process() override;
};

class SCAVE_API ModuloNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "modulo";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual void getAttrDefaults(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual void mapVectorAttributes(/*inout*/StringMap& attrs, /*out*/StringVector& warnings) const override;
};

//----

/**
 * Processing node which subtracts the previous value from every value
 */
class SCAVE_API DifferenceNode : public FilterNode
{
    protected:
        double prevy;
    public:
        DifferenceNode()  {prevy = 0;}
        virtual ~DifferenceNode() {}
        virtual bool isReady() const override;
        virtual void process() override;
};

class SCAVE_API DifferenceNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "difference";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual void getAttrDefaults(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual void mapVectorAttributes(/*inout*/StringMap& attrs, /*out*/StringVector& warnings) const override;
};

//----

/**
 * Processing node which subtracts the previous value from every value
 */
class SCAVE_API TimeDiffNode : public FilterNode
{
    protected:
        double prevx;
    public:
        TimeDiffNode()  {prevx = 0;}
        virtual ~TimeDiffNode() {}
        virtual bool isReady() const override;
        virtual void process() override;
};

class SCAVE_API TimeDiffNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "timediff";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual void getAttrDefaults(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual void mapVectorAttributes(/*inout*/StringMap& attrs, /*out*/StringVector& warnings) const override;
};

//----

/**
 * Processing node which calculates moving average
 */
class SCAVE_API MovingAverageNode : public FilterNode
{
    protected:
        double alpha;
        bool firstRead;
        double prevy;
    public:
        MovingAverageNode(double alph);
        virtual ~MovingAverageNode() {}
        virtual bool isReady() const override;
        virtual void process() override;
};

class SCAVE_API MovingAverageNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "movingavg";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual void getAttrDefaults(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual void mapVectorAttributes(/*inout*/StringMap& attrs, /*out*/StringVector& warnings) const override;
};

//----

/**
 * Processing node which sums up values
 */
class SCAVE_API SumNode : public FilterNode
{
    protected:
        double sum;
    public:
        SumNode()    {sum = 0;}
        virtual ~SumNode() {}
        virtual bool isReady() const override;
        virtual void process() override;
};

class SCAVE_API SumNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "sum";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual void getAttrDefaults(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual void mapVectorAttributes(/*inout*/StringMap& attrs, /*out*/StringVector& warnings) const override;
};
//----

/**
 * Processing node which shifts every value in time.
 */
class SCAVE_API TimeShiftNode : public FilterNode
{
    protected:
        double dt;
    public:
        TimeShiftNode(double dt)  {this->dt = dt;}
        virtual ~TimeShiftNode() {}
        virtual bool isReady() const override;
        virtual void process() override;
};

class SCAVE_API TimeShiftNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "timeshift";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
};

//----

/**
 * Processing node which adds linear trend to its input.
 */
class SCAVE_API LinearTrendNode : public FilterNode
{
    protected:
        double a;
    public:
        LinearTrendNode(double a)  {this->a = a;}
        virtual ~LinearTrendNode() {}
        virtual bool isReady() const override;
        virtual void process() override;
};

class SCAVE_API LinearTrendNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "lineartrend";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual void getAttrDefaults(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual void mapVectorAttributes(/*inout*/StringMap& attrs, /*out*/StringVector& warnings) const override;
};

//----

/**
 * Processing node which deletes values outside a specified time interval.
 */
class SCAVE_API CropNode : public FilterNode
{
    protected:
        double from, to;
    public:
        CropNode(double from, double to)  {this->from = from; this->to = to;}
        virtual ~CropNode() {}
        virtual bool isReady() const override;
        virtual void process() override;
};

class SCAVE_API CropNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "crop";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
};

//----

/**
 * Processing node which calculates mean on (0,t)
 */
class SCAVE_API MeanNode : public FilterNode
{
    protected:
        double sum;
        long count;
    public:
        MeanNode()  {sum=0; count=0;}
        virtual ~MeanNode() {}
        virtual bool isReady() const override;
        virtual void process() override;
};

class SCAVE_API MeanNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "mean";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual void mapVectorAttributes(/*inout*/StringMap& attrs, /*out*/StringVector& warnings) const override;
};

//----

/**
 * Processing node which removes repeated y values.
 */
class SCAVE_API RemoveRepeatsNode : public FilterNode
{
    protected:
        bool first;
        double prevy;
    public:
        RemoveRepeatsNode()  {first = true;}
        virtual ~RemoveRepeatsNode() {}
        virtual bool isReady() const override;
        virtual void process() override;
};

class SCAVE_API RemoveRepeatsNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "removerepeats";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual void getAttrDefaults(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual void mapVectorAttributes(/*inout*/StringMap& attrs, /*out*/StringVector& warnings) const override;
};

//----

/**
 * Processing node which compares the value against a constant.
 */
class SCAVE_API CompareNode : public FilterNode
{
    protected:
        double threshold;
        bool replaceIfLess;
        bool replaceIfGreater;
        bool replaceIfEqual;
        double valueIfLess;
        double valueIfGreater;
        double valueIfEqual;
    public:
        CompareNode()  {replaceIfLess = replaceIfGreater = replaceIfEqual = false;}
        virtual ~CompareNode() {}

        void setThreshold(double d)  {threshold = d;}
        void setLessValue(double d)  {valueIfLess = d; replaceIfLess = true;}
        void setEqualValue(double d)  {valueIfEqual = d; replaceIfEqual = true;}
        void setGreaterValue(double d)  {valueIfGreater = d; replaceIfGreater = true;}

        virtual bool isReady() const override;
        virtual void process() override;
};

class SCAVE_API CompareNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "compare";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual void getAttrDefaults(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
};

//----

/**
 * Processing node which regards the input as a step function
 * ("sample-hold"), and integrates it.
 */
class SCAVE_API IntegrateNode : public FilterNode
{
    protected:
        InterpolationMode interpolationmode;
        bool isPrevValid; // false before collecting the 1st value
        double integral;
        double prevx, prevy;
    public:
        IntegrateNode(InterpolationMode mode)  {interpolationmode = mode; integral = prevx = prevy = 0; isPrevValid = false;}
        virtual ~IntegrateNode() {}

        virtual bool isReady() const override;
        virtual void process() override;
};

class SCAVE_API IntegrateNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "integrate";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual void getAttrDefaults(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual void mapVectorAttributes(/*inout*/StringMap& attrs, /*out*/StringVector& warnings) const override;
};

//----

/**
 * Processing node which calculates time average
 */
class SCAVE_API TimeAverageNode : public FilterNode
{
    protected:
        InterpolationMode interpolationmode;
        bool isPrevValid; // false before collecting the 1st value
        double integral;
        double startx;
        double prevx, prevy;
    public:
        TimeAverageNode(InterpolationMode mode)  {interpolationmode = mode; integral = startx = prevx = prevy = 0; isPrevValid = false;}
        virtual ~TimeAverageNode() {}

        virtual bool isReady() const override;
        virtual void process() override;
};

class SCAVE_API TimeAverageNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "timeavg";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual void getAttrDefaults(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual void mapVectorAttributes(/*inout*/StringMap& attrs, /*out*/StringVector& warnings) const override;
};

//----

/**
 * Processing node that divides each value by t.
 */
class SCAVE_API DivideByTimeNode : public FilterNode
{
    public:
        DivideByTimeNode()  {}
        virtual ~DivideByTimeNode() {}

        virtual bool isReady() const override;
        virtual void process() override;
};

class SCAVE_API DivideByTimeNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "divtime";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual void mapVectorAttributes(/*inout*/StringMap& attrs, /*out*/StringVector& warnings) const override;
};

/**
 * Processing node that replaces t values with their serial.
 */
class SCAVE_API TimeToSerialNode : public FilterNode
{
    protected:
        int64_t serial;
    public:
        TimeToSerialNode() : serial(0) {}
        virtual ~TimeToSerialNode() {}

        virtual bool isReady() const override;
        virtual void process() override;
};

class SCAVE_API TimeToSerialNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "timetoserial";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual void mapVectorAttributes(/*inout*/StringMap& attrs, /*out*/StringVector& warnings) const override;
};

//----

/**
 * Processing node which subtracts the first finite value from every value.
 */
class SCAVE_API SubtractFirstValueNode : public FilterNode
{
    protected:
        bool firstValueSeen;
        double firstValue;
    public:
        SubtractFirstValueNode() : firstValueSeen(false) {}
        virtual ~SubtractFirstValueNode() {}
        virtual bool isReady() const override;
        virtual void process() override;
};

class SCAVE_API SubtractFirstValueNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "subtractfirstval";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual void mapVectorAttributes(/*inout*/StringMap& attrs, /*out*/StringVector& warnings) const override;
};

} // namespace scave
}  // namespace omnetpp

#endif


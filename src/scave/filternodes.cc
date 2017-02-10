//=========================================================================
//  FILTERNODES.CC - part of
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

#include <cmath>
#include <cstdlib>
#include "common/stringutil.h"
#include "channel.h"
#include "filternodes.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

bool NopNode::isReady() const
{
    return in()->length() > 0;
}

void NopNode::process()
{
    int n = in()->length();
    for (int i = 0; i < n; i++) {
        Datum d;
        in()->read(&d, 1);
        out()->write(&d, 1);
    }
}

//--

const char *NopNodeType::getDescription() const
{
    return "Does nothing";
}

void NopNodeType::getAttributes(StringMap& attrs) const
{
}

Node *NopNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new NopNode();
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

//-----

bool AdderNode::isReady() const
{
    return in()->length() > 0;
}

void AdderNode::process()
{
    int n = in()->length();
    for (int i = 0; i < n; i++) {
        Datum d;
        in()->read(&d, 1);
        d.y += c;
        out()->write(&d, 1);
    }
}

//--

const char *AdderNodeType::getDescription() const
{
    return "Adds a constant to the input: yout[k] = y[k] + c";
}

void AdderNodeType::getAttributes(StringMap& attrs) const
{
    attrs["c"] = "the additive constant";
}

Node *AdderNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    double c = atof(attrs["c"].c_str());

    Node *node = new AdderNode(c);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

void AdderNodeType::mapVectorAttributes(  /*inout*/ StringMap& attrs,  /*out*/ StringVector& warnings) const
{
    if (attrs["type"] == "enum")
        warnings.push_back(std::string("Applying '") + getName() + "' to an enum");
    attrs["type"] = "double";  // XXX int?
}

//-----

bool MultiplierNode::isReady() const
{
    return in()->length() > 0;
}

void MultiplierNode::process()
{
    int n = in()->length();
    for (int i = 0; i < n; i++) {
        Datum d;
        in()->read(&d, 1);
        d.y *= a;
        out()->write(&d, 1);
    }
}

//--

const char *MultiplierNodeType::getDescription() const
{
    return "Multiplies input by a constant: yout[k] = a * y[k]";
}

void MultiplierNodeType::getAttributes(StringMap& attrs) const
{
    attrs["a"] = "the multiplier constant";
}

void MultiplierNodeType::getAttrDefaults(StringMap& attrs) const
{
    attrs["a"] = "1.0";
}

Node *MultiplierNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    double a = atof(attrs["a"].c_str());

    Node *node = new MultiplierNode(a);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

void MultiplierNodeType::mapVectorAttributes(  /*inout*/ StringMap& attrs,  /*out*/ StringVector& warnings) const
{
    if (attrs["type"] == "enum")
        warnings.push_back(std::string("Applying '") + getName() + "' to an enum");
    attrs["type"] = "double";  // XXX int?
}

//-----

bool DividerNode::isReady() const
{
    return in()->length() > 0;
}

void DividerNode::process()
{
    int n = in()->length();
    for (int i = 0; i < n; i++) {
        Datum d;
        in()->read(&d, 1);
        d.y /= a;
        out()->write(&d, 1);
    }
}

//--

const char *DividerNodeType::getDescription() const
{
    return "Divides input by a constant: yout[k] = y[k] / a";
}

void DividerNodeType::getAttributes(StringMap& attrs) const
{
    attrs["a"] = "the divider constant";
}

void DividerNodeType::getAttrDefaults(StringMap& attrs) const
{
    attrs["a"] = "1.0";
}

Node *DividerNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    double a = atof(attrs["a"].c_str());

    Node *node = new DividerNode(a);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

void DividerNodeType::mapVectorAttributes(  /*inout*/ StringMap& attrs,  /*out*/ StringVector& warnings) const
{
    if (attrs["type"] == "enum")
        warnings.push_back(std::string("Applying '") + getName() + "' to an enum");
    attrs["type"] = "double";
}

//-----

bool ModuloNode::isReady() const
{
    return in()->length() > 0;
}

void ModuloNode::process()
{
    int n = in()->length();
    for (int i = 0; i < n; i++) {
        // TODO: when floor(y/a)!=floor(prevy/a), insert a NaN! so they won't get connected on the line chart
        Datum d;
        in()->read(&d, 1);
        d.y -= floor(d.y/a)*a;
        out()->write(&d, 1);
    }
}

//--

const char *ModuloNodeType::getDescription() const
{
    return "Computes input modulo a constant: yout[k] = y[k] % a";
}

void ModuloNodeType::getAttributes(StringMap& attrs) const
{
    attrs["a"] = "the modulus";
}

void ModuloNodeType::getAttrDefaults(StringMap& attrs) const
{
    attrs["a"] = "1";
}

Node *ModuloNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    double a = atof(attrs["a"].c_str());

    Node *node = new ModuloNode(a);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

void ModuloNodeType::mapVectorAttributes(  /*inout*/ StringMap& attrs,  /*out*/ StringVector& warnings) const
{
    if (attrs["type"] == "enum")
        warnings.push_back(std::string("Applying '") + getName() + "' to an enum");
    attrs["type"] = "double";  // XXX int?
}

//-----

bool DifferenceNode::isReady() const
{
    return in()->length() > 0;
}

void DifferenceNode::process()
{
    int n = in()->length();
    for (int i = 0; i < n; i++) {
        Datum d;
        in()->read(&d, 1);

        double tmp = d.y;
        d.y -= prevy;
        prevy = tmp;

        out()->write(&d, 1);
    }
}

//--

const char *DifferenceNodeType::getDescription() const
{
    return "Subtracts the previous value from every value: yout[k] = y[k] - y[k-1]";
}

void DifferenceNodeType::getAttributes(StringMap& attrs) const
{
}

void DifferenceNodeType::getAttrDefaults(StringMap& attrs) const
{
}

Node *DifferenceNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new DifferenceNode();
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

void DifferenceNodeType::mapVectorAttributes(  /*inout*/ StringMap& attrs,  /*out*/ StringVector& warnings) const
{
    if (attrs["type"] == "enum")
        warnings.push_back(std::string("Applying '") + getName() + "' to an enum");
    if (attrs["type"] != "int")
        attrs["type"] = "double";
    // TODO interpolation-mode
}

//-----

bool TimeDiffNode::isReady() const
{
    return in()->length() > 0;
}

void TimeDiffNode::process()
{
    int n = in()->length();
    for (int i = 0; i < n; i++) {
        Datum d;
        in()->read(&d, 1);

        d.y = d.x - prevx;
        prevx = d.x;

        out()->write(&d, 1);
    }
}

//--

const char *TimeDiffNodeType::getDescription() const
{
    return "Returns the difference in time between this and the previous value: yout[k] = t[k] - t[k-1]";
}

void TimeDiffNodeType::getAttributes(StringMap& attrs) const
{
}

void TimeDiffNodeType::getAttrDefaults(StringMap& attrs) const
{
}

Node *TimeDiffNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new TimeDiffNode();
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

void TimeDiffNodeType::mapVectorAttributes(  /*inout*/ StringMap& attrs,  /*out*/ StringVector& warnings) const
{
    attrs["type"] = "double";
    attrs["interpolationmode"] = "backward-sample-hold";
}

//-----

MovingAverageNode::MovingAverageNode(double alph)
{
    firstRead = true;
    prevy = 0;
    alpha = alph;
}

bool MovingAverageNode::isReady() const
{
    return in()->length() > 0;
}

void MovingAverageNode::process()
{
    if (firstRead) {
        Datum d;
        in()->read(&d, 1);
        prevy = d.y;
        out()->write(&d, 1);
        firstRead = false;
    }

    int n = in()->length();
    for (int i = 0; i < n; i++) {
        Datum d;
        in()->read(&d, 1);
        d.y = prevy = prevy + alpha*(d.y-prevy);
        out()->write(&d, 1);
    }
}

//--

const char *MovingAverageNodeType::getDescription() const
{
    return "Applies the exponentially weighted moving average filter:\n"
           "yout[k] = yout[k-1] + alpha*(y[k]-yout[k-1])";
}

void MovingAverageNodeType::getAttributes(StringMap& attrs) const
{
    attrs["alpha"] = "smoothing constant";
}

void MovingAverageNodeType::getAttrDefaults(StringMap& attrs) const
{
    attrs["alpha"] = "0.1";
}

Node *MovingAverageNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    double alpha = atof(attrs["alpha"].c_str());

    Node *node = new MovingAverageNode(alpha);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

void MovingAverageNodeType::mapVectorAttributes(  /*inout*/ StringMap& attrs,  /*out*/ StringVector& warnings) const
{
    if (attrs["type"] == "enum")
        warnings.push_back(std::string("Applying '") + getName() + "' to an enum");
    attrs["type"] = "double";
}

//-----

bool SumNode::isReady() const
{
    return in()->length() > 0;
}

void SumNode::process()
{
    int n = in()->length();
    for (int i = 0; i < n; i++) {
        Datum d;
        in()->read(&d, 1);

        sum += d.y;
        d.y = sum;

        out()->write(&d, 1);
    }
}

//--

const char *SumNodeType::getDescription() const
{
    return "Sums up values: yout[k] = SUM(y[i], i=0..k)";
}

void SumNodeType::getAttributes(StringMap& attrs) const
{
}

void SumNodeType::getAttrDefaults(StringMap& attrs) const
{
}

Node *SumNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new SumNode();
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

void SumNodeType::mapVectorAttributes(  /*inout*/ StringMap& attrs,  /*out*/ StringVector& warnings) const
{
    if (attrs["type"] == "enum")
        warnings.push_back(std::string("Applying '") + getName() + "' to an enum");
    if (attrs["type"] != "int")
        attrs["type"] = "double";
}

//------

bool TimeShiftNode::isReady() const
{
    return in()->length() > 0;
}

void TimeShiftNode::process()
{
    int n = in()->length();
    for (int i = 0; i < n; i++) {
        Datum d;
        in()->read(&d, 1);
        d.x += dt;
        d.xp = BigDecimal::Nil;
        out()->write(&d, 1);
    }
}

//--

const char *TimeShiftNodeType::getDescription() const
{
    return "Shifts the input series in time by a constant: tout[k] = t[k] + dt";
}

void TimeShiftNodeType::getAttributes(StringMap& attrs) const
{
    attrs["dt"] = "the time shift";
}

Node *TimeShiftNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    double dt = atof(attrs["dt"].c_str());

    Node *node = new TimeShiftNode(dt);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

//------

bool LinearTrendNode::isReady() const
{
    return in()->length() > 0;
}

void LinearTrendNode::process()
{
    int n = in()->length();
    for (int i = 0; i < n; i++) {
        Datum d;
        in()->read(&d, 1);
        d.y += a * d.x;
        out()->write(&d, 1);
    }
}

//--

const char *LinearTrendNodeType::getDescription() const
{
    return "Adds linear component to input series: yout[k] = y[k] + a * t[k]";
}

void LinearTrendNodeType::getAttributes(StringMap& attrs) const
{
    attrs["a"] = "coeffient of linear component";
}

void LinearTrendNodeType::getAttrDefaults(StringMap& attrs) const
{
    attrs["a"] = "1.0";
}

Node *LinearTrendNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    double a = atof(attrs["a"].c_str());

    Node *node = new LinearTrendNode(a);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

void LinearTrendNodeType::mapVectorAttributes(  /*inout*/ StringMap& attrs,  /*out*/ StringVector& warnings) const
{
    if (attrs["type"] == "enum")
        warnings.push_back(std::string("Applying '") + getName() + "' to an enum");
    attrs["type"] = "double";
}

//-----

bool CropNode::isReady() const
{
    return in()->length() > 0;
}

void CropNode::process()
{
    int n = in()->length();
    for (int i = 0; i < n; i++) {
        Datum d;
        in()->read(&d, 1);
        if (d.x >= from && d.x <= to)
            out()->write(&d, 1);
    }
}

//--

const char *CropNodeType::getDescription() const
{
    return "Discards values outside the [t1, t2] interval";
}

void CropNodeType::getAttributes(StringMap& attrs) const
{
    attrs["t1"] = "'from' time";
    attrs["t2"] = "'to' time";
}

Node *CropNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    double t1 = atof(attrs["t1"].c_str());
    double t2 = atof(attrs["t2"].c_str());

    Node *node = new CropNode(t1, t2);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

//----

bool MeanNode::isReady() const
{
    return in()->length() > 0;
}

void MeanNode::process()
{
    int n = in()->length();
    for (int i = 0; i < n; i++) {
        Datum d;
        in()->read(&d, 1);
        sum += d.y;
        count++;
        d.y = sum/count;
        out()->write(&d, 1);
    }
}

void MeanNodeType::mapVectorAttributes(  /*inout*/ StringMap& attrs,  /*out*/ StringVector& warnings) const
{
    if (attrs["type"] == "enum")
        warnings.push_back(std::string("Applying '") + getName() + "' to an enum");
    attrs["type"] = "double";
}

//--

const char *MeanNodeType::getDescription() const
{
    return "Calculates mean on (0,t)";
}

void MeanNodeType::getAttributes(StringMap& attrs) const
{
}

Node *MeanNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new MeanNode();
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

//-----

bool RemoveRepeatsNode::isReady() const
{
    return in()->length() > 0;
}

void RemoveRepeatsNode::process()
{
    int n = in()->length();
    for (int i = 0; i < n; i++) {
        Datum d;
        in()->read(&d, 1);

        if (first || prevy != d.y) {
            first = false;
            prevy = d.y;
            out()->write(&d, 1);
        }
    }
}

//--

const char *RemoveRepeatsNodeType::getDescription() const
{
    return "Removes repeated y values";
}

void RemoveRepeatsNodeType::getAttributes(StringMap& attrs) const
{
}

void RemoveRepeatsNodeType::getAttrDefaults(StringMap& attrs) const
{
}

Node *RemoveRepeatsNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new RemoveRepeatsNode();
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

void RemoveRepeatsNodeType::mapVectorAttributes(  /*inout*/ StringMap& attrs,  /*out*/ StringVector& warnings) const
{
    // TODO interpolationmode
}

//-----

bool CompareNode::isReady() const
{
    return in()->length() > 0;
}

void CompareNode::process()
{
    int n = in()->length();
    for (int i = 0; i < n; i++) {
        Datum d;
        in()->read(&d, 1);

        if (d.y < threshold) {
            if (replaceIfLess)
                d.y = valueIfLess;
        }
        else if (d.y > threshold) {
            if (replaceIfGreater)
                d.y = valueIfGreater;
        }
        else {
            if (replaceIfEqual)
                d.y = valueIfEqual;
        }
        out()->write(&d, 1);
    }
}

//--

const char *CompareNodeType::getDescription() const
{
    return "Compares value against a threshold, and optionally replaces it with a constant";
}

void CompareNodeType::getAttributes(StringMap& attrs) const
{
    attrs["threshold"] = "constant to compare against";
    attrs["ifLess"] = "number to output if y < threshold (empty=no change)";
    attrs["ifEqual"] = "number to output if y == threshold (empty=no change)";
    attrs["ifGreater"] = "number to output if y > threshold (empty=no change)";
}

void CompareNodeType::getAttrDefaults(StringMap& attrs) const
{
}

Node *CompareNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    CompareNode *node = new CompareNode();
    node->setNodeType(this);

    node->setThreshold(atof(attrs["threshold"].c_str()));
    if (!opp_isblank(attrs["ifLess"].c_str()))
        node->setLessValue(atof(attrs["ifLess"].c_str()));
    if (!opp_isblank(attrs["ifEqual"].c_str()))
        node->setEqualValue(atof(attrs["ifEqual"].c_str()));
    if (!opp_isblank(attrs["ifGreater"].c_str()))
        node->setGreaterValue(atof(attrs["ifGreater"].c_str()));

    mgr->addNode(node);
    return node;
}

//-----

bool IntegrateNode::isReady() const
{
    return in()->length() > 0;
}

void IntegrateNode::process()
{
    int n = in()->length();
    for (int i = 0; i < n; i++) {
        Datum d;
        in()->read(&d, 1);

        if (!isPrevValid) {
            prevx = d.x;
            prevy = d.y;
            isPrevValid = true;
            d.y = 0;
            out()->write(&d, 1);
        }
        else {
            switch (interpolationmode) {
                case SAMPLE_HOLD:
                    integral += prevy * (d.x-prevx);
                    break;

                case BACKWARD_SAMPLE_HOLD:
                    integral += d.y * (d.x-prevx);
                    break;

                case LINEAR:
                    integral += (prevy+d.y)/2 * (d.x-prevx);
                    break;

                default:
                    Assert(false);
            }
            prevx = d.x;
            prevy = d.y;
            d.y = integral;
            out()->write(&d, 1);
        }
    }
}

//--

const char *IntegrateNodeType::getDescription() const
{
    return "Integrates the input as a step function (sample-hold or backward-sample-hold) or with linear interpolation";
}

void IntegrateNodeType::getAttributes(StringMap& attrs) const
{
    attrs["interpolation-mode"] = "sample-hold, backward-sample-hold, or linear";
}

void IntegrateNodeType::getAttrDefaults(StringMap& attrs) const
{
    attrs["interpolation-mode"] = "sample-hold";
}

Node *IntegrateNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    // TODO we should really support combobox selection on the UI for this...
    InterpolationMode mode;
    const std::string& modeString = attrs["interpolation-mode"];
    if (modeString == "" || modeString == "sample-hold")
        mode = SAMPLE_HOLD;
    else if (modeString == "backward-sample-hold")
        mode = BACKWARD_SAMPLE_HOLD;
    else if (modeString == "linear")
        mode = LINEAR;
    else
        throw opp_runtime_error("Unknown interpolation mode: %s", modeString.c_str());

    Node *node = new IntegrateNode(mode);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

void IntegrateNodeType::mapVectorAttributes(  /*inout*/ StringMap& attrs,  /*out*/ StringVector& warnings) const
{
    if (attrs["type"] == "enum")
        warnings.push_back(std::string("Applying '") + getName() + "' to an enum");
    attrs["type"] = "double";
}

//-----

bool TimeAverageNode::isReady() const
{
    return in()->length() > 0;
}

void TimeAverageNode::process()
{
    int n = in()->length();
    for (int i = 0; i < n; i++) {
        Datum d;
        in()->read(&d, 1);

        if (!isPrevValid) {
            prevx = d.x;
            prevy = d.y;
            isPrevValid = true;
        }
        else {
            switch (interpolationmode) {
                case SAMPLE_HOLD:
                    integral += prevy * (d.x-prevx);
                    break;

                case BACKWARD_SAMPLE_HOLD:
                    integral += d.y * (d.x-prevx);
                    break;

                case LINEAR:
                    integral += (prevy+d.y)/2 * (d.x-prevx);
                    break;

                default:
                    Assert(false);
            }
            prevx = d.x;
            prevy = d.y;
            if (d.x != startx) {  // suppress 0/0 = NaN values
                d.y = integral / (d.x - startx);
                out()->write(&d, 1);
            }
        }
    }
}

//--

const char *TimeAverageNodeType::getDescription() const
{
    return "Calculates the time average of the input (integral divided by time)";
}

void TimeAverageNodeType::getAttributes(StringMap& attrs) const
{
    attrs["interpolation-mode"] = "sample-hold, backward-sample-hold, or linear";
}

void TimeAverageNodeType::getAttrDefaults(StringMap& attrs) const
{
    attrs["interpolation-mode"] = "sample-hold";
}

Node *TimeAverageNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    // TODO we should really support combobox selection on the UI for this...
    InterpolationMode mode;
    const std::string& modeString = attrs["interpolation-mode"];
    if (modeString == "" || modeString == "sample-hold")
        mode = SAMPLE_HOLD;
    else if (modeString == "backward-sample-hold")
        mode = BACKWARD_SAMPLE_HOLD;
    else if (modeString == "linear")
        mode = LINEAR;
    else
        throw opp_runtime_error("Unknown interpolation mode: %s", modeString.c_str());

    Node *node = new TimeAverageNode(mode);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

void TimeAverageNodeType::mapVectorAttributes(  /*inout*/ StringMap& attrs,  /*out*/ StringVector& warnings) const
{
    if (attrs["type"] == "enum")
        warnings.push_back(std::string("Applying '") + getName() + "' to an enum");
    attrs["type"] = "double";
}

//-----

bool DivideByTimeNode::isReady() const
{
    return in()->length() > 0;
}

void DivideByTimeNode::process()
{
    int n = in()->length();
    for (int i = 0; i < n; i++) {
        Datum d;
        in()->read(&d, 1);
        d.y /= d.x;
        out()->write(&d, 1);
    }
}

//--

const char *DivideByTimeNodeType::getDescription() const
{
    return "Divides input by the current time: yout[k] = y[k] / t[k]";
}

void DivideByTimeNodeType::getAttributes(StringMap& attrs) const
{
}

Node *DivideByTimeNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new DivideByTimeNode();
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

void DivideByTimeNodeType::mapVectorAttributes(  /*inout*/ StringMap& attrs,  /*out*/ StringVector& warnings) const
{
    if (attrs["type"] == "enum")
        warnings.push_back(std::string("Applying '") + getName() + "' to an enum");
    attrs["type"] = "double";
}

//---

bool TimeToSerialNode::isReady() const
{
    return in()->length() > 0;
}

void TimeToSerialNode::process()
{
    int n = in()->length();
    for (int i = 0; i < n; i++) {
        Datum d;
        in()->read(&d, 1);
        d.x = serial;
        d.xp = BigDecimal(serial);
        serial++;
        out()->write(&d, 1);
    }
}

//--

const char *TimeToSerialNodeType::getDescription() const
{
    return "Replaces time values with their index: tout[k] = k";
}

void TimeToSerialNodeType::getAttributes(StringMap& attrs) const
{
}

Node *TimeToSerialNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new TimeToSerialNode();
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

void TimeToSerialNodeType::mapVectorAttributes(  /*inout*/ StringMap& attrs,  /*out*/ StringVector& warnings) const
{
}

//-----

bool SubtractFirstValueNode::isReady() const
{
    return in()->length() > 0;
}

void SubtractFirstValueNode::process()
{
    int n = in()->length();
    Datum d;

    int i = 0;
    for ( ; !firstValueSeen && i < n; i++) {
        in()->read(&d, 1);
        if (!isNaN(d.y) && !isPositiveInfinity(d.y) && !isNegativeInfinity(d.y)) {
            firstValue = d.y;
            d.y = 0.0;
            firstValueSeen = true;
        }
        out()->write(&d, 1);
    }

    for ( ; i < n; i++) {
        in()->read(&d, 1);
        d.y -= firstValue;
        out()->write(&d, 1);
    }
}

//--

const char *SubtractFirstValueNodeType::getDescription() const
{
    return "Subtract the first value from every subsequent values: yout[k] = y[k] - y[0]";
}

void SubtractFirstValueNodeType::getAttributes(StringMap& attrs) const
{
}

Node *SubtractFirstValueNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new SubtractFirstValueNode();
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

void SubtractFirstValueNodeType::mapVectorAttributes(  /*inout*/ StringMap& attrs,  /*out*/ StringVector& warnings) const
{
    if (attrs["type"] == "enum")
        warnings.push_back(std::string("Applying '") + getName() + "' to an enum");
    else if (attrs["type"] == "")
        attrs["type"] = "double";
}

}  // namespace scave
}  // namespace omnetpp


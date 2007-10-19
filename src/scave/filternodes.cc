//=========================================================================
//  FILTERNODES.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <math.h>
#include "channel.h"
#include "filternodes.h"
#include "stringutil.h"


bool NopNode::isReady() const
{
    return in()->length()>0;
}

void NopNode::process()
{
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum d;
        in()->read(&d,1);
        out()->write(&d,1);
    }
}

//--

const char *NopNodeType::description() const
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
    return in()->length()>0;
}

void AdderNode::process()
{
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum d;
        in()->read(&d,1);
        d.y += c;
        out()->write(&d,1);
    }
}

//--

const char *AdderNodeType::description() const
{
    return "Adds a constant to the input: y(t) = x(t)+c";
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

//-----

bool MultiplierNode::isReady() const
{
    return in()->length()>0;
}

void MultiplierNode::process()
{
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum d;
        in()->read(&d,1);
        d.y *= a;
        out()->write(&d,1);
    }
}

//--

const char *MultiplierNodeType::description() const
{
    return "Multiplies input by a constant: y(t) = a*x(t)";
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

//-----

bool DividerNode::isReady() const
{
    return in()->length()>0;
}

void DividerNode::process()
{
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum d;
        in()->read(&d,1);
        d.y /= a;
        out()->write(&d,1);
    }
}

//--

const char *DividerNodeType::description() const
{
    return "Divides input by a constant: y(t) = x(t)/a";
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

//-----

bool ModuloNode::isReady() const
{
    return in()->length()>0;
}

void ModuloNode::process()
{
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        //TODO: when floor(y/a)!=floor(prevy/a), insert a NaN! so they won't get connected on the line chart
        Datum d;
        in()->read(&d,1);
        d.y -= floor(d.y/a)*a;
        out()->write(&d,1);
    }
}

//--

const char *ModuloNodeType::description() const
{
    return "Computes input modulo a constant: y(t) = x(t)%a";
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

//-----

bool DifferenceNode::isReady() const
{
    return in()->length()>0;
}

void DifferenceNode::process()
{
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum d;
        in()->read(&d,1);

        double tmp = d.y;
        d.y -= prevy;
        prevy = tmp;

        out()->write(&d,1);
    }
}

//--

const char *DifferenceNodeType::description() const
{
    return "Substracts the previous value from every value: y[k] = y[k] - y[k-1]";
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

//-----

bool TimeDiffNode::isReady() const
{
    return in()->length()>0;
}

void TimeDiffNode::process()
{
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum d;
        in()->read(&d,1);

        d.y = d.x - prevx;
        prevx = d.x;

        out()->write(&d,1);
    }
}

//--

const char *TimeDiffNodeType::description() const
{
    return "Returns the difference in time between this and the previous value: y[k] = x[k] - x[k-1]";
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

//-----

MovingAverageNode::MovingAverageNode(double alph)
{
    firstRead = true;
    prevy=0;
    alpha = alph;
}

bool MovingAverageNode::isReady() const
{
    return in()->length()>0;
}

void MovingAverageNode::process()
{
    if (firstRead)
    {
        Datum d;
        in()->read(&d,1);
        prevy = d.y;
        out()->write(&d,1);
        firstRead = false;
    }

    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum d;
        in()->read(&d,1);
        d.y = prevy = prevy + alpha*(d.y-prevy);
        out()->write(&d,1);
    }
}

//--

const char *MovingAverageNodeType::description() const
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

//-----

bool SumNode::isReady() const
{
    return in()->length()>0;
}

void SumNode::process()
{
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum d;
        in()->read(&d,1);

        sum += d.y;
        d.y = sum;

        out()->write(&d,1);
    }
}

//--

const char *SumNodeType::description() const
{
    return "Sums up values: y[k] = SUM(y[i], i=0..k)";
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

//------

bool TimeShiftNode::isReady() const
{
    return in()->length()>0;
}

void TimeShiftNode::process()
{
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum d;
        in()->read(&d,1);
        d.x += dt;
        d.xp = BigDecimal::Nil;
        out()->write(&d,1);
    }
}

//--

const char *TimeShiftNodeType::description() const
{
    return "Shifts the input series in time by a constant: y(t) = x(t-dt)";
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
    return in()->length()>0;
}

void LinearTrendNode::process()
{
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum d;
        in()->read(&d,1);
        d.y += a * d.x;
        out()->write(&d,1);
    }
}

//--

const char *LinearTrendNodeType::description() const
{
    return "Adds linear component to input series: y(t) = x(t) + a*t";
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

//-----

bool CropNode::isReady() const
{
    return in()->length()>0;
}

void CropNode::process()
{
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum d;
        in()->read(&d,1);
        if (d.x >= from && d.x <= to)
            out()->write(&d,1);
    }
}

//--

const char *CropNodeType::description() const
{
    return "Discards values outside the [t1, t2] interval";
}

void CropNodeType::getAttributes(StringMap& attrs) const
{
    attrs["t1"] = "`from' time";
    attrs["t2"] = "`to' time";
}

Node *CropNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    double t1 = atof(attrs["t1"].c_str());
    double t2 = atof(attrs["t2"].c_str());

    Node *node = new CropNode(t1,t2);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

//----

bool MeanNode::isReady() const
{
    return in()->length()>0;
}

void MeanNode::process()
{
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum d;
        in()->read(&d,1);
        sum += d.y;
        count++;
        d.y = sum/count;
        out()->write(&d,1);
    }
}

//--

const char *MeanNodeType::description() const
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
    return in()->length()>0;
}

void RemoveRepeatsNode::process()
{
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum d;
        in()->read(&d,1);

        if (first || prevy != d.y) {
            first = false;
            prevy = d.y;
            out()->write(&d,1);
        }
    }
}

//--

const char *RemoveRepeatsNodeType::description() const
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

//-----

bool ComparatorNode::isReady() const
{
    return in()->length()>0;
}

void ComparatorNode::process()
{
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum d;
        in()->read(&d,1);

        if (d.y < threshold)
        {
             if (replaceIfLess)
                 d.y = valueIfLess;
        }
        else if (d.y > threshold)
        {
             if (replaceIfGreater)
                 d.y = valueIfGreater;
        }
        else
        {
             if (replaceIfEqual)
                 d.y = valueIfEqual;
        }
        out()->write(&d,1);
    }
}

//--

const char *ComparatorNodeType::description() const
{
    return "Compares value against a constant, and optionally replaces "
           "it with a constant if less, equal or greater.";
}

void ComparatorNodeType::getAttributes(StringMap& attrs) const
{
    attrs["threshold"] = "constaint to compare against";
    attrs["ifLess"] = "number to output if y < threshold (empty=no change)";
    attrs["ifEqual"] = "number to output if y == threshold (empty=no change)";
    attrs["ifGreater"] = "number to output if y > threshold (empty=no change)";
}

void ComparatorNodeType::getAttrDefaults(StringMap& attrs) const
{
}

Node *ComparatorNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    ComparatorNode *node = new ComparatorNode();
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

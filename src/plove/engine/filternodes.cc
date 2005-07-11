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

Node *NopNodeType::create(DataflowManager *, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new NopNode();
    node->setNodeType(this);
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

Node *AdderNodeType::create(DataflowManager *, StringMap& attrs) const
{
    checkAttrNames(attrs);

    double c = atof(attrs["c"].c_str());

    Node *node = new AdderNode(c);
    node->setNodeType(this);
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

Node *MultiplierNodeType::create(DataflowManager *, StringMap& attrs) const
{
    checkAttrNames(attrs);

    double a = atof(attrs["a"].c_str());

    Node *node = new MultiplierNode(a);
    node->setNodeType(this);
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

Node *DividerNodeType::create(DataflowManager *, StringMap& attrs) const
{
    checkAttrNames(attrs);

    double a = atof(attrs["a"].c_str());

    Node *node = new DividerNode(a);
    node->setNodeType(this);
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

Node *ModuloNodeType::create(DataflowManager *, StringMap& attrs) const
{
    checkAttrNames(attrs);

    double a = atof(attrs["a"].c_str());

    Node *node = new ModuloNode(a);
    node->setNodeType(this);
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

Node *DifferenceNodeType::create(DataflowManager *, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new DifferenceNode();
    node->setNodeType(this);
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

Node *MovingAverageNodeType::create(DataflowManager *, StringMap& attrs) const
{
    checkAttrNames(attrs);

    double alpha = atof(attrs["alpha"].c_str());

    Node *node = new MovingAverageNode(alpha);
    node->setNodeType(this);
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

Node *SumNodeType::create(DataflowManager *, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new SumNode();
    node->setNodeType(this);
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

Node *TimeShiftNodeType::create(DataflowManager *, StringMap& attrs) const
{
    checkAttrNames(attrs);

    double dt = atof(attrs["dt"].c_str());

    Node *node = new TimeShiftNode(dt);
    node->setNodeType(this);
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

Node *LinearTrendNodeType::create(DataflowManager *, StringMap& attrs) const
{
    checkAttrNames(attrs);

    double a = atof(attrs["a"].c_str());

    Node *node = new LinearTrendNode(a);
    node->setNodeType(this);
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

Node *CropNodeType::create(DataflowManager *, StringMap& attrs) const
{
    checkAttrNames(attrs);

    double t1 = atof(attrs["t1"].c_str());
    double t2 = atof(attrs["t2"].c_str());

    Node *node = new CropNode(t1,t2);
    node->setNodeType(this);
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

Node *MeanNodeType::create(DataflowManager *, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new MeanNode();
    node->setNodeType(this);
    return node;
}


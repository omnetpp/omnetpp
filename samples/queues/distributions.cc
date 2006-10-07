//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Contributed by Nick van Foreest
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>

double deterministic(double a)
{
  return (a);
}

double perturbedExponential(double a, double b)
{
    return ( exponential(a) + uniform(0,b));
}


Define_Function( deterministic, 1);
Define_Function( perturbedExponential, 2);

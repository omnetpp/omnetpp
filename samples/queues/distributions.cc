//-------------------------------------------------------------
// file: distributions.cc
//        (part of Queues - an OMNeT++ demo simulation)
//
// Here we introduce some new distribution functions.
//-------------------------------------------------------------

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

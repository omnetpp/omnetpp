//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// A module to demonstrate the usage of the histogram classes.
//
// It generates a number of random observations, and collects them
// in various histogram-like objects: floating-point and integer
// equal-bin histograms (cDoubleHistogram, cLongHistogram),
// variable-bin histogram (cVarHistogram), the P^2 and k-split
// algorithms (cPSquare, cKSplit).
//
simple HistogramDemo
{
    parameters:
        int numObservations;
        int numCells;
        bool useFixedRange;
        int numFirstVals;
        double rangeExtFactor;
}


//
// Instantiates HistogramDemo as network consisting of just one single-module.
//
network Histograms extends HistogramDemo
{
    parameters:
        numObservations = default(10000) @prompt("Number of observations: ");
        numCells = default(20) @prompt("Number of histogram cells: ");
        useFixedRange = default(false) @prompt("Auto (false) or fixed (true) histogram range? ");
        numFirstVals = default(50) @prompt("Number of observations used for range estimation: ");
        rangeExtFactor = default(1.3) @prompt("Range extension factor (usually 1.0 .. 2): ");
}




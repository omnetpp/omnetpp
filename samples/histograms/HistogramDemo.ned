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
        int numObservations = default(10000) @prompt("Number of observations");
        int numCells = default(20) @prompt("Number of histogram cells");
        bool useFixedRange = default(false) @prompt("Auto (false) or fixed (true) histogram range? ");
        int numFirstVals = default(50) @prompt("Number of observations used for range estimation");
        double rangeExtFactor = default(1.3) @prompt("Range extension factor (usually 1.0 .. 2)");
}



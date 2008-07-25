//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2008 Andras Varga
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
        @isNetwork;
        int numObservations @prompt("Number of observations") = default(10000);
        int numCells @prompt("Number of histogram cells") = default(20);
        bool useFixedRange @prompt("Auto (false) or fixed (true) histogram range? ") = default(false);
        int numFirstVals @prompt("Number of observations used for range estimation") = default(50);
        double rangeExtFactor @prompt("Range extension factor (usually 1.0 .. 2)") = default(1.3);
}



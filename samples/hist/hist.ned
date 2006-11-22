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
        int num_obs;
        int num_cells;
        bool fixed_range;
        int num_first;
        double range_ext;
}


//
// Instantiates HistogramDemo as network consisting of just one single-module.
//
network Histograms extends HistogramDemo
{
    parameters:
        num_obs = default(10000) @prompt("Number of observations: ");
        num_cells = default(20) @prompt("Number of histogram cells: ");
        fixed_range = default(false) @prompt("Auto (false) or fixed (true) histogram range? ");
        num_first = default(50) @prompt("Number of observations used for range estimation: ");
        range_ext = default(1.3) @prompt("Range extension factor (usually 1.0 .. 2): ");
}




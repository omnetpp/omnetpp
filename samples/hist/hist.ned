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
    parameters:
        num_obs : numeric,
        num_cells : numeric,
        fixed_range : numeric,
        num_first : numeric,
        range_ext : numeric;
endsimple


//
// Instantiates HistogramDemo as network consisting of just one single-module.
//
network Histograms : HistogramDemo
    parameters:
        num_obs = input(10000, "Number of observations: "),
        num_cells = input( 20, "Number of histogram cells: "),
        fixed_range = input( 0, "Auto (0) or fixed (1) histogram range? "),
        num_first = input( 50, "Number of observations used for range estimation: "),
        range_ext = input( 1.3, "Range extension factor (usually 1.0 .. 2): ");
endnetwork




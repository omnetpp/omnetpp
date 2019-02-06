//
// This file is part of OMNeT++/OMNEST.
//
// Copyright (C) 1992-2018 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// A module to exercise histogram classes.
//
// It generates a number of random observations, and collects them
// into several differently configured histogram objects, and
// implementations of P^2 and k-split algorithms (cPSquare, cKSplit).
//
simple Histograms
{
    parameters:
        @isNetwork;
        @signal[unweightedValue](type=double);
        @statistic[unweightedValue](record=vector,histogram,ksplit,psquare);
        int count = default(-1); // -1 for none
        bool weighted = default(false);
        volatile double variable; // source of random numbers
        volatile double weight = default(1); // weight for random numbers
}


//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Classifies messages by message kind. Messages with kind=0 are sent
// out on gate out[0], those with kind=1 on out[1], etc.; other messages
// on gate "other".
//
simple QClassifier
    gates:
        in: in;
        out: out[];
        out: other;
endsimple



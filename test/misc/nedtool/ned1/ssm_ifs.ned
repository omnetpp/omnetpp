//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1998-2015 Andras Varga
// Copyright (C) 1996-97 Gabor Lencse, T.U.Budapest, Dept. of Telecommunications
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// SSM input interface: generates messages on the basis of statistics
//
simple SSM_IIF
    gates:
        in: in;
        out: out;
endsimple


//
// SSM output interface: collects statistics from messages
//
simple SSM_OIF
    parameters:
        OIFControlFile: string,
        frequency: numeric;
    gates:
        in: in;
        out: out;
endsimple

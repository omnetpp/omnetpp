//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 1992-2003 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Bursty message source
//
simple FF2BurstyGenerator
    parameters:
        sleep_time_mean : numeric,
        burst_time_mean : numeric,
        send_ia_time : numeric,
        msg_length : numeric;
    gates:
        out: out;
endsimple



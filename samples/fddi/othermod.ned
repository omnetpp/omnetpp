//==========================================================================
//  OTHERMOD.NED - Simple module declarations for
//                 Discrete System Simulation in OMNeT++
//
//  Author: Gabor.Lencse@hit.bme.hu
//==========================================================================

//--------------------------------------------------------------*
//  Copyright (C) 1996,97 Gabor Lencse,
//  Technical University of Budapest, Dept. of Telecommunications,
//  Stoczek u.2, H-1111 Budapest, Hungary.
//
//  This file is distributed WITHOUT ANY WARRANTY. See the file
//  `license' for details on this and other legal matters.
//--------------------------------------------------------------*/


// FDDI_Generator --
//
// generates messages for an FDDI Station
//
simple FDDI_Generator
    parameters:
        StationID : numeric,
        address : string;
    gates:
        out: out;
endsimple


// FDDI_Address_Generator --
//
// generates new addresses in an FDDI Router
//
simple FDDI_Address_Generator
    parameters:
        StationID : numeric,
        address : string;
    gates:
        in: in;
        out: out;
endsimple


// FDDI_Generator4Ring --
//
// generates messages
//
simple FDDI_Generator4Ring
    parameters:
        no_msg : numeric,
        no_comps : numeric const,
        wait_time : numeric;
    gates:
        out: out;
endsimple


// FDDI_Generator4Sniffer --
//
// generates messages for an FDDI Sniffer
//
simple FDDI_Generator4Sniffer
    parameters:
        StationID : numeric,
        address : string;
    gates:
        out: out;
endsimple


// Stat --
//
// keeps track of statistical info
//
simple Stat
    parameters:
        no_comps : numeric const; // it was: const no_comps;
    gates:
        in: in[];
endsimple


// FDDI_Sink --
//
// disposes the messages
//
simple FDDI_Sink
    gates:
        in: in;
endsimple


// FDDI_Monitor --
//
// monitors the network
//
simple FDDI_Monitor
    gates:
        in: in;
        // out: out;
endsimple


// LoadControl --
//
// Ringwise Load Control: Changes the value of LoadMultiplier
//
simple LoadControl
    parameters:
        LoadControlFile : string;
endsimple



//==========================================================================
//  FDDI_MAC.NED - Simple module declarations of the FDDI MAC model for
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


// FDDI_MAC --
//
// acts as an the Media Access Control system of an FDDI station
//
simple FDDI_MAC
    parameters:
        StationID : numeric,
        address : string;
    gates:
        in: in;
        in: from_LLC;
        out: out;
        out: to_LLC;
endsimple


// FDDI_MAC4Ring --
//
// acts as an the Media Access Conrol system of
// an FDDI station in a simple ring of identical nodes
//
simple FDDI_MAC4Ring
    parameters:
        no_comps : numeric const,
        no_msg : numeric;
    gates:
        in: in;
        in: from_LLC;
        out: out;
        out: to_LLC;
endsimple


// FDDI_MAC4Sniffer --
//
// acts as an the Media Access Conrol system of
// an FDDI Sniffer
//
simple FDDI_MAC4Sniffer
    parameters:
        StationID : numeric,
        address : string;
    gates:
        in: in;
        in: from_LLC;
        out: out;
        out: to_LLC;
endsimple


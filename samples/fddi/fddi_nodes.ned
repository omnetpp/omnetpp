//==========================================================================
//  FDDI_NODES.NED  -  Network Description file for
//                     Discrete System Simulation in OMNeT++
//
//  Simulated system: TUB Northen&Southen FDDI Rings
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
//--------------------------------------------------------------*

import
    "fddi_mac",
    "ssm_ifs",
    "othermod";


// FDDICable --
//
// used in the ring of identical nodes
//
channel FDDICable
    error 0;
    delay 1 us; // delay of a 200m long fiber
endchannel


// FDDI_SAC_inside_wire --
//
// represents the latency of a SAC
//
channel FDDI_SAC_inside_wire
    error 0;
    delay 100 ns; // equals to the station_latency of an FDDI station
endchannel


// FDDI_SAC --
//
// acts as a Single Attachment Concentrator
//
module FDDI_SAC
    parameters:
        StationID : numeric,
        address : string;
    gates:
        in: S_in;
        in: M_in[];
        out: S_out;
        out: M_out[];

    connections:
        S_in --> FDDI_SAC_inside_wire --> M_out[0];
        for i=0..sizeof(M_in)-2 do
            M_in[i] --> FDDI_SAC_inside_wire --> M_out[i+1];
        endfor;
        M_in[sizeof(M_in)-1] --> FDDI_SAC_inside_wire --> S_out;

endmodule


// FDDIStation --
//
// a node in a ring of identical nodes
//
module FDDIStation
    gates:
        in: in;
        out: out;
        out: to_stat;
    submodules:
        MAC: FDDI_MAC4Ring;
            parameters:
                no_comps = ancestor no_comps,
                no_msg = ancestor no_msg;
        gen: FDDI_Generator4Ring;
            parameters:
                no_msg = ancestor no_msg,
                no_comps = ancestor no_comps,
                wait_time = ancestor wait_time;
    connections:
        in --> MAC.in;
        MAC.out --> out;
        MAC.to_LLC --> to_stat;
        gen.out --> MAC.from_LLC;

endmodule


// FDDI_SAS --
//
// a Single Attachment Station in the network
//
module FDDI_SAS
    parameters:
        FDDI_Generator_type : string,
        StationID : numeric,
        address : string;
    gates:
        in: in;
        out: out;
    submodules:
        MAC: FDDI_MAC;
            parameters:
                StationID = StationID,
                address = address;
        gen: FDDI_Generator_type like FDDI_Generator;
            parameters:
                StationID = StationID,
                address = address;
        sink: FDDI_Sink;

    connections:
        in --> MAC.in;
        MAC.out --> out;
        MAC.to_LLC --> sink.in;
        gen.out --> MAC.from_LLC;

endmodule


// FDDI_Sniffer --
//
// An FDDI Sniffer (a special SAS)
//
module FDDI_Sniffer
    parameters:
        StationID : numeric,
        address : string;
    gates:
        in: in;
        out: out;
    submodules:
        MAC: FDDI_MAC4Sniffer;
            parameters:
                StationID = StationID,
                address = address;
        gen: FDDI_Generator4Sniffer;
            parameters:
                StationID = StationID,
                address = address;
        monitor: FDDI_Monitor;

    connections:
        in --> MAC.in;
        MAC.out --> out;
        MAC.to_LLC --> monitor.in;
        gen.out --> MAC.from_LLC;

endmodule


// FDDI_Router_port --
//
// Makes routing decision on the basis of the length distribution
// of the packets from this station to other stations in the ring.
// (a special SAS: has no Generator and Sink, interconnects rings)
//
module FDDI_Router_port
    parameters:
        StationID : numeric,
        address : string;
    gates:
        in: in;
        in: ring_in;
        out: out;
        out: ring_out;
    submodules:
        MAC: FDDI_MAC;
            parameters:
                StationID = StationID,
                address = address;
        addr_gen: FDDI_Address_Generator;
            parameters:
                StationID = StationID,
                address = address;
        // sink: FDDI_Sink;

    connections:
        in --> MAC.in;
        MAC.out --> out;
        MAC.to_LLC --> ring_out;
        ring_in --> addr_gen.in;
        addr_gen.out --> MAC.from_LLC;

endmodule


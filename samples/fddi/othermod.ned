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

simple FDDI_Generator // generates messages for an FDDI Station
        machines:
                host;
        parameters:
                StationID: numeric,
                address: string;
        gates:
                out: out;
endsimple

simple FDDI_Address_Generator // generates new addresses in an FDDI Router
        machines:
                host;
        parameters:
                StationID: numeric,
                address: string;
        gates:
                in: in;
                out: out;
endsimple

simple FDDI_Generator4Ring // generates messages
        parameters:
                no_msg, const no_comps, wait_time; // it was const no_comps
        gates:
                out: out;
endsimple

simple FDDI_Generator4Sniffer // generates messages for an FDDI Sniffer
        machines:
                host;
        parameters:
                StationID: numeric,
                address: string;
        gates:
                out: out;
endsimple

simple Stat // keeps track of statistical info
        parameters:
                const no_comps; //it was: const no_comps;
        gates:
                in: in[];
endsimple

simple FDDI_Sink // disposes the messages
        machines:
                host;
        gates:
                in: in;
endsimple

simple FDDI_Monitor // monitors the network
        machines:
                host;
        gates:
                in: in;
                //out: out;
endsimple

simple LoadControl // Ringwise Load Control: Changes the value of LoadMultiplier
        machines:
                host;
        parameters:
                LoadControlFile: string;
endsimple

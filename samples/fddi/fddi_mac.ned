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

simple FDDI_MAC // acts as an the Media Access Control system of an FDDI station
        machines:
                host;
        parameters:
                StationID: numeric,
                address: string;
        gates:
                in: in, from_LLC;
                out: out, to_LLC;
endsimple

simple FDDI_MAC4Ring // acts as an the Media Access Conrol system of
                     // an FDDI station in a simple ring of identical nodes
        parameters:
                const no_comps,
                no_msg;
        gates:
                in: in, from_LLC;
                out: out, to_LLC;
endsimple

simple FDDI_MAC4Sniffer // acts as an the Media Access Conrol system of
                        // an FDDI Sniffer
        machines:
                host;
        parameters:
                StationID: numeric,
                address: string;
        gates:
                in: in, from_LLC;
                out: out, to_LLC;
endsimple

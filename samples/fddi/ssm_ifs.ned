//==========================================================================
//  SSM_IFS.NED  - Simple module declarations of the SSM I/O interfaces for
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

simple SSM_IIF // input interface: generates messages on the basis of statistics
        gates:
                in: in;
                out: out;
endsimple

simple SSM_OIF // output interface: collects statistics from messages
        parameters:
                OIFControlFile: string,
                frequency: numeric;
        gates:
                in: in;
                out: out;
endsimple

//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 1992-2003 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Host
//
simple Host
    parameters:
        txRate: numeric const,
        radioDelay: numeric const,
        pkLenBits: numeric,
        iaTime: numeric;
endsimple

//
// This is the central computer in the ALOHAnet network.
//
simple Server
    parameters:
        txRate: numeric;
    gates:
        in: in;
endsimple

module ALOHAnet
    parameters:
        numHosts: numeric,
        txRate: numeric;
    submodules:
        server: Server;
            parameters:
                txRate = txRate;
            display: "i=server1";
        host: Host[numHosts];
            parameters:
                txRate = txRate;
            display: "i=ball_s";
endmodule

network alohaNet : ALOHAnet
endnetwork


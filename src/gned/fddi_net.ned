//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1998-2005 Andras Varga
// Copyright (C) 1996-97 Gabor Lencse, T.U.Budapest, Dept. of Telecommunications
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


import
    "fddi_mac",
    "ssm_ifs",
    "othermod",
    "fddi_nodes";


//
// A simple FDDI ring of identical FDDIStation nodes. The stations are
// connected in the form of a ring, plus each of them is connected to the
// stat module.
//
module FDDIRing
    parameters:
        no_comps : numeric const,
        no_msg : numeric,
        wait_time : numeric,
        RingID : numeric const,
        TTRT : numeric const;
    submodules:
        comp: FDDIStation[no_comps];

        stat: Stat;
            parameters:
                no_comps = no_comps;
            gatesizes:
                in[no_comps];
    connections:
        comp[no_comps - 1].out --> FDDICable --> comp[0].in;
        for i=0..no_comps - 2 do
            comp[i].out --> FDDICable --> comp[i + 1].in;
        endfor;
        for i=0..no_comps - 1 do
            comp[i].to_stat --> stat.in[i];
        endfor;
endmodule


//
// Technical University of Budapest Northen FDDI ring.
//
// The stations are connected in the form of a ring plus there are some
// concentrators. From the dual attached ring only one direction is modelled;
// all the stations are modelled just with SAS, and all the concentrators are
// modelled with SAC-4.
//
module TUBNRing
    parameters:
        LoadControlFile : string,
        FDDI_Generator_type : string,
        RingID : numeric const,
        TTRT : numeric const,
        LoadMultiplier : numeric;
    gates:
        in: in;
        out: out;
    submodules:
        loadCtrl: LoadControl;
            parameters:
                LoadControlFile = LoadControlFile;
            display: "p=50,50";
        bmecisco7: FDDI_Router_port; // in Building R, EIK
            parameters:
                //      FDDI_Generator_type=FDDI_Generator_type,
                StationID = 0,
                address = "DECnet000728";
            display: "p=100,125;i=router";
        bmecisco1: FDDI_SAS; // in Building R, EIK
            parameters:
                FDDI_Generator_type = FDDI_Generator_type,
                StationID = 1,
                address = "DECnet000A04";
            display: "p=150,50;i=router";
        bmecisco3: FDDI_SAS; // in Building R, EIK
            parameters:
                FDDI_Generator_type = FDDI_Generator_type,
                StationID = 2,
                address = "cisco_F99030";
            display: "p=250,50;i=router";
        bmecisco2: FDDI_SAS; // in Building K
            parameters:
                FDDI_Generator_type = FDDI_Generator_type,
                StationID = 3,
                address = "DECnet000B04";
            display: "p=350,50;i=router";
        bmeconc5: FDDI_SAC; // in Building K
            parameters:
                StationID = -1,
                address = "DEC___26AF0D";
            gatesizes:
                M_in[1],
                M_out[1];
            display: "p=400,125;i=ipc";
        bmebr1: FDDI_SAS; // in Building CH
            parameters:
                FDDI_Generator_type = FDDI_Generator_type,
                StationID = 4,
                address = "DEC___28E6AD";
            display: "p=430,70;i=ipc";
        bmeconc4: FDDI_SAC; // in Building Mt
            parameters:
                StationID = -1,
                address = "DEC___18DB64";
            gatesizes:
                M_in[4],
                M_out[4];
            display: "p=400,200;i=ipc";
        bmebr2: FDDI_SAS; // in Building F
            parameters:
                FDDI_Generator_type = FDDI_Generator_type,
                StationID = 5,
                address = "DEC___28E6AC";
            display: "p=410,260;i=ipc";
        ethsw1: FDDI_SAS; // in Building MM
            parameters:
                FDDI_Generator_type = FDDI_Generator_type,
                StationID = 6,
                address = "00C01DF447A6";
            display: "p=460,260;i=ipc";
        ethsw2: FDDI_SAS; // in Building MM
            parameters:
                FDDI_Generator_type = FDDI_Generator_type,
                StationID = 7,
                address = "00C01DF44885";
            display: "p=460,220;i=ipc";
        fizika2: FDDI_SAS; // in Building F
            parameters:
                FDDI_Generator_type = FDDI_Generator_type,
                StationID = 8,
                address = "DEC___B48FE4";
            display: "p=460,180;i=ipc";
        bmeconc3: FDDI_SAC; // in Building St
            parameters:
                StationID = -1,
                address = "DEC___26B05D";
            gatesizes:
                M_in[1],
                M_out[1];
            display: "p=360,275;i=ipc";
        bmebr3: FDDI_SAS; // in Building J
            parameters:
                FDDI_Generator_type = FDDI_Generator_type,
                StationID = 9,
                address = "DEC___286606";
            display: "p=360,330;i=ipc";
        bmeconc2: FDDI_SAC; // in Building R, EIK
            parameters:
                StationID = -1,
                address = "DEC___314A73";
            gatesizes:
                M_in[3],
                M_out[3];
            display: "p=240,275;i=ipc";
        Challenge: FDDI_SAS; // in Building R, EIK
            parameters:
                FDDI_Generator_type = FDDI_Generator_type,
                StationID = 10,
                address = "SGI___0409E8";
            display: "p=190,330;i=comp";
        gibr: FDDI_SAS; // in Building D
            parameters:
                FDDI_Generator_type = FDDI_Generator_type,
                StationID = 11,
                address = "DEC___28691D";
            display: "p=240,330;i=comp";
        sniffer: FDDI_Sniffer; // in Building R, EIK
            parameters:
                StationID = 100,
                address = "This_Monitor";
            display: "p=290,330;i=comp";
        bmeconc1: FDDI_SAC; // in Building R, EIK
            parameters:
                StationID = -1,
                address = "DEC___286C41";
            gatesizes:
                M_in[3],
                M_out[3];
            display: "p=100,200;i=ipc";
        Delfin: FDDI_SAS; // in Building R, EIK
            parameters:
                FDDI_Generator_type = FDDI_Generator_type,
                StationID = 12,
                address = "IBM___1DE0BA";
            display: "p=50,240;i=comp";
        Goliat: FDDI_SAS; // in Building R, EIK
            parameters:
                FDDI_Generator_type = FDDI_Generator_type,
                StationID = 13,
                address = "Sun___1BD035";
            display: "p=100,265;i=comp";
        bigmac: FDDI_SAS; // in Building R, EIK
            parameters:
                FDDI_Generator_type = FDDI_Generator_type,
                StationID = 14,
                address = "DEC___B11ECF";
            display: "p=150,270;i=comp";
    connections:
        // Propagation delay is calculated from the cable length [in meter].
        // In glass, light travels at approximately 200 m/microseconds.
        // Within a building no exact cable lengths used as they are very
        // short. They are just modelled with a 10 m long cable, to achive
        // a non zero delay.
        // Approximately 0.05 microsecond accuracy is used in calculations.
        bmecisco7.in <-- delay 0.05 us <-- bmecisco1.out; // R<--R
        bmecisco1.in <-- delay 0.05 us <-- bmecisco3.out; // R<--R
        bmecisco3.in <-- delay 5.1 us <-- bmecisco2.out; // R<--K 1013m
        bmecisco2.in <-- delay 0.05 us <-- bmeconc5.S_out; // K<--K
        bmeconc5.M_in[0] <-- delay 1.5 us <-- bmebr1.out; // K<--CH 298m
        bmebr1.in <-- delay 1.5 us <-- bmeconc5.M_out[0]; // CH<--K 298m
        bmeconc5.S_in <-- delay 1.5 us <-- bmeconc4.S_out; // K<--Mt 292m
        bmeconc4.M_in[0] <-- delay 0.8 us <-- bmebr2.out; // Mt<--F 157m
        bmebr2.in <-- delay 0.8 us <-- bmeconc4.M_out[0]; // F<--Mt 157m
        bmeconc4.M_in[1] <-- delay 0.6 us <-- ethsw1.out; // Mt<--MM 117m
        ethsw1.in <-- delay 0.6 us <-- bmeconc4.M_out[1]; // MM<--Mt 117m
        bmeconc4.M_in[2] <-- delay 0.6 us <-- ethsw2.out; // Mt<--MM 117m
        ethsw2.in <-- delay 0.6 us <-- bmeconc4.M_out[2]; // MM<--Mt 117m
        bmeconc4.M_in[3] <-- delay 0.8 us <-- fizika2.out; // Mt<--F 157m
        fizika2.in <-- delay 0.8 us <-- bmeconc4.M_out[3]; // F<--Mt 157m
        bmeconc4.S_in <-- delay 1.7 us <-- bmeconc3.S_out; // Mt<-St 342m
        bmeconc3.M_in[0] <-- delay 0.6 us <-- bmebr3.out; // ST<--J 125m
        bmebr3.in <-- delay 0.6 us <-- bmeconc3.M_out[0]; // J<--St 125m
        bmeconc3.S_in <-- delay 2.1 us <-- bmeconc2.S_out; // St<--R 427m
        bmeconc2.M_in[0] <-- delay 0.05 us <-- Challenge.out; // R<--R
        Challenge.in <-- delay 0.05 us <-- bmeconc2.M_out[0]; // R<--R
        bmeconc2.M_in[1] <-- delay 0.7 us <-- gibr.out; // R<--D 146m
        gibr.in <-- delay 0.7 us <-- bmeconc2.M_out[1]; // D<--R 146m
        bmeconc2.M_in[2] <-- delay 0.05 us <-- sniffer.out; // R<--R
        sniffer.in <-- delay 0.05 us <-- bmeconc2.M_out[2]; // R<--R
        bmeconc2.S_in <-- delay 0.05 us <-- bmeconc1.S_out; // R<--R
        bmeconc1.M_in[0] <-- delay 0.05 us <-- Delfin.out; // R<--R
        Delfin.in <-- delay 0.05 us <-- bmeconc1.M_out[0]; // R<--R
        bmeconc1.M_in[1] <-- delay 0.05 us <-- Goliat.out; // R<--R
        Goliat.in <-- delay 0.05 us <-- bmeconc1.M_out[1]; // R<--R
        bmeconc1.M_in[2] <-- delay 0.05 us <-- bigmac.out; // R<--R
        bigmac.in <-- delay 0.05 us <-- bmeconc1.M_out[2]; // R<--R
        bmeconc1.S_in <-- delay 0.05 us <-- bmecisco7.out; // R<--R
        bmecisco7.ring_out --> out;
        bmecisco7.ring_in <-- in;
endmodule


//
// Technical University of Budapest Southen FDDI ring.
//
// The stations are connected in the form of a ring. From the dual attached
// ring only one direction is modelled; all the stations are modelled just
// with SAS.
//
module TUBSRing
    parameters:
        LoadControlFile : string,
        FDDI_Generator_type : string,
        RingID : numeric const,
        TTRT : numeric const,
        LoadMultiplier : numeric;
    gates:
        in: in;
        out: out;
    submodules:
        loadCtrl: LoadControl;
            parameters:
                LoadControlFile = LoadControlFile;
            display: "p=50,50";
        bmecisco7: FDDI_Router_port; // in Building R, EIK
            parameters:
                //      FDDI_Generator_type=FDDI_Generator_type,
                StationID = 0,
                address = "DECnet000728";
            display: "p=150,50;i=router";
        sniffer: FDDI_Sniffer; // in Building R, EIK
            parameters:
                StationID = 100,
                address = "this_sniffer";
            display: "p=250,50;i=comp";
        bmecisco5: FDDI_SAS; // in Building V2
            parameters:
                FDDI_Generator_type = FDDI_Generator_type,
                StationID = 1,
                address = "DECnet000228";
            display: "p=350,50;i=router";
        wagner: FDDI_SAS; // in Building V2, EET
            parameters:
                FDDI_Generator_type = FDDI_Generator_type,
                StationID = 2,
                address = "Sun___213C25";
            display: "p=400,125;i=comp";
        mhtbridge: FDDI_SAS; // in Building V2, MHT
            parameters:
                FDDI_Generator_type = FDDI_Generator_type,
                StationID = 3,
                address = "Bridge1A3866";
            display: "p=350,200;i=ipc";
        bmecisco4: FDDI_SAS; // in Building St
            parameters:
                FDDI_Generator_type = FDDI_Generator_type,
                StationID = 4,
                address = "DECnet000428";
            display: "p=250,200;i=router";
        heliosz: FDDI_SAS; // in Building R, VSZK
            parameters:
                FDDI_Generator_type = FDDI_Generator_type,
                StationID = 5,
                address = "Sun___739BF9";
            display: "p=150,200;i=comp";
        xyplex: FDDI_SAS; // in Building R, VSZK
            parameters:
                FDDI_Generator_type = FDDI_Generator_type,
                StationID = 6,
                address = "Xyplex083690";
            display: "p=100,125;i=router";
    connections:
        // Propagation delay is calculated from the cable length [in meter].
        // In glass, light travels at approximately 200 m/microseconds.
        // Within a building no exact cable lengths used as they are very
        // short. They are just modelled with a 10 m long cable, to achive
        // a non zero delay.
        // Approximately 0.05 microsecond accuracy is used in calculations.
        bmecisco7.in <-- delay 0.05 us <-- sniffer.out; // R<--R
        sniffer.in <-- delay 3.1 us <-- bmecisco5.out; // R<--V2 620m
        bmecisco5.in <-- delay 0.05 us <-- wagner.out; // V2<--V2
        wagner.in <-- delay 0.05 us <-- mhtbridge.out; // V2<--V2
        mhtbridge.in <-- delay 1.4 us <-- bmecisco4.out; // V2<--St 276m
        bmecisco4.in <-- delay 2.1 us <-- heliosz.out; // St<--R 427m
        heliosz.in <-- delay 0.05 us <-- xyplex.out; // R<--R
        xyplex.in <-- delay 0.05 us <-- bmecisco7.out; // R<--R
        bmecisco7.ring_out --> out;
        bmecisco7.ring_in <-- in;
endmodule


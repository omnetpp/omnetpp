//==========================================================================
//  FDDI_NET.NED  -  Network Description file for
//                   Discrete System Simulation in OMNeT++
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

include
        "fddi_mac",
        "ssm_ifs",
        "othermod";

channel FDDICable // used in the ring of identical nodes
        error 0
        delay 1 us // it is the delay of a 200m long fiber
endchannel

channel FDDI_SAC_inside_wire // represents the latency of a SAC
        error 0
        delay 100 ns // equals to the station_latency of an FDDI station
endchannel

module FDDI_SAC // acts as a Single Attachment Concentrator
        parameters:
                StationID: numeric,
                address: string;
        gates:
                in: S_in, M_in[];
                out: S_out, M_out[];

        connections:
                S_in -->FDDI_SAC_inside_wire -->M_out[0],
                for i = 0 to sizeof(M_in)-2 do
                        M_in[i] -->FDDI_SAC_inside_wire -->M_out[i+1];
                endfor,
                M_in[sizeof(M_in)-1] -->FDDI_SAC_inside_wire -->S_out;

endmodule

module Computer // a node in a ring of identical nodes
        gates:
                in: in;
                out: out, to_stat;
        submodules:
                MAC: FDDI_MAC4Ring
                        parameters:
                                no_comps = ancestor no_comps,
                                no_msg = ancestor no_msg;
                gen: FDDI_Generator4Ring
                        parameters:
                                no_msg = ancestor no_msg,
                                no_comps = ancestor no_comps,
                                wait_time = ancestor wait_time;
        connections:
                in --> MAC.in,
                MAC.out --> out,
                MAC.to_LLC --> to_stat,
                gen.out --> MAC.from_LLC;

endmodule

module FDDI_SAS // a Single Attachment Station in the network
        machines:
                host;
        parameters:
                FDDI_Generator_type: string,
                StationID: numeric,
                address: string;
        gates:
                in: in;
                out: out;
        submodules:
                MAC: FDDI_MAC
                        on:
                                host;
                        parameters:
                                StationID = StationID,
                                address = address;
                gen: FDDI_Generator_type like FDDI_Generator
                        on:
                                host;
                        parameters:
                                StationID = StationID,
                                address = address;
                sink: FDDI_Sink
                        on:
                                host;

        connections:
                in --> MAC.in,
                MAC.out --> out,
                MAC.to_LLC --> sink.in,
                gen.out --> MAC.from_LLC;

endmodule

module FDDI_Sniffer // an FDDI Sniffer (a special SAS)
        machines:
                host;
        parameters:
                StationID: numeric,
                address: string;
        gates:
                in: in;
                out: out;
        submodules:
                MAC: FDDI_MAC4Sniffer
                        on:
                                host;
                        parameters:
                                StationID = StationID,
                                address = address;
                gen: FDDI_Generator4Sniffer
                        on:
                                host;
                        parameters:
                                StationID = StationID,
                                address = address;
                monitor: FDDI_Monitor
                        on:
                                host;

        connections:
                in --> MAC.in,
                MAC.out --> out,
                MAC.to_LLC --> monitor.in,
                gen.out --> MAC.from_LLC;

endmodule

module FDDI_Router_port // an FDDI Router port
  // (a special SAS: has no Generator and Sink, interconnects rings)
  // makes routing decision on the basis of the length distribution
  // of the packets from this station to other stations in the ring
        machines:
                host;
        parameters:
                StationID: numeric,
                address: string;
        gates:
                in: in, ring_in;
                out: out, ring_out;
        submodules:
                MAC: FDDI_MAC
                        on:
                                host;
                        parameters:
                                StationID = StationID,
                                address = address;
                addr_gen: FDDI_Address_Generator
                        on:
                                host;
                        parameters:
                                StationID = StationID,
                                address = address;
     //         sink: FDDI_Sink;

        connections:
                in --> MAC.in,
                MAC.out --> out,
                MAC.to_LLC --> ring_out,
                ring_in --> addr_gen.in,
                addr_gen.out --> MAC.from_LLC;

endmodule

module FDDIRing // a simple FDDI ring of identical nodes:
// the computers are connected in the form of a ring
// plus every computer is connected to the stat module
        parameters:
                const no_comps, no_msg, wait_time,
                const RingID, const TTRT;
        submodules:
                comp: Computer [no_comps]

                stat: Stat
                        parameters:
                                no_comps = no_comps;
                        gatesizes:
                                in[no_comps];
        connections:
                comp[no_comps - 1].out --> FDDICable --> comp[0].in,
                for i = 0 .. no_comps - 2 do
                        comp [i].out -->  FDDICable --> comp [i + 1].in;
                endfor,
                for i = 0 .. no_comps - 1 do
                        comp [i].to_stat --> stat.in [i];
                endfor;
endmodule

module TUBNRing
// Technical University of Budapest Northen FDDI ring:
// (the stations are connected in the form of a ring plus there are some
// concentrators)
// from the dual attached ring only one direction is modelled
// all the stations are modelled just with SAS
// and all the concentrators are modelled with SAC-4

        machines:
                host;
        parameters:
                LoadControlFile: string,
                FDDI_Generator_type: string,
                const RingID,
                const TTRT,
                LoadMultiplier;
        gates:
                in: in;
                out: out;
        submodules:
                loadCtrl: LoadControl
                        on:
                                host;
                        parameters:
                                LoadControlFile=LoadControlFile;
                        display: "p=50,50";
                bmecisco7: FDDI_Router_port // in Building R, EIK
                        on:
                                host;
                        parameters:
                        //      FDDI_Generator_type=FDDI_Generator_type,
                                StationID=0,
                                address="DECnet000728";
                        display: "i=router;p=100,125";
                bmecisco1: FDDI_SAS // in Building R, EIK
                        on:
                                host;
                        parameters:
                                FDDI_Generator_type=FDDI_Generator_type,
                                StationID=1,
                                address="DECnet000A04";
                        display: "i=router;p=150,50";
                bmecisco3: FDDI_SAS // in Building R, EIK
                        on:
                                host;
                        parameters:
                                FDDI_Generator_type=FDDI_Generator_type,
                                StationID=2,
                                address="cisco_F99030";
                        display: "i=router;p=250,50";
                bmecisco2: FDDI_SAS // in Building K
                        on:
                                host;
                        parameters:
                                FDDI_Generator_type=FDDI_Generator_type,
                                StationID=3,
                                address="DECnet000B04";
                        display: "i=router;p=350,50";
                bmeconc5: FDDI_SAC  // in Building K
                        on:
                                host;
                        parameters:
                                StationID=-1,
                                address="DEC___26AF0D";
                        gatesizes:
                                M_in[1],
                                M_out[1];
                        display: "i=ipc;p=400,125";
                bmebr1: FDDI_SAS    // in Building CH
                        on:
                                host;
                        parameters:
                                FDDI_Generator_type=FDDI_Generator_type,
                                StationID=4,
                                address="DEC___28E6AD";
                        display: "i=ipc;p=430,70";
                bmeconc4: FDDI_SAC  // in Building Mt
                        on:
                                host;
                        parameters:
                                StationID=-1,
                                address="DEC___18DB64";
                        gatesizes:
                                M_in[4],
                                M_out[4];
                        display: "i=ipc;p=400,200";
                bmebr2: FDDI_SAS    // in Building F
                        on:
                                host;
                        parameters:
                                FDDI_Generator_type=FDDI_Generator_type,
                                StationID=5,
                                address="DEC___28E6AC";
                        display: "i=ipc;p=410,260";
                ethsw1: FDDI_SAS    // in Building MM
                        on:
                                host;
                        parameters:
                                FDDI_Generator_type=FDDI_Generator_type,
                                StationID=6,
                                address="00C01DF447A6";
                        display: "i=ipc;p=460,260";
                ethsw2: FDDI_SAS    // in Building MM
                        on:
                                host;
                        parameters:
                                FDDI_Generator_type=FDDI_Generator_type,
                                StationID=7,
                                address="00C01DF44885";
                        display: "i=ipc;p=460,220";
                fizika2: FDDI_SAS    // in Building F
                        on:
                                host;
                        parameters:
                                FDDI_Generator_type=FDDI_Generator_type,
                                StationID=8,
                                address="DEC___B48FE4";
                        display: "i=ipc;p=460,180";
                bmeconc3: FDDI_SAC  // in Building St
                        on:
                                host;
                        parameters:
                                StationID=-1,
                                address="DEC___26B05D";
                        gatesizes:
                                M_in[1],
                                M_out[1];
                        display: "i=ipc;p=360,275";
                bmebr3: FDDI_SAS    // in Building J
                        on:
                                host;
                        parameters:
                                FDDI_Generator_type=FDDI_Generator_type,
                                StationID=9,
                                address="DEC___286606";
                        display: "i=ipc;p=360,330";
                bmeconc2: FDDI_SAC  // in Building R, EIK
                        on:
                                host;
                        parameters:
                                StationID=-1,
                                address="DEC___314A73";
                        gatesizes:
                                M_in[3],
                                M_out[3];
                        display: "i=ipc;p=240,275";
                Challenge: FDDI_SAS // in Building R, EIK
                        on:
                                host;
                        parameters:
                                FDDI_Generator_type=FDDI_Generator_type,
                                StationID=10,
                                address="SGI___0409E8";
                        display: "i=comp;p=190,330";
                gibr: FDDI_SAS // in Building D
                        on:
                                host;
                        parameters:
                                FDDI_Generator_type=FDDI_Generator_type,
                                StationID=11,
                                address="DEC___28691D";
                        display: "i=comp;p=240,330";
                sniffer: FDDI_Sniffer // in Building R, EIK
                        on:
                                host;
                        parameters:
                                StationID=100,
                                address="This_Monitor";
                        display: "i=comp;p=290,330";
                bmeconc1: FDDI_SAC  // in Building R, EIK
                        on:
                                host;
                        parameters:
                                StationID=-1,
                                address="DEC___286C41";
                        gatesizes:
                                M_in[3],
                                M_out[3];
                        display: "i=ipc;p=100,200";
                Delfin: FDDI_SAS    // in Building R, EIK
                        on:
                                host;
                        parameters:
                                FDDI_Generator_type=FDDI_Generator_type,
                                StationID=12,
                                address="IBM___1DE0BA";
                        display: "i=comp;p=50,240";
                Goliat: FDDI_SAS    // in Building R, EIK
                        on:
                                host;
                        parameters:
                                FDDI_Generator_type=FDDI_Generator_type,
                                StationID=13,
                                address="Sun___1BD035";
                        display: "i=comp;p=100,265";
                bigmac: FDDI_SAS    // in Building R, EIK
                        on:
                                host;
                        parameters:
                                FDDI_Generator_type=FDDI_Generator_type,
                                StationID=14,
                                address="DEC___B11ECF";
                        display: "i=comp;p=150,270";

        connections:
        // Propagation delay is calculated from the cable length [in meter].
        // In glass, light travels at approximately 200 m/microseconds.
        // Within a building no exact cable lengths used as they are very
        // short. They are just modelled with a 10 m long cable, to achive
        // a non zero delay.
        // Approximately 0.05 microsecond accuracy is used in calculations.
                bmecisco7.in <-- delay 0.05 us <-- bmecisco1.out, // R<--R
                bmecisco1.in <-- delay 0.05 us <-- bmecisco3.out, // R<--R
                bmecisco3.in <-- delay 5.1 us <-- bmecisco2.out,  // R<--K 1013m
                bmecisco2.in <-- delay 0.05 us <-- bmeconc5.S_out,// K<--K
                bmeconc5.M_in[0] <-- delay 1.5 us <-- bmebr1.out, // K<--CH 298m
                bmebr1.in <-- delay 1.5 us <-- bmeconc5.M_out[0], // CH<--K 298m
                bmeconc5.S_in <-- delay 1.5 us <-- bmeconc4.S_out,// K<--Mt 292m
                bmeconc4.M_in[0] <-- delay 0.8 us <-- bmebr2.out, // Mt<--F 157m
                bmebr2.in <-- delay 0.8 us <-- bmeconc4.M_out[0], // F<--Mt 157m
                bmeconc4.M_in[1] <-- delay 0.6 us <-- ethsw1.out, // Mt<--MM 117m
                ethsw1.in <-- delay 0.6 us <-- bmeconc4.M_out[1], // MM<--Mt 117m
                bmeconc4.M_in[2] <-- delay 0.6 us <-- ethsw2.out, // Mt<--MM 117m
                ethsw2.in <-- delay 0.6 us <-- bmeconc4.M_out[2], // MM<--Mt 117m
                bmeconc4.M_in[3] <-- delay 0.8 us <-- fizika2.out, // Mt<--F 157m
                fizika2.in <-- delay 0.8 us <-- bmeconc4.M_out[3], // F<--Mt 157m
                bmeconc4.S_in <-- delay 1.7 us <-- bmeconc3.S_out, // Mt<-St 342m
                bmeconc3.M_in[0] <-- delay 0.6 us <-- bmebr3.out, // ST<--J 125m
                bmebr3.in <-- delay 0.6 us <-- bmeconc3.M_out[0], // J<--St 125m
                bmeconc3.S_in <-- delay 2.1 us <-- bmeconc2.S_out, // St<--R 427m
                bmeconc2.M_in[0] <-- delay 0.05 us <-- Challenge.out, // R<--R
                Challenge.in <-- delay 0.05 us <-- bmeconc2.M_out[0], // R<--R
                bmeconc2.M_in[1] <-- delay 0.7 us <-- gibr.out, // R<--D 146m
                gibr.in <-- delay 0.7 us <-- bmeconc2.M_out[1], // D<--R 146m
                bmeconc2.M_in[2] <-- delay 0.05 us <-- sniffer.out, // R<--R
                sniffer.in <-- delay 0.05 us <-- bmeconc2.M_out[2], // R<--R
                bmeconc2.S_in <-- delay 0.05 us <-- bmeconc1.S_out, // R<--R
                bmeconc1.M_in[0] <-- delay 0.05 us <-- Delfin.out, // R<--R
                Delfin.in <-- delay 0.05 us <-- bmeconc1.M_out[0], // R<--R
                bmeconc1.M_in[1] <-- delay 0.05 us <-- Goliat.out, // R<--R
                Goliat.in <-- delay 0.05 us <-- bmeconc1.M_out[1], // R<--R
                bmeconc1.M_in[2] <-- delay 0.05 us <-- bigmac.out, // R<--R
                bigmac.in <-- delay 0.05 us <-- bmeconc1.M_out[2], // R<--R
                bmeconc1.S_in <-- delay 0.05 us <-- bmecisco7.out, // R<--R
                bmecisco7.ring_out --> out,
                bmecisco7.ring_in <-- in;
endmodule

module TUBSRing
// Technical University of Budapest Southen FDDI ring:
// (the stations are connected in the form of a ring)
// from the dual attached ring only one direction is modelled
// all the stations are modelled just with SAS

        machines:
                host;
        parameters:
                LoadControlFile: string,
                FDDI_Generator_type: string,
                const RingID,
                const TTRT,
                LoadMultiplier;
        gates:
                in: in;
                out: out;
        submodules:
                loadCtrl: LoadControl
                        on:
                                host;
                        parameters:
                                LoadControlFile=LoadControlFile;
                        display: "p=50,50";
                bmecisco7: FDDI_Router_port // in Building R, EIK
                        on:
                                host;
                        parameters:
                        //      FDDI_Generator_type=FDDI_Generator_type,
                                StationID=0,
                                address="DECnet000728";
                        display: "i=router;p=150,50";
                sniffer: FDDI_Sniffer // in Building R, EIK
                        on:
                                host;
                        parameters:
                                StationID=100,
                                address="this_sniffer";
                        display: "i=comp;p=250,50";
                bmecisco5: FDDI_SAS // in Building V2
                        on:
                                host;
                        parameters:
                                FDDI_Generator_type=FDDI_Generator_type,
                                StationID=1,
                                address="DECnet000228";
                        display: "i=router;p=350,50";
                wagner: FDDI_SAS // in Building V2, EET
                        on:
                                host;
                        parameters:
                                FDDI_Generator_type=FDDI_Generator_type,
                                StationID=2,
                                address="Sun___213C25";
                        display: "i=comp;p=400,125";
                mhtbridge: FDDI_SAS    // in Building V2, MHT
                        on:
                                host;
                        parameters:
                                FDDI_Generator_type=FDDI_Generator_type,
                                StationID=3,
                                address="Bridge1A3866";
                        display: "i=ipc;p=350,200";
                bmecisco4: FDDI_SAS // in Building St
                        on:
                                host;
                        parameters:
                                FDDI_Generator_type=FDDI_Generator_type,
                                StationID=4,
                                address="DECnet000428";
                        display: "i=router;p=250,200";
                heliosz: FDDI_SAS // in Building R, VSZK
                        on:
                                host;
                        parameters:
                                FDDI_Generator_type=FDDI_Generator_type,
                                StationID=5,
                                address="Sun___739BF9";
                        display: "i=comp;p=150,200";
                xyplex: FDDI_SAS    // in Building R, VSZK
                        on:
                                host;
                        parameters:
                                FDDI_Generator_type=FDDI_Generator_type,
                                StationID=6,
                                address="Xyplex083690";
                        display: "i=router;p=100,125";

        connections:
        // Propagation delay is calculated from the cable length [in meter].
        // In glass, light travels at approximately 200 m/microseconds.
        // Within a building no exact cable lengths used as they are very
        // short. They are just modelled with a 10 m long cable, to achive
        // a non zero delay.
        // Approximately 0.05 microsecond accuracy is used in calculations.
                bmecisco7.in <-- delay 0.05 us <-- sniffer.out, // R<--R
                sniffer.in <-- delay 3.1 us <-- bmecisco5.out, // R<--V2 620m
                bmecisco5.in <-- delay 0.05 us <-- wagner.out, // V2<--V2
                wagner.in <-- delay 0.05 us <-- mhtbridge.out, // V2<--V2
                mhtbridge.in <-- delay 1.4 us <-- bmecisco4.out, // V2<--St 276m
                bmecisco4.in <-- delay 2.1 us <-- heliosz.out, // St<--R 427m
                heliosz.in <-- delay 0.05 us <-- xyplex.out, // R<--R
                xyplex.in <-- delay 0.05 us <-- bmecisco7.out, // R<--R
                bmecisco7.ring_out --> out,
                bmecisco7.ring_in <-- in;
endmodule

network FDDI1: FDDIRing
        parameters:
                RingID = input (1, "RingID: "),
                TTRT = input (10 ms, "TTRT: "),
                no_comps = input (3, "# computers: "),
                no_msg = input (50, "# messages: "),
                wait_time = input (0.0001, "wait time (sec): ");
endnetwork

network NRing: TUBNRing
        parameters:
                LoadControlFile = input ( "nr_lctrl", "NR LoadControlFile: "),
                RingID = input (1, "RingID: "),
                FDDI_Generator_type = input ( "FDDI_GeneratorFromTraceFile",
                                              "FDDI SAS Generator type: "),
                RingID = input (1, "RingID: "),
                TTRT = input (10 ms, "TTRT: "),
                LoadMultiplier = input (10, "Load multiplier: ");
endnetwork

module TUB_wired
        machines:
                NR_host, SR_host;
        parameters:
                NRLoadControlFile: string,
                SRLoadControlFile: string,
                FDDI_Generator_type: string,
                NRLoadMultiplier,
                SRLoadMultiplier;
        submodules:
                NRing: TUBNRing
                        on:
                                NR_host;
                        parameters:
                                LoadControlFile = NRLoadControlFile,
                                FDDI_Generator_type = FDDI_Generator_type,
                                RingID = 0,
                                TTRT = 5 ms,
                                LoadMultiplier = NRLoadMultiplier;  // to be changed!!!
                        display: "i=ring";
                SRing: TUBSRing
                        on:
                                SR_host;
                        parameters:
                                LoadControlFile = SRLoadControlFile,
                                FDDI_Generator_type = FDDI_Generator_type,
                                RingID = 1,
                                TTRT = 5.017 ms,
                                LoadMultiplier = SRLoadMultiplier;  // to be changed!!!
                        display: "i=ring";
        connections:
                NRing.out --> delay 0.05 us --> SRing.in,
                SRing.out --> delay 0.05 us --> NRing.in;
endmodule

module TUB_SSM
        machines:
                NR_host, SR_host;
        parameters:
                NRLoadControlFile: string,
                SRLoadControlFile: string,
                FDDI_Generator_type: string,
                NRLoadMultiplier,
                SRLoadMultiplier,
                SSM_OIF_type: string,
                const SSM_OIF_frequency,
                NROIFControlFile: string,
                SROIFControlFile: string;
        submodules:
                NRing: TUBNRing
                        on:
                                NR_host;
                        parameters:
                                LoadControlFile = NRLoadControlFile,
                                FDDI_Generator_type = FDDI_Generator_type,
                                RingID = 0,
                                TTRT = 5 ms,
                                LoadMultiplier = ref NRLoadMultiplier;
                        display: "i=ring;p=150,50";
                SRing: TUBSRing
                        on:
                                SR_host;
                        parameters:
                                LoadControlFile = SRLoadControlFile,
                                FDDI_Generator_type = FDDI_Generator_type,
                                RingID = 1,
                                TTRT = 5.017 ms,
                                LoadMultiplier = ref SRLoadMultiplier;
                        display: "i=ring;p=150,250";
                NRing_oif: SSM_OIF_type like SSM_OIF
                        on:
                                NR_host;
                        parameters:
                                frequency = SSM_OIF_frequency,
                                OIFControlFile=NROIFControlFile;
                        display: "p=50,110";
                NRing_iif: SSM_IIF
                        on:
                                NR_host;
                        display: "p=250,110";
                SRing_oif: SSM_OIF_type like SSM_OIF
                        on:
                                SR_host;
                        parameters:
                                frequency = SSM_OIF_frequency,
                                OIFControlFile=SROIFControlFile;
                        display: "p=250,180";
                SRing_iif: SSM_IIF
                        on:
                                SR_host;
                        display: "p=50,180";
        connections:
                NRing.out --> NRing_oif.in,
                NRing_oif.out --> delay 0.05 us --> SRing_iif.in,
                SRing_iif.out --> SRing.in,

                SRing.out -->  SRing_oif.in,
                SRing_oif.out --> delay 0.05 us --> NRing_iif.in,
                NRing_iif.out --> NRing.in;
endmodule

network TUBw: TUB_wired
        on:
                NR_host, SR_host;
        parameters:
                NRLoadControlFile = input ( "nr_lctrl", "NR LoadControlFile: "),
                SRLoadControlFile = input ( "sr_lctrl", "SR LoadControlFile: "),
                FDDI_Generator_type = input ( "FDDI_GeneratorFromTraceFile",
                                              "FDDI SAS Generator type: "),
                NRLoadMultiplier = input (10, "NR Load multiplier: "),
                SRLoadMultiplier = input (10, "SR Load multiplier: ");
endnetwork

network TUBs: TUB_SSM
        on:
                NR_host, SR_host;
        parameters:
                NRLoadControlFile = input ( "nr_lctrl", "NR LoadControlFile: "),
                SRLoadControlFile = input ( "sr_lctrl", "SR LoadControlFile: "),
                FDDI_Generator_type = input ( "FDDI_GeneratorFromTraceFile",
                                              "FDDI SAS Generator type: "),
                NRLoadMultiplier = input (10, "NR Load multiplier: "),
                SRLoadMultiplier = input (10, "SR Load multiplier: "),
                SSM_OIF_type = input ("SSM_OIF_by_frequency", "SSM Output Interface Type: "),
                SSM_OIF_frequency = input (100, "SSM Output Interface Frequency: "),
                NROIFControlFile = input ("NR_OIFct","NR SSM OIF Control File: "),
                SROIFControlFile = input ("SR_OIFct","SR SSM OIF Control File: ");
endnetwork

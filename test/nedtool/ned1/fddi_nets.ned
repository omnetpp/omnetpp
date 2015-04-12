//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1998-2015 Andras Varga
// Copyright (C) 1996-97 Gabor Lencse, T.U.Budapest, Dept. of Telecommunications
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


import
    "fddi_mac",
    "ssm_ifs",
    "othermod",
    "fddi_rings";


//
// A ring with identical stations.
//
network FDDI1 : FDDIRing
    parameters:
        RingID = input (1, "RingID: "),
        TTRT = input (10 ms, "TTRT: "),
        no_comps = input (3, "# computers: "),
        no_msg = input (50, "# messages: "),
        wait_time = input (0.0001, "wait time (sec): ");
endnetwork


//
// Technical University Northern Ring, modeled standalone.
//
network NRing : TUBNRing
    parameters:
        LoadControlFile = input ( "nr_lctrl", "NR LoadControlFile: "),
        RingID = input (1, "RingID: "),
        FDDI_Generator_type = input ( "FDDI_GeneratorFromTraceFile",
                                              "FDDI SAS Generator type: "),
        TTRT = input (10 ms, "TTRT: "),
        LoadMultiplier = input (10, "Load multiplier: ");
endnetwork



//
// Technical University Northern & Southern rings.
//
module TUB_wired
    parameters:
        NRLoadControlFile: string,
        SRLoadControlFile: string,
        FDDI_Generator_type: string,
        NRLoadMultiplier: numeric,
        SRLoadMultiplier: numeric;
    submodules:
        NRing: TUBNRing;
            parameters:
                LoadControlFile = NRLoadControlFile,
                FDDI_Generator_type = FDDI_Generator_type,
                RingID = 0,
                TTRT = 5 ms,
                LoadMultiplier = NRLoadMultiplier;
            display: "p=108,105;i=device/lan-ring_l";
        SRing: TUBSRing;
            parameters:
                LoadControlFile = SRLoadControlFile,
                FDDI_Generator_type = FDDI_Generator_type,
                RingID = 1,
                TTRT = 5.017 ms,
                LoadMultiplier = SRLoadMultiplier;
            display: "p=208,225;i=device/lan-ring_l";
    connections:
        NRing.out --> delay 0.05 us --> SRing.in;
        SRing.out --> delay 0.05 us --> NRing.in;
endmodule



//
// Technical University Northern & Southern rings, interconnected by
// Statistical Synchronisation interfaces.
//
module TUB_SSM
    parameters:
        NRLoadControlFile: string,
        SRLoadControlFile: string,
        FDDI_Generator_type: string,
        NRLoadMultiplier: numeric,
        SRLoadMultiplier: numeric,
        SSM_OIF_type: string,
        SSM_OIF_frequency: numeric const,
        NROIFControlFile: string,
        SROIFControlFile: string;
    submodules:
        NRing: TUBNRing;
            parameters:
                LoadControlFile = NRLoadControlFile,
                FDDI_Generator_type = FDDI_Generator_type,
                RingID = 0,
                TTRT = 5 ms,
                LoadMultiplier = ref NRLoadMultiplier;
            display: "p=173,70;i=device/lan-ring_l";
        SRing: TUBSRing;
            parameters:
                LoadControlFile = SRLoadControlFile,
                FDDI_Generator_type = FDDI_Generator_type,
                RingID = 1,
                TTRT = 5.017 ms,
                LoadMultiplier = ref SRLoadMultiplier;
            display: "p=175,314;i=device/lan-ring_l";
        NRing_oif: SSM_OIF_type like SSM_OIF;
            parameters:
                frequency = SSM_OIF_frequency,
                OIFControlFile = NROIFControlFile;
            display: "i=block/rx_s;p=122,165";
        NRing_iif: SSM_IIF;
            display: "i=block/tx_s;p=232,165";
        SRing_oif: SSM_OIF_type like SSM_OIF;
            parameters:
                frequency = SSM_OIF_frequency,
                OIFControlFile = SROIFControlFile;
            display: "i=block/rx_s;p=232,220";
        SRing_iif: SSM_IIF;
            display: "i=block/tx_s;p=122,220";
    connections:
        NRing.out --> NRing_oif.in;
        NRing_oif.out --> delay 0.05 us --> SRing_iif.in;
        SRing_iif.out --> SRing.in;

        SRing.out --> SRing_oif.in;
        SRing_oif.out --> delay 0.05 us --> NRing_iif.in;
        NRing_iif.out --> NRing.in;
endmodule


network TUBw : TUB_wired
    parameters:
        NRLoadControlFile = input ( "nr_lctrl", "NR LoadControlFile: "),
        SRLoadControlFile = input ( "sr_lctrl", "SR LoadControlFile: "),
        FDDI_Generator_type = input ( "FDDI_GeneratorFromTraceFile",
                                              "FDDI SAS Generator type: "),
        NRLoadMultiplier = input (10, "NR Load multiplier: "),
        SRLoadMultiplier = input (10, "SR Load multiplier: ");
endnetwork


network TUBs : TUB_SSM
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



//--------------------------------------------------
// File: tknring.ned
//
// Network description of a Token Ring network
//
// Authors: Gabor Lencse, Andras Varga (TU Budapest)
// Based on the code by:
//          Maurits Andr‚, George van Montfort,
//          Gerard van de Weerd (TU Delft)
//
//--------------------------------------------------

include
        "tknmods";

// Computer: node in the network; it will be connected to its right neighbour
module Computer
        gates:
                in: in;
                out: out;

        submodules:
                mac: TokenRingMAC
                        parameters:
                                data_rate = ancestor data_rate,
                                THT = ancestor THT;
                gen: Generator
                        parameters:
                                num_messages = ancestor num_messages,
                                num_stations = ancestor num_stations,
                                ia_time = ancestor ref ia_time;
                sink: Sink;
        connections:
                in --> mac.phy_in,
                mac.phy_out --> out,
                mac.to_sink --> sink.in,
                gen.out --> mac.from_gen;
endmodule

// TokenRing: ring with a configurable number of Computers
module TokenRing
        parameters:
                const num_stations, num_messages, ia_time,
                THT, data_rate, cable_delay;

        submodules:
                comp: Computer [num_stations];
                        display: "i=pc";

        connections:
                comp[num_stations - 1].out --> delay cable_delay --> comp[0].in,

                for i = 0 .. num_stations - 2 do
                        comp [i].out -->  delay cable_delay --> comp [i + 1].in;
                endfor;
endmodule

network token: TokenRing
        parameters:
                num_stations = input( 5, "Number of stations:" ),
                ia_time = input(truncnormal(0.020, 0.006),"Frame generation inter-arrival time:"),
                THT = 10ms,
                data_rate = 4000000,    // 4 Mbit/s
                cable_delay = 1 us,
                num_messages = input( 2000 );
endnetwork

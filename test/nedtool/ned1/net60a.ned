//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


import "../node";

//
// This network adds one extra connection to the net60 model (net60.ned):
// a simplex link from node[55] to node[0]. (See end of NED code.)
//
module Net60a
    submodules:
        rte: Node[57];
            parameters:
                address = index;
            gatesizes:
                in[6],
                out[6];
            display: "i=misc/node_vs,gold,100";
    connections nocheck:
        rte[0].out[0] --> rte[1].in[0];
        rte[0].in[0] <-- rte[1].out[0];

        rte[1].out[1] --> rte[2].in[0];
        rte[1].in[1] <-- rte[2].out[0];

        rte[1].out[2] --> rte[4].in[0];
        rte[1].in[2] <-- rte[4].out[0];

        rte[3].out[0] --> rte[4].in[1];
        rte[3].in[0] <-- rte[4].out[1];

        rte[4].out[2] --> rte[5].in[0];
        rte[4].in[2] <-- rte[5].out[0];

        rte[4].out[3] --> rte[7].in[0];
        rte[4].in[3] <-- rte[7].out[0];

        rte[5].out[1] --> rte[6].in[0];
        rte[5].in[1] <-- rte[6].out[0];

        rte[5].out[2] --> rte[10].in[0];
        rte[5].in[2] <-- rte[10].out[0];

        rte[6].out[1] --> rte[7].in[1];
        rte[6].in[1] <-- rte[7].out[1];

        rte[6].out[2] --> rte[9].in[0];
        rte[6].in[2] <-- rte[9].out[0];

        rte[7].out[2] --> rte[8].in[0];
        rte[7].in[2] <-- rte[8].out[0];

        rte[7].out[3] --> rte[12].in[0];
        rte[7].in[3] <-- rte[12].out[0];

        rte[9].out[1] --> rte[11].in[0];
        rte[9].in[1] <-- rte[11].out[0];

        rte[10].out[1] --> rte[11].in[1];
        rte[10].in[1] <-- rte[11].out[1];

        rte[10].out[2] --> rte[13].in[0];
        rte[10].in[2] <-- rte[13].out[0];

        rte[11].out[2] --> rte[12].in[1];
        rte[11].in[2] <-- rte[12].out[1];

        rte[11].out[3] --> rte[14].in[0];
        rte[11].in[3] <-- rte[14].out[0];

        rte[12].out[2] --> rte[15].in[0];
        rte[12].in[2] <-- rte[15].out[0];

        rte[13].out[1] --> rte[18].in[0];
        rte[13].in[1] <-- rte[18].out[0];

        rte[14].out[1] --> rte[15].in[1];
        rte[14].in[1] <-- rte[15].out[1];

        rte[14].out[2] --> rte[22].in[0];
        rte[14].in[2] <-- rte[22].out[0];

        rte[15].out[2] --> rte[16].in[0];
        rte[15].in[2] <-- rte[16].out[0];

        rte[16].out[1] --> rte[17].in[0];
        rte[16].in[1] <-- rte[17].out[0];

        rte[16].out[2] --> rte[23].in[0];
        rte[16].in[2] <-- rte[23].out[0];

        rte[18].out[1] --> rte[19].in[0];
        rte[18].in[1] <-- rte[19].out[0];

        rte[18].out[2] --> rte[21].in[0];
        rte[18].in[2] <-- rte[21].out[0];

        rte[19].out[1] --> rte[20].in[0];
        rte[19].in[1] <-- rte[20].out[0];

        rte[19].out[2] --> rte[26].in[0];
        rte[19].in[2] <-- rte[26].out[0];

        rte[20].out[1] --> rte[21].in[1];
        rte[20].in[1] <-- rte[21].out[1];

        rte[20].out[2] --> rte[24].in[0];
        rte[20].in[2] <-- rte[24].out[0];

        rte[20].out[3] --> rte[25].in[0];
        rte[20].in[3] <-- rte[25].out[0];

        rte[21].out[2] --> rte[22].in[1];
        rte[21].in[2] <-- rte[22].out[1];

        rte[21].out[3] --> rte[23].in[1];
        rte[21].in[3] <-- rte[23].out[1];

        rte[21].out[4] --> rte[24].in[1];
        rte[21].in[4] <-- rte[24].out[1];

        rte[23].out[2] --> rte[24].in[2];
        rte[23].in[2] <-- rte[24].out[2];

        rte[24].out[3] --> rte[28].in[0];
        rte[24].in[3] <-- rte[28].out[0];

        rte[25].out[1] --> rte[27].in[0];
        rte[25].in[1] <-- rte[27].out[0];

        rte[26].out[1] --> rte[32].in[0];
        rte[26].in[1] <-- rte[32].out[0];

        rte[27].out[1] --> rte[29].in[0];
        rte[27].in[1] <-- rte[29].out[0];

        rte[27].out[2] --> rte[30].in[0];
        rte[27].in[2] <-- rte[30].out[0];

        rte[27].out[3] --> rte[32].in[1];
        rte[27].in[3] <-- rte[32].out[1];

        rte[28].out[1] --> rte[29].in[1];
        rte[28].in[1] <-- rte[29].out[1];

        rte[28].out[2] --> rte[31].in[0];
        rte[28].in[2] <-- rte[31].out[0];

        rte[29].out[2] --> rte[31].in[1];
        rte[29].in[2] <-- rte[31].out[1];

        rte[30].out[1] --> rte[36].in[0];
        rte[30].in[1] <-- rte[36].out[0];

        rte[31].out[2] --> rte[33].in[0];
        rte[31].in[2] <-- rte[33].out[0];

        rte[32].out[2] --> rte[34].in[0];
        rte[32].in[2] <-- rte[34].out[0];

        rte[33].out[1] --> rte[37].in[0];
        rte[33].in[1] <-- rte[37].out[0];

        rte[34].out[1] --> rte[35].in[0];
        rte[34].in[1] <-- rte[35].out[0];

        rte[34].out[2] --> rte[40].in[0];
        rte[34].in[2] <-- rte[40].out[0];

        rte[35].out[1] --> rte[36].in[1];
        rte[35].in[1] <-- rte[36].out[1];

        rte[35].out[2] --> rte[39].in[0];
        rte[35].in[2] <-- rte[39].out[0];

        rte[36].out[2] --> rte[37].in[1];
        rte[36].in[2] <-- rte[37].out[1];

        rte[37].out[2] --> rte[38].in[0];
        rte[37].in[2] <-- rte[38].out[0];

        rte[38].out[1] --> rte[39].in[1];
        rte[38].in[1] <-- rte[39].out[1];

        rte[38].out[2] --> rte[42].in[0];
        rte[38].in[2] <-- rte[42].out[0];

        rte[39].out[2] --> rte[40].in[1];
        rte[39].in[2] <-- rte[40].out[1];

        rte[39].out[3] --> rte[41].in[0];
        rte[39].in[3] <-- rte[41].out[0];

        rte[40].out[2] --> rte[43].in[0];
        rte[40].in[2] <-- rte[43].out[0];

        rte[40].out[3] --> rte[44].in[0];
        rte[40].in[3] <-- rte[44].out[0];

        rte[40].out[4] --> rte[52].in[0];
        rte[40].in[4] <-- rte[52].out[0];

        rte[41].out[1] --> rte[42].in[1];
        rte[41].in[1] <-- rte[42].out[1];

        rte[41].out[2] --> rte[43].in[1];
        rte[41].in[2] <-- rte[43].out[1];

        rte[42].out[2] --> rte[43].in[2];
        rte[42].in[2] <-- rte[43].out[2];

        rte[43].out[3] --> rte[46].in[0];
        rte[43].in[3] <-- rte[46].out[0];

        rte[44].out[1] --> rte[45].in[0];
        rte[44].in[1] <-- rte[45].out[0];

        rte[44].out[2] --> rte[47].in[0];
        rte[44].in[2] <-- rte[47].out[0];

        rte[45].out[1] --> rte[46].in[1];
        rte[45].in[1] <-- rte[46].out[1];

        rte[46].out[2] --> rte[49].in[0];
        rte[46].in[2] <-- rte[49].out[0];

        rte[47].out[1] --> rte[48].in[0];
        rte[47].in[1] <-- rte[48].out[0];

        rte[48].out[1] --> rte[50].in[0];
        rte[48].in[1] <-- rte[50].out[0];

        rte[48].out[2] --> rte[51].in[0];
        rte[48].in[2] <-- rte[51].out[0];

        rte[49].out[1] --> rte[50].in[1];
        rte[49].in[1] <-- rte[50].out[1];

        rte[50].out[2] --> rte[53].in[0];
        rte[50].in[2] <-- rte[53].out[0];

        rte[51].out[1] --> rte[52].in[1];
        rte[51].in[1] <-- rte[52].out[1];

        rte[51].out[2] --> rte[53].in[1];
        rte[51].in[2] <-- rte[53].out[1];

        rte[52].out[2] --> rte[53].in[2];
        rte[52].in[2] <-- rte[53].out[2];

        rte[52].out[3] --> rte[54].in[0];
        rte[52].in[3] <-- rte[54].out[0];

        rte[53].out[3] --> rte[56].in[0];
        rte[53].in[3] <-- rte[56].out[0];

        rte[54].out[1] --> rte[55].in[0];
        rte[54].in[1] <-- rte[55].out[0];

        rte[55].out[1] --> rte[56].in[1];
        rte[55].in[1] <-- rte[56].out[1];

        // the "extra" connection, compared to net60
        rte[55].out[2] --> rte[0].in[1];
endmodule

network net60a : Net60a
endnetwork


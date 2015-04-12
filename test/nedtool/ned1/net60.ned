//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

import "node";

module Net60
    submodules:
        rte: Node[57];
            parameters:
                address = index;
            display: "i=misc/node_vs,gold";
    connections nocheck:
        rte[0].out++ --> rte[1].in++;
        rte[0].in++ <-- rte[1].out++;

        rte[1].out++ --> rte[2].in++;
        rte[1].in++ <-- rte[2].out++;

        rte[1].out++ --> rte[4].in++;
        rte[1].in++ <-- rte[4].out++;

        rte[3].out++ --> rte[4].in++;
        rte[3].in++ <-- rte[4].out++;

        rte[4].out++ --> rte[5].in++;
        rte[4].in++ <-- rte[5].out++;

        rte[4].out++ --> rte[7].in++;
        rte[4].in++ <-- rte[7].out++;

        rte[5].out++ --> rte[6].in++;
        rte[5].in++ <-- rte[6].out++;

        rte[5].out++ --> rte[10].in++;
        rte[5].in++ <-- rte[10].out++;

        rte[6].out++ --> rte[7].in++;
        rte[6].in++ <-- rte[7].out++;

        rte[6].out++ --> rte[9].in++;
        rte[6].in++ <-- rte[9].out++;

        rte[7].out++ --> rte[8].in++;
        rte[7].in++ <-- rte[8].out++;

        rte[7].out++ --> rte[12].in++;
        rte[7].in++ <-- rte[12].out++;

        rte[9].out++ --> rte[11].in++;
        rte[9].in++ <-- rte[11].out++;

        rte[10].out++ --> rte[11].in++;
        rte[10].in++ <-- rte[11].out++;

        rte[10].out++ --> rte[13].in++;
        rte[10].in++ <-- rte[13].out++;

        rte[11].out++ --> rte[12].in++;
        rte[11].in++ <-- rte[12].out++;

        rte[11].out++ --> rte[14].in++;
        rte[11].in++ <-- rte[14].out++;

        rte[12].out++ --> rte[15].in++;
        rte[12].in++ <-- rte[15].out++;

        rte[13].out++ --> rte[18].in++;
        rte[13].in++ <-- rte[18].out++;

        rte[14].out++ --> rte[15].in++;
        rte[14].in++ <-- rte[15].out++;

        rte[14].out++ --> rte[22].in++;
        rte[14].in++ <-- rte[22].out++;

        rte[15].out++ --> rte[16].in++;
        rte[15].in++ <-- rte[16].out++;

        rte[16].out++ --> rte[17].in++;
        rte[16].in++ <-- rte[17].out++;

        rte[16].out++ --> rte[23].in++;
        rte[16].in++ <-- rte[23].out++;

        rte[18].out++ --> rte[19].in++;
        rte[18].in++ <-- rte[19].out++;

        rte[18].out++ --> rte[21].in++;
        rte[18].in++ <-- rte[21].out++;

        rte[19].out++ --> rte[20].in++;
        rte[19].in++ <-- rte[20].out++;

        rte[19].out++ --> rte[26].in++;
        rte[19].in++ <-- rte[26].out++;

        rte[20].out++ --> rte[21].in++;
        rte[20].in++ <-- rte[21].out++;

        rte[20].out++ --> rte[24].in++;
        rte[20].in++ <-- rte[24].out++;

        rte[20].out++ --> rte[25].in++;
        rte[20].in++ <-- rte[25].out++;

        rte[21].out++ --> rte[22].in++;
        rte[21].in++ <-- rte[22].out++;

        rte[21].out++ --> rte[23].in++;
        rte[21].in++ <-- rte[23].out++;

        rte[21].out++ --> rte[24].in++;
        rte[21].in++ <-- rte[24].out++;

        rte[23].out++ --> rte[24].in++;
        rte[23].in++ <-- rte[24].out++;

        rte[24].out++ --> rte[28].in++;
        rte[24].in++ <-- rte[28].out++;

        rte[25].out++ --> rte[27].in++;
        rte[25].in++ <-- rte[27].out++;

        rte[26].out++ --> rte[32].in++;
        rte[26].in++ <-- rte[32].out++;

        rte[27].out++ --> rte[29].in++;
        rte[27].in++ <-- rte[29].out++;

        rte[27].out++ --> rte[30].in++;
        rte[27].in++ <-- rte[30].out++;

        rte[27].out++ --> rte[32].in++;
        rte[27].in++ <-- rte[32].out++;

        rte[28].out++ --> rte[29].in++;
        rte[28].in++ <-- rte[29].out++;

        rte[28].out++ --> rte[31].in++;
        rte[28].in++ <-- rte[31].out++;

        rte[29].out++ --> rte[31].in++;
        rte[29].in++ <-- rte[31].out++;

        rte[30].out++ --> rte[36].in++;
        rte[30].in++ <-- rte[36].out++;

        rte[31].out++ --> rte[33].in++;
        rte[31].in++ <-- rte[33].out++;

        rte[32].out++ --> rte[34].in++;
        rte[32].in++ <-- rte[34].out++;

        rte[33].out++ --> rte[37].in++;
        rte[33].in++ <-- rte[37].out++;

        rte[34].out++ --> rte[35].in++;
        rte[34].in++ <-- rte[35].out++;

        rte[34].out++ --> rte[40].in++;
        rte[34].in++ <-- rte[40].out++;

        rte[35].out++ --> rte[36].in++;
        rte[35].in++ <-- rte[36].out++;

        rte[35].out++ --> rte[39].in++;
        rte[35].in++ <-- rte[39].out++;

        rte[36].out++ --> rte[37].in++;
        rte[36].in++ <-- rte[37].out++;

        rte[37].out++ --> rte[38].in++;
        rte[37].in++ <-- rte[38].out++;

        rte[38].out++ --> rte[39].in++;
        rte[38].in++ <-- rte[39].out++;

        rte[38].out++ --> rte[42].in++;
        rte[38].in++ <-- rte[42].out++;

        rte[39].out++ --> rte[40].in++;
        rte[39].in++ <-- rte[40].out++;

        rte[39].out++ --> rte[41].in++;
        rte[39].in++ <-- rte[41].out++;

        rte[40].out++ --> rte[43].in++;
        rte[40].in++ <-- rte[43].out++;

        rte[40].out++ --> rte[44].in++;
        rte[40].in++ <-- rte[44].out++;

        rte[40].out++ --> rte[52].in++;
        rte[40].in++ <-- rte[52].out++;

        rte[41].out++ --> rte[42].in++;
        rte[41].in++ <-- rte[42].out++;

        rte[41].out++ --> rte[43].in++;
        rte[41].in++ <-- rte[43].out++;

        rte[42].out++ --> rte[43].in++;
        rte[42].in++ <-- rte[43].out++;

        rte[43].out++ --> rte[46].in++;
        rte[43].in++ <-- rte[46].out++;

        rte[44].out++ --> rte[45].in++;
        rte[44].in++ <-- rte[45].out++;

        rte[44].out++ --> rte[47].in++;
        rte[44].in++ <-- rte[47].out++;

        rte[45].out++ --> rte[46].in++;
        rte[45].in++ <-- rte[46].out++;

        rte[46].out++ --> rte[49].in++;
        rte[46].in++ <-- rte[49].out++;

        rte[47].out++ --> rte[48].in++;
        rte[47].in++ <-- rte[48].out++;

        rte[48].out++ --> rte[50].in++;
        rte[48].in++ <-- rte[50].out++;

        rte[48].out++ --> rte[51].in++;
        rte[48].in++ <-- rte[51].out++;

        rte[49].out++ --> rte[50].in++;
        rte[49].in++ <-- rte[50].out++;

        rte[50].out++ --> rte[53].in++;
        rte[50].in++ <-- rte[53].out++;

        rte[51].out++ --> rte[52].in++;
        rte[51].in++ <-- rte[52].out++;

        rte[51].out++ --> rte[53].in++;
        rte[51].in++ <-- rte[53].out++;

        rte[52].out++ --> rte[53].in++;
        rte[52].in++ <-- rte[53].out++;

        rte[52].out++ --> rte[54].in++;
        rte[52].in++ <-- rte[54].out++;

        rte[53].out++ --> rte[56].in++;
        rte[53].in++ <-- rte[56].out++;

        rte[54].out++ --> rte[55].in++;
        rte[54].in++ <-- rte[55].out++;

        rte[55].out++ --> rte[56].in++;
        rte[55].in++ <-- rte[56].out++;
endmodule

network net60 : Net60
endnetwork


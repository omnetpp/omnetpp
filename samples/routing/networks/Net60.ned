//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

package networks;

import node.Node;
import ned.DatarateChannel;

//
// A network topology commonly known as the "NTT backbone".
//
network Net60
{
    types:
        channel Link extends DatarateChannel
        {
            parameters:
                delay = default(0.1ms);
                datarate = default(1Gbps);
        }
    submodules:
        rte[57]: Node {
            parameters:
                address = index;
        }
    connections allowunconnected:
        rte[0].port++ <--> Link <--> rte[1].port++;
        rte[1].port++ <--> Link <--> rte[2].port++;
        rte[1].port++ <--> Link <--> rte[4].port++;
        rte[3].port++ <--> Link <--> rte[4].port++;
        rte[4].port++ <--> Link <--> rte[5].port++;
        rte[4].port++ <--> Link <--> rte[7].port++;
        rte[5].port++ <--> Link <--> rte[6].port++;
        rte[5].port++ <--> Link <--> rte[10].port++;
        rte[6].port++ <--> Link <--> rte[7].port++;
        rte[6].port++ <--> Link <--> rte[9].port++;
        rte[7].port++ <--> Link <--> rte[8].port++;
        rte[7].port++ <--> Link <--> rte[12].port++;
        rte[9].port++ <--> Link <--> rte[11].port++;
        rte[10].port++ <--> Link <--> rte[11].port++;
        rte[10].port++ <--> Link <--> rte[13].port++;
        rte[11].port++ <--> Link <--> rte[12].port++;
        rte[11].port++ <--> Link <--> rte[14].port++;
        rte[12].port++ <--> Link <--> rte[15].port++;
        rte[13].port++ <--> Link <--> rte[18].port++;
        rte[14].port++ <--> Link <--> rte[15].port++;
        rte[14].port++ <--> Link <--> rte[22].port++;
        rte[15].port++ <--> Link <--> rte[16].port++;
        rte[16].port++ <--> Link <--> rte[17].port++;
        rte[16].port++ <--> Link <--> rte[23].port++;
        rte[18].port++ <--> Link <--> rte[19].port++;
        rte[18].port++ <--> Link <--> rte[21].port++;
        rte[19].port++ <--> Link <--> rte[20].port++;
        rte[19].port++ <--> Link <--> rte[26].port++;
        rte[20].port++ <--> Link <--> rte[21].port++;
        rte[20].port++ <--> Link <--> rte[24].port++;
        rte[20].port++ <--> Link <--> rte[25].port++;
        rte[21].port++ <--> Link <--> rte[22].port++;
        rte[21].port++ <--> Link <--> rte[23].port++;
        rte[21].port++ <--> Link <--> rte[24].port++;
        rte[23].port++ <--> Link <--> rte[24].port++;
        rte[24].port++ <--> Link <--> rte[28].port++;
        rte[25].port++ <--> Link <--> rte[27].port++;
        rte[26].port++ <--> Link <--> rte[32].port++;
        rte[27].port++ <--> Link <--> rte[29].port++;
        rte[27].port++ <--> Link <--> rte[30].port++;
        rte[27].port++ <--> Link <--> rte[32].port++;
        rte[28].port++ <--> Link <--> rte[29].port++;
        rte[28].port++ <--> Link <--> rte[31].port++;
        rte[29].port++ <--> Link <--> rte[31].port++;
        rte[30].port++ <--> Link <--> rte[36].port++;
        rte[31].port++ <--> Link <--> rte[33].port++;
        rte[32].port++ <--> Link <--> rte[34].port++;
        rte[33].port++ <--> Link <--> rte[37].port++;
        rte[34].port++ <--> Link <--> rte[35].port++;
        rte[34].port++ <--> Link <--> rte[40].port++;
        rte[35].port++ <--> Link <--> rte[36].port++;
        rte[35].port++ <--> Link <--> rte[39].port++;
        rte[36].port++ <--> Link <--> rte[37].port++;
        rte[37].port++ <--> Link <--> rte[38].port++;
        rte[38].port++ <--> Link <--> rte[39].port++;
        rte[38].port++ <--> Link <--> rte[42].port++;
        rte[39].port++ <--> Link <--> rte[40].port++;
        rte[39].port++ <--> Link <--> rte[41].port++;
        rte[40].port++ <--> Link <--> rte[43].port++;
        rte[40].port++ <--> Link <--> rte[44].port++;
        rte[40].port++ <--> Link <--> rte[52].port++;
        rte[41].port++ <--> Link <--> rte[42].port++;
        rte[41].port++ <--> Link <--> rte[43].port++;
        rte[42].port++ <--> Link <--> rte[43].port++;
        rte[43].port++ <--> Link <--> rte[46].port++;
        rte[44].port++ <--> Link <--> rte[45].port++;
        rte[44].port++ <--> Link <--> rte[47].port++;
        rte[45].port++ <--> Link <--> rte[46].port++;
        rte[46].port++ <--> Link <--> rte[49].port++;
        rte[47].port++ <--> Link <--> rte[48].port++;
        rte[48].port++ <--> Link <--> rte[50].port++;
        rte[48].port++ <--> Link <--> rte[51].port++;
        rte[49].port++ <--> Link <--> rte[50].port++;
        rte[50].port++ <--> Link <--> rte[53].port++;
        rte[51].port++ <--> Link <--> rte[52].port++;
        rte[51].port++ <--> Link <--> rte[53].port++;
        rte[52].port++ <--> Link <--> rte[53].port++;
        rte[52].port++ <--> Link <--> rte[54].port++;
        rte[53].port++ <--> Link <--> rte[56].port++;
        rte[54].port++ <--> Link <--> rte[55].port++;
        rte[55].port++ <--> Link <--> rte[56].port++;
}


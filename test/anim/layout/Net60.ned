network Net60 extends Base
{
    types:
        channel C extends ned.DatarateChannel
        {
            parameters:
                delay = 0.1ms;
                datarate = 1Gbps;
        }
	module Node {
            @display("i=abstract/router_s");
	    gates:
		inout port[];
	    connections allowunconnected:
	}
    submodules:
        rte[57]: Node;

    connections allowunconnected:
        rte[0].port++ <--> C <--> rte[1].port++;
        rte[1].port++ <--> C <--> rte[2].port++;
        rte[1].port++ <--> C <--> rte[4].port++;
        rte[3].port++ <--> C <--> rte[4].port++;
        rte[4].port++ <--> C <--> rte[5].port++;
        rte[4].port++ <--> C <--> rte[7].port++;
        rte[5].port++ <--> C <--> rte[6].port++;
        rte[5].port++ <--> C <--> rte[10].port++;
        rte[6].port++ <--> C <--> rte[7].port++;
        rte[6].port++ <--> C <--> rte[9].port++;
        rte[7].port++ <--> C <--> rte[8].port++;
        rte[7].port++ <--> C <--> rte[12].port++;
        rte[9].port++ <--> C <--> rte[11].port++;
        rte[10].port++ <--> C <--> rte[11].port++;
        rte[10].port++ <--> C <--> rte[13].port++;
        rte[11].port++ <--> C <--> rte[12].port++;
        rte[11].port++ <--> C <--> rte[14].port++;
        rte[12].port++ <--> C <--> rte[15].port++;
        rte[13].port++ <--> C <--> rte[18].port++;
        rte[14].port++ <--> C <--> rte[15].port++;
        rte[14].port++ <--> C <--> rte[22].port++;
        rte[15].port++ <--> C <--> rte[16].port++;
        rte[16].port++ <--> C <--> rte[17].port++;
        rte[16].port++ <--> C <--> rte[23].port++;
        rte[18].port++ <--> C <--> rte[19].port++;
        rte[18].port++ <--> C <--> rte[21].port++;
        rte[19].port++ <--> C <--> rte[20].port++;
        rte[19].port++ <--> C <--> rte[26].port++;
        rte[20].port++ <--> C <--> rte[21].port++;
        rte[20].port++ <--> C <--> rte[24].port++;
        rte[20].port++ <--> C <--> rte[25].port++;
        rte[21].port++ <--> C <--> rte[22].port++;
        rte[21].port++ <--> C <--> rte[23].port++;
        rte[21].port++ <--> C <--> rte[24].port++;
        rte[23].port++ <--> C <--> rte[24].port++;
        rte[24].port++ <--> C <--> rte[28].port++;
        rte[25].port++ <--> C <--> rte[27].port++;
        rte[26].port++ <--> C <--> rte[32].port++;
        rte[27].port++ <--> C <--> rte[29].port++;
        rte[27].port++ <--> C <--> rte[30].port++;
        rte[27].port++ <--> C <--> rte[32].port++;
        rte[28].port++ <--> C <--> rte[29].port++;
        rte[28].port++ <--> C <--> rte[31].port++;
        rte[29].port++ <--> C <--> rte[31].port++;
        rte[30].port++ <--> C <--> rte[36].port++;
        rte[31].port++ <--> C <--> rte[33].port++;
        rte[32].port++ <--> C <--> rte[34].port++;
        rte[33].port++ <--> C <--> rte[37].port++;
        rte[34].port++ <--> C <--> rte[35].port++;
        rte[34].port++ <--> C <--> rte[40].port++;
        rte[35].port++ <--> C <--> rte[36].port++;
        rte[35].port++ <--> C <--> rte[39].port++;
        rte[36].port++ <--> C <--> rte[37].port++;
        rte[37].port++ <--> C <--> rte[38].port++;
        rte[38].port++ <--> C <--> rte[39].port++;
        rte[38].port++ <--> C <--> rte[42].port++;
        rte[39].port++ <--> C <--> rte[40].port++;
        rte[39].port++ <--> C <--> rte[41].port++;
        rte[40].port++ <--> C <--> rte[43].port++;
        rte[40].port++ <--> C <--> rte[44].port++;
        rte[40].port++ <--> C <--> rte[52].port++;
        rte[41].port++ <--> C <--> rte[42].port++;
        rte[41].port++ <--> C <--> rte[43].port++;
        rte[42].port++ <--> C <--> rte[43].port++;
        rte[43].port++ <--> C <--> rte[46].port++;
        rte[44].port++ <--> C <--> rte[45].port++;
        rte[44].port++ <--> C <--> rte[47].port++;
        rte[45].port++ <--> C <--> rte[46].port++;
        rte[46].port++ <--> C <--> rte[49].port++;
        rte[47].port++ <--> C <--> rte[48].port++;
        rte[48].port++ <--> C <--> rte[50].port++;
        rte[48].port++ <--> C <--> rte[51].port++;
        rte[49].port++ <--> C <--> rte[50].port++;
        rte[50].port++ <--> C <--> rte[53].port++;
        rte[51].port++ <--> C <--> rte[52].port++;
        rte[51].port++ <--> C <--> rte[53].port++;
        rte[52].port++ <--> C <--> rte[53].port++;
        rte[52].port++ <--> C <--> rte[54].port++;
        rte[53].port++ <--> C <--> rte[56].port++;
        rte[54].port++ <--> C <--> rte[55].port++;
        rte[55].port++ <--> C <--> rte[56].port++;
}



module Node {  // for testing that the local Node type takes precedence (this one has no gates which would cause an error)
}

network Cn {
  types:
    channel Bogus extends NuSuchChannel { }  //XXX test that corresponding warning is printed correctly
    channel C extends DatarateChannel {
      parameters:
        delay = 1ms;
        datarate = 1Gbps;
    }
    module Node {
      gates:
        output out[];
        input in[];
        //output scalarOut; //XXX just for testing ++, see below
      connections allowunconnected:
    }
  submodules:
    n1: Node;
    n2: Node;
  connections:
    n1.out++ --> C --> n2.in++;
    n1.in++ <-- C <-- n2.out++;

    //n1.scalarOut++ --> C --> n2.in++;
    //n1.out++ --> C --> n2.nope++;
}

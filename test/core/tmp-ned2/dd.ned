// bidirectional connection with scalar gates
network DDa {
  types:
    module Node {
      gates:
        inout p;
      connections allowunconnected:
    }
  submodules:
    n1: Node;  
    n2: Node;
  connections:
    n1.p <--> n2.p;
}


// bidirectional connection with indexed gates
network DDb {
  parameters:
    int n = 5;
  types:
    module Node {
      gates:
        //inout p[n]; //FIXME "n" should probably be visible here
        inout p[3];
      connections allowunconnected:
    }
  submodules:
    n1: Node;  
    n2: Node;
  connections allowunconnected:
    n1.p[0] <--> n2.p[0];
}

// bidirectional connection with gate++
network DDc {
  types:
    module Node {
      gates:
        inout p[];
      connections allowunconnected:
    }
  submodules:
    n1: Node;  
    n2: Node;
  connections allowunconnected:
    n1.p++ <--> n2.p++;
}
 
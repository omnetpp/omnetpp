module Base {
    @display("i=misc/node,red,100;p=100,100,row");
}

module Derived1 {
    @display("i=block/cogwheel,green,-;p=-,,column;t=Hello");  // override, unset, add new
}

network DispStr {
  submodules:
    dump: Dump;
    derived1: Derived1 {
        @display("p=,,ring");
    }
}


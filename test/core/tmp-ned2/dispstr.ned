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


network Error1 {
    @display("=foo");  // missing tagname
}

network Error2 {
    @display(",foo");  // missing tagname
}

network Error3 {
    @display("=;=;=");  // missing tagname (?)
}

network Error4 {
    @display("i**=foo"); // illegal tag name
}

network Error5 {
    @display("i,,=foo"); // illegal tag name (?)
}

network Error6 {
    @display("x=1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1"); // too many args
}

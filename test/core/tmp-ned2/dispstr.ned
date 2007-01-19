module Base {
    @display("i=misc/node");
}

module Node extends Base {
    @display(",foo,bar");  // syntax errors should not cause crash!
}

network Net {
    submodules:
        n : Node {}
}
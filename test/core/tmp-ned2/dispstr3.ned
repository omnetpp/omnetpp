//
// Cases during display string merging:
//  add tag
//  add tagarg
//  set (overwrite) tagarg
//  delete (unset) tagarg
//

network Base3 {
    @display("aa=aaa1,aaa2,,aaa4;bb=bbb1,bbb2");
}

network AddTag3 extends Base3 {
    @display("cc=CCC1,,CCC3");  // add
  submodules:
    dump: Dump;
}

network AddTagArg3 extends Base3 {
    @display("aa=,,,,AAA5,AAA6;bb=,,BBB3");  // add tagarg
  submodules:
    dump: Dump;
}

network Overwrite3 extends Base3 {
    @display("aa=AAA1,,,AAA4;bb=,BBB2");  // overwrite tagarg
  submodules:
    dump: Dump;
}

network Unset3 extends Base3 {
    @display("aa=,-;bb=-,-");  // delete tagarg  FIXME unset does not work yet
  submodules:
    dump: Dump;
}


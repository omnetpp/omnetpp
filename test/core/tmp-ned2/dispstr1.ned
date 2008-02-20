//
// Cases during display string merging:
//  add tag
//  add tagarg
//  set (overwrite) tagarg
//  delete (unset) tagarg
//

module Base {
    @display("aa=aaa1,aaa2,,aaa4;bb=bbb1,bbb2");
}

module AddTag extends Base {
    @display("cc=CCC1,,CCC3");  // add
}

module AddTagArg extends Base {
    @display("aa=,,,,AAA5,AAA6;bb=,,BBB3");  // add tagarg
}

module Overwrite extends Base {
    @display("aa=AAA1,,,AAA4;bb=,BBB2");  // overwrite tagarg
}

module Unset extends Base {
    @display("aa=,-;bb=-,-");  // delete tagarg  FIXME unset does not work yet
}

network DispStr {
  submodules:
    dump: Dump;
    base : Base;
    addtag: AddTag;
    addtagarg: AddTagArg;
    overwrite: Overwrite;
    unset: Unset;
}


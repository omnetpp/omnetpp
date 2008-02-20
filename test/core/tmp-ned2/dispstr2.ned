//
// Cases during display string merging:
//  add tag
//  add tagarg
//  set (overwrite) tagarg
//  delete (unset) tagarg
//


network DispStr2 {
  types:
    module Base {
        @display("aa=aaa1,aaa2,,aaa4;bb=bbb1,bbb2");
    }
  submodules:
    dump: Dump;
    base : Base;
    addTag : Base {
        @display("cc=CCC1,,CCC3");  // add
    }
    addTagArg : Base {
        @display("aa=,,,,AAA5,AAA6;bb=,,BBB3");  // add tagarg
    }
    overwrite : Base {
        @display("aa=AAA1,,,AAA4;bb=,BBB2");  // overwrite tagarg
    }
    unset : Base {
        @display("aa=,-;bb=-,-");  // delete tagarg  FIXME unset does not work yet
    }
}


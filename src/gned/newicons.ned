import
    "one",
    "two"; // es meg valami
           // meg trailing

           // lofasz

module NewIcons
    submodules:
        submod: ANode;
            display: "p=61,54;i=card1";
        submod1: ANode;
            display: "p=151,55;i=monitor";
        submod2: ANode;
            display: "p=125,122;i=card2";
        submod3: ANode;
            display: "p=227,55;i=router2";
        submod4: ANode;
            display: "p=92,232;i=drive1";
        submod5: ANode;
            display: "p=150,249;i=floppy1";
        submod6: ANode;
            display: "p=282,115;i=mailbox";
        submod7: ANode;
            display: "p=238,184;i=harddisk";
        submod8: ANode;
            display: "p=52,119;i=printer";
        submod9: ANode;
            display: "p=50,187;i=folder";
        submod10: ANode;
            display: "p=197,120;i=comp2";
        submod12: ANode;
            display: "p=159,185;i=pccard";
        submod13: ANode;
            display: "p=212,246;i=ball2";
        submod14: ANode;
            display: "p=189,303;i=pccard_s";
        submod15: ANode;
            display: "p=61,271;i=handset1_s";
        submod17: ANode;
            display: "p=288,309;i=cloud_l";
        submod18: ANode;
            display: "p=333,214;i=cogwheel2";
        submod19: ANode;
            display: "p=374,47;i=pctower1";
        submod20: ANode;
            display: "p=301,49;i=pctower2";
        submod21: ANode;
            display: "p=374,119;i=switch2";
        submod22: ANode;
            display: "p=419,227;i=switch1";
        submod23: ANode;
            display: "p=387,302;i=dialphone";
        submod24: ANode;
            display: "p=67,352;i=app";
        submod25: ANode;
            display: "p=142,370;i=redbox";
        submod26: ANode;
            display: "p=225,374;i=cloud";
        submod28: ANode;
            display: "p=507,165;i=switch1_s";
        submod29: ANode;
            display: "p=496,89;i=router3";
        submod30: ANode;
            display: "p=446,38;i=penguin";
        submod33: ANode;
            display: "p=507,298;i=telnet";
        submod34: ANode;
            display: "p=437,341;i=browser";
        submod35: ANode;
            display: "p=350,351;i=ball2_vs";
        submod36: ANode;
            display: "p=342,415;i=ball";
        submod37: ANode;
            display: "p=396,420;i=pda1";
        submod38: ANode;
            display: "p=456,420;i=pda2";
        submod39: ANode;
            display: "p=512,416;i=pda3";
        submod40: ANode;
            display: "p=104,288;i=handset1";
        submod41: ANode;
            display: "p=144,308;i=handset2_s";
        submod42: ANode;
            display: "p=48,436;i=laptop";
        submod43: ANode;
            display: "p=104,428;i=laptop2";
        submod44: ANode;
            display: "p=516,220;i=server1";
        submod46: ANode;
            display: "p=288,376;i=ball2_s";
        submod47: ANode;
            display: "p=184,416;i=cloud_s";
    connections:
        submod.out --> submod1.in;
        submod1.out --> submod3.in;
        submod3.out --> submod6.in;
        submod6.out --> submod10.in;
        submod10.out --> submod2.in;
        submod2.out --> submod8.in;
        submod8.out --> submod9.in;
        submod9.out --> submod4.in;
        submod4.out --> submod5.in;
        submod5.out --> submod12.in;
        submod12.out --> submod7.in;
        submod7.out --> submod6.in;
        submod12.out --> submod2.in;
        submod15.out --> submod4.in;
        submod13.out --> submod17.in;
        submod14.out --> submod17.in;
        submod18.out --> submod17.in;
        submod7.out --> submod18.in;
        submod7.out --> submod13.in;
        submod3.out --> submod20.in;
        submod20.out --> submod19.in;
        submod19.out --> submod21.in;
        submod21.out --> submod22.in;
        submod22.out --> submod23.in;
        submod17.out --> submod23.in;
        submod21.out --> submod30.in;
        submod21.out --> submod29.in;
        submod21.out --> submod28.in;
        submod34.out --> submod22.in;
        submod33.out --> submod22.in;
        submod33.out --> submod34.in;
        submod18.out --> submod21.in;
        submod17.out --> submod26.in;
        submod24.out --> submod25.in;
        submod17.out --> submod35.in;
        submod35.out --> submod36.in;
        submod39.out --> submod34.in;
        submod34.out --> submod38.in;
        submod34.out --> submod37.in;
        submod21.out --> submod44.in;
        submod36.out --> submod46.in;
        submod26.out --> submod47.in;
endmodule












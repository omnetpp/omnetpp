module Module 
    submodules: 
        submod: Module; 
            display: "p=65,54;i=card1;b=31,31";
        submod1: Module; 
            display: "p=155,55;i=monitor;b=32,32";
        submod2: Module; 
            display: "p=125,122;i=card2;b=31,31";
        submod3: Module; 
            display: "p=227,55;i=router2;b=32,32";
        submod4: Module; 
            display: "p=84,224;i=drive1;b=32,32";
        submod5: Module; 
            display: "p=150,249;i=floppy1;b=32,32";
        submod6: Module; 
            display: "p=282,115;i=mailbox;b=32,32";
        submod7: Module; 
            display: "p=238,184;i=harddisk;b=32,25";
        submod8: Module; 
            display: "p=52,119;i=printer;b=32,32";
        submod9: Module; 
            display: "p=50,187;i=folder;b=31,25";
        submod10: Module; 
            display: "p=197,120;i=comp2;b=32,32";
        submod12: Module; 
            display: "p=159,185;i=pccard;b=32,32";
        submod13: Module; 
            display: "p=228,254;i=ball2;b=32,32";
        submod14: Module; 
            display: "p=197,311;i=pccard_s;b=30,16";
        submod15: Module; 
            display: "p=61,271;i=handset1_s;b=20,30";
        submod17: Module; 
            display: "p=296,301;i=ball2_s;b=32,32";
        submod18: Module; 
            display: "p=333,214;i=cogwheel2;b=30,20";
        submod19: Module; 
            display: "p=382,47;i=pctower1;b=18,31";
        submod20: Module; 
            display: "p=301,49;i=pctower2;b=22,31";
        submod21: Module; 
            display: "p=414,119;i=switch2;b=17,18";
        submod22: Module; 
            display: "p=419,243;i=dialphone;b=32,19";
        submod23: Module; 
            display: "p=371,310;i=switch1;b=32,32";
        submod24: Module; 
            display: "p=67,352;i=app;b=32,32";
        submod25: Module; 
            display: "p=142,370;i=redbox;b=32,32";
        submod26: Module; 
            display: "p=257,374;i=gen;b=32,30";
        submod28: Module; 
            display: "p=491,181;i=switch1_s;b=32,32";
        submod29: Module; 
            display: "p=504,113;i=router3;b=32,32";
        submod30: Module; 
            display: "p=486,54;i=penguin;b=30,35";
        submod33: Module; 
            display: "p=499,266;i=telnet;b=32,32";
        submod34: Module; 
            display: "p=437,341;i=browser;b=40,37";
        submod35: Module; 
            display: "p=326,343;i=ball2_vs;b=12,6";
        submod36: Module; 
            display: "p=342,391;i=ball;b=30,30";
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
        submod21.out --> submod22.in display "m=,60,0,60,0"; 
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
endmodule 












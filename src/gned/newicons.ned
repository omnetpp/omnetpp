module Module 
    submodules: 
        submod: Module; 
            display: "p=65,54;i=card1;b=31,31";
        submod1: Module; 
            display: "p=155,55;i=monitor;b=32,32";
        submod2: Module; 
            display: "p=125,122;i=card2;b=31,31";
        submod3: Module; 
            display: "p=227,55;i=earth2;b=32,32";
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
            display: "p=197,120;i=terminal;b=32,32";
        submod12: Module; 
            display: "p=159,185;i=pccard;b=32,32";
        submod13: Module; 
            display: "p=228,254;i=node;b=32,32";
        submod14: Module; 
            display: "p=197,311;i=pccard_s;b=30,16";
        submod15: Module; 
            display: "p=61,271;i=handset1_s;b=20,30";
        submod17: Module; 
            display: "p=296,302;i=floppy;b=32,32";
        submod18: Module; 
            display: "p=316,205;i=node_s;b=22,36";
        submod19: Module; 
            display: "p=382,47;i=tower1;b=18,31";
        submod20: Module; 
            display: "p=301,49;i=tower2;b=22,31";
        submod21: Module; 
            display: "p=414,119;i=terminal_s;b=17,18";
        submod22: Module; 
            display: "p=419,243;i=dialphone;b=32,19";
        submod23: Module; 
            display: "p=371,310;i=film;b=32,32";
        submod24: Module; 
            display: "p=67,352;i=app;b=32,32";
        submod25: Module; 
            display: "p=142,370;i=redbox;b=32,32";
        submod26: Module; 
            display: "p=257,374;i=gen;b=32,30";
        submod28: Module; 
            display: "p=491,181;i=macos;b=32,32";
        submod29: Module; 
            display: "p=504,113;i=windows;b=32,32";
        submod30: Module; 
            display: "p=486,54;i=penguin;b=30,35";
        submod33: Module; 
            display: "p=499,266;i=telnet;b=32,32";
        submod34: Module; 
            display: "p=437,341;i=browser;b=40,37";
    connections: 
        submod.out --> submod1.in display "m=m,64,69,16,47"; 
        submod1.out --> submod3.in display "m=m,58,31,47,69"; 
        submod3.out --> submod6.in display "m=m,61,56,34,34"; 
        submod6.out --> submod10.in display "m=m,22,33,44,41"; 
        submod10.out --> submod2.in display "m=m,44,42,47,47"; 
        submod2.out --> submod8.in display "m=m,47,50,44,44"; 
        submod8.out --> submod9.in display "m=m,25,50,40,50"; 
        submod9.out --> submod4.in display "m=m,47,75,50,75"; 
        submod4.out --> submod5.in display "m=m,50,70,53,69"; 
        submod5.out --> submod12.in display "m=m,53,50,36,54"; 
        submod12.out --> submod7.in display "m=m,44,50,50,29"; 
        submod7.out --> submod6.in display "m=m,50,32,38,59"; 
        submod12.out --> submod2.in display "m=m,47,50,50,77"; 
        submod15.out --> submod4.in display "m=m,82,29,47,56"; 
        submod13.out --> submod17.in display "m=m,44,58,71,50"; 
        submod14.out --> submod17.in display "m=m,53,55,64,38"; 
        submod18.out --> submod17.in display "m=m,54,56,79,48"; 
        submod7.out --> submod18.in display "m=m,47,50,60,50"; 
        submod7.out --> submod13.in display "m=m,53,64,42,41"; 
        submod3.out --> submod20.in display "m=m,47,36,27,23"; 
        submod20.out --> submod19.in display "m=m,50,50,83,47"; 
        submod19.out --> submod21.in display "m=m,36,56,50,39"; 
        submod21.out --> submod22.in display "m=m,50,41,56,50"; 
        submod22.out --> submod23.in display "m=m,56,50,75,34"; 
        submod17.out --> submod23.in display "m=m,72,61,25,28"; 
        submod21.out --> submod30.in display "m=m,65,18,57,35"; 
        submod21.out --> submod29.in display "m=m,65,41,25,53"; 
        submod21.out --> submod28.in display "m=m,55,23,25,47"; 
        submod34.out --> submod22.in display "m=m,45,30,66,106"; 
        submod33.out --> submod22.in display "m=m,50,33,63,50"; 
        submod33.out --> submod34.in display "m=m,33,72,68,58"; 
        submod18.out --> submod21.in display "m=m,63,56,69,17"; 
        submod17.out --> submod26.in display "m=m,25,61,47,20"; 
        submod24.out --> submod25.in display "m=m,69,34,88,34"; 
endmodule 











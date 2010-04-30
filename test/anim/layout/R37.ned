network R37 extends Base
{
    submodules:
        LSR0: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR1: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR2: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR3: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR4: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR5: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR6: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR7: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR8: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR9: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR10: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR11: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR12: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR13: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR14: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR15: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR16: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR17: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR18: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR19: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR20: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR21: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR22: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR23: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR24: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR25: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR26: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR27: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR28: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR29: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR30: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR31: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR32: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR33: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR34: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR35: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR36: Node {
            parameters:
                @display("i=abstract/router_s");

        };
        host1: Node {
            parameters:
                @display("i=device/laptop_s");
        };
        host2: Node {
            parameters:
                @display("i=device/laptop_s");

        };
    connections:
        LSR7.out++ --> LSR0.in++;
        LSR7.in++ <-- LSR0.out++; // cost 259
        LSR8.out++ --> LSR3.in++;
        LSR8.in++ <-- LSR3.out++; // cost 474
        LSR9.out++ --> LSR4.in++;
        LSR9.in++ <-- LSR4.out++; // cost 540
        LSR11.out++ --> LSR7.in++;
        LSR11.in++ <-- LSR7.out++; // cost 264
        LSR12.out++ --> LSR11.in++;
        LSR12.in++ <-- LSR11.out++; // cost 275
        LSR13.out++ --> LSR0.in++;
        LSR13.in++ <-- LSR0.out++; // cost 1067
        LSR13.out++ --> LSR5.in++;
        LSR13.in++ <-- LSR5.out++; // cost 610
        LSR13.out++ --> LSR10.in++;
        LSR13.in++ <-- LSR10.out++; // cost 462
        LSR14.out++ --> LSR0.in++;
        LSR14.in++ <-- LSR0.out++; // cost 552
        LSR14.out++ --> LSR4.in++;
        LSR14.in++ <-- LSR4.out++; // cost 381
        LSR14.out++ --> LSR12.in++;
        LSR14.in++ <-- LSR12.out++; // cost 592
        LSR16.out++ --> LSR8.in++;
        LSR16.in++ <-- LSR8.out++; // cost 436
        LSR18.out++ --> LSR0.in++;
        LSR18.in++ <-- LSR0.out++; // cost 540
        LSR18.out++ --> LSR5.in++;
        LSR18.in++ <-- LSR5.out++; // cost 239
        LSR18.out++ --> LSR10.in++;
        LSR18.in++ <-- LSR10.out++; // cost 690
        LSR18.out++ --> LSR17.in++;
        LSR18.in++ <-- LSR17.out++; // cost 1977
        LSR20.out++ --> LSR2.in++;
        LSR20.in++ <-- LSR2.out++; // cost 760
        LSR20.out++ --> LSR6.in++;
        LSR20.in++ <-- LSR6.out++; // cost 834
        LSR20.out++ --> LSR17.in++;
        LSR20.in++ <-- LSR17.out++; // cost 751
        LSR21.out++ --> LSR2.in++;
        LSR21.in++ <-- LSR2.out++; // cost 508
        LSR21.out++ --> LSR6.in++;
        LSR21.in++ <-- LSR6.out++; // cost 756
        LSR21.out++ --> LSR19.in++;
        LSR21.in++ <-- LSR19.out++; // cost 411
        LSR23.out++ --> LSR4.in++;
        LSR23.in++ <-- LSR4.out++; // cost 757
        LSR23.out++ --> LSR12.in++;
        LSR23.in++ <-- LSR12.out++; // cost 456
        LSR23.out++ --> LSR22.in++;
        LSR23.in++ <-- LSR22.out++; // cost 522
        LSR24.out++ --> LSR9.in++;
        LSR24.in++ <-- LSR9.out++; // cost 722
        LSR24.out++ --> LSR15.in++;
        LSR24.in++ <-- LSR15.out++; // cost 1182
        LSR25.out++ --> LSR1.in++;
        LSR25.in++ <-- LSR1.out++; // cost 1363
        LSR26.out++ --> LSR6.in++;
        LSR26.in++ <-- LSR6.out++; // cost 747
        LSR26.out++ --> LSR7.in++;
        LSR26.in++ <-- LSR7.out++; // cost 393
        LSR26.out++ --> LSR18.in++;
        LSR26.in++ <-- LSR18.out++; // cost 514
        LSR26.out++ --> LSR19.in++;
        LSR26.in++ <-- LSR19.out++; // cost 594
        LSR27.out++ --> LSR4.in++;
        LSR27.in++ <-- LSR4.out++; // cost 420
        LSR27.out++ --> LSR8.in++;
        LSR27.in++ <-- LSR8.out++; // cost 668
        LSR28.out++ --> LSR21.in++;
        LSR28.in++ <-- LSR21.out++; // cost 907
        LSR28.out++ --> LSR22.in++;
        LSR28.in++ <-- LSR22.out++; // cost 720
        LSR28.out++ --> LSR25.in++;
        LSR28.in++ <-- LSR25.out++; // cost 638
        LSR29.out++ --> LSR2.in++;
        LSR29.in++ <-- LSR2.out++; // cost 1244
        LSR29.out++ --> LSR17.in++;
        LSR29.in++ <-- LSR17.out++; // cost 471
        LSR30.out++ --> LSR1.in++;
        LSR30.in++ <-- LSR1.out++; // cost 794
        LSR30.out++ --> LSR3.in++;
        LSR30.in++ <-- LSR3.out++; // cost 487
        LSR31.out++ --> LSR9.in++;
        LSR31.in++ <-- LSR9.out++; // cost 777
        LSR31.out++ --> LSR15.in++;
        LSR31.in++ <-- LSR15.out++; // cost 597
        LSR32.out++ --> LSR12.in++;
        LSR32.in++ <-- LSR12.out++; // cost 271
        LSR32.out++ --> LSR26.in++;
        LSR32.in++ <-- LSR26.out++; // cost 600
        LSR33.out++ --> LSR23.in++;
        LSR33.in++ <-- LSR23.out++; // cost 534
        LSR33.out++ --> LSR27.in++;
        LSR33.in++ <-- LSR27.out++; // cost 376
        LSR34.out++ --> LSR4.in++;
        LSR34.in++ <-- LSR4.out++; // cost 775
        LSR34.out++ --> LSR15.in++;
        LSR34.in++ <-- LSR15.out++; // cost 1370
        LSR34.out++ --> LSR16.in++;
        LSR34.in++ <-- LSR16.out++; // cost 383
        LSR35.out++ --> LSR1.in++;
        LSR35.in++ <-- LSR1.out++; // cost 1500
        LSR35.out++ --> LSR3.in++;
        LSR35.in++ <-- LSR3.out++; // cost 551
        LSR35.out++ --> LSR28.in++;
        LSR35.in++ <-- LSR28.out++; // cost 783
        LSR35.out++ --> LSR33.in++;
        LSR35.in++ <-- LSR33.out++; // cost 400
        LSR36.out++ --> LSR19.in++;
        LSR36.in++ <-- LSR19.out++; // cost 507
        LSR36.out++ --> LSR22.in++;
        LSR36.in++ <-- LSR22.out++; // cost 327
        LSR36.out++ --> LSR32.in++;
        LSR36.in++ <-- LSR32.out++; // cost 218

        host1.out++ --> LSR2.in++;
        host1.in++ <-- LSR2.out++;
        host2.out++ --> LSR34.in++;
        host2.in++ <-- LSR34.out++;

}

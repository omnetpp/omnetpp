
simple Mod
endsimple

module Box1
    submodules:
        mod: Mod;
            display: "b=32,30;p=68,69;i=gen";
endmodule

module Box2
endmodule

module ChangeParent
    submodules:
        box1: Box1;
            display: "p=71,109;b=40,28";
        box2: Box2;
            display: "p=230,143;b=40,28";
endmodule

network changeParent : ChangeParent
endnetwork


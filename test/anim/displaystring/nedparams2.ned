module EmptyStringParams
{
    parameters:
        @display("bgb=$width,$height;bgg=$gridmajor,$gridminor;bgi=$image,$imagemode;bgt=15,15,$label");
        string width = default("");
        string height = default("");
        string gridmajor = default("");
        string gridminor = default("");
        string image = default("background/green");
        string imagemode = default("");
        string label = default("");
        string nodex = default("");
        string nodey = default("");
    types:
        module Node {}
    submodules:
        node: Node {@display("p=$nodex,$nodey");}
}

module NonemptyStringParams extends EmptyStringParams
{
    parameters:
        width = "50";
        height = "30";
        gridmajor = "10";
        gridminor = "5";
        image = "background/green";
        imagemode = "tile";
        label = "Hello";
        nodex = "10";
        nodey = "10";
}

module NumericParams
{
    parameters:
        @display("bgb=$width,$height;bgg=$gridmajor,$gridminor");
        int width = 50;
        int height = 30;
        int gridmajor = 10;
        int gridminor = 5;
        int nodex = 10;
        int nodey = 10;
    types:
        module Node {}
    submodules:
        node: Node {@display("p=$nodex,$nodey");}
}

module ParamsWithBraces
{
    parameters:
        @display("bgb=0${ww}0,0${hh}0;bgg=${gridmajor},${gridminor};bgi=backgro${ii}en,${imagemode};bgt=8,8,Hel${label}ld");
        int ww = 5; // --> "050"
        int hh = 3; // --> "030"
        int gridmajor = 10;
        int gridminor = 5;
        string ii = "und/gre";
        string imagemode = "tile";
        string label = "lo wor";
        int nodex = 10;
        int nodey = 10;
    types:
        module Node {}
    submodules:
        node: Node {@display("p=0${nodex},0${nodey}");}
}



{{copyright}}

{{rootpackage:}}package {{rootpackage}};

//
// Two instances (tic and toc) of Txc connected.
//
network {{ProjectName}}
{
    submodules:
        tic: Txc;
        toc: Txc;
    connections:
        tic.out --> {delay = 100ms;} --> toc.in;
        tic.in <-- {delay = 100ms;} <-- toc.out;
}


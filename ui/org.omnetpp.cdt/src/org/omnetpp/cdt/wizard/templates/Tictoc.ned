${bannercomment}

#if($simulationspackage!="")package ${simulationspackage};#end
#if($rootpackage!="")import ${rootpackage}.Txc;#end

//
// Two instances (tic and toc) of Txc connected.
//
network Tictoc
{
    submodules:
        tic: Txc;
        toc: Txc;
    connections:
        tic.out --> {delay = 100ms;} --> toc.in;
        tic.in <-- {delay = 100ms;} <-- toc.out;
}


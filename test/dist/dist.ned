//-------------------------------------------------------------
// file: dist.ned
//-------------------------------------------------------------

simple Dist
    parameters:
        variate : numeric,
        excel: string,
        n : numeric,
        discrete: bool,
        numcells: numeric,
        firstvals : numeric,
        file: string;
endsimple


network dist : Dist
endnetwork



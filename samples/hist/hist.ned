//-------------------------------------------------------------
// file: hist.ned
//        (part of HIST - an OMNeT++ demo simulation)
//-------------------------------------------------------------

simple HistogramDemo
        parameters: num_obs, num_cells, fixed_range, num_first, range_ext;
endsimple

network Histograms : HistogramDemo
         parameters:
            num_obs = input( 200, "Number of observations: "),
            num_cells = input( 20, "Number of cells: "),
            fixed_range = input( 0, "Auto (0) or fixed (1) histogram range? "),
            num_first = input( 50, "Number of observations used for range estimation: "),
            range_ext = input( 1.3, "Range extension factor (usually 1.0 .. 2): ");
endnetwork


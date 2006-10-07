// Please check syntax highlight of code below.
module SyntaxHighlight_1
    parameters:
        numApps : numeric,
        destAddr: string, // this is <i>italic</i>, and we have "string contstants"
                          // here as well as numbers 1, 2, 3.14, 6e23 and
                          // keywords simple, module, gate, index, sizeof. 
                          // In a comment, none of them should be highlighted.
                          // But links to module etc. names should be
                          // hyperlinked, such as SyntaxHighlight_1 or
                          // Formatting_1.
        protId: numeric,  // This the <u>first</u> line
                          //
                          // of a really long comment that should describe
                          // where protocolIds come from, and so on...
        waitMean: numeric;// this comment contains a link to an external page:
                          // mean for <a href="http://mathworld.wolfram.com/ExponentialDistribution.html">
                          // exponential</a> interarrival times

    gates:
        in: in;
        out: out;

    submodules:
        gen: Gen[numApps];
            parameters:
                alpha = 6,
                beta = 2*1.5708,
                gamma = a*6e+23+c,
                label = "just a string";
            display: "p=60,60,col;b=40,24";
        sink: Sink[numApps];
            display: "p=250,60,col;b=40,24";
        llc: LLC;
            gatesizes:
                UpperLayer_in[numApps],
                UpperLayer_out[numApps];
            display: "i=fork;p=155,120";
        mac: MAC;
            display: "i=queue;p=155,200";
    connections:
        llc.LowerLayer_in <-- mac.UpperLayer_out;
        llc.LowerLayer_out --> mac.UpperLayer_in;
        mac.LowerLayer_in <-- in;
        mac.LowerLayer_out --> out;

        for i=0..numApps-1 do
            gen[i].out --> llc.UpperLayer_in[i];
            sink[i].in <-- llc.UpperLayer_out[i];
        endfor;
endmodule


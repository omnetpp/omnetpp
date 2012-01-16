network ConnLabelPlacement
{
    submodules:
        foooL: Dummy;
        baaaarL: Dummy;

        foooR: Dummy;
        baaaarR: Dummy;

        foooT: Dummy;
        baaaarT: Dummy;

    connections allowunconnected:
        // "t" tag 2nd arg: l/r/t (for beginning/end/middle)
        foooL.out++ --> {@display("t=fooo>baaar 192.168.123.210,l");} --> baaaarL.in++;
        foooL.in++  <-- {@display("t=baaar>fooo 192.168.123.210,l");} <-- baaaarL.out++;

        foooR.out++ --> {@display("t=fooo>baaar 192.168.123.210,r");} --> baaaarR.in++;
        foooR.in++  <-- {@display("t=baaar>fooo 192.168.123.210,r");} <-- baaaarR.out++;

        foooT.out++ --> {@display("t=fooo>baaar 192.168.123.210,t");} --> baaaarT.in++;
        foooT.in++  <-- {@display("t=baaar>fooo 192.168.123.210,t");} <-- baaaarT.out++;
}



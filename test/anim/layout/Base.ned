module Base 
{
        int numFixed = default(0);
        string bgWidth = default("");
        string bgHeight = default("");
//        @display("bgb=$bgWidth,$bgHeight");
//        @display("bgb=500,300");
    submodules:
        fixed[numFixed] : Node {
            @display("i=block/broadcast_s;p=100,100,row");
        }
}

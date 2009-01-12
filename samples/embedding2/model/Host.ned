@namespace(aloha);
simple Host
{
    parameters:
        double txRate @unit(bps);
        double radioDelay @unit(s) = 10ms;
        volatile int pkLenBits @unit(b) = default(952b);
        volatile double iaTime @unit(s);
        double slotTime @unit(s);
}

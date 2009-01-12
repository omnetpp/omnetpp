@namespace(aloha);
network Aloha
{
    parameters:
        int numHosts;
        double txRate @unit(bps) = default(9.6Kbps);
        double slotTime @unit(ms) = default(100ms);
    submodules:
        server: Server;
        host[numHosts]: Host {
            txRate = txRate;
            slotTime = slotTime;
        }
}


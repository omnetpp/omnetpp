//
// For testing
//


message IPHeader
{
    fields:
        int source;
        int destination;
        int ttl = 32;
        string extrainfo;
}
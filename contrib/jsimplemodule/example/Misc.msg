//
// Dummy message type, for testing the generic Java API for accessing messages
//
message IPHeader
{
    fields:
        int source;
        int destination;
        int ttl = 32;
        string extrainfo;
}
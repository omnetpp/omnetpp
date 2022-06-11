// For checking that hyperlinking doesn't accidentally use substring match

message Bar {}
message FooBar {}  // ends in 'Bar'
message BarTender {} // starts with 'Bar'

// Namespaces

packet Packet {}; // non-unique simple name
enum State { IDLE=1; BUSY=2; }

namespace acme;
packet AcmePacket {}; // unique name
packet Packet {}; // non-unique simple name
enum State { IDLE=1; BUSY=2; }

namespace acme::foo;
packet FooPacket {};
packet Packet {};
enum State { IDLE=1; BUSY=2; }

namespace acme::bar::foo;  // not acme::foo::bar, to make foo::Packet ambiguous
packet BarPacket {};
packet Packet {};
enum State { IDLE=1; BUSY=2; }

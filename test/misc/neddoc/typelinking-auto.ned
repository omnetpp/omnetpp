//
// @page typelinking-basic, Hyperlinking of Types (basic test)
//
// Some type references without the "@debug links" option.
//
// Here are some with the tilde notation:~AcmePacket, ~FooPacket, ~BarPacket, ~Packet, ~State,
//  ~acme::foo::State::IDLE, ~State::IDLE, ~foo::NoSuchType, ~::foo::NoSuchType.
//
// And some without tilde (i.e. for automatic hyperlinking): AcmePacket, FooPacket,
// BarPacket, Packet, State, acme::foo::State::IDLE, State::IDLE, foo::NoSuchType, ::foo::NoSuchType.
//
//#----------------------------------------------------------------------------------------------
//
// @page typelinking-full, Hyperlinking of Types (full test)
// @debug links
//
// This page contains the "@debug links" option which results in links being rendered as:
/// <nohtml>[original-text -> rendered-link]</nohtml>.
//
// <h3>Automatic Type Hyperlinking</h3>
//
// THIS SECTION ASSUMES "AUTOLINKING ON" SETTING -- IGNORE OTHERWISE!
//
// In this mode, words that match existing NED of MSG types are hyperlinked.
// You can prevent hyperlinking of a word by putting a backslash in front of it;
// this backslash will be removed in the HTML output.
//
// A backslash-plus-word can be produced in the output by either using
// "nohtml" (<nohtml>\TCP</nohtml>), or by using a double backslash (\\TCP).
// Backslashes not immediately followed by an identifier
// don't have a special meaning either, and are preserved in the output.
//
// In this mode, tildes don't have any special meaning (and they are preserved
// in the output).
//
// Test cases with expected results:
//
// Linking:
//    - TCP: <nohtml>"TCP" (hyperlinked)</nohtml>
//    - org.omnetpp.neddoc.a.TCP: <nohtml>"TCP" (hyperlinked, fully qualified name replace with simple name)</nohtml>
//    - NClients: <nohtml>"NClients(1,2)" (1 and 2 are hyperlinked to a.NClients and b.NClients)</nohtml>
//    - org.omnetpp.neddoc.a.NClients: <nohtml>"NClients" (hyperlinked to a.Clients)</nohtml>
//    - org.omnetpp.neddoc.b.NClients: <nohtml>"NClients" (hyperlinked to b.Clients)</nohtml>
//    - NoSuchType: <nohtml>NoSuchType (unrecognized word, not hyperlinked at all)</nohtml>
//
// Backslash, tilde (in this mode, tilde has no special meaning):
//    - \TCP: <nohtml>"TCP" (backslash removed)</nohtml>
//    - \Hello: <nohtml>"Hello" (backslash removed)</nohtml>
//    - \\TCP: <nohtml>\TCP (backslash, plus "TCP" hyperlinked)</nohtml>
//    - \\Hello: <nohtml>\Hello (backslash plus "Hello")</nohtml>
//    - \100: <nohtml>unchanged: \100 (backslash plus 100)</nohtml>
//    - ~TCP: <nohtml>unchanged: ~TCP (tilde plus TCP, hyperlinked)</nohtml>
//    - ~~TCP: <nohtml>unchanged: ~~TCP (double tilde plus TCP, hyperlinked)</nohtml>
//    - ~, ~~ : <nohtml>unchanged: ~, ~~ (a tilde character, two tilde chars)</nohtml>
//    - \, \\ : <nohtml>unchanged: \, \\ (a backslash character, two backslash chars)</nohtml>
//
// Classes with namespaces:
//
//     - Unqualified, unique names: AcmePacket, FooPacket, BarPacket
//     - Unqualified, non-unique names: Packet, State (should be four of each!)
//     - Fully qualified: acme::Packet, acme::foo::Packet, acme::bar::foo::Packet
//     - Explicitly fully qualified: ::Packet, ::acme::Packet, ::acme::foo::Packet, ::acme::bar::foo::Packet
//     - Partially qualified: foo::Packet, bar::foo::Packet
//     - Referencing to enum members: acme::foo::State::IDLE, State::IDLE
//     - No such type: bar::Packet, ::bar::Packet
//
// ----------------
//
// <h3>Type Hyperlinking With Tildes</h3>
//
// THIS SECTION ASSUMES "AUTOLINKING OFF" (=TILDES) SETTING -- IGNORE OTHERWISE!
//
// In this mode, a word needs to be prefixed with a tilde to count as a type reference.
// Such words which do not refer to an existing type will be marked as errors (e.g. will appear in red).
//
// A tilde-plus-word can be produced in the output by either using
// "nohtml" (<nohtml>~TCP</nohtml>), or by using a double tilde (~~TCP).
// Tildes not immediately followed by an identifier don't have a special meaning,
// and are preserved in the output.
//
// In this mode, backslashes don't have any special meaning (and they are preserved
// in the output).
//
// Basic cases with expected results:
//
//    - ~TCP: <nohtml>"TCP", hyperlinked</nohtml>
//    - ~Hello: <nohtml>"Hello", with broken link indicated (e.g. with red text color)</nohtml>
//    - ~~TCP: <nohtml>~TCP (tilde plus "TCP")</nohtml>
//    - ~~Hello: <nohtml>~Hello (tilde plus "Hello")</nohtml>
//    - ~~~TCP: <nohtml>tilde, plus "TCP" hyperlinked</nohtml>
//    - ~~~Hello: <nohtml>tilde, plus "Hello" with indication of broken hyperlink</nohtml>
//    - ~100: <nohtml>unchanged: ~100 (tilde plus 100)</nohtml>
//    - ~~100: <nohtml>unchanged: ~~100 (double tilde plus 100)</nohtml>
//    - ~, ~~: <nohtml>unchanged: ~, ~~ (tilde, double tilde)</nohtml>
//    - \TCP: <nohtml>unchanged: \TCP (backslash plus TCP)</nohtml>
//    - \\TCP: <nohtml>unchanged: \\TCP (double backslash plus TCP)</nohtml>
//    - \, \\: <nohtml>unchanged: \, \\ (backslash, double backslash)</nohtml>
//
// Classes with namespaces:
//
//     - Unqualified, unique names: ~AcmePacket, ~FooPacket, ~BarPacket
//     - Unqualified, non-unique names: ~Packet, ~State (should be four of each!)
//     - Fully qualified: ~acme::Packet, ~acme::foo::Packet, ~acme::bar::foo::Packet
//     - Explicitly fully qualified: ~::Packet, ~::acme::Packet, ~::acme::foo::Packet, ~::acme::bar::foo::Packet
//     - Partially qualified: ~foo::Packet, ~bar::foo::Packet
//     - Referencing to enum members: ~acme::foo::State::IDLE, ~State::IDLE
//     - No such type: ~bar::Packet, ~::bar::Packet
//

package org.omnetpp.neddoc;


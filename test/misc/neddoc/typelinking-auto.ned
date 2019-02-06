//
// @page Automatic linking
//
// THIS PAGE REQUIRES the "AUTOLINKING ON" SETTING.
//
// In this mode, words that match existing NED of MSG types are hyperlinked.
// You can prevent hyperlinking of a word by putting a backslash in front of it;
// this backslash will be removed in the HTML output.
//
// A backslash-plus-word can be produced in the output by either using
// "nohtml" (<nohtml>\TCP</nohtml>), or by using a double backslash (\\TCP).
//
// In this mode, tildes don't have any special meaning (and they are preserved
// in the output). Backslashes not immediately followed by an identifier
// don't have a special meaning either, and are preserved in the output.
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
//
//
// The existing types are 'r', 'Bar' and 'FooBar' and 'Bartender'; the others are
// nonexistent.
//
// Plain: r, Bar, FooBar, BarTender, ZooBar, Barack, rrr.
//
// Backslashed: \r, \Bar, \FooBar, \BarTender, \ZooBar, \Barack, \rrr.
//
// With tilde: ~r, ~Bar, ~FooBar, ~Bartender, ~zooBar, ~Barack, ~rrr. (tilde should be removed even in the autolinking case?)
//
// A tilde: ~~
//
// A backslash: \\
//

package org.omnetpp.neddoc;


//
// @page Automatic linking
//
// THIS PAGE SHOULD REQUIRES the "AUTOLINKING ON" SETTING.
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
//    - TCP: "<nohtml>TCP</nohtml>", hyperlinked
//    - \TCP: "<nohtml>TCP</nohtml>" (backslash removed)
//    - \Hello: "Hello" (backslash removed)
//    - \\TCP: <nohtml>\</nohtml>TCP (backslash, plus "<nohtml>TCP</nohtml>" hyperlinked)
//    - \\Hello: <nohtml>\Hello</nohtml> (backslash plus "Hello")
//    - \100: unchanged: <nohtml>\100</nohtml> (backslash plus 100)
//    - ~TCP: unchanged: <nohtml>~</nohtml>TCP (tilde plus <nohtml>TCP</nohtml>, hyperlinked)
//    - ~~TCP: unchanged: <nohtml>~~</nohtml>TCP (double tilde plus <nohtml>TCP</nohtml>, hyperlinked)
//
//
// The existing types are 'a', 'Bar' and 'FooBar' and 'Bartender'; the others are 
// nonexistent.
//
// Plain: a, Bar, FooBar, BarTender, ZooBar, Barack, aaa.
//
// Backslashed: \a, \Bar, \FooBar, \BarTender, \ZooBar, \Barack, \aaa.
//
// With tilde: ~a, ~Bar, ~FooBar, ~Bartender, ~zooBar, ~Barack, ~aaa. (tilde should be removed even in the autolinking case?)
//
// A tilde: ~~
//
// A backslash: \\
//

package org.omnetpp.neddoc;


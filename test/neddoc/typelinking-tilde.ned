//
// @page Tilde notation for links
//
// THIS PAGE REQUIRES the "AUTOLINKING OFF" SETTING.
//
// Test cases with expected results:
//
//    - ~TCP: "TCP", hyperlinked
//    - ~Hello: "Hello", with broken link indicated (e.g. with red text color)
//    - ~~TCP: <nohtml>~TCP</nohtml> (tilde plus "TCP")
//    - ~~Hello: <nohtml>~Hello</nohtml> (tilde plus "Hello")
//    - ~~~TCP: tilde, plus "TCP" hyperlinked 
//    - ~~~Hello: tilde, plus "Hello" with indication of broken hyperlink
//    - ~100: unchanged: <nohtml>~100</nohtml> (tilde plus 100)
//    - ~~100: unchanged: <nohtml>~~100</nohtml> (double tilde plus 100)
//    - ~, ~~: unchanged: <nohtml>~, ~~</nohtml> (tilde, double tilde)
//    - \TCP: unchanged: <nohtml>\TCP</nohtml> (backslash plus TCP)
//    - \\TCP: unchanged: <nohtml>\\TCP</nohtml> (double backslash plus TCP)
//    - \, \\: unchanged: <nohtml>\, \\</nohtml> (backslash, double backslash)
//

package org.omnetpp.neddoc;


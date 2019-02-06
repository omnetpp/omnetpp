//
// @page Tilde notation for links
//
// THIS PAGE REQUIRES the "AUTOLINKING OFF" SETTING.
//
// Test cases with expected results:
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

package org.omnetpp.neddoc;


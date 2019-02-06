//
// @page Source syntax highlighting
//
// Please check the source of the SyntaxHighlight_Example module.
//

package org.omnetpp.neddoc;

//
// Please check the syntax highlighing of this module's source.
//
module SyntaxHighlight_Example
{
    parameters:
        int numApps;
        string destAddr; // this is <i>italic</i>, and we have "string constants"
                          // here as well as numbers 1, 2, 3.14, 6e23 and
                          // keywords simple, module, gate, index, sizeof.
                          // In a comment, none of them should be highlighted.
                          // But links to module etc. names should be
                          // hyperlinked, such as SyntaxHighlight_1 or
                          // Formatting_1.
        int protId;       // This the <u>first</u> line
                          //
                          // of a really long comment that should describe
                          // where protocolIds come from, and so on...
        int waitMean;     // this comment contains a link to an external page:
                          // mean for <a href="http://mathworld.wolfram.com/ExponentialDistribution.html">
                          // exponential</a> interarrival times

    gates:
        input in;
        output out;
}


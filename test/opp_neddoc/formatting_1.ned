//
// This file is commented with this one sentence only, but HTML
// formatting should work here too: <i>italic</i>, <b>bold</b>.
//

//
// This comment tests HTML formatting of a module's comment.
//
// Lines beginning with "//#" are ignored, so you should not see the
// "this should be invisible text below".
//
//# THIS SHOULD BE INVISIBLE
//
// Basic formatting: <b>bold</b>, <i>italic</i>, <u>underline</u>,
// super<sup>script</sup>, sub<sub>script</sub>, <tt>teletype</tt> ("tt" tag),
// font coloring: <font color="red">red</font>, <font color="#00ff00">green</font>,
// <font color="BLUE">blue</font>,
// font size: <font size="1">size 1</font>, <font size="2">size 2</font>,
// <font size="3">size 3</font>,
// and <b><i><sub>all</sub> in <sup>one</sup></i></b>.
//
// "br" can be used for line breaks,<br>like<br>this (this paragraph should be 3 lines).
//
// Accented characters and special symbols: T&uuml;bingen, G&aacute;bor,
// Andr&eacute;, ampersand &amp;, quote &quot;.
// But &gar-bage; and &n0th1ng; are left with & and ; marks.
//
// Tables can be drawn in HTML, as below. First line is heading.
//
// <table border="1">
//   <tr><th>#</th><th>number</th></tr>
//   <tr><td>1</td><td>one</td></tr>
//   <tr><td>2</td><td>two</td></tr>
//   <tr><td>3</td><td>three</td></tr>
// </table>
//
// And there should be an 50% divider line below, drawn with HR:
// <hr width=50%>
// <center>^^^ line<br>this text should be centered.
// </center>
//
// Several hyphens should also produce a divider line, like the one below:
// -----------------
//
// Invalid HTML tags are not interpreted: <><x><yy><zzz>foo</zzz></yy>.
//
// The "nohtml" tag can be used to turn off interpreting html tags:
// <nohtml><i>This</i> <br>is <b>not</b> formatted</nohtml>,
// but <font color="red">this</font> <b>is</b>!
//
// Printing "nohtml" itself: <nohtml><nohtml></nohtml> or &lt;/nohtml>.
//
// Some @-tags (a`la Doxygen) are also supported:
//
// @author Andras @date 17/07/2004
// @todo more testing
// @bug so far none noticed
// @see manual
//
// Backslash handling: a single backslash should prevent hyperlinking a word.
// So this should be hyperlinked: Formatting_1, but this not: \Formatting_1.
// Single backslashes should also be removed, so none should be here: 
// \one,\Two, Th\ree. Double backslashes should become one, so you should
// see one \\here, another here: \\, and two here: \\\\.
//
// Preformatted text ("pre" tag, should ideally be displayed in fixed-width font):
// <pre>
// // C++ code example (body should be 4 spaces indented)
// for (i=0; i<10; i++)
// {
//     printf("<><><>"); // prints 3 angle brace pairs
//     cout << "2+3 > 4"; // C++ stream I/O
// }
//
// Indented text follows, with indent growing by 2 spaces:
//   HTML tags are interpreted, so this is <i>italic</i>.
//     - one
//       - <b>two</b>, in bold
//         - three
//           - 7 special characters: < > / \ & ; ^
//
// And invalid HTML tags are not interpreted here either:
// <><x><yy><zzz>foo</zzz></yy>.
//
// And this is the last line of the "pre" tag.
// </pre>
//
module Formatting_1
endmodule



//
// @page Automatic hyperlinking of URLs in the text
//
//
// These should be hyperlinked: http://www1.index.hu, https://www2.index.hu; https://www3.index.hu.
// And: https://www4.index.hu "https://www5.index.hu", 'https://www6.index.hu',...
//
// URL at the end of the line: https://www7.index.hu
//
// Hyperlinking also works within pre tags:
// <pre>
// opp_run -h  # see http://omnetpp.org for docu
// </pre>
//
// <nohtml>But NO automatic hyperlinking within <nohtml> sections, like here: http://www8.index.hu nor this: http://www9.index.hu</nohtml>.
//
// The following link was given with a HTML tag, and it should not be messed up: <a href="http://www10.index.hu">index.hu</a>.
//
// Nor should tags that contain URL in an attribute, like img: <img src="https://www.google.com/images/branding/googlelogo/2x/googlelogo_color_92x30dp.png">.
//

package org.omnetpp.neddoc;


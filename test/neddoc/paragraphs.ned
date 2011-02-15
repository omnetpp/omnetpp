//
// @page Paragraphs
//
// This NED files tests if paragraph boundaries are properly recognized. 
// In the following test cases, "[]" denotes where the paragraph should end. 
//
//
// Paragraphs are
// separated by
// blank lines.
// The
// length
// of
// the paragraph or a line is not significant. []
//
//                 It also
//should  not matter how many
//                        spaces are left
// after the
// // mark and the start of the
//                                                line
//.
//(This should still be the same paragraph as
//  above.) []
//
// If /// or //// or longer / sequnce is used at the start of the line,
// that should be accepted (and ignored) as well,
/// like
///here, or
///////////////////////////////////////here. []
//
//
//
// @page Bulleted lists
//
// Here we test enumerated lists. Basic version in several flavours:
//
// Items are indented, no blank before list:
//  - one
//  - two
//  - three
//
// Items are indented, blank line before list:
//
//  - one
//  - two
//  - three
//
// Items not indented, no blank before list:
// - one
//
// - two (blank line before this item should be ignored)
// - three
//
// Items not indented, blank line before list:
//
// - one
// - two
// - three
//
//Everything right against the // mark, no blank before list:
//- one
//- two
//- three
//
//Everything right against the // mark, blank line before list:
//
//- one
//
//- two (blank line before this item should be ignored)
//- three
//
// And
//a
//    multi-line
//          list-head paragraph:
// - one
// - two
// - three
//
// If there's no space after the "-" mark, it is <b>not</b> recognised as a
// list, see:
// -one
// -two
// -three
//
//
// Items may span multiple lines:
//  - one one one one one one
//one
//      one one one one one
//- two two two two two two two two two two two two
//          two two two
//two two two
// -  three three three
//three
//
// Indent skips level with every 3 spaces, up to 4 levels. Like this:
// - 0 space
//  - 1 space
//   - 2 spaces
//    - 3 spaces
//     - 4 spaces
//      - 5 spaces
//       - 6 spaces
//        - 7 spaces
//         - 8 spaces
//          - 9 spaces
//         - 8 spaces
//        - 7 spaces
//       - 6 spaces
//      - 5 spaces
//     - 4 spaces
//    - 3 spaces
//   - 2 spaces
//  - 1 space
// - 0 space
//
//
// That was it...
//
//
//
// @page Numbered lists
//
// This is the sames as the "b" version, only for numbered lists ("-#" instead
// of "-".)
//
// Here we test enumerated lists. Basic version in several flavours:
//
// Items are indented, no blank before list:
//  -# one
//  -# two
//  -# three
//
// Items are indented, blank line before list:
//
//  -# one
//  -# two
//  -# three
//
// Items not indented, no blank before list:
// -# one
//
// -# two (blank line before this item should be ignored)
// -# three
//
// Items not indented, blank line before list:
//
// -# one
// -# two
// -# three
//
//Everything right against the // mark, no blank before list:
//-# one
//-# two
//-# three
//
//Everything right against the // mark, blank line before list:
//
//-# one
//
//-# two (blank line before this item should be ignored)
//-# three
//
// And
//a
//    multi-line
//          list-head paragraph:
// -# one
// -# two
// -# three
//
// If there's no space after the "-" mark, it is <b>not</b> recognised as a
// list, see:
// -one
// -two
// -three
//
//
// Items may span multiple lines:
//  -# one one one one one one
//one
//      one one one one one
//-# two two two two two two two two two two two two
//          two two two
//two two two
// -#  three three three
//three
//
// Indent skips level with every 3 spaces, up to 4 levels. Like this:
// -# 0 space
//  -# 1 space
//   -# 2 spaces
//    -# 3 spaces
//     -# 4 spaces
//      -# 5 spaces
//       -# 6 spaces
//        -# 7 spaces
//         -# 8 spaces
//          -# 9 spaces
//         -# 8 spaces
//        -# 7 spaces
//       -# 6 spaces
//      -# 5 spaces
//     -# 4 spaces
//    -# 3 spaces
//   -# 2 spaces
//  -# 1 space
// -# 0 space
//
//
// That was it...
//
//
//
// @page Nested Lists
//
// Nested lists can be controlled with indentation level (3 spaces = 1 level).
//
// - one
// - two
//    -# first
//    -# second
//    -# third
// - three
// - four
//    -# alpha
//    -# beta
//
// Another try:
// -# first
// -# second
//    - one
//    - two
//       -# alpha
//       -# beta
// (let beta be a long line)
//    - three
// -# third
//    -# charlie
//    -# delta
//
// That was it...
//
//
// @page Included file
//
// See if @include works:
//
// <<begin included text>>
// @include included.txt
// <<end included text>>
//

package org.omnetpp.neddoc;

//
// Well, not everything seems to pass in this test...
//



//
// This NED files tests if paragraph boundaries are recognised properly
// by opp_nedc.
//
// Paragraphs are 
// separated by
// blank lines.
// The
// length 
// of
// the paragraph or a line is not significant.
//
//                 It also
//should  not matter how many
//                        spaces are left
// after the
// // mark and the start of the 
//                                                line
//.
//(This should still be the same paragraph as
//	above.) Next line contains several spaces:
//               
// (a) and then some tabs:
//	
//(b) and they should separate the paragraphs -- (a) and (b) marks above 
// should be at start of separate paragraphs.
//
//
// If /// or //// or longer / sequnce is used at the start of the line, 
// that should be accepted (and ignored) as well,
/// like
///here, or
///////////////////////////////////////here.
//
// A //------ sequence is translated to a horizontal divider line, see below:
//       -----------------------------
// OK.
//
simple Paragraphs_1a
endsimple



//
// Here we test enumerated lists: 
//
// Items are indented:
//  - one
//  - two
//  - three
//
// Items not indented:
// - one
// - two
// - three
//
//Everything right against the // mark:
//- one
//- two
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
// But if there's multiple spaces, that shouldn't matter:
// -      one
//-    two
//   -     three
//
// You can use tab(s) instead of spaces:
// -	one
//-	two
//	- three
//	-	four
//
//
// You can have extra lines before the first item, like here:
//
// - one
// - two
// - three
//
// Items may span multiple lines:
//  - one one one one one one 
//one 
//      one one one one one
//- two two two two two two two two two two two two
//          two two two
//two two two
//      -  three three three
//three
//
// That was it...
//
//
simple Paragraphs_1b
endsimple



//
// This is the sames as the "b" version, only for numbered lists ("-#" instead
// of "-".)
//
// Items are indented:
//  -# one
//  -# two
//  -# three
//
// Items not indented:
// -# one
// -# two
// -# three
//
//Everything right against the // mark:
//-# one
//-# two
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
// If there's no space after the "-#" mark, it is <b>not</b> recognised as a
// list, see:
// -#one
// -#two
// -#three
//
// But if there's multiple spaces, that shouldn't matter:
// -#      one
//-#    two
//   -#     three
//
// You can use tab(s) instead of spaces:
// -#	one
//-#	two
//	-# three
//	-#	four
//
//
// You can have extra lines before the first item, like here:
//
// -# one
// -# two
// -# three
//
// Items may span multiple lines:
//  -# one one one one one one 
//one 
//      one one one one one
//-# two two two two two two two two two two two two
//          two two two
//two two two
//      -#  three three three
//three
//
// That was it...
//
simple Paragraphs_1c
endsimple



//
// Well, nested list don't really work...
//
// - one
// - two
//    -# first 
//    -# second
//    -# third
// - three
// -four
//
// Another try:
// -# one
// -# two
//    - first 
//    - second
//    - third
// -# three
// -# four
//
// That was it...
//
simple Paragraphs_1d
endsimple



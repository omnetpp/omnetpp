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
//	above. Next line contains several spaces:
//               
// and then some tabs:
//	
//and they should separate the paragraphs.
//
// If /// is used at the start of the line, that should be accepted as well,
/// like
///this.
//But not any more "/" -- this:
//////////// is really too many. Just as four slashes:
////like here.
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



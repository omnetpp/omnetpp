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
//OK.
//
// Now we test enumerated lists: 
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
//Ditto, and even no space between "-" and the item text
//-one
//-two
//-three
//
// Items are indented:
//
//  - one
//  - two
//  - three
//
// Items not indented:
//
// - one
// - two
// - three
//
//Everything right against the // mark:
//
//- one
//- two
//- three
//
//Ditto, and even no space between "-" and the item text
//
//-one
//-two
//-three
//
// Now we have to test multi-line items.
// Then numbered lists.
// Then nested lists.
//
simple Paragraphs_1
endsimple


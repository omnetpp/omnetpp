//==========================================================================
// EBNFCFG.H
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*
* The NEDC grammar files, ebnf.y and ebnf.lex are shared between
* NEDC and GNED. (Exactly the same files are present in both packages;
* this is to eliminate the burden of keeping 2 sets of files in sync.)
*
* Naturally, NEDC and GNED want to do different things while parsing.
* This file (ebnfcfg.h) is included by both ebnf.y and ebnf.lex; the NEDC()
* and GNED() macros defined here select the code for either the NEDC
* or the GNED program. The ebnfcfg.h in the NEDC package defines NEDC(),
* while that in GNED defines GNED().
*/

/* This is the GNED version: */
#define DOING_GNED

#define GNED(code) code
#define NEDC(code) /**/

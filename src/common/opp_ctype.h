//=========================================================================
//  OPP_CTYPE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_OPP_CTYPE_H
#define __OMNETPP_COMMON_OPP_CTYPE_H

#include <cassert>
#include <cctype>
#include "commondefs.h"

/**
 * Replacement for the standard C characher type macros.
 * Reason: in Visual C++ 8.0, isspace() etc abort for characters above 128,
 * which renders them unusable. More precisely: they abort if the
 * "int" they receive is outside the 0..255 range and not EOF -- which can
 * very easily happen when signed chars are converted to int.
 *
 * Looking at Microsofts documentation for isspace at:
 * http://msdn2.microsoft.com/en-us/library/y13z34da(VS.80).aspx
 *
 * "When used with a debug CRT library, isspace will display a CRT assert
 * if passed a parameter that is not EOF or in the range of 0 through
 * 0xFF. When used with a non-debug CRT library, isspace will use the
 * parameter as an index into an array, with undefined results if the
 * parameter is not EOF or in the range of 0 through 0xFF."
 *
 * See also "C Language Gotchas",
 * http://www.greenend.org.uk/rjk/2001/02/cfu.html#ctypechar
 *
 * @{
 */
inline bool opp_isspace(unsigned char c)  {return isspace(c);}
inline bool opp_isalpha(unsigned char c)  {return isalpha(c);}
inline bool opp_isdigit(unsigned char c)  {return isdigit(c);}
inline bool opp_isalnum(unsigned char c)  {return isalnum(c);}
inline bool opp_isxdigit(unsigned char c) {return isxdigit(c);}
inline bool opp_islower(unsigned char c)  {return islower(c);}
inline bool opp_isupper(unsigned char c)  {return isupper(c);}
inline bool opp_iscntrl(unsigned char c)  {return iscntrl(c);}
inline char opp_tolower(unsigned char c)  {return tolower(c);}
inline char opp_toupper(unsigned char c)  {return toupper(c);}
inline bool opp_isext(unsigned char c)    {return c>=128;}  //new
inline bool opp_isalphaext(unsigned char c) {return opp_isalpha(c) || opp_isext(c) || c == '_';}  //new
inline bool opp_isalnumext(unsigned char c) {return opp_isalnum(c) || opp_isext(c) || c == '_';}  //new
/** @} */

#endif



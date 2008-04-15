//==========================================================================
//  STLWRAP.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __STLWRAP_H
#define __STLWRAP_H

#include "simkerneldefs.h"
#include <string>
#include <vector>

NAMESPACE_BEGIN

//
// Note: We *may* need to use these classes in public API functions exposed
// in a dll interface
//

/**
 * Wrap std::string templated class into an equivalent non-templated class.
 * Motivation: Microsoft Visual C++ has problems exporting templated
 * classes from DLLs, so we cannot use "raw" STL in our public API.
 *
 * This is related to VC71 warning C4251: <tt>class 'std::vector<_Ty>' needs
 * to have dll-interface to be used by clients of class 'cIniFile'</tt>.
 * For discussion of the issue, see
 * http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
 *
 * CAREFUL! This class may cause linker error: std::string methods multiply defined
 */
class SIM_API stdstring : public std::string
{
  public:
    explicit stdstring() {}
    stdstring(const std::string& rhs) : std::string(rhs) {}
    stdstring(const std::string& rhs, size_type pos, size_type n) : std::string(rhs,pos,n) {}
    stdstring(const char *s, size_type n) : std::string(s,n) {}
    stdstring(const char *s) : std::string(s) {}
    stdstring(size_type n, char c) : std::string(n,c) {}
    stdstring(const_iterator first, const_iterator last) : std::string(first,last) {}
};


/**
 * Wrap std::vector<stdtring> templated class into an equivalent non-templated class.
 * Motivation: see description at stdstring.
 */
class SIM_API stdstringvector : public std::vector<stdstring>
{
  public:
    explicit stdstringvector() {}
    explicit stdstringvector(size_type n, const stdstring& v = stdstring()) : std::vector<stdstring>(n,v) {}
    stdstringvector(const stdstringvector& x) : std::vector<stdstring>(x) {}
    stdstringvector(const_iterator first, const_iterator last) : std::vector<stdstring>(first,last) {}
};

/**
 * Wrap std::vector<const char *> templated class into an equivalent non-templated class.
 * Motivation: see description at stdstring.
 */
class SIM_API stdcharpvector : public std::vector<const char *>
{
  public:
    explicit stdcharpvector() {}
    explicit stdcharpvector(size_type n, const char *v = 0) : std::vector<const char *>(n,v) {}
    stdcharpvector(const stdcharpvector& x) : std::vector<const char *>(x) {}
    stdcharpvector(const_iterator first, const_iterator last) : std::vector<const char *>(first,last) {}
};


NAMESPACE_END


#endif


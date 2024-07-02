//==========================================================================
//   DISPLAYSTRINGACCESS.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_QTENV_DISPLAYSTRINGACCESS_H
#define __OMNETPP_QTENV_DISPLAYSTRINGACCESS_H


#include <string>
#include "omnetpp/cvalue.h"

#include "qtenv.h"

namespace omnetpp {

class cDisplayString;
class cComponent;
class cPar;

namespace qtenv {

/**
 * Provides access to a cDisplayString, with references to parameters and
 * NED expressions in each tag argument evaluated/substituted.
 *
 * - `$param` references are substituted with the value of the parameter
 *   in the context component OR its parent.
 *
 * - `${NED expression}` references are substituted with the value of the
 *   contained NED expression after it is evaluated in the context of the
 *   component (not considering its parent).
 *
 * - `$$` is substituted with a single '$' character.
 *
 * Substitutions are done on each tag argument separately, so the substitutions
 * cannot possibly change the structure of the original display string, only
 * its values (which, after substitution, may contain , ; and = characters).
 */
class QTENV_API DisplayStringAccess {

  private:
    const cDisplayString *ds;
    cComponent *component; // may be nullptr

    /**
    * Returns true if the given string may contain parameter references
    * ("$param" or "${param}"). It can give false positives, as it only checks for '$'.
    */
    static bool mayContainParamRefs(const char *arg);

    /**
    * Looks up the given parameter in the context component or its parent
    */
    cPar *lookupPar(const char *parname) const;

    /**
    * If the given string is a single parameter reference or expression in the syntax "$name" or
    * "${name}", returns the parameter or evaulated value, otherwise returns a cValue of type UNDEF.
    */
    cValue resolveSingleValue(const char *arg) const;

  protected:

    /**
    * Substitutes module/channel param references into the given (partial)
    * display string. The returned string may be the same as the one passed in,
    * or may point into the buffer std::string.
    */
    const char *substituteParamRefs(const char *arg, std::string& buffer) const;

    bool parseTagArgAsBool(const char *arg, bool defaultValue, bool *okp = nullptr) const;
    long parseTagArgAsLong(const char *arg, long defaultValue, bool *okp = nullptr) const;
    double parseTagArgAsDouble(const char *arg, double defaultValue, bool *okp = nullptr) const;

  public:

    /**
     * Constructs the accessor object for the given display string with the given context component.
     *
     * Ownership of the objects pointed to by either parameter is not taken over, they
     * are never deleted, and they must exist for the whole duration this object exists.
     */
    DisplayStringAccess(const cDisplayString *ds, cComponent *component)
        : ds(ds), component(component) { ASSERT(ds); }

    explicit DisplayStringAccess(cComponent *component);

    const char *getTagArg(const char *tagname, int index, std::string& buffer) const;
    const char *getTagArg(int tagindex, int index, std::string& buffer) const;

    std::string getTagArg(const char *tagname, int index) const;
    std::string getTagArg(int tagindex, int index) const;

    bool getTagArgAsBool(const char *tagname, int index, bool defaultValue = false, bool *okp = nullptr) const;
    bool getTagArgAsBool(int tagindex, int index, bool defaultValue = false, bool *okp = nullptr) const;

    long getTagArgAsLong(const char *tagname, int index, long defaultValue = 0, bool *okp = nullptr) const;
    long getTagArgAsLong(int tagindex, int index, long defaultValue = 0, bool *okp = nullptr) const;

    double getTagArgAsDouble(const char *tagname, int index, double defaultValue = 0.0, bool *okp = nullptr) const;
    double getTagArgAsDouble(int tagindex, int index, double defaultValue = 0.0, bool *okp = nullptr) const;
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_DISPLAYSTRINGACCESS_H
//==========================================================================
//  ERRMSG.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_ERRMSG_H
#define __OMNETPP_ERRMSG_H

#include "simkerneldefs.h"

namespace omnetpp {

/* Specifying that the values of this enum should be represented by an int, because otherwise the variadic
 * constructors of cException, cTerminationException, and cRuntimeError in cexception.cc that accept an OppErrorCode
 * as their last named parameter will trigger a warning with clang 3.9, saying
 * "passing an object that undergoes default argument promotion to 'va_start' has undefined behavior".
 * This is perfectly reasonable, as the integral type "behind" the enum could be shorter than int, see:
 * https://www.securecoding.cert.org/confluence/display/cplusplus/EXP58-CPP.+Pass+an+object+of+the+correct+type+to+va_start
 * However, in practice, most compilers (including clang) will use an int for all enums by default, so it's not an issue here,
 * see the comment on the article above, and these messages: http://lists.llvm.org/pipermail/cfe-commits/Week-of-Mon-20160822/169004.html
 * And even if we explicitly request int, the warning does not go away, even though it no longer applies.
 * This has been fixed in LLVM: https://reviews.llvm.org/D23921
 * But got reverted not long after for some reason: https://www.mail-archive.com/cfe-commits@lists.llvm.org/msg35386.html
 * So we are still left with a few false-positive warnings... At least not undefined behavior anymore!
 * I posted a comment asking about it: https://reviews.llvm.org/D23921#624870 Waiting for answer...
 * Until then, they could be silenced with -Wno-varargs though.
 */
/**
 * @brief Error codes
 */
enum OppErrorCode : int
{
    E_OK = 0,  // E_OK must have zero numeric value
    E_BACKSCHED,
    E_BADCAST,
    E_BADEXP,
    E_BADINIT,
    E_CANCEL,
    E_CANTCOPY,
    E_CANTDUP,
    E_CANTPACK,
    E_CUSTOM,
    E_DIMLESS,
    E_EBADARGS,
    E_ECANTCAST,
    E_ENDEDOK,
    E_ENDSIM,
    E_ENOPARENT,
    E_ESTKOFLOW,
    E_ESTKUFLOW,
    E_FINISH,
    E_GATEID,
    E_INFLOOP,
    E_LOCKED,
    E_MISSINGTAG,
    E_MODFIN,
    E_MODINI,
    E_NEGTIME,
    E_NEGTOUT,
    E_NOCHANDEF,
    E_NOPARENTMOD,
    E_NOPARSIM,
    E_NORECV,
    E_NOSUCHNET,
    E_NUMARGS,
    E_PARAM,
    E_PARNOTSET,
    E_PARAMSNOTREADY,
    E_CPUTIME,
    E_REALTIME,
    E_SIMTIME,
    E_STATECHG,
    E_STKLOW,
    E_STOPSIMRCVD,
    E_USER,
    E_WRONGSIM
};

/**
 * @brief Provides error messages for error codes.
 *
 * @ingroup Internals
 */
class SIM_API cErrorMessages
{
  public:
    cErrorMessages() {}
    static const char *get(OppErrorCode e);
};

}  // namespace omnetpp


#endif

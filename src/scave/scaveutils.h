//=========================================================================
//  SCAVEUTILS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _SCAVEUTILS_H_
#define _SCAVEUTILS_H_

#include <string>
#include <set>
#include <functional>
#include "intxtypes.h"
#include "scavedefs.h"
#include "timeutil.h"
#include "commonutil.h"

NAMESPACE_BEGIN

SCAVE_API bool parseInt(const char *str, int &dest);
SCAVE_API bool parseLong(const char *str, long &dest);
SCAVE_API bool parseInt64(const char *str, int64 &dest);
SCAVE_API bool parseDouble(const char *str, double &dest);
SCAVE_API bool parseSimtime(const char *str, simultime_t &dest);
SCAVE_API std::string unquoteString(const char *str);

// simple profiling macro
// var is a long variable collecting the execution time of stmt in usec
#define TIME(var,stmt) { timeval start,end; \
                         gettimeofday(&start,NULL); \
                         stmt; \
                         gettimeofday(&end,NULL); \
                         var += timeval_diff_usec(end,start); }

template <class Operation>
class FlipArgs
    : public std::binary_function<typename Operation::second_argument_type,
                                    typename Operation::first_argument_type,
                                    typename Operation::result_type>
{
    public:
        typedef typename Operation::second_argument_type first_argument_type;
        typedef typename Operation::first_argument_type second_argument_type;
        typedef typename Operation::result_type result_type;
        FlipArgs(const Operation & _Func) : op(_Func) {};
        result_type operator()(const first_argument_type& _Left, const second_argument_type& _Right)
        {
            return op(_Right, _Left);
        }
    protected:
        Operation op;
};

template<class Operation>
inline FlipArgs<Operation> flipArgs(const Operation& op)
{
    return FlipArgs<Operation>(op);
}

class StringPool
{
    private:
        std::set<std::string> pool;
        const std::string *lastInsertedPtr;
    public:
        StringPool() : lastInsertedPtr(NULL) {}
        const std::string *insert(const std::string& str);
        const std::string *find(const std::string& str) const;
        void clear() { lastInsertedPtr = NULL; pool.clear(); }
};

NAMESPACE_END


#endif


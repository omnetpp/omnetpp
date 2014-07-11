//=========================================================================
//  CENVIR.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cenvir.h"
#include "commonutil.h"
#include "cconfiguration.h"
#include "csimulation.h"

NAMESPACE_BEGIN

/**
 * The std::streambuf class used by cEnvir's ostream. It redirects writes to
 * cEnvir::sputn(s,n). Flush is done at the end of each line, meanwhile
 * writes are buffered in a stringbuf.
 */
class evbuf : public std::basic_stringbuf<char>
{
  public:
    evbuf() {}
    // note: this method is needed because pptr() and pbase() are protected
    bool isempty() {return this->pptr()==this->pbase();}
  protected:
    virtual int sync()  {
        ev.sputn(this->pbase(), this->pptr()-this->pbase());
        setp(this->pbase(),this->epptr());
        return 0;
    }
    virtual std::streamsize xsputn(const char *str, std::streamsize n) {
        const char *end = str + n;
        std::streamsize res = 0;
        for (const char *li = str; li < end; /**/) {
            const char *s = li;
            while (s < end && *s != '\n') // find end of line
                s++;
            if (s == end)
                res += std::basic_stringbuf<char>::xsputn(li, s-li);
            else {
                s++;
                res += std::basic_stringbuf<char>::xsputn(li, s-li);
                sync();
            }
            li = s;
        }
        return res;
    }
};

//----

cEnvir::cEnvir() : out(new evbuf())
{
    disable_tracing = false;
    debug_on_errors = false;
    attach_debugger_on_errors = false;
    suppress_notifications = false; //FIXME set to true when not needed!
}

cEnvir::~cEnvir()
{
    delete out.rdbuf();
}

cConfigurationEx *cEnvir::getConfigEx()
{
    cConfigurationEx *cfg = dynamic_cast<cConfigurationEx *>(getConfig());
    if (!cfg)
        throw cRuntimeError("Illegal call to ev.getConfigEx(): configuration object is not subclassed from cConfigurationEx");
    return cfg;
}

// note: exploits the fact that evbuf does sync() on "\n"'s
void cEnvir::flushLastLine()
{
    evbuf *buf = (evbuf *)out.rdbuf();
    if (!buf->isempty())
        buf->sputn("\n",1);
}

//
// Temp buffer for vararg functions below.
// Note: using a static buffer reduces stack usage of activity() modules;
// it also makes the following functions non-reentrant, but we don't need
// them to be reentrant anyway.
//
#define BUFLEN 1024
static char staticbuf[BUFLEN];

void cEnvir::printfmsg(const char *fmt,...)
{
    VSNPRINTF(staticbuf, BUFLEN, fmt);
    putsmsg(staticbuf);
}

int cEnvir::printf(const char *fmt,...)
{
    if (disable_tracing)
        return 0;

    va_list va;
    va_start(va, fmt);
    int len = vsnprintf(staticbuf, BUFLEN, fmt, va);
    staticbuf[BUFLEN-1] = '\0';
    va_end(va);

    // route it through streambuf to preserve ordering
    out.rdbuf()->sputn(staticbuf, len);
    return len;
}

bool cEnvir::askYesNo(const char *fmt,...)
{
    VSNPRINTF(staticbuf, BUFLEN, fmt);
    return askyesno(staticbuf);
}

NAMESPACE_END

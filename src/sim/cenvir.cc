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
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cenvir.h"

cEnvir *evPtr;


//
// std::streambuf used by cEnvir's ostream base. It redirects writes to
// cEnvir::sputn(s,n). Flush is done at the end of each line, meanwhile
// writes are buffered in a stringbuf.
//
template <class E, class T = std::char_traits<E> >
class basic_evbuf : public std::basic_stringbuf<E,T> {
  public:
    basic_evbuf() {}
    // gcc>=3.4 needs either this-> or std::basic_stringbuf<E,T>:: in front of pptr()/pbase()
    // note: this method is needed because pptr() and pbase() are protected
    bool isempty() {return this->pptr()==this->pbase();}
  protected:
    virtual int sync()  {
        ev.sputn(this->pbase(), this->pptr()-this->pbase());
        setp(this->pbase(),this->epptr());
        return 0;
    }
    virtual std::streamsize xsputn(const E *s, std::streamsize n) {
        std::streamsize r = std::basic_stringbuf<E,T>::xsputn(s,n);
        for(;n>0;n--,s++)
            if (*s=='\n')
               {sync();break;}
        return r;
    }
};

typedef basic_evbuf<char> evbuf;

//----

cEnvir::cEnvir() : out(new evbuf())
{
    disable_tracing = false;
    debug_on_errors = false;
    suppress_notifications = false; //FIXME set to true when not needed!
}

cEnvir::~cEnvir()
{
}

// note: exploits the fact that evbuf does sync() on "\n"'s
//XXX make it go inline? capitalization?
void cEnvir::flushlastline()
{
    evbuf *buf = (evbuf *)out.rdbuf();
    if (!buf->isempty())
        buf->sputn("\n",1);
}




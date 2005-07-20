//=========================================================================
//  FILETOKENIZER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <assert.h>
#include <sstream>
#include "filetokenizer.h"


FileTokenizer::FileTokenizer(const char *fileName, size_t bufferSize)
{
    fname = fileName;
    f = NULL;
    eofreached = false;

    buffersize = bufferSize;
    buffer = new char[buffersize+1]; // +1 for EOS
    bufferend = buffer+buffersize;

    databeg = dataend = buffer;
    *dataend = 0; // sentry

    linenum = 0;

    vecsize = 16000;  // max 16,000 tokens per line (still vec<64K)
    vec = new char *[vecsize];

    errcode = OK;
}

FileTokenizer::~FileTokenizer()
{
    delete [] buffer;
    delete [] vec;
}

size_t FileTokenizer::readMore()
{
    // open file if needed
    if (!f)
    {
        f = fopen(fname.c_str(), "rb");  // 'b' turns off CR/LF translation and might be faster
        if (!f)
        {
            errcode = CANNOTOPEN;
            return 0;
        }
    }

    // if eof was reached already in the file, set status
    if (eofreached)
    {
        errcode = EOFREACHED;
        return 0;
    }

    // move remaining data to beginning of buffer
    if (databeg!=dataend)
        memcpy(buffer, databeg, dataend-databeg);
    size_t offset = databeg - buffer;
    databeg = buffer;
    dataend -= offset;

    // read enough bytes to fill the buffer
    int bytesread = fread(dataend, 1, bufferend-dataend, f);
    if (feof(f))
        eofreached = true;
    if (ferror(f))
        errcode = CANNOTREAD;
    dataend += bytesread;
    *dataend = 0;  // sentry

    // modify already stored pointers by offset
    for (int i=0; i<numtokens; i++)
        vec[i] -= offset;

    // return offset
    return offset;
}


bool FileTokenizer::readLine()
{
    if (errcode!=OK)
        return false;

    // check sentry
    assert(*dataend==0);

    // zap previous contents
    numtokens = 0;

    linenum++;
    char *s = databeg;
    char eolcharseen = 0;

    // loop through the tokens on the line
    for (;;)
    {
        if (eolcharseen)
        {
            // '\r' or '\n' was seen after last token, but it was overwritten
            // with the token's terminating zero char
            break;
        }

        // skip spaces before token (needs *dataend==0 sentry!)
        while (*s==' ' || *s=='\t') s++;
        if (s==dataend)
        {
            // if we reached end of buffer meanwhile, read more data and try again
            s -= readMore();
            if (errcode==EOFREACHED)
               errcode = INCOMPLETELINE;
            if (errcode!=OK)
                return false;
            while (*s==' ' || *s=='\t') s++;
            if (s==dataend)
            {
                errcode = LINETOOLONG;
                return false;
            }
        }

        if (*s=='\r' || *s=='\n')
        {
            // end of line found -- exit loop
            break;
        }
        else if (*s=='"')
        {
            // parse quoted string
            char *start = s+1;
            s++;
            // try find end of quoted string
            while (s!=dataend && (*s!='"' || *(s-1)=='\\') && *s!='\r' && *s!='\n') s++;
            if (s==dataend)
            {
                // if we reached end of buffer meanwhile, read more data and try again
                int offset = readMore();
                s -= offset;
                start -= offset;
                if (errcode==EOFREACHED)
                    errcode = INCOMPLETELINE;
                if (errcode!=OK)
                    return false;
                while (s!=dataend && (*s!='"' || *(s-1)=='\\') && *s!='\r' && *s!='\n') s++;
                if (s==dataend)
                {
                    errcode = LINETOOLONG;
                    return false;
                }
            }
            // check we found the close quote
            if (*s!='"')
            {
                errcode = UNMATCHEDQUOTE;
                return false;
            }
            // terminate quoted string with zero, overwriting close quote
            *s = 0;
            // add string to array (if there's room)
            if (numtokens==vecsize)
            {
                errcode = TOOMANYTOKENS;
                return false;
            }
            vec[numtokens++] = start;
            // skip to next valid character
            s++;
            if (s==dataend)
            {
                s -= readMore();
                if (errcode==EOFREACHED)
                    errcode = INCOMPLETELINE;
                if (errcode!=OK)
                    return false;
            }
        }
        else
        {
            // parse unquoted string
            char *start = s;
            // try find end of string (tab, space or cr/lf)
            while (s!=dataend && *s!=' ' && *s!='\t' && *s!='\r' && *s!='\n') s++;
            if (s==dataend)
            {
                // if we reached end of buffer meanwhile, read more data and try again
                int offset = readMore();
                s -= offset;
                start -= offset;
                if (errcode==EOFREACHED)
                   errcode = INCOMPLETELINE;
                if (errcode!=OK)
                    return false;
                while (s!=dataend && *s!=' ' && *s!='\t' && *s!='\r' && *s!='\n') s++;
                if (s==dataend)
                {
                    errcode = LINETOOLONG;
                    return false;
                }
            }
            // terminate string with zero, but take note if we overwrite cr/lf with that!
            if (*s=='\r' || *s=='\n')
                eolcharseen = *s;
            *s = 0;
            // add string to array (if there's room)
            if (numtokens==vecsize)
            {
                errcode = TOOMANYTOKENS;
                return false;
            }
            vec[numtokens++] = start;
            // skip to next valid character (unless we just overwrote a cr/lf,
            // in which case its location counts as next valid char)
            if (!eolcharseen)
            {
                s++;
                if (s==dataend)
                {
                    s -= readMore();
                    if (errcode==EOFREACHED)
                       errcode = INCOMPLETELINE;
                    if (errcode!=OK)
                        return false;
                }
            }
        }
    }

    // here, s still points to a valid character (1st char of cr/lf)
    assert(s<dataend);
    assert(eolcharseen==0 ? (*s=='\r' || *s=='\n') : (*s==0));

    // skip cr/lf. Note: last line should NOT YET report eof!
    if (eolcharseen=='\r' || *s=='\r')
    {
        s++;
        if (s==dataend)
        {
            s -= readMore();
            if (errcode!=OK)
                return errcode==EOFREACHED ? true : false;
        }
    }
    if (eolcharseen=='\n' || *s=='\n')
    {
        s++;
        if (s==dataend)
        {
            s -= readMore();
            if (errcode!=OK)
                return errcode==EOFREACHED ? true : false;
        }
    }

    // next line will start from our current s
    databeg = s;
    return true;
}

std::string FileTokenizer::errorMsg() const
{
    std::stringstream out;
    switch (errcode)
    {
        case OK:             out << "OK"; break;
        case EOFREACHED:     out << "end of file"; break;
        case CANNOTOPEN:     out << "cannot open file"; break;
        case CANNOTREAD:     out << "error reading from file"; break;
        case INCOMPLETELINE: out << "incomplete last line"; break;
        case UNMATCHEDQUOTE: out << "unmatched quote"; break;
        case LINETOOLONG:    out << "line too long"; break;
        case TOOMANYTOKENS:  out << "too many tokens per line"; break;
        default:             out << "???";
    }
    if (errcode!=CANNOTOPEN)
        out << ", line " << linenum;
    return out.str();
}

/*
Example code:

#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
    if (argc<2)
        return 1;

    FileTokenizer ftok(argv[1],200);
    while (ftok.readLine())
    {
        int numtokens = ftok.numTokens();
        char **vec = ftok.tokens();
        for (int i=0; i<numtokens; i++)
            cout << (i==0?"":" ") << vec[i];
        cout << "\n";
    }
    cout << ftok.errorMsg();
    return 0;
}
*/

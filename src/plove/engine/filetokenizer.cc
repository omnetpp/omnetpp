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

    errcode = OK;
}

FileTokenizer::~FileTokenizer()
{
    delete [] buffer;
}

size_t FileTokenizer::readMore(CharPVector& vec)
{
    // open file if needed
    if (!f)
    {
        f = fopen(fname.c_str(), "rb");
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
    for (unsigned int i=0; i<vec.size(); i++)
        vec[i] -= offset;

    // return offset
    return offset;
}


bool FileTokenizer::getLine(CharPVector& vec)
{
    // check sentry
    assert(*dataend==0);

    // zap previous contents
    vec.resize(0);

    linenum++;
    char *s = databeg;

    // loop through the tokens on the line
    for (;;)
    {
        // skip spaces before token (needs *dataend==0 sentry!)
        //SKIPWHILE(*s==' ' || *s=='\t');
        while (*s==' ' || *s=='\t') s++;
        if (s==dataend)
        {
            s -= readMore(vec);
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
            // end of line -- exit loop
            break;
        }
        else if (*s=='"')
        {
            // parse quoted string
            char *start = s+1;
            s++;
            //SKIPWHILE(s!=dataend && (*s!='"' || *(s-1)=='\\') && *s!='\r' && *s!='\n');
            while (s!=dataend && (*s!='"' || *(s-1)=='\\') && *s!='\r' && *s!='\n') s++;
            if (s==dataend)
            {
                s -= readMore(vec);
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
            if (*s!='"')
            {
                errcode = UNMATCHEDQUOTE;
                return false;
            }
            *s = 0;
            vec.push_back(start);
            s++;
            if (s==dataend)
            {
                s -= readMore(vec);
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
            while (s!=dataend && *s!=' ' && *s!='\t' && *s!='\r' && *s!='\n') s++;
            if (s==dataend)
            {
                s -= readMore(vec);
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
            *s = 0;
            vec.push_back(start);
            s++;
            if (s==dataend)
            {
                s -= readMore(vec);
                if (errcode==EOFREACHED)
                   errcode = INCOMPLETELINE;
                if (errcode!=OK)
                    return false;
            }
        }
    }

//FIXME: last line should NOT YET REPORT EOF!!!!

    // here, s still points to a valid character
    assert(s<dataend);

    // skip cr/lf
    if (*s=='\r')
    {
        s++;
        if (s==dataend)
        {
            s -= readMore(vec);
            if (errcode!=OK)
                return false;
        }
    }
    if (*s=='\n')
    {
        s++;
        if (s==dataend)
        {
            // we only call readMore() here so that we can report EOF already
            s -= readMore(vec);
            if (errcode!=OK)
                return false;
        }
    }

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
        case CANNOTOPEN:     out << "cannot open " << fname; break;
        case CANNOTREAD:     out << "error reading " << fname; break;
        case INCOMPLETELINE: out << "incomplete last line in " << fname; break;
        case UNMATCHEDQUOTE: out << "unmatched quote in " << fname; break;
        case LINETOOLONG:    out << "line too long in " << fname; break;
        default:             out << "???";
    }
    out << ", line " << linenum;
    return out.str();
}



//=========================================================================
//  FILEREADER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <assert.h>
#include <sstream>
#include "FileReader.h"


FileReader::FileReader(const char *fileName, size_t bufferSize)
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

    wholeline = wholelineend = NULL;

    errcode = OK;
}

FileReader::~FileReader()
{
    delete [] buffer;
}

size_t FileReader::readMore()
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
    if (wholeline)
        wholeline -= offset;

    // return offset
    return offset;
}


char *FileReader::readLine()
{
    if (errcode!=OK)
        return NULL;

    // check sentry
    assert(*dataend==0);

    linenum++;
    char *s = wholeline = databeg;
    wholelineend = NULL;

    // find end of line
    while (*s && *s!='\r' && *s!='\n') s++;

    if (s==dataend)
    {
        // if we reached end of buffer meanwhile, read more data
        s -= readMore();
        if (errcode==EOFREACHED)
           errcode = INCOMPLETELINE;
        if (errcode!=OK)
            return zapline();

        // find end of line (provided we're not yet there)
        while (*s && *s!='\r' && *s!='\n') s++;
        if (s==dataend)
        {
            errcode = LINETOOLONG;
            return zapline();
        }
    }

    // skip cr/lf. Note: last line should NOT YET report eof!
    if (*s=='\r')
    {
        *s = '\0';
        wholelineend = s;

        s++;
        if (s==dataend)
        {
            s -= readMore();
            if (errcode!=OK)
                return errcode==EOFREACHED ? wholeline : zapline();
        }
    }
    if (*s=='\n')
    {
        *s = '\0';
        s++;
        if (s==dataend)
        {
            s -= readMore();
            if (errcode!=OK)
                return errcode==EOFREACHED ? wholeline : zapline();
        }
    }

    // next line will start from our current s
    databeg = s;
    return wholeline;
}

char *FileReader::zapline()
{
    return wholeline = wholelineend = NULL;
}

std::string FileReader::errorMsg() const
{
    std::stringstream out;
    switch (errcode)
    {
        case OK:             out << "OK"; break;
        case EOFREACHED:     out << "end of file"; break;
        case CANNOTOPEN:     out << "cannot open file"; break;
        case CANNOTREAD:     out << "error reading from file"; break;
        case INCOMPLETELINE: out << "incomplete last line"; break;
        case LINETOOLONG:    out << "line too long"; break;
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

    FileReader freader(argv[1],200);
    char *line;
    while ((line = freader.readLine())!=NULL)
    {
        cout << line << "\n";
    }
    cout << freader.errorMsg();
    return 0;
}
*/

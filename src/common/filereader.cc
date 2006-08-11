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
#include "filereader.h"

//FIXME throw Exception on read errors!!!
FileReader::FileReader(const char *fileName, size_t bufferSize)
{
    fname = fileName;
    f = NULL;
    eofreached = false;

    buffersize = bufferSize;
    buffer = new char[buffersize+1]; // +1 for EOS
    bufferend = buffer+buffersize;
    bufferfileoffset = 0;

    databeg = dataend = bufferend;
    *dataend = 0; // sentry

    linenum = 0;

    wholeline = NULL;

    errcode = OK;
}

FileReader::~FileReader()
{
    delete [] buffer;
}

void FileReader::openFile()
{
    f = fopen(fname.c_str(), "rb");  // 'b' turns off CR/LF translation and might be faster
    if (!f)
        errcode = CANNOTOPEN;
}

size_t FileReader::readMore()
{
    // open file if needed
    if (!f)
    {
        openFile();
        if (!f)
            return 0;
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
    bufferfileoffset = ftell(f) - (dataend-buffer);
//XXX printf("   dataend-buffer=%ld, bufferfileoffset=0x%lx\n", dataend-buffer, bufferfileoffset);
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

    if (linenum!=-1)  // after seek() we don't maintain it (stays -1)
        linenum++;
    char *s = wholeline = databeg;

    // find end of line
    while (*s && *s!='\r' && *s!='\n') s++;

    if (s==dataend)
    {
        // if we reached end of buffer meanwhile, read more data
        s -= readMore();
        if (errcode==EOFREACHED)
           errcode = INCOMPLETELINE;
        if (errcode!=OK)
            return NULL;

        // find end of line (provided we're not yet there)
        while (*s && *s!='\r' && *s!='\n') s++;
        if (s==dataend)
        {
            errcode = LINETOOLONG;  //FIXME why does it come here after seek???????????????
            return NULL;
        }
    }

    // skip cr/lf. Note: last line should NOT YET report eof!
    if (*s=='\r')
    {
        *s = '\0';

        s++;
        if (s==dataend)
        {
            s -= readMore();
            if (errcode!=OK)
                return errcode==EOFREACHED ? wholeline : NULL;
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
                return errcode==EOFREACHED ? wholeline : NULL;
        }
    }

    // next line will start from our current s
    databeg = s;
    return wholeline;
}

long FileReader::fileSize()
{
    // open file if needed
    if (!f)
    {
        openFile();
        if (!f)
            return 0;
    }

    long tmp = ftell(f);
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, tmp, SEEK_SET);
    return size;
}

bool FileReader::seekTo(long offset)
{
    // open file if needed
    if (!f)
    {
        openFile();
        if (!f)
            return 0;
    }

    // seek to given offset
    int err = fseek(f, offset, SEEK_SET);
    if (err!=0)
    {
        errcode = CANNOTREAD;
        return false;
    }

    // flush buffer
    databeg = dataend = bufferend;
    *dataend = 0; // sentry
    eofreached = false;
    errcode = OK; //FIXME this should not be here, but EOFREACHED should be cleared

    linenum = -1; // after seekTo() we lose line number info

    // if we're at the very beginning, return
    if (offset==0)
        return true;

    // find beginning of first whole line by reading up to an end-of-line
    int c = ' ';
    while (c!=EOF && c!='\r' && c!='\n')
        c = fgetc(f);
    while (c=='\r' || c=='\n')
        c = fgetc(f);
    if (c!=EOF)
        ungetc(c,f);
    if (ferror(f))
    {
        errcode = CANNOTREAD;
        return false;
    }
    return true;
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
    if (errcode!=CANNOTOPEN && linenum!=-1)
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

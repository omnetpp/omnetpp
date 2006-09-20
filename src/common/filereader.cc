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
#include "exception.h"

long FileReader::numReadLines = 0;

FileReader::FileReader(const char *fileName, size_t bufferSize)
{
    fname = fileName;
    f = NULL;

    buffersize = bufferSize;
    buffer = new char[buffersize+1]; // +1 for EOS
    bufferend = buffer+buffersize;
    bufferfileoffset = 0;

    databeg = dataend = bufferend;
    *dataend = 0; // sentry

    linenum = 0;

    wholeline = NULL;
}

FileReader::~FileReader()
{
    delete [] buffer;
}

void FileReader::openFile()
{
    // open file. Note: 'b' mode turns off CR/LF translation and might be faster
    f = fopen(fname.c_str(), "rb");
    if (!f)
        throw new Exception("Cannot open file `%s'", fname.c_str());
}

void FileReader::checkConsistence()
{
    bool ok = bufferend-buffer==buffersize &&
              databeg<=dataend && databeg>=buffer && dataend<=bufferend &&
              strlen(databeg)==dataend-databeg;
    if (!ok)
        __asm int 3;
}

size_t FileReader::readMore()
{
    // open file if needed
    if (!f)
        openFile();

    // if we are at EOF, nothing to do
    if (feof(f))
        return 0;

    // move remaining data to beginning of buffer
    if (databeg!=dataend)
        memcpy(buffer, databeg, dataend-databeg);
    size_t offset = databeg - buffer;
    databeg = buffer;
    dataend -= offset;

    // read enough bytes to fill the buffer
    bufferfileoffset = ftell(f) - (dataend-buffer);
    int bytesread = fread(dataend, 1, bufferend-dataend, f);
    if (ferror(f))
        throw new Exception("Read error in file `%s'", fname.c_str());
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
    numReadLines++;

    // check sentry
    assert(*dataend==0);

    if (linenum!=-1)  // after seek() we don't maintain it (stays -1)
        linenum++;
    char *s = wholeline = databeg;

    // find end of line
    while (*s && *s!='\r' && *s!='\n')
        s++;

    if (s==dataend)
    {
        // if we reached end of buffer meanwhile, read more data
        s -= readMore();

        // if we couldn't get any more, then this is the last line and missing CR/LF.
        // ignore this incomplete last line, as the file might be currently being written
        if (s==dataend)
        {
            databeg = s;
            return NULL;
        }

        // find end of line (provided we're not yet there)
        while (*s && *s!='\r' && *s!='\n')
            s++;
        if (s==dataend)
            throw new Exception("Line too long, should be below %d in file `%s'", buffersize, fname.c_str());
    }

    // skip CR/LF. Note: last line should not yet report EOF.
    if (*s=='\r')
    {
        *s++ = '\0';
        if (s==dataend)
        {
            s -= readMore();
            if (s==dataend)
            {
                databeg = s;
                return wholeline;
            }
        }
    }
    if (*s=='\n')
    {
        *s++ = '\0';
        if (s==dataend)
        {
            s -= readMore();
            if (s==dataend)
            {
                databeg = s;
                return wholeline;
            }
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
        openFile();

    long tmp = ftell(f);
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, tmp, SEEK_SET);
    if (ferror(f))
        throw new Exception("Cannot seek in file `%s'", fname.c_str());
    return size;
}

void FileReader::seekTo(long offset)
{
    // open file if needed
    if (!f)
        openFile();

    // seek to one smaller value, in order to not skip the line which
    // starts exactly on the given offset
    if (offset!=0)
        offset--;

    // seek to given offset
    int err = fseek(f, offset, SEEK_SET);
    if (err!=0)
        throw new Exception("Cannot seek in file `%s'", fname.c_str());

    // flush buffer
    databeg = dataend = bufferend;
    *dataend = 0; // sentry

    linenum = -1; // after seekTo() we lose line number info

    // if we're at the very beginning, return
    if (offset==0)
        return;

    // find beginning of first whole line by reading up to an end-of-line
    int c = ' ';
    while (c!=EOF && c!='\r' && c!='\n')
        c = fgetc(f);
    while (c=='\r' || c=='\n')
        c = fgetc(f);
    if (c!=EOF)
        ungetc(c,f);
    if (ferror(f))
        throw new Exception("Read error in file `%s'", fname.c_str());
}

/*
Example code:

#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
    if (argc<2)
        return 1;

    try {
        FileReader freader(argv[1],200);
        char *line;
        while ((line = freader.readLine())!=NULL)
            cout << line << "\n";
    } catch (Exception *e) {
        cout << "Error: " << e->message() << endl;
    }
    return 0;
}
*/

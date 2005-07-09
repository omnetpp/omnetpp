//=========================================================================
//  GETVECTORS.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <stdlib.h>
#include "getvectors.h"
#include "util.h"


inline void skipSpace(char *&s)
{
    while (*s==' ' || *s=='\t') s++;
}

static void parseAndAppendQuotedString(char *&s, std::string& dest)
{
    while (*s==' ' || *s=='\t') s++;
    if (*s!='"')
        throw new Exception("invalid vector file syntax: invalid vector definition: missing opening quote");
    char *start = s+1;
    s++;
    while (*s && (*s!='"' || *(s-1)=='\\') && *s!='\r' && *s!='\n') s++;
    if (*s!='"')
        throw new Exception("invalid vector file syntax: invalid vector definition: missing closing quote");
    *s = '\0';
    dest += start;
    *s = '"';
    s++;
}

void getVectors(const char *fname, OutVectorArray& array)
{
    const int buffersize = 64*1024;  // 64K buffer
    //const int buffersize = 200;
    char *buffer = new char[buffersize+100];  // +1 for EOS, +100 for MSVC hack (see later)
    int bufferused = 0;
    FILE *f = NULL;
    bool eofreached = false;

    // open file if needed
    if (!f)
    {
        f = fopen(fname, "r");
        if (!f)
            throw new Exception("cannot open `%s' for read", fname);
    }

    // process all full lines
    while (!eofreached)
    {
        // read enough bytes to fill the buffer
        int bytesread = fread(buffer+bufferused, 1, buffersize-bufferused, f);
        if (feof(f))
            eofreached = true;
        if (ferror(f))
            throw new Exception("error reading from `%s'", fname);

        buffer[bufferused+bytesread] = '\0';
        const char *endbuffer = buffer+bufferused+bytesread;

        char *line = buffer;

        // process all full lines
        while (1)
        {
            // do we have a full line?
            char *s = line;
            while (*s && *s!='\r' && *s!='\n') s++;
            if (!*s && !eofreached)  // if at eof, we have to process unterminated last line, too
                break;

            s = line;
            if (!*s && eofreached)
            {
                // end of file, no more work
                break;
            }
            else if (!*s || *s=='\r' || *s=='\n')
            {
                // blank line, ignore
            }
            else if (*s=='v' && !strncmp(s,"vector",6))
            {
                s+=6;

                VecData vecdata;

                // parse line

                // MSVC hack: their strtol() calls strlen() first (!!!), so we have to shorten the string somewhat
                char oldchar = *(s+60);
                char *olds = s;
                *(s+60) = 0;

                // vectorId
                char *e;
                vecdata.vectorId = (int) strtol(s,&e,10);
                if (s==e)
                    throw new Exception("invalid vector file syntax: invalid vector id in vector definition");
                s = e;

                *(olds+60) = oldchar; // MSVC hack

                DBG(("getvectors: seen vector %d\n", vecdata.vectorId));

                // module name, vector name
                parseAndAppendQuotedString(s,vecdata.moduleName);
                parseAndAppendQuotedString(s,vecdata.vectorName);

                array.push_back(vecdata);

                // skip optional "1"
                skipSpace(s);
                if (*s=='1') s++;
                skipSpace(s);
            }
            else
            {
                // data line, skip it
                while (*s && *s!='\r' && *s!='\n') s++;
            }

            // skip line termination
            if (*s && *s!='\r' && *s!='\n')
                throw new Exception("invalid vector file syntax: garbage at end of line ('%c')", *s);
            while (*s=='\r' || *s=='\n') s++;
            line = s;
        }

        // copy the last (partial) line to beginning of buffer
        bufferused = endbuffer-line;
        memcpy(buffer, line, bufferused);
    }

    fclose(f);
}


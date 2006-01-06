//==========================================================================
//  NEDFILE.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <assert.h>
#include "nedfile.h"

#ifdef _MSC_VER
#pragma warning(disable:4996) // disable VC8.0 warnings on stat(), etc
#endif

//-----------------------------------------------------------

NEDFile::NEDFile()
{
    wholeFile = NULL;

    numLines = 0;
    lineBeg = NULL;

    end = 0;

    commentBufLen = 1024;
    commentBuf = new char[commentBufLen];
    if (!commentBuf) commentBufLen=0;
}

NEDFile::~NEDFile()
{
    delete wholeFile;
    delete lineBeg;
    delete commentBuf;
}

bool NEDFile::readFile(const char *filename)
{
    if (wholeFile) return false; // reinit not supported

    // Note: must use binary mode on the file, otherwise due to CR/LF conversion
    // the number of characters actually stored will be less than "size"
    // (which is the same as fread()'s return value), and we'll get garbage
    // at the end of the buffer.
    FILE *intmp = fopen(filename,"rb");
    if (!intmp) return false;

    struct stat statbuf;
    fstat(fileno(intmp), &statbuf);
    int size = statbuf.st_size;
    wholeFile = new char [size+2];  // +1 because last line may need an extra '\n'
    if (!wholeFile) return false;

    fread(wholeFile,size,1,intmp);
    fclose(intmp);
    wholeFile[size]='\0';

    return indexLines();
}

bool NEDFile::setData(const char *data)
{
    if (wholeFile) return false;  // reinit not supported

    wholeFile = new char [strlen(data)+2]; // +1 because last line may need an extra '\n'
    if (!wholeFile) return false;
    strcpy(wholeFile,data);
    return indexLines();
}

// indexLines()
//   Sets up the lineBeg[] array. Line numbering goes from 1, ie. the first line
//   is lineBeg[1]
bool NEDFile::indexLines()
{
    // convert all CR and CR+LF into LF to avoid trouble
    char *s, *d;
    for (s=d=wholeFile; d==wholeFile || *(d-1); )
    {
        if (*s=='\r' && *(s+1)=='\n')  s++;
        else if (*s=='\r') {s++; *d++ = '\n';}
        else *d++ = *s++;
    }

    // terminate last line if necessary
    d--;  // now d points to terminating zero
    if (*(d-1)!='\n') {*d++ = '\n'; *d = '\0';}

    // count lines
    numLines = 0;
    for (s = wholeFile; *s; s++)
        if (*s=='\n')
            numLines++;

    // allocate array
    lineBeg = new char * [numLines+2];

    // fill in array
    lineBeg[0] = NULL;
    lineBeg[1] = wholeFile;
    int line = 2;
    for (s = wholeFile; *s; s++)
        if (*s=='\n')
            lineBeg[line++] = s+1;

    // last line plus one points to end of file (terminating zero)
    assert(line==numLines+2);
    assert(lineBeg[numLines+1]==s);

    return true;
}

int NEDFile::lineType(char *s)
{
    while (*s==' ' || *s=='\t') s++;
    if (*s=='/' && *(s+1)=='/') return COMMENT_LINE;
    if (!*s || *s=='\n') return BLANK_LINE;
    return CODE_LINE;
}

int NEDFile::lineContainsCode(char *s)
{
    // tolerant version: punctuation (,;:) does not count as code
    while (*s==' ' || *s=='\t' || *s==':' || *s==',' || *s==';') s++;
    if (*s=='/' && *(s+1)=='/') return false;
    if (!*s || *s=='\n') return false;
    return true;
}


int NEDFile::getIndent(char *s)
{
    int co = 0;
    while (*s==' ' || *s=='\t')
    {
        co += (*s=='\t') ? 8-(co%8) : 1;
        s++;
    }
    return co;
}

int NEDFile::lastColumn(char *s)
{
    int co = 0;
    while (*(s+1) && *(s+1)!='\n')
    {
        co += (*s=='\t') ? 8-(co%8) : 1;
        s++;
    }
    return co;
}

char *NEDFile::getPosition(int line, int column)
{
    // tolerant version: if line is out of range, return beginning or end of file
    if (line<1)
        return lineBeg[1];
    if (line>numLines)
        return lineBeg[numLines]+strlen(lineBeg[numLines]);

    char *s = lineBeg[line];

    int co = 0;
    while (co<column)
    {
        if (!*s) return s;
        if (*s=='\n')
            {column-=co; co=0;}
        else if (*s=='\t')
            co += 8-(co%8);
        else
            co++;
        s++;
    }
    return s;
}

char *NEDFile::get(YYLTYPE pos)
{
    if (end) {*end = savedChar; end=NULL;}

    // check: maybe NULLPOS
    if (!pos.first_line || !pos.last_line)
        return "";

    // the meat of the whole stuff:
    end = getPosition(pos.last_line,  pos.last_column);
    savedChar = *end;
    *end = '\0';

    char *beg = getPosition(pos.first_line, pos.first_column);
    return beg;
}

// getFileComment()
//   all subsequent comment and blank lines will be included,
//   up to the _last blank_ line
//
char *NEDFile::getFileComment()
{
    if (end) {*end = savedChar; end=NULL;}

    // seek end of comment block (that is, last blank line)
    int li2 = 1;
    int lastblank=0, lt;
    while (li2<=numLines && (lt=lineType(getPosition(li2,0)))!=CODE_LINE)
    {
        if (lt==BLANK_LINE) lastblank=li2;
        li2++;
    }

    // if file doesn't contain code line, take the whole file
    if (li2>numLines) lastblank=numLines;

    // return comment block
    YYLTYPE comment;
    comment.first_line = 1;
    comment.first_column = 0;
    comment.last_line = lastblank+1;
    comment.last_column = 0;
    return stripComment(get(comment));
}

// topLineOfBannerComment()
//   li: code line below the comment
//   result: ==li     there was no comment
//           ==li-1   single-line comment on line li-1
//
int NEDFile::topLineOfBannerComment(int li)
{
    // seek beginning of comment block
    int indent = getIndent(getPosition(li,0));
    while (li>=2 &&
           lineType(getPosition(li-1,0))==COMMENT_LINE  &&
           getIndent(getPosition(li-1,0))<=indent
           )
        li--;
    return li;
}

// getBannerComment()
//
char *NEDFile::getBannerComment(YYLTYPE pos)
{
    if (end) {*end = savedChar; end=NULL;}

    // there must be nothing before it on the same line
    char *beg = getPosition(pos.first_line, pos.first_column);
    for (char *s=getPosition(pos.first_line, 0); s<beg; s++)
        if (*s!=' ' && *s!='\t')
            return "";

    // return comment block
    YYLTYPE comment;
    comment.first_line = topLineOfBannerComment(pos.first_line);
    comment.first_column = 0;
    comment.last_line = pos.first_line;
    comment.last_column = 0;
    return stripComment(get(comment));
}

// getTrailingComment()
//  also serves as "getRightComment"
//  NOTE: only handles really trailing comments, ie. those
//        after last_line.last_column
//
char *NEDFile::getTrailingComment(YYLTYPE pos)
{
    if (end) {*end = savedChar; end=NULL;}

    // there must be no code after it on the same line
    char *endp = getPosition(pos.last_line, pos.last_column);
    if (lineContainsCode(endp))
        return "";

    // seek 1st line after comment (lineafter)
    int lineafter;

    if (pos.last_line>=numLines) // 'pos' ends on last line of file
    {
        lineafter = numLines+1;
    }
    else
    {
        // seek fwd to next code line (or end of file)
        lineafter = pos.last_line+1;
        while (lineafter<numLines && lineType(getPosition(lineafter,0))!=CODE_LINE)
            lineafter++;

        // now seek back to beginning of comment block
        lineafter = topLineOfBannerComment(lineafter);
    }

    // return comment block
    YYLTYPE comment;
    comment.first_line = pos.last_line;
    comment.first_column = pos.last_column;
    comment.last_line = lineafter;
    comment.last_column = 0;
    return stripComment(get(comment));
}

// stripComment()
//  return a "stripped" version of a multi-line comment --
//  comment marks "//" and spaces before them removed,
//  lines without comments are replaced by a pure "-" line
//
char *NEDFile::stripComment(char *comment)
{
    int numlines = numLines; // comment is at most as long as whole file

    // expand buffer if necessary
    if (commentBufLen <= int(strlen(comment))+numlines)
    {
        commentBufLen = strlen(comment)+numlines+256;
        delete commentBuf;
        commentBuf = new char[commentBufLen];
        if (!commentBuf) {commentBufLen=0;return NULL;}
    }

    char *s = comment;
    char *d = commentBuf;

    while(*s)
    {
        // find beg. of comment on the line (or end of line)
        while (!(*s=='/' && *(s+1)=='/') && *s!='\n') s++;

        if (*s=='\n')
        {
            // a no-comment line is replaced by a line containing a bare '-'
            *d++ = '-';
            *d++ = *s++;
        }
        else
        {
            // add the comment (chars after the '//') to the buffer
            s+=2;
            //if (*s==' ') s++;
            while (*s && *s!='\n') *d++ = *s++;
            if (*s=='\n') *d++ = *s++;
        }
    }
    *d = '\0';
    return commentBuf;
}


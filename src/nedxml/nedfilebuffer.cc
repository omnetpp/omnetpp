//==========================================================================
//   NEDFILEBUFFER.CC -
//            part of OMNeT++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2002 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <sys/stat.h>
#include <assert.h>
#include "nedfilebuffer.h"

//-----------------------------------------------------------

NEDFileBuffer::NEDFileBuffer()
{
    wholeFile = NULL;

    numLines = 0;
    lineBeg = NULL;

    end = 0;

    commentBufLen = 1024;
    commentBuf = new char[commentBufLen];
    if (!commentBuf) commentBufLen=0;
}

NEDFileBuffer::~NEDFileBuffer()
{
    delete wholeFile;
    delete lineBeg;
    delete commentBuf;
}

bool NEDFileBuffer::readFile(const char *filename)
{
    if (wholeFile) return false; // reinit not supported

    // load file into memory
    FILE *intmp = fopen(filename,"r");
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

bool NEDFileBuffer::setData(const char *data)
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
bool NEDFileBuffer::indexLines()
{
    // convert all CR and CR+LF into LF to avoid trouble
    char *s, *d;
    for (s=d=wholeFile; *d; )
    {
        if (*s=='\r' && *(s+1)=='\n')  s++;
        else if (*s=='\r') {s++; *d++ = '\n';}
        else *d++ = *s++;
    }

    // terminate last line if necessary
    if (*(d-1)!='\n') *d++ = '\n';
    *d = '\0';

    // count lines
    numLines = 0;
    for (s = wholeFile; *s; s++)
        if (*s=='\n')
            numLines++;

    // allocate array
    lineBeg = new char * [numLines+2];
    if (!lineBeg) return false;

    // fill in array
    lineBeg[1] = wholeFile;
    int line = 2;
    for (s = wholeFile; *s; s++)
        if (*s=='\n')
            lineBeg[line++] = s+1;
    return true;
}

int NEDFileBuffer::lineType(char *s)
{
    while (*s==' ' || *s=='\t') s++;
    if (*s=='/' && *(s+1)=='/') return COMMENT_LINE;
    if (!*s || *s=='\n') return BLANK_LINE;
    return CODE_LINE;
}

int NEDFileBuffer::lineContainsCode(char *s)
{
    // tolerant version: punctuation (,;:) does not count as code
    while (*s==' ' || *s=='\t' || *s==':' || *s==',' || *s==';') s++;
    if (*s=='/' && *(s+1)=='/') return false;
    if (!*s || *s=='\n') return false;
    return true;
}


int NEDFileBuffer::getIndent(char *s)
{
    int co = 0;
    while (*s==' ' || *s=='\t')
    {
        co += (*s=='\t') ? 8-(co%8) : 1;
        s++;
    }
    return co;
}

int NEDFileBuffer::lastColumn(char *s)
{
    int co = 0;
    while (*(s+1) && *(s+1)!='\n')
    {
        co += (*s=='\t') ? 8-(co%8) : 1;
        s++;
    }
    return co;
}

char *NEDFileBuffer::getPosition(int line, int column)
{
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

const char *NEDFileBuffer::get(YYLTYPE pos)
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
const char *NEDFileBuffer::getFileComment()
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

    // return comment block
    YYLTYPE comment;
    comment.first_line = 1;
    comment.first_column = 0;
    comment.last_line = lastblank+1;
    comment.last_column = 0;
    return stripComment(get(comment),lastblank);
}

// topLineOfBannerComment()
//   li: code line below the comment
//   result: ==li     there was no comment
//           ==li-1   single-line comment on line li-1
//
int NEDFileBuffer::topLineOfBannerComment(int li)
{
    // seek beginning of comment block
    int indent = getIndent(lineBeg[li]);
    while (li>=2 &&
           lineType(lineBeg[li-1])==COMMENT_LINE  &&
           getIndent(lineBeg[li-1])<=indent
           )
        li--;
    return li;
}

// getBannerComment()
//
const char *NEDFileBuffer::getBannerComment(YYLTYPE pos)
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
    return stripComment(get(comment),comment.last_line-comment.first_line);
}

// getTrailingComment()
//  also serves as "getRightComment"
//  NOTE: only handles really trailing comments, ie. those
//        after last_line.last_column
//
const char *NEDFileBuffer::getTrailingComment(YYLTYPE pos)
{
    if (end) {*end = savedChar; end=NULL;}

    // there must be no code after it on the same line
    char *endp = getPosition(pos.last_line, pos.last_column);
    if (lineContainsCode(endp))
        return "";

    // seek 1st line after comment (lineafter)
    int lineafter;

    if (pos.last_line==numLines) // 'pos' ends on last line of file
    {
        lineafter = numLines+1;
    }
    else
    {
        // seek fwd to next code line (or end of file)
        lineafter = pos.last_line+1;
        while (lineafter<numLines && lineType(lineBeg[lineafter])!=CODE_LINE)
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
    return stripComment(get(comment),comment.last_line-comment.first_line);
}

// stripComment()
//  return a "stripped" version of a multi-line comment --
//  all non-comment elements (comma, etc) are deleted
//
char *NEDFileBuffer::stripComment(const char *comment, int numlines)
{
    // expand buffer if necessary
    if (commentBufLen < strlen(comment)+1)
    {
        commentBufLen = strlen(comment)+1;
        delete commentBuf;
        commentBuf = new char[commentBufLen];
        if (!commentBuf) {commentBufLen=0;return NULL;}
    }

    const char *s = comment;
    char *d = commentBuf;
    bool incomment = false;
    while(*s)
    {
        if ((*s=='/' && *(s+1)=='/')) incomment = true;
        if (*s=='\n') incomment = false;

        if (incomment || isspace(*s))
            *d++ = *s++;
        else
            s++;
    }
    *d = '\0';
    return commentBuf;
}


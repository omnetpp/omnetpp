//==========================================================================
//   NEDFILEBUFFER.H -
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

#ifndef __NEDFILEBUFFER_H
#define __NEDFILEBUFFER_H

#include "nedparser.h"   // for YYLTYPE

/**
 * Used internally by NEDParser. Stores the full text of a NED file,
 * and makes it possible to retrieve parts of it by (line1,col1,line2,col2)
 * coordinates passed in an YYLTYPE structure. Also finds and retrieves
 * comments near a position passed in an YYLTYPE.
 *
 * @ingroup NEDParser
 */
class NEDFileBuffer
{
  private:
    enum {COMMENT_LINE, BLANK_LINE, CODE_LINE};

    char *wholeFile;

    int numLines;
    char **lineBeg;

    char *end;
    char savedChar;

    char *commentBuf;
    int commentBufLen;

  private:
    int lineType(char *s);
    int lineContainsCode(char *s);
    int getIndent(char *s);
    int lastColumn(char *s);
    char *getPosition(int line, int column);

    bool indexLines();
    int topLineOfBannerComment(int li);
    char *stripComment(const char *s, int numlines);

  public:
    NEDFileBuffer();
    ~NEDFileBuffer();

    bool readFile(const char *filename);
    bool setData(const char *data);

    const char *get(YYLTYPE pos);

    const char *getFileComment();
    const char *getBannerComment(YYLTYPE pos);
    const char *getTrailingComment(YYLTYPE pos);
};

#endif


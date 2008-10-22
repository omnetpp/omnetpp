//==========================================================================
//  NEDFILE.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __NEDFILE_H
#define __NEDFILE_H

#include "parsened.h"   // for YYLTYPE

enum {COMMENT_LINE, BLANK_LINE, CODE_LINE};

class NEDFile
{
  private:
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
    const char *stripComment(const char *s);

  public:
    NEDFile();
    ~NEDFile();

    bool readFile(const char *filename);
    bool setData(const char *data);

    const char *get(YYLTYPE pos);

    const char *getFileComment();
    const char *getBannerComment(YYLTYPE pos);
    const char *getTrailingComment(YYLTYPE pos);
};

#endif


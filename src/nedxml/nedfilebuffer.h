//==========================================================================
//  NEDFILEBUFFER.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
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
    char *stripComment(const char *s);

  public:
    /**
     * Constructor.
     */
    NEDFileBuffer();

    /**
     * Destructor.
     */
    ~NEDFileBuffer();

    /**
     * Reads NED file given with its filename.
     *
     * @see setData()
     */
    bool readFile(const char *filename);

    /**
     * Uses literal NED text.
     *
     * @see readFile()
     */
    bool setData(const char *data);

    /**
     * Returns pointer to a text region defined by (beg-line, beg-col) and
     * (end-line, end-col). The text is NOT copied, only a null character
     * is written temporarily into the stored string at (end-line, end-col) --
     * this also means you should NOT keep more than one pointer returned by
     * get()!
     */
    const char *get(YYLTYPE pos);

    /**
     * Returns comment at top of file. Uses get()!
     */
    const char *getFileComment();

    /**
     * Returns banner comment above text range passed in pos. Uses get()!
     */
    const char *getBannerComment(YYLTYPE pos);

    /**
     * Returns trailing comment below text range passed in pos. Uses get()!
     */
    const char *getTrailingComment(YYLTYPE pos);

    /**
     * Returns pointer to the full source code. Uses get()!
     */
    const char *getFullText();
};

#endif


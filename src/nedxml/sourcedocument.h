//==========================================================================
//  SOURCEDOCUMENT.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_NEDXML_SOURCEDOCUMENT_H
#define __OMNETPP_NEDXML_SOURCEDOCUMENT_H

#include "nedxmldefs.h"

namespace omnetpp {
namespace nedxml {

struct YYLoc;

/**
 * @brief Stores the full text of a NED/MSG file.
 *
 * Used internally by NedParser. Stores the full text of a source file,
 * and makes it possible to retrieve parts of it by (line1,col1,line2,col2)
 * coordinates passed in an YYLTYPE structure. Also finds and retrieves
 * comments near a position passed in an YYLTYPE.
 *
 * @ingroup NedParser
 */
class NEDXML_API SourceDocument
{
  private:
    enum {COMMENT_LINE, BLANK_LINE, CODE_LINE};

    char *wholeFile = nullptr;

    int numLines = 0;
    char **lineBeg = nullptr;

    char *end = nullptr;
    char savedChar = 0;

    int commentBufLen = 1024; // has to be before commentBuf to get initialized before it
    char *commentBuf;

  private:
    int getLineType(int lineNumber);
    int getLineType(const char *s);
    bool lineContainsCode(const char *s);
    int getLineIndent(int lineNumber);
    int getLineIndent(const char *s);
    char *getPosition(int line, int column);

    bool indexLines();
    int topLineOfBannerComment(int li);
    char *stripComment(const char *s);

    YYLoc getFileCommentPos();
    YYLoc getBannerCommentPos(YYLoc pos);
    YYLoc getTrailingCommentPos(YYLoc pos);

  public:
    /**
     * Constructor.
     */
    SourceDocument();

    /**
     * Destructor.
     */
    ~SourceDocument();

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
    const char *get(YYLoc pos);

    /**
     * Returns comment at top of file. Uses get()!
     */
    const char *getFileComment();

    /**
     * Returns banner comment above text range passed in pos. Uses get()!
     */
    const char *getBannerComment(YYLoc pos);

    /**
     * Returns trailing comment below text range passed in pos. Uses get()!
     */
    const char *getTrailingComment(YYLoc pos);

    /**
     * Returns the next comment inside the given region, and moves
     * the region's start past the comment. Uses get()!
     */
    const char *getNextInnerComment(YYLoc& pos);

    /**
     * Returns 1:0...end.
     */
    YYLoc getFullTextPos();

    /**
     * Returns pointer to the full source code. Uses get()!
     */
    const char *getFullText();

    /**
     * Shrinks the given region to exclude any leading/trailing whitespace
     * and comments.
     */
    void trimSpaceAndComments(YYLoc& pos);
};

}  // namespace nedxml
}  // namespace omnetpp


#endif


//=========================================================================
//  FILETOKENIZER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __FILETOKENIZER_H_
#define __FILETOKENIZER_H_

#include <vector>
#include <string>

/**
 * Tokenizes a file, line by line. Supposed to be VERY efficient because
 * it'll be used on several hundred-megabyte files.
 */
class FileTokenizer
{
  public:
    typedef std::vector<char *> CharPVector;
    enum StatusCode {OK, EOFREACHED, CANNOTOPEN, CANNOTREAD, INCOMPLETELINE,
                     UNMATCHEDQUOTE, LINETOOLONG};

  private:
    // the file
    std::string fname;
    FILE *f;
    bool eofreached;

    // the buffer
    size_t buffersize;
    char *buffer;
    char *bufferend;  // =buffer+buffersize

    // the currently used region in buffer
    char *databeg;
    char *dataend;

    // num of line last returned
    int linenum;

    // status
    StatusCode errcode;

  private:
    // moves remaining data (databeg,dataend) to beginning of buffer,
    // reads new bytes to the buffer, and returns offset by which
    // existing data were moved.
    size_t readMore(CharPVector& vec);

  public:
    FileTokenizer(const char *fileName, size_t bufferSize=64*1024);
    ~FileTokenizer();

    /**
     * Returns false if: cannot read, incomplete line (missing CRLF),
     * unmatched quote.
     */
    bool getLine(CharPVector& vec);

    bool eof() const {return errcode==EOFREACHED;}

    int lineNum() const {return linenum;}

    StatusCode errorCode() const {return errcode;}

    std::string errorMsg() const;
};

#endif


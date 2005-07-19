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
    enum StatusCode {OK, EOFREACHED, CANNOTOPEN, CANNOTREAD, INCOMPLETELINE,
                     UNMATCHEDQUOTE, LINETOOLONG, TOOMANYTOKENS};

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

    // storage of tokens
    char **vec;
    int vecsize;
    int numtokens;

    // num of line last returned
    int linenum;

    // status
    StatusCode errcode;

  private:
    // moves remaining data (databeg,dataend) to beginning of buffer,
    // reads new bytes to the buffer, and returns offset by which
    // existing data were moved.
    size_t readMore();

  public:
    /**
     * Creates a tokenizer object for the given file, with the given
     * buffer size. The file doesn't get opened yet.
     */
    FileTokenizer(const char *fileName, size_t bufferSize=64*1024);

    /**
     * Destructor.
     */
    ~FileTokenizer();

    /**
     * Tokenizes a line, results can be accessed via numTokens() and
     * tokens(). Return is true if there's no error; it's false if
     * file could not be opened/read, last line was incomplete line
     * (missing CRLF), line was too long (>bufferSize) or there
     * were too many items on the line (currently >16,000), or
     * there was an unmatched quote. See StatusCode values.
     */
    bool readLine();

    /**
     * Number of tokens read by the last readLine() call.
     */
    int numTokens() {return numtokens;}

    /**
     * Array that holds the tokens read by the last readLine() call;
     * contents will be overwritten with the next call.
     */
    char **tokens() {return vec;}

    /**
     * True if more readLine() calls are permitted.
     */
    bool ok() const {return errcode==OK;}

    /**
     * True if end of input file was reached without any error.
     */
    bool eof() const {return errcode==EOFREACHED;}

    /**
     * Number of last line parsed by readLine().
     */
    int lineNum() const {return linenum;}

    /**
     * Detailed error code; this completes ok() and eof().
     */
    StatusCode errorCode() const {return errcode;}

    /**
     * Produces a textual error message, based on the error code
     * and the line number.
     */
    std::string errorMsg() const;
};

#endif


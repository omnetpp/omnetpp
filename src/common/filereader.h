//=========================================================================
//  FILEREADER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __FILEREADER_H_
#define __FILEREADER_H_

#include <vector>
#include <string>

/**
 * Reads a file line by line. It has to be very efficient since
 * it may be used up to gigabyte-sized files (output vector files
 * and event logs). File reading is done in large chunks, and
 * the code avoids string copying and duplicating.
 */
class FileReader
{
  public:
    enum StatusCode {OK, EOFREACHED, CANNOTOPEN, CANNOTREAD, INCOMPLETELINE, LINETOOLONG};

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

    // the pointer returned by readLine()
    char *wholeline;
    char *wholelineend;

    // num of line last returned
    int linenum;

    // status
    StatusCode errcode;

  private:
    // moves remaining data (databeg,dataend) to beginning of buffer,
    // reads new bytes to the buffer, and returns offset by which
    // existing data were moved.
    size_t readMore();

    // nulls out wholeline/wholelineend, and returns null
    char *zapline();

  public:
    /**
     * Creates a tokenizer object for the given file, with the given
     * buffer size. The file doesn't get opened yet.
     */
    FileReader(const char *fileName, size_t bufferSize=64*1024);

    /**
     * Destructor.
     */
    ~FileReader();

    /**
     * Reads a line from the file, and returns a pointer to its first character.
     * It returns NULL after EOF and on error (file could not be opened/read,
     * last line was incomplete (missing CRLF), line was too long
     * (>bufferSize)); see StatusCode values.
     */
    char *readLine();

    /**
     * True if more readAndTokenizeLine() calls are permitted.
     */
    bool ok() const {return errcode==OK;}

    /**
     * True if end of input file was reached without any error.
     */
    bool eof() const {return errcode==EOFREACHED;}

    /**
     * Number of last line parsed by readAndTokenizeLine().
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


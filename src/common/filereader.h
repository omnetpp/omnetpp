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
 * All functions throw class Exception on error.
 */
class FileReader
{
  private:
    // the file
    std::string fname;
    FILE *f;

    // the buffer
    size_t buffersize;
    char *buffer;
    char *bufferend;  // =buffer+buffersize
    long bufferfileoffset;

    // the currently used region in buffer
    char *databeg;
    char *dataend;

    // the pointer returned by readLine()
    char *wholeline;

    // num of line last returned
    int linenum;

  private:
    // moves remaining data (databeg,dataend) to beginning of buffer,
    // reads new bytes to the buffer, and returns offset by which
    // existing data were moved.
    size_t readMore();

    // opens the given file
    void openFile();

    // assert data structure consistence
    void checkConsistence();

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
     * It returns NULL after EOF. Incomplete last line gets ignored (returns NULL)
     * as it is possible that the file is currently being written into.
     */
    char *readLine();

    /**
     * Number of last line parsed by readAndTokenizeLine().
     */
    int lineNum() const {return linenum;}

    /**
     * Returns the offset of the line last parsed with readLine()
     */
    long lineStartOffset() {return (wholeline-buffer) + bufferfileoffset;}

    /**
     * Returns the size of the file
     */
    long fileSize();

    /**
     * Positions to the beginning of the first full line following the given offset.
     */
    void seekTo(long offset);
};

#endif


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
 * the code avoids string copying and duplicating. The in memory
 * buffer must be able to contain at least two lines, therefore
 * the maximum line length is limited to buffer size divided by 2.
 *
 * All functions throw class Exception on error.
 */
class FileReader
{
  private:
    // the file
    std::string fileName;
    FILE *f;

    // the buffer
    size_t bufferSize;
    char *bufferBegin;
    char *bufferEnd;  // = buffer + bufferSize
    long bufferFileOffset;
    long fileSize;

    // the currently used (filled with data) region in buffer
    size_t maxLineSize;
    char *dataBegin;
    char *dataEnd;

    // the position where readNextLine() or readPreviousLine() starts from
    char *currentDataPointer;

    // the pointer returned by readNextLine() or readPreviousLine()
    long lastLineStartOffset;
    long lastLineEndOffset;

    // total number of lines read in so far
    long numReadLines;

    // total bytes read in so far
    long numReadBytes;

  private:
    /**
     * Reads data into the buffer till the end in the given direction.
     */
    void fillBuffer(bool forward);

    // opens the given file
    void openFile();
    void closeFile();

    // assert data structure consistence
    void checkConsistence();

    long pointerToFileOffset(char *pointer) { return pointer - bufferBegin + bufferFileOffset; }
    char* fileOffsetToPointer(long offset) { return offset - bufferFileOffset + bufferBegin; }

    long getDataBeginFileOffset() { return pointerToFileOffset(dataBegin); }
    long getDataEndFileOffset() { return pointerToFileOffset(dataEnd); }
    bool hasData() { return dataBegin != dataEnd; }

    bool isLineStart(char *&s);
    char *findNextLineStart(char *s, bool bufferFilled = false);
    char *findPreviousLineStart(char *s, bool bufferFilled = false);

  public:
    /**
     * Creates a tokenizer object for the given file, with the given
     * buffer size. The file doesn't get opened yet.
     */
    FileReader(const char *fileName, size_t bufferSize = 64 * 1024);

    /**
     * Destructor.
     */
    ~FileReader();

    /**
     * Reads the next line from the file starting from the current position, and returns a pointer to its first character.
     * It returns NULL after EOF. Incomplete last line gets ignored (returns NULL)
     * as it is possible that the file is currently being written into.
     * Moves the current position to the end of the line just returned.
     */
    char *readNextLine();

    /**
     * Reads the previous line from the file ending at the current position, and returns a pointer to its first character.
     * It returns NULL when the beginning of file reached. 
     * Moves the current position to the beginning of the line just returned.
     */
    char *readPreviousLine();

    /**
     * Returns the start offset of the line last parsed with readNextLine() or readPreviousLine().
     */
    long getLastLineStartOffset() { return lastLineStartOffset; }

    /**
     * Returns the end offset of the line last parsed with readNextLine() or readPreviousLine().
     * This points to the start offset of the next line, so it includes the CR/LF of the previous line.
     */
    long getLastLineEndOffset() { return lastLineEndOffset; }

    /**
     * Returns the length of the last line including CR/LF.
     */
    long getLastLineLength() { return lastLineEndOffset - lastLineStartOffset; }

    /**
     * Returns the size of the file.
     */
    long getFileSize();

    /**
     * Moves the current position to the given offset.
     */
    void seekTo(long offset, long ensureBufferSizeAround = 0);

    /**
     * Returns the total number of lines read in so far.
     */
    long getNumReadLines() { return numReadLines; };

    /**
     * Returns the total number of bytes read in so far.
     */
    long getNumReadBytes() { return numReadBytes; }
};

#endif

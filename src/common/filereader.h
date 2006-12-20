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

#ifdef _MSC_VER
typedef __int64 int64;
typedef __int64 file_offset_t;
#else
typedef long long int64;
typedef off_t file_offset_t;
#endif

#if defined _MSC_VER && (_MSC_VER >= 1400)
#define filereader_ftell _ftelli64
#define filereader_fseek _fseeki64
#elif defined _MSC_VER
#define filereader_ftell ftell
#define filereader_fseek fseek
#else
#define filereader_ftell ftello
#define filereader_fseek fseeko
#endif

/**
 * Reads a file line by line. It has to be very efficient since
 * it may be used up to gigabyte-sized files (output vector files
 * and event logs). File reading is done in large chunks, and
 * the code avoids string copying and duplicating. The in memory
 * buffer must be able to contain at least two lines, therefore
 * the maximum line length is limited to buffer size divided by 2.
 *
 * All functions throw class opp_runtime_error on error.
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
    file_offset_t bufferFileOffset;
    file_offset_t fileSize;

    // the currently used (filled with data) region in buffer
    size_t maxLineSize;
    char *dataBegin;
    char *dataEnd;

    // the position where readNextLine() or readPreviousLine() starts from
    char *currentDataPointer;

    // the pointer returned by readNextLine() or readPreviousLine()
    file_offset_t lastLineStartOffset;
    file_offset_t lastLineEndOffset;

    // total number of lines read in so far
    int64 numReadLines;

    // total bytes read in so far
    int64 numReadBytes;

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

    file_offset_t pointerToFileOffset(char *pointer) { return pointer - bufferBegin + bufferFileOffset; }
    char* fileOffsetToPointer(file_offset_t offset) { return offset - bufferFileOffset + bufferBegin; }

    file_offset_t getDataBeginFileOffset() { return pointerToFileOffset(dataBegin); }
    file_offset_t getDataEndFileOffset() { return pointerToFileOffset(dataEnd); }
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
    file_offset_t getLastLineStartOffset() { return lastLineStartOffset; }

    /**
     * Returns the end offset of the line last parsed with readNextLine() or readPreviousLine().
     * This points to the start offset of the next line, so it includes the CR/LF of the previous line.
     */
    file_offset_t getLastLineEndOffset() { return lastLineEndOffset; }

    /**
     * Returns the length of the last line including CR/LF.
     */
    int getLastLineLength() { return lastLineEndOffset - lastLineStartOffset; }

    /**
     * Returns the size of the file.
     */
    int64 getFileSize();

    /**
     * Moves the current position to the given offset.
     */
    void seekTo(file_offset_t offset, int ensureBufferSizeAround = 0);

    /**
     * Returns the total number of lines read in so far.
     */
    int64 getNumReadLines() { return numReadLines; };

    /**
     * Returns the total number of bytes read in so far.
     */
    int64 getNumReadBytes() { return numReadBytes; }
};

#endif

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
#include "commondefs.h"
#include "inttypes.h"   // for int64

typedef int64 file_offset_t;  // off_t on Linux

/*
#if defined _MSC_VER
# define filereader_ftell _ftelli64
# define filereader_fseek _fseeki64
# if _MSC_VER < 1400
   // Kludge: in Visual C++ 7.1, 64-bit fseek/ftell is not part of the public
   // API, but the functions are there in the CRT. Declare them here.
   int __cdecl _fseeki64 (FILE *str, __int64 offset, int whence);
   __int64 __cdecl _ftelli64 (FILE *stream);
# endif
#else
# define filereader_ftell ftello
# define filereader_fseek fseeko
#endif
*/

//FIXME replace this with the above code!!! once it compiles with 7.1
#if defined _MSC_VER && (_MSC_VER >= 1400)
#define filereader_ftell _ftelli64
#define filereader_fseek _fseeki64
#elif defined _MSC_VER || __MINGW32__ // FIXME: no 64 bit version under mingw?
// for Visual C++ 7.1, fall back to 32-bit functions
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
class COMMON_API FileReader
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
    int64 fileSize;

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

    int64 getFileSizeInternal();

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
     * This method is called automatically whenever the file is accessed.
     */
    void ensureFileOpen();

    /**
     * This method is called automatically from the destructor, but might be useful to release the file when it is not needed for a long period of time.
     */
    void ensureFileClosed();

    /**
     * Checks if file has been changed on disk.
     */
    bool isChanged();

    /**
     * Updates internal state to reflect file changes on disk.
     */
    void synchronize();

    /**
     * Reads the next line from the file starting from the current position, and returns a pointer to its first character.
     * It returns NULL after EOF. Incomplete last line gets ignored (returns NULL)
     * as it is possible that the file is currently being written into.
     * Moves the current position to the end of the line just returned.
     */
    char *getNextLineBufferPointer();

    /**
     * Reads the previous line from the file ending at the current position, and returns a pointer to its first character.
     * It returns NULL when the beginning of file reached.
     * Moves the current position to the beginning of the line just returned.
     */
    char *getPreviousLineBufferPointer();

    /**
     * Searches through the file from the current position for the given text and returns the first matching line.
     */
    char *findNextLineBufferPointer(const char *search);

    /**
     * Searches through the file from the current position for the given text and returns the first matching line.
     */
    char *findPreviousLineBufferPointer(const char *search);

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
    void seekTo(file_offset_t offset, unsigned int ensureBufferSizeAround = 0);

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

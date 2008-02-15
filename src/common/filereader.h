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
#include "platmisc.h"
#include "commondefs.h"
#include "inttypes.h"   // for int64

NAMESPACE_BEGIN

/**
 * Reads a file line by line. It has to be very efficient since
 * it may be used up to several gigabyte-sized files (output vector files
 * and event logs). File reading is done in large chunks, and
 * the code avoids string copying and duplicating. The in memory
 * buffer must be able to contain at least two lines, therefore
 * the maximum line length is limited to buffer size divided by 2.
 *
 * It maintains a position which is used to return subsequent lines from
 * the file in both directions from both ends. Automatically follows file
 * content when appended, but overwriting the file causes an exception to be thrown.
 *
 * All functions throw class opp_runtime_error on error.
 */
class COMMON_API FileReader
{
  private:
    // the file
    const std::string fileName;
    FILE *f;
    bool checkFileChanged;
    bool synchronizeWhenAppended;

    // the buffer
    const size_t bufferSize;
    const char *bufferBegin;
    const char *bufferEnd; // = buffer + bufferSize
    const size_t maxLineSize;

    // file positions and size
    file_offset_t bufferFileOffset;
    file_offset_t storedBufferFileOffset;
    int64 fileSize;

    // the currently used (filled with data) region in buffer
    char *dataBegin; // must point between bufferBegin and bufferEnd
    char *dataEnd; // must point between bufferBegin and bufferEnd

    // the very last line of the file as currently known
    file_offset_t lastLineOffset;
    int lastLineLength;
    std::string lastLine;

    // the position where readNextLine() or readPreviousLine() starts from
    char *currentDataPointer; // must point between dataBegin and dataEnd when used
    char *storedDataPointer;

    // the pointer returned by readNextLine() or readPreviousLine()
    file_offset_t currentLineStartOffset;
    file_offset_t currentLineEndOffset;

    // total number of lines read in so far
    int64 numReadLines;

    // total bytes read in so far
    int64 numReadBytes;

  public:
    enum FileChangedState {
        UNCHANGED,
        APPENDED,
        OVERWRITTEN,
    };

  private:
    /**
     * Reads data into the buffer till the end of the buffer in the given direction
     * starting from the current data pointer.
     * May read from 0 up to bufferSize number of bytes.
     */
    void fillBuffer(bool forward);

    void ensureFileOpenInternal();

    // assert data structure consistence
    void checkConsistence(bool checkDataPointer = false) const;
    FileChangedState checkFileChangedAndSynchronize();

    // store and restore state to be able to read at another position
    void storePosition();
    void restorePosition();

    file_offset_t pointerToFileOffset(char *pointer) const;
    char* fileOffsetToPointer(file_offset_t offset) const { return offset - bufferFileOffset + (char *)bufferBegin; }

    file_offset_t getDataBeginFileOffset() const { return pointerToFileOffset(dataBegin); }
    file_offset_t getDataEndFileOffset() const { return pointerToFileOffset(dataEnd); }
    bool hasData() const { return dataBegin != dataEnd; }

    void setCurrentDataPointer(char *pointer);

    bool isLineStart(char *s);
    char *findNextLineStart(char *s, bool bufferFilled = false);
    char *findPreviousLineStart(char *s, bool bufferFilled = false);

    int64 getFileSizeInternal();

  public:
    /**
     * Creates a tokenizer object for the given file, with the given buffer size.
     * The file doesn't get opened yet.
     */
    FileReader(const char *fileName, size_t bufferSize = 64 * 1024);

    /**
     * Destructor.
     */
    virtual ~FileReader();

    /**
     * Controls whether file is checked for changes each time before physically accessing it.
     */
    void setCheckFileChanged(bool value) { checkFileChanged = value; }

    /**
     * Controls what happens when it is determined that new content has been appended to the file.
     */
    void setSynchronizeWhenAppended(bool value) { synchronizeWhenAppended = value; }

    /**
     * This method is called automatically whenever the file is accessed through a public function.
     */
    void ensureFileOpen();

    /**
     * This method is called automatically from the destructor, but might be useful to release the file when it is
     * not needed for a long period of time.
     */
    void ensureFileClosed();

    /**
     * Checks if file has been changed on disk. A file change is considered to be an append if it did not change the
     * content of the last line (starting at the very same offset).
     */
    FileChangedState getFileChangedState();

    /**
     * Updates internal state to reflect file changes on disk, it does not move the current position
     * and thus it may become invalid for truncated files.
     */
    void synchronize();

    /**
     * Returns the first line from the file, see getNextLineBufferPointer.
     */
    char *getFirstLineBufferPointer();

    /**
     * Returns the last line from the file, see getPreviousLineBufferPointer.
     */
    char *getLastLineBufferPointer();

    /**
     * Reads the next line from the file starting from the current position, and returns a pointer to its first character.
     * It returns NULL after EOF. Incomplete last line gets ignored (returns NULL)
     * as it is possible that the file is currently being written into. When new lines are appended to the file subsequent
     * calls will return non NULL and continue reading lines from that on.
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
    file_offset_t getCurrentLineStartOffset() { return currentLineStartOffset; }

    /**
     * Returns the end offset of the line last parsed with readNextLine() or readPreviousLine().
     * This points to the start offset of the next line, so it includes the CR/LF of the previous line.
     */
    file_offset_t getCurrentLineEndOffset() { return currentLineEndOffset; }

    /**
     * Returns the length of the last line including CR/LF.
     */
    int getCurrentLineLength() { return currentLineEndOffset - currentLineStartOffset; }

    /**
     * Returns the size of the file when last time synchronize was called.
     */
    int64 getFileSize();

    /**
     * Moves the current position to the given offset.
     */
    void seekTo(file_offset_t offset, unsigned int ensureBufferSizeAround = 0);

    /**
     * Returns the total number of lines requested so far.
     */
    int64 getNumReadLines() { return numReadLines; };

    /**
     * Returns the total number of bytes read in so far.
     */
    int64 getNumReadBytes() { return numReadBytes; }
};

NAMESPACE_END


#endif

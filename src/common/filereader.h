//=========================================================================
//  FILEREADER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_FILEREADER_H
#define __OMNETPP_COMMON_FILEREADER_H

#include <vector>
#include <string>
#include <cstdint>
#include "omnetpp/platdep/platmisc.h"
#include "exception.h"
#include "commondefs.h"

namespace omnetpp {
namespace common {

/**
 * Reads a file line by line. It has to be very efficient since
 * it may be used up to several gigabyte-sized files (output vector files
 * and event logs). File reading is done in large chunks, and
 * the code avoids string copying and duplicating. The in-memory
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
    FILE *file;
    bool enableCheckFileForChanges;
    bool enableIgnoreAppendChanges;

    // the buffer
    const size_t bufferSize;
    const char *bufferBegin;
    const char *bufferEnd; // = buffer + bufferSize
    const size_t maxLineSize;

    // file positions and size
    file_offset_t bufferFileOffset;
    int64_t fileSize;

    // the currently used (filled with data) region in buffer
    char *dataBegin; // must point between bufferBegin and bufferEnd
    char *dataEnd; // must point between bufferBegin and bufferEnd

    // the very end of the file as currently known
    int lastSavedSize;
    char* lastSavedBufferBegin;

    // the position where readNextLine() or readPreviousLine() starts from
    char *currentDataPointer; // must point between dataBegin and dataEnd when used

    // the pointer returned by readNextLine() or readPreviousLine()
    file_offset_t currentLineStartOffset;
    file_offset_t currentLineEndOffset;

    // total number of lines read in so far
    int64_t numReadLines;

    // total bytes read in so far
    int64_t numReadBytes;

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
    int readFileEnd(void *dataPointer);
    void ensureFileOpenInternal();
    int64_t getFileSizeInternal();
    void checkConsistency(bool checkDataPointer = false) const;

    file_offset_t pointerToFileOffset(char *pointer) const;
    char* fileOffsetToPointer(file_offset_t offset) const { return offset - bufferFileOffset + (char *)bufferBegin; }

    file_offset_t getDataBeginFileOffset() const { return pointerToFileOffset(dataBegin); }
    file_offset_t getDataEndFileOffset() const { return pointerToFileOffset(dataEnd); }
    bool hasData() const { return dataBegin != dataEnd; }

    void setCurrentDataPointer(char *pointer);

    bool isLineStart(char *s);
    char *findNextLineStart(char *s, bool bufferFilled = false);
    char *findPreviousLineStart(char *s, bool bufferFilled = false);

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

    const char *getFileName() { return fileName.c_str(); }

    /**
     * Controls whether the file is checked for changes each time before physically accessing it.
     * See checkFileForChanges() for more details.
     */
    void setCheckFileForChanges(bool value) { enableCheckFileForChanges = value; }

    /**
     * Controls what happens when it is determined that new content has been appended to the file.
     * If append changes are not ignored an exception will be thrown.
     */
    void setIgnoreAppendChanges(bool value) { enableIgnoreAppendChanges = value; }

    /**
     * Returns true if the file is open, otherwise returns false.
     */
    bool isFileOpen() { return file; }

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
     * Returns the first line from the file, see getNextLineBufferPointer.
     */
    char *getFirstLineBufferPointer();

    /**
     * Returns the last line from the file, see getPreviousLineBufferPointer.
     */
    char *getLastLineBufferPointer();

    /**
     * Reads the next line from the file starting from the current position, and returns a pointer to its first character.
     * It returns nullptr after EOF. Incomplete last line gets ignored (returns nullptr)
     * as it is possible that the file is currently being written into. When new lines are appended to the file subsequent
     * calls will return non nullptr and continue reading lines from that on.
     * Moves the current position to the end of the line just returned.
     */
    char *getNextLineBufferPointer();

    /**
     * Reads the previous line from the file ending at the current position, and returns a pointer to its first character.
     * It returns nullptr when the beginning of file reached.
     * Moves the current position to the beginning of the line just returned.
     */
    char *getPreviousLineBufferPointer();

    /**
     * Searches through the file from the current position for the given text and returns the first matching line.
     */
    char *findNextLineBufferPointer(const char *search, bool caseSensitive = true);

    /**
     * Searches through the file from the current position for the given text and returns the first matching line.
     */
    char *findPreviousLineBufferPointer(const char *search, bool caseSensitive = true);

    /**
     * Returns the start offset of the line last parsed with readNextLine() or readPreviousLine().
     */
    file_offset_t getCurrentLineStartOffset() const { return currentLineStartOffset; }

    /**
     * Returns the end offset of the line last parsed with readNextLine() or readPreviousLine().
     * This points to the start offset of the next line, so it includes the CR/LF of the previous line.
     */
    file_offset_t getCurrentLineEndOffset() const { return currentLineEndOffset; }

    /**
     * Returns the length of the last line including CR/LF.
     */
    int getCurrentLineLength() const;

    /**
     * Returns the size of the file when last time checkFileForChanges() was called.
     */
    int64_t getFileSize();

    /**
     * Moves the current position to the given offset.
     */
    void seekTo(file_offset_t offset, unsigned int ensureBufferSizeAround = 0);

    /**
     * Returns the total number of lines requested so far.
     */
    int64_t getNumReadLines() const { return numReadLines; };

    /**
     * Returns the total number of bytes read in so far.
     */
    int64_t getNumReadBytes() const { return numReadBytes; }

    /**
     * Checks if the file has been changed. A file change is considered to be an append if it did not change the
     * content of the last line (starting at the very same file offset) and the file size has been increased.
     * Otherwise the change is considered to be an overwrite. If the file content is changed and the file size
     * remains the same, then it is not considered to be a change at all.
     */
    FileChangedState checkFileForChanges();

    /**
     * May or may not throw a FileChangedError depending on the current configuration.
     */
    void signalFileChanges(FileChangedState change);
};

/**
 * Special purpose exception to signal when the underlying file changes.
 */
class COMMON_API FileChangedError : public opp_runtime_error
{
  public:
    FileReader::FileChangedState change;

    /**
     * The error message can be generated in a printf-like manner.
     */
    FileChangedError(FileReader::FileChangedState change, const char *msg, ...);

    /**
     * Destructor with throw clause required by gcc.
     */
    virtual ~FileChangedError() throw() {}
};

} // namespace common
}  // namespace omnetpp


#endif

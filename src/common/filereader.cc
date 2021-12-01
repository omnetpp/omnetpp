//=========================================================================
//  FILEREADER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <sys/stat.h>
#include <sstream>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cinttypes>
#include <algorithm>
#include "omnetpp/platdep/platmisc.h"
#include "commonutil.h"
#include "filereader.h"
#include "exception.h"
#include "stringutil.h"

namespace omnetpp {
namespace common {

// #define TRACE_FILEREADER

std::string FileReader::staticBuffer;

FileChangedError::FileChangedError(FileReader::FileChange change, const char *messagefmt, ...) : opp_runtime_error(""), change(change)
{
    char buf[1024];
    VSNPRINTF(buf, 1024, messagefmt);
    errormsg = buf;
}

FileReader::FileReader(const char *fileName, size_t bufferSize)
   : fileName(fileName), bufferSize(bufferSize),
     bufferBegin(new char[bufferSize]),
     bufferEnd(bufferBegin + bufferSize),
     maxLineSize(bufferSize / 2),
     savedBufferSize(bufferSize / 1024),
     lastSavedBufferBegin(new char[savedBufferSize]),
//     lastSavedBufferEnd(lastSavedBufferBegin + savedBufferSize),
     newSavedBufferBegin(new char[savedBufferSize])
//     newSavedBufferEnd(newSavedBufferBegin + savedBufferSize)
{
#ifdef TRACE_FILEREADER
    TRACE_CALL("FileReader::FileReader(%s, %d)", fileName, bufferSize);
#endif
    file = nullptr;
    fileLock = nullptr;
    lastFileSize = -1;
    lastSavedSize = -1;
    bufferFileOffset = -1;
    enableCheckFileForChanges = true;
    enableFileLocking = false;
    fileAppendedAction = IGNORE;
    fileOverwrittenAction = SIGNAL;
    numReadLines = 0;
    numReadBytes = 0;
    dataBegin = nullptr;
    dataEnd = nullptr;
    currentDataPointer = nullptr;
    currentLineStartOffset = -1;
    currentLineEndOffset = -1;
}

FileReader::~FileReader()
{
#ifdef TRACE_FILEREADER
    TRACE_CALL("FileReader::~FileReader(%s)", fileName.c_str());
#endif
    delete[] bufferBegin;
    delete[] lastSavedBufferBegin;
    delete[] newSavedBufferBegin;
    ensureFileClosed();
}

void FileReader::ensureFileOpenInternal()
{
    if (!file) {
        // Note: 'b' mode turns off CR/LF translation and might be faster
        file = fopen(fileName.c_str(), "rb");
        if (!file)
            throw opp_runtime_error("Cannot open file '%s'", fileName.c_str());
        fileLock = new FileLock(file, fileName.c_str());
        if (bufferFileOffset == -1)
            seekTo(0);
    }
}

size_t FileReader::readFileEnd(file_offset_t fileSize, size_t size, const char *dataPointer)
{
    FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_SHARED, enableFileLocking);
    if (!file)
        throw opp_runtime_error("File is not open '%s'", fileName.c_str());
    opp_fseek(file, std::max((file_offset_t)0, (file_offset_t)(fileSize - size)), SEEK_SET);
    if (ferror(file))
        throw opp_runtime_error("Cannot seek in file '%s', error code %d", fileName.c_str(), ferror(file));
    int bytesRead = fread((char *)dataPointer, 1, std::min((int64_t)size, fileSize), file);
    if (ferror(file))
        throw opp_runtime_error("Read error in file '%s', error code %d", fileName.c_str(), ferror(file));
    return bytesRead;
}

void FileReader::ensureFileOpen()
{
    if (!file) {
        ensureFileOpenInternal();
        synchronize(OVERWRITTEN);
    }
}

void FileReader::ensureFileClosed()
{
    if (file) {
        fclose(file);
        file = nullptr;
    }
    if (fileLock) {
        delete fileLock;
        fileLock = nullptr;
    }
}

file_offset_t FileReader::pointerToFileOffset(char *dataPointer) const
{
    Assert(bufferFileOffset >= 0);
    Assert(bufferBegin <= dataPointer && dataPointer <= bufferEnd);
    file_offset_t fileOffset = dataPointer - bufferBegin + bufferFileOffset;
    Assert(fileOffset >= 0 && fileOffset <= lastFileSize);
    return fileOffset;
}

char *FileReader::fileOffsetToPointer(file_offset_t fileOffset) const
{
    Assert(bufferFileOffset >= 0);
    Assert(fileOffset >= 0 && fileOffset <= lastFileSize);
    return fileOffset - bufferFileOffset + (char *)bufferBegin;
}

void FileReader::checkConsistency(bool checkDataPointer) const
{
    bool ok = (size_t)(bufferEnd - bufferBegin) == bufferSize &&
      ((!dataBegin && !dataEnd) ||
       (dataBegin <= dataEnd && bufferBegin <= dataBegin && dataEnd <= bufferEnd &&
        (!checkDataPointer || (dataBegin <= currentDataPointer && currentDataPointer <= dataEnd))));
    if (!ok) {
//        TRACE("%d, %d, %ld, %ld, %ld ", bufferBegin, bufferEnd, dataBegin, dataEnd, currentDataPointer);
        throw opp_runtime_error("FileReader: internal error");
    }
}

FileReader::FileChange FileReader::getFileChange()
{
    int64_t newFileSize;
    time_t newLastModificationTime;
    getFileInformation(newFileSize, newLastModificationTime);
    if (newLastModificationTime == lastModificationTime && newFileSize == lastFileSize)
        return UNCHANGED;
    else if (newFileSize < lastFileSize)
        return OVERWRITTEN;
    else {
        FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_SHARED, enableFileLocking);
        readFileEnd(lastFileSize, savedBufferSize, newSavedBufferBegin);
        if (!memcmp(newSavedBufferBegin, lastSavedBufferBegin, lastSavedSize))
            return newFileSize == lastFileSize ? UNCHANGED : APPENDED;
        else
            return OVERWRITTEN;
    }
}

void FileReader::processFileChange(FileChange change)
{
    switch (change) {
        case UNCHANGED:
            break;
        case OVERWRITTEN:
            switch (fileOverwrittenAction) {
                case IGNORE:
                    break;
                case SIGNAL:
                    throw FileChangedError(change, "File changed: '%s' has been overwritten", fileName.c_str());
                case SYNCHRONIZE:
                    synchronize(change);
                    break;
                default:
                    throw opp_runtime_error("Unknown file change action: %d", change);
            }
            break;
        case APPENDED:
            switch (fileAppendedAction) {
                case IGNORE:
                    break;
                case SIGNAL:
                    throw FileChangedError(change, "File changed: '%s' has been appended", fileName.c_str());
                case SYNCHRONIZE:
                    synchronize(change);
                    break;
                default:
                    throw opp_runtime_error("Unknown file change action: %d", change);
            }
            break;
        default:
            throw opp_runtime_error("Unknown file change: %d", change);
    }
}

void FileReader::synchronize(FileChange change)
{
    FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_SHARED, enableFileLocking);
    getFileInformation(lastFileSize, lastModificationTime);
    lastSavedSize = readFileEnd(lastFileSize, savedBufferSize, lastSavedBufferBegin);
    dataBegin = nullptr;
    dataEnd = nullptr;
}

void FileReader::setCurrentDataPointer(char *dataPointer)
{
    currentDataPointer = dataPointer;
#ifndef NDEBUG
    checkConsistency();
#endif
}

void FileReader::fillBuffer(bool forward)
{
#ifndef NDEBUG
    checkConsistency();
#endif
#ifdef TRACE_FILEREADER
    TRACE_CALL("FileReader::fillBuffer %s", forward ? "forward" : "backward");
#endif

    char *dataPointer;
    int dataLength;

    if (!hasData()) {
        dataPointer = (char *)bufferBegin;
        dataLength = bufferSize;
    }
    else if (forward) {
        Assert(currentDataPointer);

        if (currentDataPointer < dataBegin) {
            dataPointer = currentDataPointer;
            dataLength = dataBegin - currentDataPointer;
        }
        else {
            dataPointer = dataEnd;
            dataLength = std::min((int64_t)(bufferEnd - dataEnd), getFileSize() - pointerToFileOffset(dataEnd));
        }
    }
    else {
        Assert(currentDataPointer);

        if (currentDataPointer > dataEnd) {
            dataPointer = dataEnd;
            dataLength = currentDataPointer - dataEnd;
        }
        else {
            dataPointer = (char *)bufferBegin;
            dataLength = dataBegin - bufferBegin;
        }
    }

    if (dataLength > 0) {
        FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_SHARED, enableFileLocking);

        // check for changes before reading the file
        if (enableCheckFileForChanges) {
            FileChange change = getFileChange();
            processFileChange(change);
            if (change != UNCHANGED) {
                dataPointer = (char *)bufferBegin;
                dataLength = bufferSize;
            }
        }

        file_offset_t fileOffset = pointerToFileOffset(dataPointer);
        if (!file)
            throw opp_runtime_error("File is not open '%s'", fileName.c_str());
        opp_fseek(file, fileOffset, SEEK_SET);
        if (ferror(file))
            throw opp_runtime_error("Cannot seek in file '%s', error code %d", fileName.c_str(), ferror(file));
        dataLength = std::min((int64_t)dataLength, lastFileSize - fileOffset);
        int bytesRead = fread(dataPointer, 1, dataLength, file);
        if (ferror(file))
            throw opp_runtime_error("Read error in file '%s', error code %d", fileName.c_str(), ferror(file));
        if (bytesRead != dataLength)
            throw opp_runtime_error("Cannot read %d bytes (got %d) from file '%s'", dataLength, bytesRead, fileName.c_str());

#ifdef TRACE_FILEREADER
        TPRINTF("FileReader::fillBuffer data at file offset: %" PRId64 ", length: %d", fileOffset, bytesRead);
#endif

        if (!hasData()) {
            dataBegin = dataPointer;
            dataEnd = dataPointer + bytesRead;
        }
        else if (forward) {
            if (currentDataPointer < dataBegin)
                dataBegin = currentDataPointer;
            else
                dataEnd = dataPointer + bytesRead;
        }
        else {
            if (currentDataPointer > dataEnd)
                dataEnd = currentDataPointer;
            else
                dataBegin = (char *)bufferBegin;
        }

        numReadBytes += bytesRead;
    }

#ifndef NDEBUG
    checkConsistency(true);
#endif
}

bool FileReader::isLineStart(char *s)
{
    Assert(bufferBegin <= s && s <= bufferEnd);
    if (s == bufferBegin) {
        // first line of file
        if (bufferFileOffset == 0)
            return true;
        else { // slow path
            FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_SHARED, enableFileLocking);
            file_offset_t fileOffset = pointerToFileOffset(s) - 1;
            if (!file)
                throw opp_runtime_error("File is not open '%s'", fileName.c_str());
            opp_fseek(file, fileOffset, SEEK_SET);
            if (ferror(file))
                throw opp_runtime_error("Cannot seek in file '%s', error code %d", fileName.c_str(), ferror(file));
            char previousChar;
            int bytesRead = fread(&previousChar, 1, 1, file);
            if (ferror(file))
                throw opp_runtime_error("Read error in file '%s', error code %d", fileName.c_str(), ferror(file));
            if (bytesRead != 1)
                throw opp_runtime_error("Cannot read 1 bytes (got %d) from file '%s'", bytesRead, fileName.c_str());
            return previousChar == '\n';
        }
    }
    else if (s - 1 < dataBegin)
        fillBuffer(false);
    else if (s - 1 >= dataEnd)
        fillBuffer(true);

    return *(s - 1) == '\n';
}

char *FileReader::findNextLineStart(char *start, bool bufferFilled)
{
#ifdef TRACE_FILEREADER
    TRACE_CALL("FileReader::findNextLineStart(start: %p)", start);
#endif

    char *s = start;

    // find next CR/LF (fast path)
    while (s < dataEnd && *s != '\r' && *s != '\n')
        s++;

    if (s < dataEnd && *s == '\r')
        s++;

    if (s < dataEnd && *s == '\n')
        s++;

    Assert(s <= dataEnd);
    if (s == dataEnd) {  // did we reach the end of the data in the buffer? (slow path)
        file_offset_t fileOffset = pointerToFileOffset(start);

#ifdef TRACE_FILEREADER
        TPRINTF("FileReader::findNextLineStart start file offset %" PRId64, fileOffset);
#endif

        if (s != start && isLineStart(s))  // line just ends at the end of data buffer
            ;  // void
        else if (fileOffset == getFileSize())  // searching from the end of the file
            return nullptr;
        else if (!bufferFilled) {  // refill buffer
            seekTo(fileOffset, maxLineSize);

#ifdef TRACE_FILEREADER
            TPRINTF("FileReader::findNextLineStart bufffer file offset before fillBuffer is called %" PRId64, bufferFileOffset);
#endif

            fillBuffer(true);
            s = findNextLineStart(fileOffsetToPointer(fileOffset), true);
        }
        else if (getDataEndFileOffset() == getFileSize()) // searching reached to the end of the file without CR/LF
            return nullptr;
        else // line too long
            throw opp_runtime_error("Line too long, should be below %zu in file '%s'", maxLineSize, fileName.c_str());
    }

#ifdef TRACE_FILEREADER
    TPRINTF("FileReader::findNextLineStart returns with next line file offset %" PRId64, pointerToFileOffset(s));
#endif

    return s;
}

char *FileReader::findPreviousLineStart(char *start, bool bufferFilled)
{
#ifdef TRACE_FILEREADER
    TRACE_CALL("FileReader::findPreviousLineStart(start: %p)", start);
#endif

    char *s = start - 1;

    if (s >= dataBegin && *s == '\n')
        s--;

    if (s >= dataBegin && *s == '\r')
        s--;

    // find previous CR/LF (fast path)
    while (s >= dataBegin && *s != '\r' && *s != '\n')
        s--;

    s++;

    Assert(s >= dataBegin);
    if (s == dataBegin) {  // did we reach the beginning of the data in the buffer? (slow path)
        file_offset_t fileOffset = pointerToFileOffset(start);

#ifdef TRACE_FILEREADER
        TPRINTF("FileReader::findPreviousLineStart start file offset %" PRId64, fileOffset);
#endif

        if (s != start && isLineStart(s))  // line starts at the beginning of the data buffer
            ;  // void
        else if (fileOffset == 0)  // searching from the beginning of the file
            return nullptr;
        else if (!bufferFilled) {  // refill buffer
            seekTo(fileOffset, maxLineSize);

#ifdef TRACE_FILEREADER
            TPRINTF("FileReader::findPreviousLineStart bufffer file offset before refill %" PRId64, bufferFileOffset);
#endif

            fillBuffer(false);
            s = findPreviousLineStart(fileOffsetToPointer(fileOffset), true);
        }
        else if (getDataBeginFileOffset() == 0)  // searching reached to the beginning of the file without CR/LF
            return dataBegin;
        else // line too long
            throw opp_runtime_error("Line too long, should be below %zu in file '%s'", maxLineSize, fileName.c_str());
    }

#ifdef TRACE_FILEREADER
    TPRINTF("FileReader::findPreviousLineStart: previous line file offset %" PRId64, pointerToFileOffset(s));
#endif

    return s;
}

char *FileReader::getNextLineBufferPointer()
{
    numReadLines++;
    ensureFileOpen();

    Assert(currentDataPointer);

#ifdef TRACE_FILEREADER
    TRACE_CALL("FileReader::Reading in next line at file offset: %" PRId64, pointerToFileOffset(currentDataPointer));
#endif

    // read forward if needed
    fillBuffer(true);

    // when starting in the middle of a line
    if (!isLineStart(currentDataPointer)) {
        char *nextLineDataPointer = findNextLineStart(currentDataPointer);

        if (nextLineDataPointer)
            setCurrentDataPointer(nextLineDataPointer);
        else {
            currentLineStartOffset = currentLineEndOffset = -1;

            return nullptr;
        }
    }

    // save offset to avoid problems related to recursive calls from find
    file_offset_t savedCurrentLineStartOffset = pointerToFileOffset(currentDataPointer);
    char *nextLineDataPointer = findNextLineStart(currentDataPointer);

    if (nextLineDataPointer) {
        setCurrentDataPointer(nextLineDataPointer);
        currentLineStartOffset = savedCurrentLineStartOffset;
        currentLineEndOffset = pointerToFileOffset(currentDataPointer);

#ifdef TRACE_FILEREADER
        TPRINTF("FileReader::getNextLineBufferPointer: currentLineStartOffset: %" PRId64 ", currentLineEndOffset: %" PRId64, currentLineStartOffset, currentLineEndOffset);
#endif

        Assert(currentLineEndOffset >= currentLineStartOffset);
        return fileOffsetToPointer(currentLineStartOffset);
    }
    else {
        currentLineStartOffset = currentLineEndOffset = -1;
        return nullptr;
    }
}

char *FileReader::getPreviousLineBufferPointer()
{
    numReadLines++;
    ensureFileOpen();

    Assert(currentDataPointer);

#ifdef TRACE_FILEREADER
    TRACE_CALL("FileReader::Reading in previous line at file offset: %" PRId64, pointerToFileOffset(currentDataPointer));
#endif

    // read backward if needed
    fillBuffer(false);

    // when starting in the middle of a line
    if (!isLineStart(currentDataPointer)) {
        char *previousLineDataPointer = findPreviousLineStart(currentDataPointer);

        if (previousLineDataPointer)
            setCurrentDataPointer(previousLineDataPointer);
        else {
            currentLineStartOffset = currentLineEndOffset = -1;

            return nullptr;
        }
    }

    // save offset to avoid problems related to recursive calls from find
    file_offset_t savedCurrentLineEndOffset = pointerToFileOffset(currentDataPointer);
    char *previousLineDataPointer = findPreviousLineStart(currentDataPointer);

    if (previousLineDataPointer) {
        setCurrentDataPointer(previousLineDataPointer);
        currentLineStartOffset = pointerToFileOffset(currentDataPointer);
        currentLineEndOffset = savedCurrentLineEndOffset;

#ifdef TRACE_FILEREADER
        TPRINTF("FileReader::getPreviousLineBufferPointer: currentLineStartOffset: %" PRId64 ", currentLineEndOffset: %" PRId64, currentLineStartOffset, currentLineEndOffset);
#endif

        Assert(currentLineEndOffset >= currentLineStartOffset);
        return fileOffsetToPointer(currentLineStartOffset);
    }
    else {
        currentLineStartOffset = currentLineEndOffset = -1;
        return nullptr;
    }
}

char *FileReader::getFirstLineBufferPointer()
{
    seekTo(0);
    return getNextLineBufferPointer();
}

char *FileReader::getLastLineBufferPointer()
{
    seekTo(getFileSize());
    return getPreviousLineBufferPointer();
}

char *FileReader::findNextLineBufferPointer(const char *search, bool caseSensitive)
{
    char *line;
    while ((line = getNextLineBufferPointer()) != nullptr)
        if (opp_strnistr(line, search, getCurrentLineLength(), caseSensitive))
            return line;

    return nullptr;
}

char *FileReader::findPreviousLineBufferPointer(const char *search, bool caseSensitive)
{
    char *line;
    while ((line = getPreviousLineBufferPointer()) != nullptr)
        if (opp_strnistr(line, search, getCurrentLineLength(), caseSensitive))
            return line;

    return nullptr;
}

size_t FileReader::getCurrentLineLength() const
{
    int result = currentLineEndOffset - currentLineStartOffset;
    Assert(result >= 0);
    return result;
}

int64_t FileReader::getFileSize()
{
    if (lastFileSize == -1) {
        ensureFileOpen();
        getFileInformation(lastFileSize, lastModificationTime);
    }
    return lastFileSize;
}

void FileReader::getFileInformation(int64_t& size, time_t& lastModificationTime)
{
    struct opp_stat_t s;
    if (!file)
        throw opp_runtime_error("File is not open '%s'", fileName.c_str());
    if (opp_fstat(fileno(file), &s) != 0)
        throw opp_runtime_error("Cannot stat file '%s'", fileName.c_str());
    lastModificationTime = s.st_mtime;
    size = s.st_size;
}

void FileReader::seekTo(file_offset_t fileOffset, size_t ensureBufferSizeAround)
{
#ifdef TRACE_FILEREADER
    TRACE_CALL("FileReader::seekTo seeking to file offset: %" PRId64, fileOffset);
#endif
#ifndef NDEBUG
    checkConsistency();
#endif

    if (fileOffset < 0 || fileOffset > getFileSize())
        throw opp_runtime_error("Invalid file offset: %" PRId64, fileOffset);

    ensureFileOpen();

    // check if requested offset is already in memory
    if (bufferFileOffset != -1 &&
        (file_offset_t)(bufferFileOffset + ensureBufferSizeAround) <= fileOffset &&
        fileOffset <= (file_offset_t)(bufferFileOffset + bufferSize - ensureBufferSizeAround))
    {
        setCurrentDataPointer(fileOffsetToPointer(fileOffset));
        Assert(currentDataPointer);
        return;
    }

    file_offset_t newBufferFileOffset = std::min(std::max((int64_t)0L, getFileSize() - (int64_t)bufferSize), std::max((int64_t)0L, fileOffset - (int64_t)bufferSize / 2));
    setCurrentDataPointer((char *)bufferBegin + fileOffset - newBufferFileOffset);
    Assert(currentDataPointer);

#ifdef TRACE_FILEREADER
    TPRINTF("FileReader::seekTo setting buffer file offset to: %" PRId64, newBufferFileOffset);
#endif

    // try to keep as much data as possible
    if (hasData()) {
        file_offset_t oldDataBeginFileOffset = getDataBeginFileOffset();
        file_offset_t oldDataEndFileOffset = getDataEndFileOffset();

#ifdef TRACE_FILEREADER
        TPRINTF("FileReader::seekTo data before seek: from file offset: %" PRId64 " to file offset: %" PRId64, oldDataBeginFileOffset, oldDataEndFileOffset);
#endif

        file_offset_t newBufferBeginFileOffset = newBufferFileOffset;
        file_offset_t newBufferEndFileOffset = newBufferFileOffset + bufferSize;
        file_offset_t moveSrcBeginFileOffset = std::min(newBufferEndFileOffset, std::max(newBufferBeginFileOffset, oldDataBeginFileOffset));
        file_offset_t moveSrcEndFileOffset = std::min(newBufferEndFileOffset, std::max(newBufferBeginFileOffset, oldDataEndFileOffset));
        char *moveSrc = fileOffsetToPointer(moveSrcBeginFileOffset);
        char *moveDest = moveSrcBeginFileOffset - newBufferBeginFileOffset + (char *)bufferBegin;
        int moveSize = moveSrcEndFileOffset - moveSrcBeginFileOffset;

        if (moveSize > 0 && moveSrc != moveDest) {
#ifdef TRACE_FILEREADER
            TPRINTF("FileReader::Keeping data from file offset: %" PRId64 " with length: %d", pointerToFileOffset(moveSrc), moveSize);
#endif
            memmove(moveDest, moveSrc, moveSize);
        }

        bufferFileOffset = newBufferFileOffset;
        dataBegin = moveDest;
        dataEnd = moveDest + moveSize;

#ifdef TRACE_FILEREADER
        TPRINTF("FileReader::Data after seek: from file offset: %" PRId64 " to file offset: %" PRId64, getDataBeginFileOffset(), getDataEndFileOffset());
#endif
    }
    else {
        bufferFileOffset = newBufferFileOffset;
        dataBegin = currentDataPointer;
        dataEnd = currentDataPointer;
    }

#ifndef NDEBUG
    checkConsistency();
#endif
}

/*
   Example code:

   #include <iostream>
   using namespace std;

   int main(int argc, char **argv)
   {
    if (argc<2)
        return 1;

    try {
        FileReader freader(argv[1],200);
        char *line;
        while ((line = freader.getNextLineBufferPointer()) != nullptr)
            cout << line << "\n";
    }
    catch (std::exception& e) {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
   }
 */
}  // namespace common
}  // namespace omnetpp


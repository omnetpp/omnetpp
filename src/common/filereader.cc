//=========================================================================
//  FILEREADER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <sstream>
#include <string.h>
#include <sys/stat.h>
#include "platmisc.h"
#include "commonutil.h"
#include "filereader.h"
#include "exception.h"

USING_NAMESPACE

#define LL  INT64_PRINTF_FORMAT

#define PRINT_DEBUG_MESSAGES false

FileReader::FileReader(const char *fileName, size_t bufferSize)
   : fileName(fileName), bufferSize(bufferSize),
     bufferBegin(new char[bufferSize]), bufferEnd(bufferBegin + bufferSize),
     maxLineSize(bufferSize / 2)
{
    f = NULL;
    checkFileChanged = true;
    synchronizeWhenAppended = true;

    numReadLines = 0;
    numReadBytes = 0;

    fileSize = -1;

    bufferFileOffset = -1;
    storedBufferFileOffset = -1;

    dataBegin = NULL;
    dataEnd = NULL;
    currentDataPointer = NULL;
    storedDataPointer = NULL;

    lastLineOffset = -1;
    lastLineLength = -1;

    currentLineStartOffset = -1;
    currentLineEndOffset = -1;
}

FileReader::~FileReader()
{
    delete [] bufferBegin;
    ensureFileClosed();
}

void FileReader::ensureFileOpenInternal()
{
    if (!f) {
        // Note: 'b' mode turns off CR/LF translation and might be faster
        f = fopen(fileName.c_str(), "rb");

        if (!f)
            throw opp_runtime_error("Cannot open file `%s'", fileName.c_str());

        if (bufferFileOffset == -1)
            seekTo(0);
    }
}

void FileReader::ensureFileOpen()
{
    if (!f) {
        ensureFileOpenInternal();
        synchronize();
    }
}

void FileReader::ensureFileClosed()
{
    if (f) {
        fclose(f);
        f = NULL;
    }
}

void FileReader::storePosition()
{
    Assert(storedBufferFileOffset == -1 && !storedDataPointer);
    storedBufferFileOffset = bufferFileOffset;
    storedDataPointer = currentDataPointer;
}

void FileReader::restorePosition()
{
    Assert(storedBufferFileOffset != -1 && storedDataPointer);
    bufferFileOffset = storedBufferFileOffset;
    setCurrentDataPointer(storedDataPointer);
    dataBegin = dataEnd = NULL;
    storedBufferFileOffset = -1;
    storedDataPointer = NULL;
}

file_offset_t FileReader::pointerToFileOffset(char *pointer) const
{
    file_offset_t fileOffset = pointer - bufferBegin + bufferFileOffset;
    Assert(fileOffset >= 0 && fileOffset <= fileSize);
    return fileOffset;
}

FileReader::FileChangedState FileReader::getFileChangedState()
{
    int64 newFileSize = getFileSizeInternal();

    if (newFileSize > fileSize) {
        // compare the stored last line with the one in the file
        storePosition();

        // avoid recursively checking the file for changes
        bool oldCheckFileChanged = checkFileChanged;
        checkFileChanged = false;
        char *currentLastLine = getLastLineBufferPointer();
        checkFileChanged = oldCheckFileChanged;

        int currentLineLength = getCurrentLineLength();
        restorePosition();

        if (currentLastLine && currentLineLength == lastLineLength && !strncmp(currentLastLine, lastLine.c_str(), lastLineLength))
            return APPENDED;
        else
            return OVERWRITTEN;
    }
    else if (newFileSize < fileSize)
        return OVERWRITTEN;
    else
        return UNCHANGED;
}

void FileReader::synchronize()
{
    ensureFileOpenInternal();
    int64 newFileSize = getFileSizeInternal();

    if (newFileSize != fileSize || lastLineOffset == -1) {
        fileSize = newFileSize;
        dataBegin = dataEnd = NULL;

        // read in the last line from the file
        storePosition();

        // avoid recursively checking the file for changes
        bool oldCheckFileChanged = checkFileChanged;
        checkFileChanged = false;
        char *line = getLastLineBufferPointer();
        checkFileChanged = oldCheckFileChanged;

        if (line) {
            lastLineLength = getCurrentLineLength();
            lastLineOffset = getCurrentLineStartOffset();
            lastLine.assign(line, lastLineLength);
        }
        else {
            lastLineLength = -1;
            lastLineOffset = -1;
            lastLine.clear();
        }

        restorePosition();
    }
}

void FileReader::checkConsistence(bool checkDataPointer) const
{
    bool ok = (size_t)(bufferEnd - bufferBegin) == bufferSize &&
      ((!dataBegin && !dataEnd) ||
       (dataBegin <= dataEnd && bufferBegin <= dataBegin && dataEnd <= bufferEnd &&
	(!checkDataPointer || (dataBegin <= currentDataPointer && currentDataPointer <= dataEnd))));

    if (!ok)
        throw opp_runtime_error("FileReader: internal error");
}

FileReader::FileChangedState FileReader::checkFileChangedAndSynchronize()
{
    FileReader::FileChangedState changed = getFileChangedState();

    switch (changed) {
        case OVERWRITTEN:
            throw opp_runtime_error("File changed: `%s' has been overwritten", fileName.c_str());
        case APPENDED:
            if (synchronizeWhenAppended)
                synchronize();
            else
                throw opp_runtime_error("File changed: `%s' has been appended", fileName.c_str());
        default:
           break;
    }

    return changed;
}

void FileReader::setCurrentDataPointer(char *pointer)
{
    currentDataPointer = pointer;

#ifndef NDEBUG
    checkConsistence();
#endif
}

void FileReader::fillBuffer(bool forward)
{
#ifndef NDEBUG
    checkConsistence();
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
            dataLength = std::min((int64)(bufferEnd - dataEnd), getFileSize() - pointerToFileOffset(dataEnd));
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
        // check for changes before reading the file
        if (checkFileChanged) {
            if (checkFileChangedAndSynchronize() != UNCHANGED) {
                fillBuffer(forward);
                return;
            }
        }

        file_offset_t fileOffset = pointerToFileOffset(dataPointer);
        opp_fseek(f, fileOffset, SEEK_SET);
        if (ferror(f))
            throw opp_runtime_error("Cannot seek in file `%s'", fileName.c_str());

        int bytesRead = fread(dataPointer, 1, dataLength, f);
        if (ferror(f))
            throw opp_runtime_error("Read error in file `%s'", fileName.c_str());

#ifndef NDEBUG
        if (PRINT_DEBUG_MESSAGES) printf("Reading data at file offset: %"LL"d, length: %d\n", fileOffset, bytesRead);
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
    checkConsistence(true);
#endif
}

bool FileReader::isLineStart(char *s) {
    Assert(bufferBegin <= s && s <= bufferEnd);

    if (s == bufferBegin) {
        // first line of file
        if (bufferFileOffset == 0)
            return true;
        else { // slow path
           file_offset_t fileOffset = pointerToFileOffset(s) - 1;

           opp_fseek(f, fileOffset, SEEK_SET);
           if (ferror(f))
               throw opp_runtime_error("Cannot seek in file `%s'", fileName.c_str());

           char previousChar;
           fread(&previousChar, 1, 1, f);

           if (ferror(f))
               throw opp_runtime_error("Read error in file `%s'", fileName.c_str());

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
    char *s = start;

    // find next CR/LF (fast path)
    while (s < dataEnd && *s != '\r' && *s!= '\n')
        s++;

    if (s < dataEnd && *s == '\r')
        s++;

    if (s < dataEnd && *s == '\n')
        s++;

    Assert(s <= dataEnd);
    if (s == dataEnd) // did we reach the end of the data in the buffer? (slow path)
    {
        file_offset_t fileOffset = pointerToFileOffset(start);

        if (s != start && isLineStart(s)) // line just ends at the end of data buffer
            return s;
        else if (fileOffset == getFileSize()) // searching from the end of the file
            return NULL;
        else if (!bufferFilled) { // refill buffer
            seekTo(fileOffset, maxLineSize);
            fillBuffer(true);
            s = fileOffsetToPointer(fileOffset);

            return findNextLineStart(s, true);
        }
        else if (getDataEndFileOffset() == getFileSize()) // searching reached to the end of the file without CR/LF
            return NULL;
        else // line too long
            throw opp_runtime_error("Line too long, should be below %d in file `%s'", maxLineSize, fileName.c_str());
    }

    return s;
}

char *FileReader::findPreviousLineStart(char *start, bool bufferFilled)
{
    char *s = start - 1;

    if (s >= dataBegin && *s == '\n')
        s--;

    if (s >= dataBegin && *s == '\r')
        s--;

    // find previous CR/LF (fast path)
    while (s >= dataBegin && *s != '\r' && *s!= '\n')
        s--;

    s++;

    Assert(s >= dataBegin);
    if (s == dataBegin) // did we reach the beginning of the data in the buffer? (slow path)
    {
        file_offset_t fileOffset = pointerToFileOffset(start);

        if (s != start && isLineStart(s)) // line starts at the beginning of the data buffer
            return s;
        else if (fileOffset == 0) // searching from the beginning of the file
            return NULL;
        else if (!bufferFilled) { // refill buffer
            seekTo(fileOffset, maxLineSize);
            fillBuffer(false);
            s = fileOffsetToPointer(fileOffset);

            return findPreviousLineStart(s, true);
        }
        else if (getDataBeginFileOffset() == 0) // searching reached to the beginning of the file without CR/LF
            return dataBegin;
        else // line too long
            throw opp_runtime_error("Line too long, should be below %d in file `%s'", maxLineSize, fileName.c_str());
    }

    return s;
}

char *FileReader::getNextLineBufferPointer()
{
    numReadLines++;
    ensureFileOpen();

    Assert(currentDataPointer);

#ifndef NDEBUG
    if (PRINT_DEBUG_MESSAGES) printf("Reading in next line at file offset: %"LL"d\n", pointerToFileOffset(currentDataPointer));
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

            return NULL;
        }
    }

    currentLineStartOffset = pointerToFileOffset(currentDataPointer);
    char *nextLineDataPointer = findNextLineStart(currentDataPointer);

    if (nextLineDataPointer) {
        setCurrentDataPointer(nextLineDataPointer);
        currentLineEndOffset = pointerToFileOffset(currentDataPointer);

        return fileOffsetToPointer(currentLineStartOffset);
    }
    else {
        currentLineStartOffset = currentLineEndOffset = -1;

        return NULL;
    }
}

char *FileReader::getPreviousLineBufferPointer()
{
    numReadLines++;
    ensureFileOpen();

    Assert(currentDataPointer);

#ifndef NDEBUG
    if (PRINT_DEBUG_MESSAGES) printf("Reading in previous line at file offset: %"LL"d\n", pointerToFileOffset(currentDataPointer));
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

            return NULL;
        }
    }

    currentLineEndOffset = pointerToFileOffset(currentDataPointer);
    char *previousLineDataPointer = findPreviousLineStart(currentDataPointer);

    if (previousLineDataPointer) {
        setCurrentDataPointer(previousLineDataPointer);
        currentLineStartOffset = pointerToFileOffset(currentDataPointer);

        return fileOffsetToPointer(currentLineStartOffset);
    }
    else {
        currentLineStartOffset = currentLineEndOffset = -1;

        return NULL;
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

const char *strnistr(const char *haystack, const char *needle, int n, bool caseSensitive)
{
    int needleLen = strlen(needle);
    if (n == 0)
        n = strlen(haystack);

    int slen = n - needleLen;

    for (const char *s = haystack; slen>0 && *s; s++, slen--)
        if (!(caseSensitive ? strncmp(s, needle, needleLen) : strncasecmp(s, needle, needleLen)))
            return s;
    return NULL;
}

char *FileReader::findNextLineBufferPointer(const char *search, bool caseSensitive)
{
    char *line;
    while ((line = getNextLineBufferPointer()) != NULL)
        if (strnistr(line, search, getCurrentLineLength(), caseSensitive))
            return line;

    return NULL;
}

char *FileReader::findPreviousLineBufferPointer(const char *search, bool caseSensitive)
{
    char *line;
    while ((line = getPreviousLineBufferPointer()) != NULL)
        if (strnistr(line, search, getCurrentLineLength(), caseSensitive))
            return line;

    return NULL;
}

int64 FileReader::getFileSize()
{
    if (fileSize == -1)
        fileSize = getFileSizeInternal();

    return fileSize;
}

int64 FileReader::getFileSizeInternal()
{
    ensureFileOpen();

    struct opp_stat_t s;
    opp_fstat(fileno(f), &s);
    return s.st_size;
}

void FileReader::seekTo(file_offset_t fileOffset, unsigned int ensureBufferSizeAround)
{
#ifndef NDEBUG
    if (PRINT_DEBUG_MESSAGES) printf("Seeking to file offset: %"LL"d\n", fileOffset);
    checkConsistence();
#endif

    if (fileOffset < 0 || fileOffset > getFileSize())
        throw opp_runtime_error("Invalid file offset: %"LL"d", fileOffset);

    ensureFileOpen();

    // check if requested offset is already in memory
    if (bufferFileOffset != -1 &&
        bufferFileOffset + ensureBufferSizeAround <= fileOffset &&
        fileOffset <= (file_offset_t)(bufferFileOffset + bufferSize - ensureBufferSizeAround))
    {
        setCurrentDataPointer(fileOffsetToPointer(fileOffset));
        Assert(currentDataPointer);
        return;
    }

    file_offset_t newBufferFileOffset = std::min(std::max((int64)0L, getFileSize() - (int64)bufferSize), std::max((int64)0L, fileOffset - (int64)bufferSize / 2));
    setCurrentDataPointer((char *)bufferBegin + fileOffset - newBufferFileOffset);
    Assert(currentDataPointer);

#ifndef NDEBUG
    if (PRINT_DEBUG_MESSAGES) printf("Setting buffer file offset to: %"LL"d\n", newBufferFileOffset);
#endif

    // try to keep as much data as possible
    if (hasData()) {
        file_offset_t oldDataBeginFileOffset = getDataBeginFileOffset();
        file_offset_t oldDataEndFileOffset = getDataEndFileOffset();

#ifndef NDEBUG
        if (PRINT_DEBUG_MESSAGES) printf("Data before: from file offset: %"LL"d to file offset: %"LL"d\n", oldDataBeginFileOffset, oldDataEndFileOffset);
#endif

        file_offset_t newBufferBeginFileOffset = newBufferFileOffset;
        file_offset_t newBufferEndFileOffset = newBufferFileOffset + bufferSize;
        file_offset_t moveSrcBeginFileOffset = std::min(newBufferEndFileOffset, std::max(newBufferBeginFileOffset, oldDataBeginFileOffset));
        file_offset_t moveSrcEndFileOffset = std::min(newBufferEndFileOffset, std::max(newBufferBeginFileOffset, oldDataEndFileOffset));
        char *moveSrc = fileOffsetToPointer(moveSrcBeginFileOffset);
        char *moveDest = moveSrcBeginFileOffset - newBufferBeginFileOffset + (char *)bufferBegin;
        int moveSize = moveSrcEndFileOffset - moveSrcBeginFileOffset;

        if (moveSize > 0 && moveSrc != moveDest) {
#ifndef NDEBUG
            if (PRINT_DEBUG_MESSAGES) printf("Keeping data from file offset: %"LL"d with length: %d\n", pointerToFileOffset(moveSrc), moveSize);
#endif

            fflush(stdout);

            memmove(moveDest, moveSrc, moveSize);
        }

        bufferFileOffset = newBufferFileOffset;
        dataBegin = moveDest;
        dataEnd = moveDest + moveSize;

#ifndef NDEBUG
        if (PRINT_DEBUG_MESSAGES) printf("Data after: from file offset: %"LL"d to file offset: %"LL"d\n", getDataBeginFileOffset(), getDataEndFileOffset());
#endif
    }
    else {
        bufferFileOffset = newBufferFileOffset;
        dataBegin = currentDataPointer;
        dataEnd = currentDataPointer;
    }

#ifndef NDEBUG
    checkConsistence();
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
        while ((line = freader.getNextLineBufferPointer()) != NULL)
            cout << line << "\n";
    }
    catch (std::exception& e) {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}
*/

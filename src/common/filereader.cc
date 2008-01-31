//=========================================================================
//  FILEREADER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

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

#define PRINT_DEBUG_MESSAGES false


FileReader::FileReader(const char *fileName, size_t bufferSize)
{
    this->fileName = fileName;
    this->bufferSize = bufferSize;

    f = NULL;
    synchronizeWhenAppended = true;

    bufferBegin = new char[bufferSize];
    bufferEnd = bufferBegin + bufferSize;

    maxLineSize = bufferSize / 2;

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

void FileReader::ensureFileOpen()
{
    if (!f) {
        // Note: 'b' mode turns off CR/LF translation and might be faster
        f = fopen(fileName.c_str(), "rb");

        if (!f)
            throw opp_runtime_error("Cannot open file `%s'", fileName.c_str());

        if (bufferFileOffset == -1)
            seekTo(0);

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
    currentDataPointer = storedDataPointer;
    dataBegin = dataEnd = NULL;
    storedBufferFileOffset = -1;
    storedDataPointer = NULL;
}

file_offset_t FileReader::pointerToFileOffset(char *pointer)
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
        char *currentLastLine = getLastLineBufferPointer(false);
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
    int64 newFileSize = getFileSizeInternal();

    if (newFileSize != fileSize || lastLineOffset == -1) {
        fileSize = newFileSize;
        dataBegin = dataEnd = NULL;

        // read in the last line from the file
        storePosition();
        char *line = getLastLineBufferPointer(false);

        if (line) {
            lastLineLength = getCurrentLineLength();
            lastLineOffset = getCurrentLineStartOffset();
            lastLine.assign(line, lastLineLength);
        }

        restorePosition();
    }
}

void FileReader::checkConsistence()
{
    bool ok = (size_t)(bufferEnd - bufferBegin) == bufferSize &&
      ((!dataBegin && !dataEnd) || 
       dataBegin <= dataEnd && bufferBegin <= dataBegin && dataEnd <= bufferEnd &&
       dataBegin <= currentDataPointer && currentDataPointer <= dataEnd);

    if (!ok)
        throw opp_runtime_error("FileReader: internal error");
}

void FileReader::checkFileChangedAndSynchronize()
{
    switch (getFileChangedState()) {
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
}

void FileReader::fillBuffer(bool forward)
{
    char *dataPointer;
    int dataLength;

    if (!hasData()) {
        dataPointer = bufferBegin;
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
            dataPointer = bufferBegin;
            dataLength = dataBegin - bufferBegin;
        }
    }

    if (dataLength > 0) {
        file_offset_t fileOffset = pointerToFileOffset(dataPointer);
        filereader_fseek(f, fileOffset, SEEK_SET);
        if (ferror(f))
            throw opp_runtime_error("Cannot seek in file `%s'", fileName.c_str());

        int bytesRead = fread(dataPointer, 1, dataLength, f);
        if (ferror(f))
            throw opp_runtime_error("Read error in file `%s'", fileName.c_str());

        if (PRINT_DEBUG_MESSAGES) printf("Reading data at file offset: %lld, length: %d\n", fileOffset, bytesRead);

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
                dataBegin = bufferBegin;
        }

        numReadBytes += bytesRead;
    }
}

bool FileReader::isLineStart(char *s) {
    Assert(bufferBegin <= s && s <= bufferEnd);

    if (s == bufferBegin) {
        // first line of file
        if (bufferFileOffset == 0)
            return true;
        else { // slow path
           file_offset_t fileOffset = pointerToFileOffset(s) - 1;

           filereader_fseek(f, fileOffset, SEEK_SET);
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
        else if (fileOffset == getFileSize()) // searching form the end of the file
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

char *FileReader::getNextLineBufferPointer(bool checkFileChanged)
{
    numReadLines++;
    ensureFileOpen();

    Assert(currentDataPointer);

    if (PRINT_DEBUG_MESSAGES) printf("Reading in next line at file offset: %lld\n", pointerToFileOffset(currentDataPointer));

    if (checkFileChanged)
        checkFileChangedAndSynchronize();

    fillBuffer(true);
#ifndef NDEBUG
    checkConsistence();
#endif

    if (!isLineStart(currentDataPointer)) {
        char *nextLineDataPointer = findNextLineStart(currentDataPointer);

        if (nextLineDataPointer)
            currentDataPointer = nextLineDataPointer;
        else {
            currentLineStartOffset = currentLineEndOffset = -1;

            return NULL;
        }
    }

    currentLineStartOffset = pointerToFileOffset(currentDataPointer);
    char *nextLineDataPointer = findNextLineStart(currentDataPointer);

    if (nextLineDataPointer) {
        currentDataPointer = nextLineDataPointer;
        currentLineEndOffset = pointerToFileOffset(currentDataPointer);

        return fileOffsetToPointer(currentLineStartOffset);
    }
    else {
        currentLineStartOffset = currentLineEndOffset = -1;

        return NULL;
    }
}

char *FileReader::getPreviousLineBufferPointer(bool checkFileChanged)
{
    numReadLines++;
    ensureFileOpen();

    Assert(currentDataPointer);

    if (PRINT_DEBUG_MESSAGES) printf("Reading in previous line at file offset: %lld\n", pointerToFileOffset(currentDataPointer));

    if (checkFileChanged)
        checkFileChangedAndSynchronize();

    fillBuffer(false);
#ifndef NDEBUG
    checkConsistence();
#endif

    if (!isLineStart(currentDataPointer)) {
        char *previousLineDataPointer = findPreviousLineStart(currentDataPointer);

        if (previousLineDataPointer)
            currentDataPointer = previousLineDataPointer;
        else {
            currentLineStartOffset = currentLineEndOffset = -1;

            return NULL;
        }
    }

    currentLineEndOffset = pointerToFileOffset(currentDataPointer);
    char *previousLineDataPointer = findPreviousLineStart(currentDataPointer);

    if (previousLineDataPointer) {
        currentDataPointer = previousLineDataPointer;
        currentLineStartOffset = pointerToFileOffset(currentDataPointer);

        return fileOffsetToPointer(currentLineStartOffset);
    }
    else {
        currentLineStartOffset = currentLineEndOffset = -1;

        return NULL;
    }
}

char *FileReader::getFirstLineBufferPointer(bool checkFileChanged)
{
    seekTo(0);
    return getNextLineBufferPointer(checkFileChanged);
}

char *FileReader::getLastLineBufferPointer(bool checkFileChanged)
{
    if (checkFileChanged)
        checkFileChangedAndSynchronize();

    seekTo(getFileSize());
    return getPreviousLineBufferPointer(checkFileChanged);
}

const char *strnistr(const char *haystack, const char *needle, int n)
{
    int needleLen = strlen(needle);
    if (n == 0)
        n = strlen(haystack);

    int slen = n - needleLen;

    for (const char *s = haystack; slen>0 && *s; s++, slen--)
        if (strncasecmp(s, needle, needleLen) == 0)
            return s;
    return NULL;
}

char *FileReader::findNextLineBufferPointer(const char *search)
{
    char *line;
    while ((line = getNextLineBufferPointer()) != NULL)
        if (strnistr(line, search, getCurrentLineLength()))
            return line;

    return NULL;
}

char *FileReader::findPreviousLineBufferPointer(const char *search)
{
    char *line;
    while ((line = getPreviousLineBufferPointer()) != NULL)
        if (strnistr(line, search, getCurrentLineLength()))
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

    struct filereader_stat s;
    filereader_fstat(fileno(f), &s);
    return s.st_size;
}

void FileReader::seekTo(file_offset_t fileOffset, unsigned int ensureBufferSizeAround)
{
    if (PRINT_DEBUG_MESSAGES) printf("Seeking to file offset: %lld\n", fileOffset);

    if (fileOffset < 0 || fileOffset > getFileSize())
        throw opp_runtime_error("Invalid file offset: %lld", fileOffset);

    ensureFileOpen();

    if (bufferFileOffset != -1 &&
        bufferFileOffset + ensureBufferSizeAround <= fileOffset &&
        fileOffset <= (file_offset_t)(bufferFileOffset + bufferSize - ensureBufferSizeAround))
    {
        currentDataPointer = fileOffsetToPointer(fileOffset);
        Assert(currentDataPointer);
        return;
    }

    file_offset_t newBufferFileOffset = std::min(std::max((int64)0L, getFileSize() - (int64)bufferSize), std::max((int64)0L, fileOffset - (int64)bufferSize / 2));
    currentDataPointer = bufferBegin + fileOffset - newBufferFileOffset;
    Assert(currentDataPointer);

    if (PRINT_DEBUG_MESSAGES) printf("Setting buffer file offset to: %lld\n", newBufferFileOffset);

    if (hasData()) {
        file_offset_t oldDataBeginFileOffset = getDataBeginFileOffset();
        file_offset_t oldDataEndFileOffset = getDataEndFileOffset();

        if (PRINT_DEBUG_MESSAGES) printf("Data before: from file offset: %lld to file offset: %lld\n", oldDataBeginFileOffset, oldDataEndFileOffset);

        file_offset_t newBufferBeginFileOffset = newBufferFileOffset;
        file_offset_t newBufferEndFileOffset = newBufferFileOffset + bufferSize;
        file_offset_t moveSrcBeginFileOffset = std::min(newBufferEndFileOffset, std::max(newBufferBeginFileOffset, oldDataBeginFileOffset));
        file_offset_t moveSrcEndFileOffset = std::min(newBufferEndFileOffset, std::max(newBufferBeginFileOffset, oldDataEndFileOffset));
        char *moveSrc = fileOffsetToPointer(moveSrcBeginFileOffset);
        char *moveDest = moveSrcBeginFileOffset - newBufferBeginFileOffset + bufferBegin;
        int moveSize = moveSrcEndFileOffset - moveSrcBeginFileOffset;

        if (moveSize > 0 && moveSrc != moveDest) {
            if (PRINT_DEBUG_MESSAGES) printf("Keeping data from file offset: %lld with length: %d\n", pointerToFileOffset(moveSrc), moveSize);
            fflush(stdout);

            memmove(moveDest, moveSrc, moveSize);
        }

        bufferFileOffset = newBufferFileOffset;
        dataBegin = moveDest;
        dataEnd = moveDest + moveSize;

        if (PRINT_DEBUG_MESSAGES) printf("Data after: from file offset: %lld to file offset: %lld\n", getDataBeginFileOffset(), getDataEndFileOffset());
    }
    else {
        bufferFileOffset = newBufferFileOffset;
        dataBegin = currentDataPointer;
        dataEnd = currentDataPointer;
    }
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

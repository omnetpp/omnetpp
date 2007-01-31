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
#include "commonutil.h"
#include "filereader.h"
#include "exception.h"

#define PRINT_DEBUG_MESSAGES false

FileReader::FileReader(const char *fileName, size_t bufferSize)
{
    this->fileName = fileName;
    this->bufferSize = bufferSize;

    f = NULL;
    fileSize = -1;

    bufferFileOffset = 0;
    bufferBegin = new char[bufferSize];
    bufferEnd = bufferBegin + bufferSize;

    maxLineSize = bufferSize / 2;
    dataBegin = NULL;
    dataEnd = NULL;
    currentDataPointer = NULL;

    lastLineStartOffset = -1;
    lastLineEndOffset = -1;

    numReadLines = 0;
    numReadBytes = 0;
}

FileReader::~FileReader()
{
    delete [] bufferBegin;
    closeFile();
}

void FileReader::openFile()
{
    // open file. Note: 'b' mode turns off CR/LF translation and might be faster
    f = fopen(fileName.c_str(), "rb");

    if (!f)
        throw opp_runtime_error("Cannot open file `%s'", fileName.c_str());

    seekTo(0);
}

void FileReader::closeFile()
{
    if (f)
        fclose(f);
}

void FileReader::checkConsistence()
{
    bool ok = bufferEnd - bufferBegin == bufferSize &&
              dataBegin <= dataEnd && dataBegin >= bufferBegin && dataEnd <= bufferEnd &&
              strlen(dataBegin) == dataEnd - dataBegin;
    if (!ok)
        throw opp_runtime_error("FileReader: internal error");
}

void FileReader::fillBuffer(bool forward)
{
    if (!f) openFile();

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
            dataLength = bufferEnd - dataEnd;
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

bool FileReader::isLineStart(char *&s) {
    file_offset_t fileOffset = pointerToFileOffset(s);

    if (fileOffset == 0)
        return true;

    if (dataBegin == s) {
        seekTo(fileOffset, 1);
        fillBuffer(false);
        s = fileOffsetToPointer(fileOffset);
    }

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

    // did we reach the end of the buffer? (slow path)
    if (s >= dataEnd)
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

    // did we reach the beginning of the buffer? (slow path)
    if (s < dataBegin)
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
        if (getDataBeginFileOffset() == 0) // searching reached to the beginning of the file without CR/LF
            return dataBegin;
        else // line too long
            throw opp_runtime_error("Line too long, should be below %d in file `%s'", maxLineSize, fileName.c_str());
    }

    return s + 1;
}

char *FileReader::getNextLineBufferPointer()
{
    numReadLines++;
    if (!f) openFile();
    Assert(currentDataPointer);
    if (PRINT_DEBUG_MESSAGES) printf("Reading in next line at file offset: %lld\n", pointerToFileOffset(currentDataPointer));

    fillBuffer(true);

    if (!isLineStart(currentDataPointer)) {
        char *nextLineDataPointer = findNextLineStart(currentDataPointer);

        if (nextLineDataPointer)
            currentDataPointer = nextLineDataPointer;
        else {
            lastLineStartOffset = lastLineEndOffset = -1;

            return NULL;
        }
    }

    lastLineStartOffset = pointerToFileOffset(currentDataPointer);
    char *nextLineDataPointer = findNextLineStart(currentDataPointer);

    if (nextLineDataPointer) {
        currentDataPointer = nextLineDataPointer;
        lastLineEndOffset = pointerToFileOffset(currentDataPointer);

        return fileOffsetToPointer(lastLineStartOffset);
    }
    else {
        lastLineStartOffset = lastLineEndOffset = -1;

        return NULL;
    }
}

char *FileReader::getPreviousLineBufferPointer()
{
    numReadLines++;
    if (!f) openFile();
    Assert(currentDataPointer);
    if (PRINT_DEBUG_MESSAGES) printf("Reading in previous line at file offset: %lld\n", pointerToFileOffset(currentDataPointer));

    fillBuffer(false);

    if (!isLineStart(currentDataPointer)) {
        char *previousLineDataPointer = findPreviousLineStart(currentDataPointer);

        if (previousLineDataPointer)
            currentDataPointer = previousLineDataPointer;
        else {
            lastLineStartOffset = lastLineEndOffset = -1;

            return NULL;
        }
    }

    lastLineEndOffset = pointerToFileOffset(currentDataPointer);
    char *previousLineDataPointer = findPreviousLineStart(currentDataPointer);

    if (previousLineDataPointer) {
        currentDataPointer = previousLineDataPointer;
        lastLineStartOffset = pointerToFileOffset(currentDataPointer);

        return fileOffsetToPointer(lastLineStartOffset);
    }
    else {
        lastLineStartOffset = lastLineEndOffset = -1;

        return NULL;
    }
}

int64 FileReader::getFileSize()
{
    if (fileSize == -1) {
        if (!f) openFile();

        file_offset_t tmp = filereader_ftell(f);
        filereader_fseek(f, 0, SEEK_END);
        fileSize = filereader_ftell(f);
        filereader_fseek(f, tmp, SEEK_SET);

        if (ferror(f))
            throw opp_runtime_error("Cannot seek in file `%s'", fileName.c_str());
    }

    return fileSize;
}

void FileReader::seekTo(file_offset_t fileOffset, int ensureBufferSizeAround)
{
    if (PRINT_DEBUG_MESSAGES) printf("Seeking to file offset: %lld\n", fileOffset);

    if (fileOffset < 0 || fileOffset > getFileSize())
        throw opp_runtime_error("Invalid file offset: %lld", fileOffset);

    if (!f) openFile();

    if (bufferFileOffset + ensureBufferSizeAround <= fileOffset &&
        fileOffset <= bufferFileOffset + bufferSize - ensureBufferSizeAround)
    {
        currentDataPointer = fileOffsetToPointer(fileOffset);
        Assert(currentDataPointer);
        return;
    }

    file_offset_t newBufferFileOffset = std::min(std::max((int64)0L, getFileSize() - (int64)bufferSize), std::max((int64)0L, fileOffset - (int64)bufferSize / 2));
    //file_offset_t fileOffsetDelta = newBufferFileOffset - bufferFileOffset;
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

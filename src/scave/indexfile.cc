//=========================================================================
//  INDEXFILE.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

#include <algorithm>
#include <sys/stat.h>
#include "exception.h"
#include "filereader.h"
#include "linetokenizer.h"
#include "stringutil.h"
#include "scaveutils.h"
#include "indexfile.h"

static bool serialLess(const Block &first, const Block &second)
{
    return first.endSerial() < second.endSerial();
}

Block* VectorData::getBlockForEntry(long serial) const
{
    if (serial < 0 || serial >= count())
        return NULL;

    Block blockToFind;
    blockToFind.startSerial = serial;
    Blocks::const_iterator first = std::upper_bound(blocks.begin(), blocks.end(), blockToFind, serialLess);
    assert(first == blocks.end() || first->endSerial() > serial); // first block ending after serial

    if (first != blocks.end()) {
        assert(first->contains(serial));
        return (Block*)&(*first);
    }
    else
        return NULL;
}


static bool simtimeLess(const Block &first, const Block &second)
{
    return first.endTime < second.startTime;
}

Blocks::size_type VectorData::getBlocksInSimtimeInterval(double startTime, double endTime, Blocks::size_type &startIndex, Blocks::size_type &endIndex) const
{
    Block blockToFind;
    blockToFind.startTime = startTime;
    blockToFind.endTime = endTime;

    Blocks::const_iterator first = std::lower_bound(blocks.begin(), blocks.end(), blockToFind, simtimeLess);
    Blocks::const_iterator last = std::upper_bound(blocks.begin(), blocks.end(), blockToFind, simtimeLess);

    assert(first == blocks.end() || first->endTime >= startTime);
    assert(last == blocks.end() || last->startTime > endTime); 
    assert(first <= last);

    startIndex = first - blocks.begin();
    endIndex = last - blocks.begin();
    return endIndex - startIndex;
}

static bool eventnumLess(const Block &first, const Block &second)
{
    return first.endEventNum < second.startEventNum;
}

Blocks::size_type VectorData::getBlocksInEventnumInterval(long startEventNum, long endEventNum, Blocks::size_type &startIndex, Blocks::size_type &endIndex) const
{
    Block blockToFind;
    blockToFind.startEventNum = startEventNum;
    blockToFind.endEventNum = endEventNum;

    Blocks::const_iterator first = std::lower_bound(blocks.begin(), blocks.end(), blockToFind, eventnumLess);
    Blocks::const_iterator last = std::upper_bound(blocks.begin(), blocks.end(), blockToFind, eventnumLess);
    
    assert(first == blocks.end() || first->endEventNum >= startEventNum);
    assert(last == blocks.end() || last->startEventNum > endEventNum); 
    assert(first <= last);

    startIndex = first - blocks.begin();
    endIndex = last - blocks.begin();
    return endIndex - startIndex;
}

//=========================================================================

VectorData *VectorFileIndex::getVector(int vectorId)
{
    for (Vectors::iterator it=vectors.begin(); it != vectors.end(); ++it)
    {
        if (it->vectorId == vectorId)
            return &(*it);
    }
    return NULL;
}

//=========================================================================
static bool isFileReadable(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (f != NULL)
        fclose(f);
    return f != NULL;
}

bool IndexFile::isIndexFile(const char *filename)
{
    int len = strlen(filename);
    return (len >= 4) && (strcmp(filename+len-4, ".vci") == 0);
}

bool IndexFile::isVectorFile(const char *filename)
{
    // XXX check contents?
    int len = strlen(filename);
    return (len >= 4) && (strcmp(filename+len-4, ".vec") == 0);
}

std::string IndexFile::getVectorFileName(const char *filename)
{
    std::string vectorFileName(filename);
    std::string::size_type pos = vectorFileName.rfind('.');
    if (pos != std::string::npos)
        vectorFileName.replace(vectorFileName.begin()+pos, vectorFileName.end(), ".vec");
    else
        vectorFileName.append(".vec");
    return vectorFileName;
}

std::string IndexFile::getIndexFileName(const char *filename)
{
    std::string indexFileName(filename);
    std::string::size_type pos = indexFileName.rfind('.');
    if (pos != std::string::npos)
        indexFileName.replace(indexFileName.begin()+pos, indexFileName.end(), ".vci");
    else
        indexFileName.append(".vci");
    return indexFileName;
}

bool IndexFile::isIndexFileUpToDate(const char *filename)
{
    std::string indexFileName, vectorFileName;

    if (isIndexFile(filename))
    {
        indexFileName = std::string(filename);
        vectorFileName = getVectorFileName(filename);
    }
    else
    {
        indexFileName = getIndexFileName(filename);
        vectorFileName = std::string(filename);
    }

    if (!isFileReadable(indexFileName.c_str()))
        return false;

    IndexFileReader reader(indexFileName.c_str());
    VectorFileIndex *index = reader.readFingerprint();

    struct stat s;
    bool uptodate = false;
    if (stat(vectorFileName.c_str(), &s) == 0)
    {
        uptodate = (s.st_mtime == index->vectorFileLastModified) && (s.st_size == index->vectorFileSize);
    }

    delete index;
    return uptodate;
}

//=========================================================================

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(cond,msg,line) if (!(cond)) throw opp_runtime_error("Invalid index file syntax: %s, file %s, line %d", msg, filename.c_str(), line);

// see ifilemgr.h
#define MIN_BUFFER_SIZE 512

IndexFileReader::IndexFileReader(const char *filename)
    : filename(filename)
{
}

VectorFileIndex *IndexFileReader::readAll()
{
    // read vector and blocks
    FileReader reader(filename.c_str());
    LineTokenizer tokenizer(1024);
    int numTokens;
    char *line, **tokens;

    VectorFileIndex *index = new VectorFileIndex();
    long numOfEntries = 0;
    while ((line=reader.getNextLineBufferPointer())!=NULL)
    {
        int lineNum = reader.getNumReadLines();
        int len=reader.getLastLineLength();
        numTokens=tokenizer.tokenize(line, len);
        tokens=tokenizer.tokens();
        parseLine(tokens, numTokens, index, numOfEntries, lineNum);
    }
    return index;
}

VectorFileIndex *IndexFileReader::readFingerprint()
{
    FileReader reader(filename.c_str());
    LineTokenizer tokenizer(1024);
    int numTokens;
    char *line, **tokens;

    VectorFileIndex *index = NULL;
    while ((line=reader.getNextLineBufferPointer())!=NULL)
    {
        int lineNum = reader.getNumReadLines();
        int len = reader.getLastLineLength();
        numTokens = tokenizer.tokenize(line,len);
        tokens = tokenizer.tokens();

        if (numTokens == 0 || tokens[0][0] == '#')
            continue;
        else if (tokens[0][0] == 'f' && strcmp(tokens[0], "file") == 0)
        {
            long dummy=0;
            index = new VectorFileIndex();
            parseLine(tokens, numTokens, index, dummy, lineNum);
        }
        else
            break;
    }

    CHECK(index, "missing fingerprint", -1);
    return index;
}


void IndexFileReader::parseLine(char **tokens, int numTokens, VectorFileIndex *index, long& numOfEntries, int lineNum)
{
    if (numTokens == 0 || tokens[0][0] == '#')
        return;

    char *endPtr;
    long count;
    double min;
    double max;
    double sum;
    double sumSqr;

    if (tokens[0][0] == 'v' && strcmp(tokens[0], "vector") == 0)
    {
        CHECK(numTokens >= 11, "invalid vector declaration", lineNum);

        VectorData vector;
        CHECK(parseInt(tokens[1], vector.vectorId), "invalid vector id", lineNum);
        vector.moduleName = tokens[2];
        vector.name = tokens[3];
        vector.columns = tokens[4];
        CHECK(parseLong(tokens[5], vector.blockSize) && parseLong(tokens[6], count) &&
              parseDouble(tokens[7], min) && parseDouble(tokens[8], max) &&
              parseDouble(tokens[9], sum) && parseDouble(tokens[10], sumSqr),
              "invalid vector declaration", lineNum);
        vector.stat = Statistics(count, min, max, sum, sumSqr);

        index->vectors.push_back(vector);
        numOfEntries = 0;
    }
    else if (tokens[0][0] == 'f' && strcmp(tokens[0], "file") == 0)
    {
        long fileSize, lastModified;
        CHECK(numTokens >= 3, "missing file attributes", lineNum);
        CHECK(parseLong(tokens[1], fileSize), "file size is not a number", lineNum);
        CHECK(parseLong(tokens[2], lastModified), "modification date is not a number", lineNum);
        index->vectorFileSize = fileSize;
        index->vectorFileLastModified = lastModified;
    }
    else
    {
        CHECK(index->vectors.size() > 0, "missing vector definition", lineNum);
        CHECK(numTokens >= 9, "missing fields from block", lineNum);

        VectorData &vector = index->vectors.back();
        int id;
        CHECK(parseInt(tokens[0], id) && id==vector.vectorId, "unexpected vector id", lineNum);

        Block block;
        int i = 1; // column index
        block.startSerial = numOfEntries;
        CHECK(parseLong(tokens[i++], block.startOffset), "invalid file offset", lineNum);
        if (vector.hasColumn('E')) 
        {
            CHECK(parseLong(tokens[i++], block.startEventNum) && parseLong(tokens[i++], block.endEventNum),
                "invalid event numbers", lineNum);
        }
        if (vector.hasColumn('T'))
        {
            CHECK(parseDouble(tokens[i++], block.startTime) && parseDouble(tokens[i++], block.endTime),
                "invalid simulation time", lineNum);
        }
        if (vector.hasColumn('V'))
        {
            CHECK(parseLong(tokens[i++], count) && parseDouble(tokens[i++], min) && parseDouble(tokens[i++], max) &&
                    parseDouble(tokens[i++], sum) && parseDouble(tokens[i++], sumSqr), "invalid statistics data", lineNum);
            block.stat = Statistics(count, min, max, sum, sumSqr);
        }

        vector.blocks.push_back(block);
        numOfEntries += block.count();
    }
}

//=========================================================================

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(fprintf)    if (fprintf<0) throw new opp_runtime_error("Cannot write output file `%s'", filename.c_str())

IndexFileWriter::IndexFileWriter(const char *filename, int precision)
    : filename(filename), precision(precision), file(NULL)
{
}

IndexFileWriter::~IndexFileWriter()
{
    if (file != NULL)
    {
        closeFile();
    }
}

void IndexFileWriter::writeAll(const VectorFileIndex& index)
{
    openFile();
    writeFingerprint(index.vectorFileName);

    for (Vectors::const_iterator vectorRef = index.vectors.begin(); vectorRef != index.vectors.end(); ++vectorRef)
    {
        writeVector(*vectorRef);
    }

    closeFile();
}

void IndexFileWriter::writeFingerprint(std::string vectorFileName)
{
    if (file == NULL)
        openFile();

    struct stat s;
    if (stat(vectorFileName.c_str(), &s) != 0)
        throw opp_runtime_error("vector file %s does not exist", vectorFileName.c_str());

    long saveOffset = ftell(file);
    fseek(file, 0, SEEK_SET);
    CHECK(fprintf(file, "file %ld %ld", (long)s.st_size, (long)s.st_mtime));
    fseek(file,saveOffset, SEEK_SET);
}

void IndexFileWriter::writeVector(const VectorData& vector)
{
    if (file == NULL)
        openFile();

    int numBlocks = vector.blocks.size();
    if (numBlocks > 0)
    {
        writeVectorDeclaration(vector);

        for (int i=0; i<numBlocks; i++)
        {
            writeBlock(vector, vector.blocks[i]);
        }
    }
}

void IndexFileWriter::writeVectorDeclaration(const VectorData& vector)
{
    CHECK(fprintf(file, "vector %d  %s  %s  %s  %ld  %ld  %.*g  %.*g  %.*g  %.*g\n",
          vector.vectorId, QUOTE(vector.moduleName.c_str()), QUOTE(vector.name.c_str()), vector.columns.c_str(),
          vector.blockSize, vector.count(), precision, vector.min(), precision, vector.max(),
          precision, vector.sum(), precision, vector.sumSqr()));

}

void IndexFileWriter::writeBlock(const VectorData &vector, const Block& block)
{
    if (block.count() > 0)
    {
        CHECK(fprintf(file, "%d\t%ld", vector.vectorId, block.startOffset));
        if (vector.hasColumn('E')) { CHECK(fprintf(file, " %ld %ld", block.startEventNum, block.endEventNum)); }
        if (vector.hasColumn('T')) { CHECK(fprintf(file, " %.*g %.*g", precision, block.startTime, precision, block.endTime)); }
        if (vector.hasColumn('V')) { CHECK(fprintf(file, " %ld %.*g %.*g %.*g %.*g",
                                                block.count(), precision, block.min(), precision, block.max(),
                                                precision, block.sum(), precision, block.sumSqr())); }
        CHECK(fprintf(file, "\n"));
    }
}

void IndexFileWriter::openFile()
{
    file = fopen(filename.c_str(), "w");
    if (file == NULL)
        throw opp_runtime_error("Cannot open index file: %s", filename.c_str());

    // space for header
    CHECK(fprintf(file, "%64s\n", ""));
}

void IndexFileWriter::closeFile()
{
    if (file != NULL)
    {
        fclose(file);
        file = NULL;
    }
}


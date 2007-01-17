//=========================================================================
//  INDEXFILE.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

#include <sys/stat.h>
#include "exception.h"
#include "filereader.h"
#include "linetokenizer.h"
#include "stringutil.h"
#include "indexfile.h"

Block* VectorData::getBlockForEntry(long serial)
{
    int low = 0;
    int high = blocks.size()-1;

    while (low<=high)
    {
        int mid = (low+high)/2;
        long midStart = blocks[mid].startSerial;
        long midEnd = blocks[mid].endSerial;

        if (midEnd <= serial)
            low = mid+1;
        else if (midStart > serial)
            high = mid-1;
        else
            return &blocks[mid];
    }

    return NULL;
}

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
    VectorFileIndex *index = reader.readHeader();

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
    while ((line=reader.readNextLine())!=NULL)
    {
        int lineNum = reader.getNumReadLines();
        int len=reader.getLastLineLength();
        numTokens=tokenizer.tokenize(line, len);
        tokens=tokenizer.tokens();

        parseLine(tokens, numTokens, index, numOfEntries, lineNum);
    }
    return index;
}

VectorFileIndex *IndexFileReader::readHeader()
{
    FileReader reader(filename.c_str());
    LineTokenizer tokenizer(1024);
    int numTokens;
    char *line, **tokens;

    VectorFileIndex *index = NULL;
    while ((line=reader.readNextLine())!=NULL)
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

    if (index == NULL)
        throw opp_runtime_error("invalid index file syntax: missing header, file %s", filename.c_str());

    return index;
}


void IndexFileReader::parseLine(char **tokens, int numTokens, VectorFileIndex *index, long &numOfEntries, int lineNum)
{
    if (numTokens == 0 || tokens[0][0] == '#')
        return;

    char *endPtr;

    if (tokens[0][0] == 'v' && strcmp(tokens[0], "vector") == 0)
    {
        if (numTokens < 11)
            throw opp_runtime_error("invalid index file syntax: invalid vector definition, line %d", lineNum);

        VectorData vector;

        char *e;
        vector.vectorId = (int) strtol(tokens[1],&e,10);
        if (*e)
            throw opp_runtime_error("invalid index file syntax: invalid vector id in vector definition, line %d", lineNum);

        vector.moduleName = tokens[2];
        vector.name = tokens[3];
        vector.blockSize = strtol(tokens[5],&e,10);
        vector.count = strtol(tokens[6],&e,10);
        vector.min = strtod(tokens[7],&e);
        vector.max = strtod(tokens[8],&e);
        vector.sum = strtod(tokens[9],&e);
        vector.sumSqr = strtod(tokens[10],&e);

        index->vectors.push_back(vector);
        numOfEntries = 0;
    }
    else if (tokens[0][0] == 'f' && strcmp(tokens[0], "file") == 0)
    {
        if (numTokens < 3)
            throw opp_runtime_error("invalid index file syntax: missing file attributes, file %s, line %d", filename.c_str(), lineNum);

        long fileSize = strtol(tokens[1], &endPtr, 10);
        if (*endPtr)
            throw opp_runtime_error("invalid index file syntax: file size is not a number, file %s, line %d", filename.c_str(), lineNum);
            
        long lastModified = strtol(tokens[2], &endPtr, 10);
        if (*endPtr)
            throw opp_runtime_error("invalid index file syntax: modification date is not a number, file %s, line %d", filename.c_str(), lineNum);

        index->vectorFileSize = fileSize;
        index->vectorFileLastModified = lastModified;
    }
    else
    {
        if (index->vectors.size() == 0)
            throw opp_runtime_error("invalid index file syntax: missing vector definition, line %d", lineNum);

        VectorData &vector = index->vectors.back();
        char *e;
        int id = (int)strtol(tokens[0],&e,10);
        if (*e || id!=vector.vectorId)
            throw opp_runtime_error("invalid index file syntax: expected vector id %d in line %d, but found: %s",
                vector.vectorId, lineNum, tokens[0]);

        long offset, count;
        for (int i=1; i<numTokens; ++i)
        {
            if (sscanf(tokens[i], "%ld:%ld", &offset, &count)==2)
            {
                vector.blocks.push_back(Block(numOfEntries, numOfEntries+count, offset));
                numOfEntries+=count;
            }
            else
                throw opp_runtime_error("invalid index file syntax: invalid block definition '', line %d",
                    tokens[i], lineNum);
        }
    }
}

//=========================================================================
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

void IndexFileWriter::writeAll(VectorFileIndex &index)
{
    openFile();
    writeFingerprint(index.vectorFileName);

    for (Vectors::iterator vectorRef = index.vectors.begin(); vectorRef != index.vectors.end(); ++vectorRef)
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
        throw opp_runtime_error("vector file %s does not exists", vectorFileName.c_str());

    long saveOffset = ftell(file);
    fseek(file, 0, SEEK_SET);
    fprintf(file, "file %ld %ld", (long)s.st_size, (long)s.st_mtime);
    fseek(file,saveOffset, SEEK_SET);
}

void IndexFileWriter::writeVector(VectorData &vector)
{
    if (file == NULL)
        openFile();

    int numBlocks = vector.blocks.size();
    if (numBlocks > 0)
    {
        writeVectorDeclaration(vector);

        for (int i=0; i<numBlocks; i+=10)
        {
            fprintf(file, "%ld\t", vector.vectorId);
            for (int j = 0; j<10 && i+j < numBlocks; ++j)
            {
                writeBlock(vector.blocks[i+j]);
            }
            fprintf(file, "\n");
        }
    }
}

void IndexFileWriter::writeVectorDeclaration(VectorData &vector)
{
    fprintf(file, "vector %d  %s  %s  %d  %ld  %ld  %.*g  %.*g  %.*g  %.*g\n",
        vector.vectorId, QUOTE(vector.moduleName.c_str()), QUOTE(vector.name.c_str()), 1/*tuple*/,
        vector.blockSize, vector.count, precision, vector.min, precision, vector.max,
        precision, vector.sum, precision, vector.sumSqr);

}

void IndexFileWriter::writeBlock(Block &block)
{
    fprintf(file, "%ld:%ld ", block.startOffset, block.numOfEntries());
}

void IndexFileWriter::openFile()
{
    file = fopen(filename.c_str(), "w");
    if (file == NULL)
        throw opp_runtime_error("");

    // space for header
    fprintf(file, "%64s\n", "");
}

void IndexFileWriter::closeFile()
{
    if (file != NULL)
    {
        fclose(file);
        file = NULL;
    }
}


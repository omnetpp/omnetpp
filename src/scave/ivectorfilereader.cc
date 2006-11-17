//=========================================================================
//  IVECTORFILEREADER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

// TODO: setting correct buffer sizes
// TODO: vectors with multiple values
// TODO: code duplication with vectorfilereader.cc
// TODO: minimize memory allocation

#include "exception.h"
#include "linetokenizer.h"
#include "ivectorfilereader.h"

IndexedVectorFileReader::IndexedVectorFileReader(const char *filename, int vectorId)
    : reader(NULL), vectorId(vectorId), numOfEntries(0), currentBlock(NULL)
{
    int len = strlen(filename);
    fname=new char[len];
    strcpy(fname, filename);

    ifname = new char[len+4];
    strcpy(ifname, filename);
    char *extension = strrchr(ifname, '.');
    if (!extension) extension = ifname+len;
    strcpy(extension, ".vci");

    loadIndex();
}

IndexedVectorFileReader::~IndexedVectorFileReader()
{
    if (reader!=NULL)
        delete reader;
    delete fname;
    delete ifname;
}

//=========================================================================
void IndexedVectorFileReader::loadIndex()
{
    // read vector and blocks
    FileReader reader(ifname);
    int numTokens;
    char *line, **tokens;
    LineTokenizer tokenizer(64*1024);
    int id;
    long offset, count;

    numOfEntries = 0;
    while ((line=reader.readNextLine())!=NULL)
    {
        numTokens = tokenizer.tokenize(line, reader.getLastLineLength());
        tokens = tokenizer.tokens();

        if (numTokens >= 1)
        {
            if (sscanf(tokens[0], "%d",&id)==1 && id==vectorId)
            {
                for (int i=1; i<numTokens; ++i)
                {
                    if (sscanf(tokens[i], "%ld:%ld", &offset, &count)==2) 
                    {
                        blocks.push_back(Block(numOfEntries, numOfEntries+count, offset));
                        numOfEntries+=count;
                    }
                }
                return;
            }
        }
    }
}

Block* IndexedVectorFileReader::getBlockForEntry(long serial)
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

static double zero =0;

static bool parseDouble(char *s, double& dest)
{
    char *e;
    dest = strtod(s,&e);
    if (!*e)
    {
        return true;
    }
    if (strstr(s,"INF") || strstr(s, "inf"))
    {
        dest = 1/zero;  // +INF or -INF
        if (*s=='-') dest = -dest;
        return true;
    }
    return false;
}

void IndexedVectorFileReader::loadBlock(Block &block)
{
    if (reader==NULL)
        reader=new FileReader(fname);
    
    long count=block.endSerial-block.startSerial;
    reader->seekTo(block.startOffset);
    block.entries=new OutputVectorEntry[count];
    
    char *line, **tokens, *end;
    int numTokens;
    LineTokenizer tokenizer;
    int id;
    double t, val;

    for (int i=0; i<count; ++i) 
    {
        if ((line=reader->readNextLine())==NULL)
            throw new Exception("Unexpected end of file in '%s'", fname);
        int len = reader->getLastLineLength();

        tokenizer.tokenize(line, len);
        tokens=tokenizer.tokens();
        numTokens = tokenizer.numTokens();
        if (numTokens < 3)
            throw new Exception("Line to short: %.*s", len, line);

        id = (int)strtol(tokens[0], &end, 10);
        if (*end || id!=vectorId)
            throw new Exception("Missing or unexpected vector id: %.*s", len, line);

        if (!parseDouble(tokens[1], t) || !parseDouble(tokens[2], val))
            throw new Exception("Malformed line: %.*s", len, line);

        block.entries[i] = OutputVectorEntry(block.startSerial+i, t, val);
    }
}

OutputVectorEntry *IndexedVectorFileReader::getEntryBySerial(long serial)
{
    if (serial<0 || serial>=numOfEntries)
        return NULL;
    
    if (currentBlock == NULL || !currentBlock->contains(serial)) 
    {
        if (currentBlock != NULL)
            currentBlock->deleteEntries();
        currentBlock = getBlockForEntry(serial);
        loadBlock(*currentBlock);
    }

    return currentBlock->getEntryBySerial(serial);
}

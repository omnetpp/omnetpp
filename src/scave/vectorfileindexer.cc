//=========================================================================
//  VECTORFILEINDEXER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <sys/stat.h>
#include <errno.h>
#include <sstream>
#include <ostream>
#include "opp_ctype.h"
#include "platmisc.h"
#include "stringutil.h"
#include "scaveutils.h"
#include "scaveexception.h"
#include "filereader.h"
#include "linetokenizer.h"
#include "indexfile.h"
#include "vectorfileindexer.h"

static inline bool existsFile(const std::string fileName)
{
    struct stat s;
    return stat(fileName.c_str(), &s)==0;
}

static std::string createTempFileName(const std::string baseFileName)
{
    std::string prefix = baseFileName;
    prefix.append(".temp");
    std::string tmpFileName = prefix;
    int serial = 0;

    while (existsFile(tmpFileName))
        tmpFileName = opp_stringf("%s%d", prefix.c_str(), serial++);
    return tmpFileName;
}

// TODO: adjacent blocks are merged
void VectorFileIndexer::generateIndex(const char *vectorFileName, IProgressMonitor *monitor)
{
    FileReader reader(vectorFileName);
    LineTokenizer tokenizer(1024);
    VectorFileIndex index;
    index.vectorFileName = vectorFileName;

    char *line;
    char **tokens;
    int numTokens, lineNo, numOfUnrecognizedLines = 0;
    VectorData *currentVectorRef = NULL;
    VectorData *lastVectorDecl = NULL;
    Block currentBlock;
    
    int64 onePercentFileSize = reader.getFileSize() / 100;
    int readPercentage = 0;
    
    if (monitor)
    	monitor->beginTask(std::string("Indexing ")+vectorFileName, 110);
    
    try
    {

	    while ((line=reader.getNextLineBufferPointer())!=NULL)
	    {
	    	if (monitor)
	    	{
	    		if (monitor->isCanceled())
	    		{
		    		monitor->done();
		    		return;
	    		}
	    		if (onePercentFileSize > 0)
	    		{
		    		int64 readBytes = reader.getNumReadBytes();
		    		int currentPercentage = readBytes / onePercentFileSize;
		    		if (currentPercentage > readPercentage)
		    		{
		    			monitor->worked(currentPercentage - readPercentage);
		    			readPercentage = currentPercentage;
		    		}
	    		}
	    	}
	    	
	        tokenizer.tokenize(line, reader.getCurrentLineLength());
	        numTokens = tokenizer.numTokens();
	        tokens = tokenizer.tokens();
	        lineNo = reader.getNumReadLines();
	
	        if (numTokens == 0 || tokens[0][0] == '#')
	            continue;
	        else if (tokens[0][0] == 'r' && strcmp(tokens[0], "run") == 0 ||
	                 tokens[0][0] == 'p' && strcmp(tokens[0], "param") == 0)
	        {
	            index.run.parseLine(tokens, numTokens, vectorFileName, lineNo);
	        }
	        else if (tokens[0][0] == 'a' && strcmp(tokens[0], "attr") == 0)
	        {
	            if (lastVectorDecl == NULL) // run attribute
	            {
	                index.run.parseLine(tokens, numTokens, vectorFileName, lineNo);
	            }
	            else // vector attribute
	            {
	                if (numTokens < 3)
	                    throw ResultFileFormatException("vector file indexer: missing attribute name or value", vectorFileName, lineNo);
	                lastVectorDecl->attributes[tokens[1]] = tokens[2];
	            }
	        }
	        else if (tokens[0][0] == 'v' && strcmp(tokens[0], "vector") == 0)
	        {
	            if (numTokens < 4)
	                throw ResultFileFormatException("vector file indexer: broken vector declaration", vectorFileName, lineNo);
	
	            VectorData vector;
	            if (!parseInt(tokens[1], vector.vectorId))
	                throw ResultFileFormatException("vector file indexer: malformed vector in vector declaration", vectorFileName, lineNo);
	            vector.moduleName = tokens[2];
	            vector.name = tokens[3];
	            vector.columns = (numTokens < 5 || opp_isdigit(tokens[4][0]) ? "TV" : tokens[4]);
	            vector.blockSize = 0;
	
	            index.addVector(vector);
	            lastVectorDecl = index.getVectorAt(index.getNumberOfVectors() - 1);
	            currentVectorRef = NULL;
	        }
	        else // data line
	        {
	            int vectorId;
	            simultime_t simTime;
	            double value;
	            long eventNum = -1;
	
	            if (!parseInt(tokens[0], vectorId))
	            {
	                numOfUnrecognizedLines++;
	                continue;
	            }
	
	            if (currentVectorRef == NULL || vectorId != currentVectorRef->vectorId)
	            {
	                if (currentVectorRef != NULL)
	                {
	                    currentBlock.size = (long)(reader.getCurrentLineStartOffset() - currentBlock.startOffset);
	                    if (currentBlock.size > currentVectorRef->blockSize)
	                        currentVectorRef->blockSize = currentBlock.size;
	                    currentVectorRef->addBlock(currentBlock);
	                }
	
	                currentBlock = Block();
	                currentBlock.startOffset = reader.getCurrentLineStartOffset();
	                currentVectorRef = index.getVectorById(vectorId);
	                if (currentVectorRef == NULL)
	                    throw ResultFileFormatException("vector file indexer: missing vector declaration", vectorFileName, lineNo);
	            }
	
	            for (int i = 0; i < (int)currentVectorRef->columns.size(); ++i)
	            {
	                char column = currentVectorRef->columns[i];
	                if (i+1 >= numTokens)
	                    throw ResultFileFormatException("vector file indexer: data line too short", vectorFileName, lineNo);
	
	                char *token = tokens[i+1];
	                switch (column)
	                {
	                case 'T':
	                    if (!parseSimtime(token, simTime))
	                        throw ResultFileFormatException("vector file indexer: malformed simulation time", vectorFileName, lineNo);
	                    break;
	                case 'V':
	                    if (!parseDouble(token, value))
	                        throw ResultFileFormatException("vector file indexer: malformed data value", vectorFileName, lineNo);
	                    break;
	                case 'E':
	                    if (!parseLong(token, eventNum))
	                        throw ResultFileFormatException("vector file indexer: malformed event number", vectorFileName, lineNo);
	                    break;
	                }
	            }
	
	            currentBlock.collect(eventNum, simTime, value);
	        }
	    }
	
	    // finish last block
	    if (currentBlock.count() > 0)
	    {
	        assert(currentVectorRef != NULL);
	        currentBlock.size = (long)(reader.getFileSize() - currentBlock.startOffset);
	        if (currentBlock.size > currentVectorRef->blockSize)
	            currentVectorRef->blockSize = currentBlock.size;
	        currentVectorRef->addBlock(currentBlock);
	    }
	
	    if (numOfUnrecognizedLines > 0)
	    {
	        fprintf(stderr, "Found %d unrecognized lines in %s.\n", numOfUnrecognizedLines, vectorFileName);
	    }
    }
    catch (std::exception&)
    {
    	if (monitor)
    		monitor->done();
    	throw;
    }
    
    
    if (monitor)
    {
    	if (monitor->isCanceled())
    	{
			monitor->done();
			return;
    	}
    	if (readPercentage < 100)
    		monitor->worked(100 - readPercentage);
    }

	// generate index file: first write it to a temp file then rename it to .vci;
    // we do this in order to prevent race conditions from other processes/threads
    // reading an incomplete .vci file
    std::string indexFileName = IndexFile::getIndexFileName(vectorFileName);
    std::string tempIndexFileName = createTempFileName(indexFileName);

    try
    {
        IndexFileWriter writer(tempIndexFileName.c_str());
        writer.writeAll(index);
        
        if (monitor)
        	monitor->worked(10);

        // rename generated index file
        if (unlink(indexFileName.c_str())!=0 && errno!=ENOENT)
            throw opp_runtime_error("Cannot remove original index file `%s': %s", indexFileName.c_str(), strerror(errno));
        if (rename(tempIndexFileName.c_str(), indexFileName.c_str())!=0)
            throw opp_runtime_error("Cannot rename index file from '%s' to '%s': %s", tempIndexFileName.c_str(), indexFileName.c_str(), strerror(errno));
    }
    catch (std::exception&)
    {
    	if (monitor)
    		monitor->done();
    	
        // if something wrong happened, we remove the temp files
        unlink(indexFileName.c_str());
        unlink(tempIndexFileName.c_str());
        throw;
    }
    
    if (monitor)
    	monitor->done();
}


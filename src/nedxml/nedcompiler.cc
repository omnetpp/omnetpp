//==========================================================================
// nedcompiler.cc -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contents:
//   class NEDCompiler
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "nederror.h"
#include "nedcompiler.h"
#include "nedparser.h"
#include "neddtdvalidator.h"
#include "nedbasicvalidator.h"
#include "nedsemanticvalidator.h"


// luckily, "/" satisfies windows too :)
#define PATHSEPARATOR "/"

static bool debug=false;


NEDFileCache::NEDFileCache()
{
}

NEDFileCache::~NEDFileCache()
{
    for (NEDMap::iterator i = importedfiles.begin(); i!=importedfiles.end(); ++i)
        delete (*i).second;
}

void NEDFileCache::addFile(const char *name, NEDElement *node)
{
    importedfiles[name] = node;
}

NEDElement *NEDFileCache::getFile(const char *name)
{
    // find name in hash table
    NEDMap::iterator i = importedfiles.find(name);
    return i==importedfiles.end() ? NULL : (*i).second;
}

//---------------

NEDSymbolTable::NEDSymbolTable()
{
}

NEDSymbolTable::~NEDSymbolTable()
{
}

NEDElement *NEDSymbolTable::getChannelDeclaration(const char *name)
{
    // find name in hash table
    NEDMap::iterator i = channels.find(name);
    return i==channels.end() ? NULL : (*i).second;
}

NEDElement *NEDSymbolTable::getModuleDeclaration(const char *name)
{
    // find name in hash table
    NEDMap::iterator i = modules.find(name);
    return i==modules.end() ? NULL : (*i).second;
}

NEDElement *NEDSymbolTable::getNetworkDeclaration(const char *name)
{
    // find name in hash table
    NEDMap::iterator i = networks.find(name);
    return i==networks.end() ? NULL : (*i).second;
}

NEDElement *NEDSymbolTable::getEnumDeclaration(const char *name)
{
    // find name in hash table
    NEDMap::iterator i = enums.find(name);
    return i==enums.end() ? NULL : (*i).second;
}

NEDElement *NEDSymbolTable::getClassDeclaration(const char *name)
{
    // find name in hash table
    NEDMap::iterator i = classes.find(name);
    return i==classes.end() ? NULL : (*i).second;
}

void NEDSymbolTable::add(NEDElement *node)
{
    int tag = node->getTagCode();
    if (tag==NED_CHANNEL)
    {
        // add to channels table
        ChannelNode *chan = (ChannelNode *)node;
        channels[chan->getName()] = chan;
    }
    else if (tag==NED_SIMPLE_MODULE || tag==NED_COMPOUND_MODULE)
    {
        // add to modules table
        modules[node->getAttribute("name")] = node;
    }
    else if (tag==NED_NETWORK)
    {
        // add to networks table
        NetworkNode *net = (NetworkNode *)node;
        networks[net->getName()] = net;
    }
    else if (tag==NED_ENUM)
    {
        // add to enums table
        EnumNode *e = (EnumNode *)node;
        enums[e->getName()] = e;
    }
    else if (tag==NED_STRUCT || tag==NED_CLASS || tag==NED_MESSAGE)
    {
        // add to classes table
        classes[node->getAttribute("name")] = node;
    }
    else
    {
        // collect stuff recursively from inside and add to hash tables
        for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
            add(child);
    }
}

//------------------

void NEDClassicImportResolver::addImportPath(const char *dir)
{
    importpath.push_back(std::string(dir));
}

NEDElement *NEDClassicImportResolver::loadImport(const char *import)
{
    // FIXME: relative paths should be relative to the importing (parent) file
    std::string fpath;
    std::string fname = import;

    // add ".ned" suffix if it doesn't have one
    if (strlen(import)<4 || strcmp(import+strlen(import)-4, ".ned"))
        fname += ".ned";

    // try to find import file: first in local dir
    FILE *f;
    if (debug) fprintf(stderr,"dbg: trying to open %s\n",fname.c_str());
    if ((f=fopen(fname.c_str(),"r"))!=NULL)
    {
        fclose(f);
        fpath = fname;
    }
    else
    {
        // then with import prefixes
        bool found=false;
        for (NEDStringVector::iterator i=importpath.begin(); !found && i!=importpath.end(); ++i)
        {
            fpath = (*i) + PATHSEPARATOR + fname;
            if (debug) fprintf(stderr,"dbg: trying to open %s\n",fpath.c_str());
            if ((f=fopen(fpath.c_str(),"r"))!=NULL)
            {
                fclose(f);
                found=true;
            }
        }
        if (!found)
            return NULL;
    }

    // load and parse it
    NEDParser parser;
    parser.setParseExpressions(true);
    parser.parseFile(fpath.c_str());
    NEDElement *tree = parser.getTree();

    return tree;
}

//-----------

NEDCompiler::NEDCompiler(NEDFileCache *fcache, NEDSymbolTable *symbtab, NEDImportResolver *importres)
{
    filecache = fcache;
    symboltable = symbtab;
    importresolver = importres;
}

NEDCompiler::~NEDCompiler()
{
}

void NEDCompiler::addImport(const char *name)
{
    imports[name] = NULL;
}

bool NEDCompiler::isImported(const char *name)
{
    return imports.find(name)!=imports.end();
}

void NEDCompiler::validate(NEDElement *tree)
{
    // could add to cache if fname was known?
    // filecache->addFile(fname, importedtree);
    doValidate(tree);
}

void NEDCompiler::doValidate(NEDElement *tree)
{
    // DTD validation and additional basic validation
    NEDDTDValidator dtdvalidator;
    dtdvalidator.validate(tree);
    if (errorsOccurred()) return;

    NEDBasicValidator basicvalidator(true);
    basicvalidator.validate(tree);
    if (errorsOccurred()) return;

    // import what's necessary and do semantic checks meanwhile
    for (NEDElement *node=tree->getFirstChild(); node; node=node->getNextSibling())
    {
        if (node->getTagCode()==NED_IMPORT)
        {
            // import external declarations
            for (ImportedFileNode *import=((ImportNode *)node)->getFirstImportedFileChild();
                 import;
                 import=import->getNextImportedFileNodeSibling())
            {
                const char *fname = import->getFilename();

                // if already imported, nothing to do
                if (!isImported(fname))
                {
                    // register we've seen this import before
                    addImport(fname);

                    // try to find file in cache
                    NEDElement *importedtree = filecache->getFile(fname);

                    // not in cache, try to load via importresolver
                    if (!importedtree)
                    {
                        if (debug) fprintf(stderr,"dbg: importing %s\n",fname);
                        importedtree = importresolver->loadImport(fname);

                        // if couldn't load, skip
                        if (!importedtree)
                        {
                           NEDError(import, "could not import '%s'",fname);
                           continue;
                        }

                        // add to file cache
                        filecache->addFile(fname, importedtree);
                    }

                    // recursive processing: add stuff in it to symbol table, further processing imports...
                    if (debug) fprintf(stderr,"dbg: processing import %s\n",fname);
                    doValidate(importedtree);
                    if (debug) fprintf(stderr,"dbg: leaving import %s\n",fname);
                }
            }
        }
        else
        {
            // semantic validation for this top-level element
            NEDSemanticValidator validator(true,symboltable);
            validator.validate(node);
            // no return on errorsOccurred() -- keep on until end of this file

            // add to symbol table
            symboltable->add(node);
            if (debug) fprintf(stderr,"dbg: %s %s added to symbol table\n",node->getTagName(), node->getAttribute("name"));
        }
    }
}


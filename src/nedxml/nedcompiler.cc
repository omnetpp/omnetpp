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
#include "nedsyntaxvalidator.h"
#include "nedsemanticvalidator.h"
#include "stringutil.h"

USING_NAMESPACE


// luckily, "/" satisfies windows too :)
#define PATHSEPARATOR "/"

static bool debug=false;


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
    if (!opp_stringendswith(import, ".ned"))
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
NEDErrorStore *errors = new NEDErrorStore(); //FIXME should take NECCompiler's instead!!!!!
    NEDParser parser(errors);
    parser.setParseExpressions(true);
    NEDElement *tree = parser.parseNEDFile(fpath.c_str());
    return tree;
}

//-----------

NEDCompiler::NEDCompiler(NEDResourceCache *cache, NEDImportResolver *importres, NEDErrorStore *e)
{
    nedcache = cache;
    importresolver = importres;
    errors = e;
}

NEDCompiler::~NEDCompiler()
{
}

void NEDCompiler::addImport(const char *name)
{
//XXX    imports[name] = NULL;
}

bool NEDCompiler::isImported(const char *name)
{
//XXX    return imports.find(name)!=imports.end();
return true;
}

void NEDCompiler::validate(NEDElement *tree)
{
    // could add to cache if fname was known?
    // filecache->addFile(fname, importedtree);
    doValidate(tree);
}

void NEDCompiler::doValidate(NEDElement *tree)
{
//FIXME this logic is broken!!!!

    // DTD validation and additional syntax validation
    NEDDTDValidator dtdvalidator(errors);
    dtdvalidator.validate(tree);
    if (errors->containsError())
        return;

    NEDSyntaxValidator syntaxvalidator(true, errors);
    syntaxvalidator.validate(tree);
    if (errors->containsError()) return;

    // import what's necessary and do semantic checks meanwhile
    for (NEDElement *node=tree->getFirstChild(); node; node=node->getNextSibling())
    {
        if (node->getTagCode()==NED_IMPORT)
        {
            ImportNode *import = (ImportNode *)node;
            const char *fname = import->getImportSpec();

            // if already imported, nothing to do
            if (!isImported(fname))
            {
                // register we've seen this import before
                addImport(fname);

                // try to find file in cache
                NEDElement *importedtree = nedcache->getFile(fname);

                // not in cache, try to load via importresolver
                if (!importedtree)
                {
                    if (debug) fprintf(stderr,"dbg: importing %s\n",fname);
                    importedtree = importresolver->loadImport(fname);

                    // if couldn't load, skip
                    if (!importedtree)
                    {
                       errors->addError(import, "could not import '%s'",fname);
                       continue;
                    }

                    // add to file cache
                    nedcache->addFile(fname, importedtree);
                }

                // recursive processing: add stuff in it to symbol table, further processing imports...
                if (debug) fprintf(stderr,"dbg: processing import %s\n",fname);
                doValidate(importedtree);
                if (debug) fprintf(stderr,"dbg: leaving import %s\n",fname);
            }
        }
/*FIXME
        else
        {
            // semantic validation for this top-level element
            NEDSemanticValidator validator(true, nedcache, errors);
            validator.validate(node);
            // no return on errors -- keep on until end of this file

            // add to symbol table
            nedcache->addFile(node);
            if (debug) fprintf(stderr,"dbg: %s %s added to symbol table\n",node->getTagName(), node->getAttribute("name"));
        }
*/
    }
}


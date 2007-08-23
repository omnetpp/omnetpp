//==========================================================================
// nedcompiler.h -
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


#ifndef __NEDCOMPILER_H
#define __NEDCOMPILER_H

#include <map>
#include <vector>
#include <string>
#include "nedelements.h"
#include "nedresourcecache.h"


/**
 * Abstract interface for loading NED imports; to be used with NEDCompiler.
 *
 * @ingroup NEDCompiler
 */
class NEDXML_API NEDImportResolver
{
  public:
    /** Constructor */
    NEDImportResolver() {}

    /** Destructor */
    virtual ~NEDImportResolver() {}

    /**
     * Load the given import (e.g. NED file) and return it
     */
    virtual NEDElement *loadImport(const char *import) = 0;
};


/**
 * Importresolver for NED-I; to be used with NEDCompiler.
 * A NEDClassicImportResolver instance can be reused when compiling several
 * independent files in a row.
 *
 * @ingroup NEDCompiler
 */
class NEDXML_API NEDClassicImportResolver : public NEDImportResolver
{
  protected:
    typedef std::vector<std::string> NEDStringVector;
    NEDStringVector importpath;

  public:
    /** Constructor */
    NEDClassicImportResolver() {}

    /** Destructor */
    virtual ~NEDClassicImportResolver() {}

    /**
     * Add an import directory to the import path.
     */
    void addImportPath(const char *dir);

    /**
     * Load the given NED file, located in the import path somewhere.
     */
    virtual NEDElement *loadImport(const char *import);
};


/**
 * Manages the "middle" part of the compilation process for NED.
 * Implemented in validate(), the process involves:
 *   -# DTD and syntactic validation of the input (see NEDDTDValidator
 *      and NEDSyntaxValidator),
 *   -# loading of imports via the import resolver (see NEDImportResolver),
 *      adding them into the file cached (see NEDFileCache) and recursively
 *      validating them
 *   -# semantic validation of the input, e.g. making sure all referenced
 *      module types exist (see NEDSemanticValidator)
 *
 * NEDCompiler does NOT do parsing (only for the imports, via the import
 * resolver, but the main file it already receives a NED object tree form).
 * Code (i.e. C++) generation is also not covered.
 *
 * @ingroup NEDCompiler
 */
class NEDXML_API NEDCompiler
{
  protected:
//XXX    NEDMap imports;  // list of already imported modules (to avoid double importing)

    NEDResourceCache *nedcache;
    NEDImportResolver *importresolver;
    NEDErrorStore *errors;

    void addImport(const char *name);
    bool isImported(const char *name);
    void doValidate(NEDElement *tree);

  public:
    /** Constructor */
    NEDCompiler(NEDResourceCache *nedcache, NEDImportResolver *importres, NEDErrorStore *e);

    /** Destructor */
    virtual ~NEDCompiler();

    /**
     * Performs the import resolution and validation process described in the
     * class documentation.
     */
    void validate(NEDElement *tree);
};

#endif


//==========================================================================
//  NEDTOOL.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contains: main() for nedtool
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

#include "nedparser.h"
#include "nederror.h"
#include "nedxmlparser.h"
#include "neddtdvalidator.h"
#include "nedbasicvalidator.h"
#include "nedsemanticvalidator.h"
#include "nedgenerator.h"
#include "xmlgenerator.h"
#include "cppgenerator.h"
#include "nedcompiler.h"
#include "platdep/misc.h"
#include "platdep/fileutil.h"  // splitFileName


using std::ofstream;
using std::ifstream;
using std::ios;


// file types
enum {XML, NED, CPP, NOTHING};

// option variables
bool opt_gencpp = false;           // -c
bool opt_genxml = false;           // -x
bool opt_genned = false;           // -n
bool opt_validateonly = false;     // -v
int opt_nextfiletype = NOTHING;    // -X
bool opt_newsyntax = false;        // -N
const char *opt_suffix = NULL;     // -s
const char *opt_hdrsuffix = NULL;  // -S
bool opt_unparsedexpr = false;     // -e
bool opt_storesrc = false;         // -t
bool opt_novalidation = false;     // -y
bool opt_noimports = false;        // -z
bool opt_srcloc = false;           // -p
bool opt_mergeoutput = false;      // -m
bool opt_verbose = false;          // -V
const char *opt_outputfile = NULL; // -o
bool opt_here = false;             // -h

// global variables
NEDFileCache filecache;
NEDClassicImportResolver importresolver;

NedFilesNode *outputtree;


void printUsage()
{
    fprintf(stderr,
       "nedtool -- part of OMNeT++/OMNEST, (C) 2002-2005 Andras Varga\n"
       "Syntax: nedtool [options] <file1> <file2> ...\n"
       "    or: nedtool [options] @<filelist-file>\n"
       "  -c: generate C++ (default)\n"
       "  -x: generate XML (you may need -y, -e and -p as well)\n"
       "  -n: generate NED (you may need -y and -e as well)\n"
       "      HINT: NED-to-NED conversion performs pretty-printing.\n"
       "  -v: no output (only validate input)\n"
       "  -m: output is a single file (out_n.* by default)\n"
       "  -o <filename>: with -m: output file name\n"
       "  -h  place output file into current directory\n"
       "  -I <dir>: add directory to NED include path\n"
       "  -X xml/ned/off: following files are XML/NED up to '-X off'\n"
       "  -N: with -n: use new NED syntax (experimental)\n"
       "  -s <suffix>: suffix for generated files\n"
       "  -S <suffix>: when generating C++, suffix for generated header files\n"
       "  -e: do not parse expressions in NED input; expect unparsed expressions in XML\n"
       "  -y: skip semantic validation (implies -z, skip processing imports)\n"
       "  -z: skip processing imports\n"
       "  -t: with NED parsing: include source code of components in XML\n"
       "  -p: with -x: add source location info (src-loc attributes) to XML output\n"
       "  -V: verbose\n"
       "NOTE: C++ code generation from .msg files and the new NED-2 syntax are still\n"
       "experimental and should not be used in production environment. Message (.msg)\n"
       "should be processed with opp_msgc.\n"
    );
}

void createFileNameWithSuffix(char *outfname, const char *infname, const char *suffix)
{
    if (opt_here)
    {
        // remove directory part
        const char *s = infname+strlen(infname);
        while (s>infname && *s!='/' && *s!='\\') s--;
        if (*s=='/' || *s=='\\') s++;
        strcpy(outfname, s);
    }
    else
    {
        strcpy(outfname, infname);
    }

    // replace extension with suffix.
    char *s = outfname+strlen(outfname);
    while (s>outfname && *s!='/' && *s!='\\' && *s!='.') s--;
    if (*s!='.') s=outfname+strlen(outfname);
    strcpy(s,suffix);
}

bool processFile(const char *fname)
{
    if (opt_verbose) fprintf(stderr,"processing '%s'...\n",fname);

    // determine file type
    int ftype = opt_nextfiletype;
    if (ftype==NOTHING)
    {
        if (!strcmp(".ned", fname+strlen(fname)-4))
            ftype=NED;
        if (!strcmp(".xml", fname+strlen(fname)-4))
            ftype=XML;
        if (ftype==NOTHING)
            ftype=NED;
    }

    // process input tree
    NEDElement *tree = 0;
    clearErrors();
    if (ftype==XML)
    {
        tree = parseXML(fname);
    }
    else if (ftype==NED)
    {
        NEDParser parser;
        parser.setParseExpressions(!opt_unparsedexpr);
        parser.setStoreSource(opt_storesrc);
        parser.parseFile(fname);
        tree = parser.getTree();
    }
    if (errorsOccurred())
    {
        delete tree;
        return false;
    }

    // DTD validation and additional basic validation
    NEDDTDValidator dtdvalidator;
    dtdvalidator.validate(tree);
    if (errorsOccurred())
    {
        delete tree;
        return false;
    }

    NEDBasicValidator basicvalidator(!opt_unparsedexpr);
    basicvalidator.validate(tree);
    if (errorsOccurred())
    {
        delete tree;
        return false;
    }

    // symbol table is needed for further validation and C++ code generation
    NEDSymbolTable symboltable;

    // semantic validation (will load imports too)
    if (!opt_novalidation)
    {
        if (!opt_noimports)
        {
            // invoke NEDCompiler (will process imports and do semantic validation)
            NEDCompiler nedc(&filecache, &symboltable, &importresolver);
            nedc.validate(tree);
        }
        else
        {
            // simple semantic validation (without imports)
            NEDSemanticValidator validator(!opt_unparsedexpr,&symboltable);
            validator.validate(tree);
        }
    }
    if (errorsOccurred())
    {
        delete tree;
        return false;
    }

    if (opt_mergeoutput)
    {
        outputtree->appendChild(tree);
    }
    else if (!opt_validateonly)
    {
        // generate output file name
        const char *suffix = opt_suffix;
        const char *hdrsuffix = opt_hdrsuffix;
        if (!suffix)
        {
            if (opt_genxml)
                suffix = "_n.xml";
            else if (opt_genned)
                suffix = "_n.ned";
            else
                suffix = "_n.cc";
        }
        if (!hdrsuffix)
        {
            hdrsuffix = "_n.h";
        }

        char outfname[1024];
        char outhdrfname[1024];
        createFileNameWithSuffix(outfname, fname, suffix);
        createFileNameWithSuffix(outhdrfname, fname, hdrsuffix);

        // TBD check output file for errors
        if (opt_genxml)
        {
            ofstream out(outfname);
            generateXML(out, tree, opt_srcloc);
            out.close();
        }
        else if (opt_genned)
        {
            ofstream out(outfname);
            generateNed(out, tree, opt_newsyntax);
            out.close();
        }
        else
        {
            ofstream out(outfname);
            //ofstream outh(outhdrfname);
            ofstream outh; // TBD open if we process msg files (not yet supported)
            generateCpp(out, outh, tree, &symboltable);
            out.close();
            outh.close();
        }

        delete tree;

        if (errorsOccurred())
            return false;
    }
    return true;
}


bool processListFile(const char *listfilename)
{
    const int maxline=1024;
    char line[maxline];

    if (opt_verbose) fprintf(stderr,"processing list file '%s'...\n",listfilename);

    ifstream in(listfilename, ios::in);
    if (in.fail())
    {
        fprintf(stderr,"nedtool: cannot open list file '%s'\n",listfilename);
        return false;
    }

    while (in.getline(line, maxline))
    {
        int len = in.gcount();
        if (line[len-1]=='\n')
            line[len-1] = '\0';
        const char *fname = line;
        if (fname[0]=='@')
        {
            // included list files are relative to this list file
            std::string dir = directoryOf(listfilename);
            std::string newlistfile = tidyFilename(concatDirAndFile(dir.c_str(), fname+1).c_str());
            if (!processListFile(newlistfile.c_str()))
            {
                in.close();
                return false;
            }
        }
        else if (fname[0] && fname[0]!='#')
        {
            if (!processFile(fname))
            {
                in.close();
                return false;
            }
        }
    }

    if (in.bad())
    {
        fprintf(stderr,"nedtool: error reading list file '%s'\n",listfilename);
        return false;
    }
    in.close();
    return true;
}


int main(int argc, char **argv)
{
    // print usage
    if (argc<2)
    {
        printUsage();
        return 1;
    }

    // process options
    for (int i=1; i<argc; i++)
    {
        if (!strcmp(argv[i],"-c"))
        {
            opt_gencpp = true;
        }
        else if (!strcmp(argv[i],"-x"))
        {
            opt_genxml = true;
        }
        else if (!strcmp(argv[i],"-n"))
        {
            opt_genned = true;
        }
        else if (!strcmp(argv[i],"-v"))
        {
            opt_validateonly = true;
        }
        else if (!strcmp(argv[i],"-I"))
        {
            i++;
            if (i==argc) {
                fprintf(stderr,"nedtool: unexpected end of arguments after -I\n");
                return 1;
            }
            importresolver.addImportPath(argv[i]);
        }
        else if (argv[i][0]=='-' && argv[i][1]=='I')
        {
            importresolver.addImportPath(argv[i]+2);
        }
        else if (!strcmp(argv[i],"-X"))
        {
            i++;
            if (i==argc) {
                fprintf(stderr,"nedtool: unexpected end of arguments after -X\n");
                return 1;
            }
            if (!strcmp(argv[i],"ned"))
                opt_nextfiletype = NED;
            else if (!strcmp(argv[i],"xml"))
                opt_nextfiletype = XML;
            else if (!strcmp(argv[i],"off"))
                opt_nextfiletype = NOTHING;
            else {
                fprintf(stderr,"nedtool: unknown file type %s after -X\n",argv[i]);
                return 1;
            }
        }
        else if (!strcmp(argv[i],"-N"))
        {
            opt_newsyntax = true;
        }
        else if (!strcmp(argv[i],"-s"))
        {
            i++;
            if (i==argc) {
                fprintf(stderr,"nedtool: unexpected end of arguments after -s\n");
                return 1;
            }
            opt_suffix = argv[i];
        }
        else if (!strcmp(argv[i],"-S"))
        {
            i++;
            if (i==argc) {
                fprintf(stderr,"nedtool: unexpected end of arguments after -S\n");
                return 1;
            }
            opt_hdrsuffix = argv[i];
        }
        else if (!strcmp(argv[i],"-e"))
        {
            opt_unparsedexpr = true;
        }
        else if (!strcmp(argv[i],"-t"))
        {
            opt_storesrc = true;
        }
        else if (!strcmp(argv[i],"-y"))
        {
            opt_novalidation = true;
        }
        else if (!strcmp(argv[i],"-z"))
        {
            opt_noimports = true;
        }
        else if (!strcmp(argv[i],"-p"))
        {
            opt_srcloc = true;
        }
        else if (!strcmp(argv[i],"-m"))
        {
            opt_mergeoutput = true;
            outputtree = new NedFilesNode;
        }
        else if (!strcmp(argv[i],"-o"))
        {
            i++;
            if (i==argc) {
                fprintf(stderr,"nedtool: unexpected end of arguments after -o\n");
                return 1;
            }
            opt_outputfile = argv[i];
        }
        else if (!strcmp(argv[i],"-V"))
        {
            opt_verbose = true;
        }
        else if (!strcmp(argv[i], "-h"))
        {
            opt_here = true;
        }
        else if (argv[i][0]=='-')
        {
            fprintf(stderr,"nedtool: unknown flag %s\n",argv[i]);
            return 1;
        }
        else if (argv[i][0]=='@')
        {
            if (!processListFile(argv[i]+1))
                return 1;
        }
        else
        {
            if (opt_mergeoutput && !opt_genxml && !opt_genned)
            {
                fprintf(stderr,"nedtool: option -m not supported with C++ output\n");
                return 1;
            }
#ifdef SHELL_DOES_NOT_EXPAND_WILDCARDS
            const char *fname = findFirstFile(argv[i]);
            if (!fname) {
                fprintf(stderr,"nedtool: not found: %s\n",argv[i]);
                return 1;
            }
            while (fname)
            {
                if (!processFile(fname)) return 1;
                fname = findNextFile();
            }
            findCleanup();
#else
            if (!processFile(argv[i])) return 1;
#endif
        }
    }

    if (opt_mergeoutput)
    {
        if (errorsOccurred())
        {
            delete outputtree;
            return 1;
        }

        const char *outfname;

        if (opt_outputfile)
            outfname = opt_outputfile;
        else if (opt_genxml)
            outfname = "out_n.xml";
        else if (opt_genned)
            outfname = "out_n.ned";
        else
            outfname = "out_n.cc";

        ofstream out(outfname);

        if (opt_genxml)
            generateXML(out, outputtree, opt_srcloc);
        else if (opt_genned)
            generateNed(out, outputtree, opt_newsyntax);
        else
            return 1; // mergeoutput with C++ output not supported
            // generateCpp(out, cout, outputtree);
        out.close();

        delete outputtree;

        if (errorsOccurred())
            return 1;
    }

    return 0;
}



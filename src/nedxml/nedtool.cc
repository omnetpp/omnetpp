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
#include <errno.h>

#include "nedparser.h"
#include "nederror.h"
#include "nedxmlparser.h"
#include "neddtdvalidator.h"
#include "nedsyntaxvalidator.h"
#include "nedsemanticvalidator.h"
#include "ned2generator.h"
#include "ned1generator.h"
#include "xmlgenerator.h"
#include "nedtools.h"
//XXX #include "cppgenerator.h"
//XXX #include "nedcompiler.h"
#include "fileglobber.h"
#include "fileutil.h"
#include "stringutil.h"
#include "platmisc.h"   // getcwd, chdir
#include "../utils/ver.h"

USING_NAMESPACE

using std::ofstream;
using std::ifstream;
using std::ios;

//TODO: "preserve format" flag (genererate ned2 as ned2, and ned1 as ned1)

// file types
enum {XML_FILE, NED_FILE, MSG_FILE, CPP_FILE, UNKNOWN_FILE};

// option variables
bool opt_genxml = false;           // -x
bool opt_gensrc = false;           // -n
bool opt_validateonly = false;     // -v
int opt_nextfiletype = UNKNOWN_FILE; // -X
bool opt_oldsyntax = false;        // -Q
const char *opt_suffix = NULL;     // -s
const char *opt_hdrsuffix = NULL;  // -S
bool opt_inplace = false;          // -k
bool opt_unparsedexpr = false;     // -e
bool opt_storesrc = false;         // -t
bool opt_novalidation = false;     // -y
bool opt_noimports = false;        // -z
bool opt_srcloc = false;           // -p
bool opt_mergeoutput = false;      // -m
bool opt_verbose = false;          // -V
const char *opt_outputfile = NULL; // -o
bool opt_here = false;             // -h
bool opt_splitnedfiles = false;    // -u

// global variables
//XXX NEDFileCache filecache;
//XXX NEDClassicImportResolver importresolver;

FilesNode *outputtree;


void printUsage()
{
    fprintf(stderr,
       "nedtool -- part of OMNeT++/OMNEST, (C) 2002-2005 Andras Varga\n"
       "Release: " OMNETPP_RELEASE ", " OMNETPP_EDITION ".\n"
       "\n"
       "Usage: nedtool [options] <file1> <file2> ...\n"
       "Files may be given in a listfile as well, with the @listfile or @@listfile\n"
       "syntax (check the difference below.) By default, if neither -n nor -x is\n"
       "specified, nedtool generates C++ source.\n"
       "  -x: generate XML (you may need -y, -e and -p as well)\n"
       "  -n: generate source (NED or MSG; you may need -y and -e as well)\n"
       "  -P: pretty-print and/or convert 3.x NED files to the current syntax;\n"
       "      this is a shortcut for -n -k -y\n"
       "  -v: no output (only validate input)\n"
       "  -m: output is a single file (out_n.* by default, see also -o)\n"
       "  -o <filename>: output file name (don't use when processing multiple files)\n"
       "  -h  place output file into current directory\n"
       "  -I <dir>: add directory to NED include path\n"
       "  -X xml/ned/msg/off: following files are XML, NED or MSG up to '-X off'\n"
       "  -Q: with -n: use old (3.x) NED syntax\n"
       "  -s <suffix>: suffix for generated files\n"
       "  -S <suffix>: when generating C++, suffix for generated header files\n"
       "  -k: with -n: replace original file and create backup (.bak). If input is a\n"
       "      single XML file created by `nedtool -m -x': replace original NED files\n"
       "  -u: with -m or -k: split NED files to one NED component per file\n" //XXX refine help text
       "  -e: do not parse expressions in NED input; expect unparsed expressions in XML\n"
       "  -y: skip semantic validation (implies -z, skip processing imports)\n"
       "  -z: skip processing imports\n"
       "  -t: with NED parsing: include source code of components in XML\n"
       "  -p: with -x: add source location info (src-loc attributes) to XML output\n"
       "  -V: verbose\n"
       "  @listfile: listfile should contain one file per line (@ or @@ listfiles\n"
       "      also accepted). Files are interpreted as relative to the listfile.\n"
       "      @ listfiles can be invoked from anywhere, with the same effect.\n"
       "  @@listfile: like @listfile, but contents is interpreted as relative to\n"
       "      the current working directory. @@ listfiles can be put anywhere,\n"
       "      including /tmp -- effect only depends on the working directory.\n"
       "Message (.msg) files should be processed with opp_msgc.\n"
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

bool renameFileToBAK(const char *fname)
{
    // returns false on failure, true if successfully renamed or no such file
    char bakfname[1024];
    createFileNameWithSuffix(bakfname, fname, ".bak");

    if (unlink(bakfname)!=0 && errno!=ENOENT)
    {
        fprintf(stderr,"nedtool: cannot remove old backup file %s, leaving file %s unchanged\n", bakfname, fname);
        return false;
    }
    if (rename(fname, bakfname)!=0 && errno!=ENOENT)
    {
        fprintf(stderr,"nedtool: cannot rename original %s to %s, leaving file unchanged\n", fname, bakfname);
        return false;
    }
    return true;
}

void generateNED(std::ostream& out, NEDElement *node, NEDErrorStore *e, bool oldsyntax)
{
    if (oldsyntax)
        generateNED1(out, node, e);
    else
        generateNED2(out, node, e);
}

bool processFile(const char *fname, NEDErrorStore *errors)
{
    if (opt_verbose) fprintf(stdout,"processing '%s'...\n",fname);

    // determine file type
    int ftype = opt_nextfiletype;
    if (ftype==UNKNOWN_FILE)
    {
        if (opp_stringendswith(fname, ".ned"))
            ftype=NED_FILE;
        else if (opp_stringendswith(fname, ".msg"))
            ftype=MSG_FILE;
        else if (opp_stringendswith(fname, ".xml"))
            ftype=XML_FILE;
        else
            ftype=NED_FILE;
    }

    // process input tree
    NEDElement *tree = 0;
    errors->clear();
    if (ftype==XML_FILE)
    {
        tree = parseXML(fname, errors);
    }
    else if (ftype==NED_FILE || ftype==MSG_FILE)
    {
        NEDParser parser(errors);
        parser.setParseExpressions(!opt_unparsedexpr);
        parser.setStoreSource(opt_storesrc);
        tree = (ftype==NED_FILE) ? parser.parseNEDFile(fname) : parser.parseMSGFile(fname);
    }
    if (errors->containsError())
    {
        delete tree;
        return false;
    }

    //XXX NEDTools::repairNEDElementTree(tree);

    // DTD validation and additional syntax validation
    NEDDTDValidator dtdvalidator(errors);
    try {
        dtdvalidator.validate(tree);
    }
    catch(NEDException& e) {
        // FIXME embed exception handling in validate()!
        fprintf(stderr,"nedtool: NEDException: %s\n", e.what());
        delete tree;
        return false;
    }
    if (errors->containsError())
    {
        delete tree;
        return false;
    }

    NEDSyntaxValidator syntaxvalidator(!opt_unparsedexpr, errors);
    syntaxvalidator.validate(tree);
    if (errors->containsError())
    {
        delete tree;
        return false;
    }

    // symbol table is needed for further validation and C++ code generation
//XXX    NEDSymbolTable symboltable;

    // semantic validation (will load imports too)
    if (!opt_novalidation)
    {
        if (!opt_noimports)
        {
            // invoke NEDCompiler (will process imports and do semantic validation)
//XXX            NEDCompiler nedc(&filecache, &symboltable, &importresolver, errors);
//XXX            nedc.validate(tree);
        }
        else
        {
            // simple semantic validation (without imports)
//XXX            NEDSemanticValidator validator(!opt_unparsedexpr,&symboltable, errors);
//XXX            validator.validate(tree);
        }
    }
    if (errors->containsError())
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
        char outfname[1024];
        char outhdrfname[1024];

        if (opt_inplace)
        {
            // won't be used if we're to split a single XML to several NED files
            strcpy(outfname, fname);
            strcpy(outhdrfname, "");  // unused
        }
        else if (opt_outputfile && (opt_genxml || opt_gensrc))
        {
            strcpy(outfname, opt_outputfile);
            strcpy(outhdrfname, "");  // unused
        }
        else
        {
            // generate output file name
            const char *suffix = opt_suffix;
            const char *hdrsuffix = opt_hdrsuffix;
            if (!suffix)
            {
                if (opt_genxml)
                    suffix = (ftype==MSG_FILE) ? "_m.xml" : "_n.xml";
                else if (opt_gensrc)
                    suffix = (ftype==MSG_FILE) ? "_m.msg" : "_n.ned";
                else
                    suffix = (ftype==MSG_FILE) ? "_m.cc" : "_n.cc";
            }
            if (!hdrsuffix)
            {
                hdrsuffix = "_m.h";
            }
            createFileNameWithSuffix(outfname, fname, suffix);
            createFileNameWithSuffix(outhdrfname, fname, hdrsuffix);
        }

        // TBD check output file for write errors!
        if (opt_genxml)
        {
            if (opt_inplace && !renameFileToBAK(outfname))
                return false;
            ofstream out(outfname);
            generateXML(out, tree, opt_srcloc);
            out.close();
        }
        else if (opt_inplace && opt_gensrc && (tree->getTagCode()==NED_FILES ||
                 tree->getTagCode()==NED_NED_FILE || tree->getTagCode()==NED_MSG_FILE))
        {
             if (tree->getTagCode()==NED_NED_FILE || tree->getTagCode()==NED_MSG_FILE)
             {
                 // wrap the tree into a FilesNode
                 NEDElement *file = tree;
                 tree = new FilesNode();
                 tree->appendChild(file);
             }

             if (opt_splitnedfiles)
                NEDTools::splitToFiles((FilesNode *)tree);

            for (NEDElement *child=tree->getFirstChild(); child; child=child->getNextSibling())
            {
                // extract file name
                if (child->getTagCode()==NED_NED_FILE)
                    strcpy(outfname, ((NedFileNode *)child)->getFilename());
                else if (child->getTagCode()==NED_MSG_FILE)
                    strcpy(outfname, ((MsgFileNode *)child)->getFilename());
                else
                    continue; // if there's anything else, ignore it

                // generate the file
                if (opt_inplace && !renameFileToBAK(outfname))
                    return false;
                ofstream out(outfname);
                generateNED(out, child, errors, opt_oldsyntax);
                out.close();
            }
        }
        else if (opt_gensrc)
        {
            if (opt_inplace && !renameFileToBAK(outfname))
                return false;
            ofstream out(outfname);
            generateNED(out, tree, errors, opt_oldsyntax);
            out.close();
        }
        else
        {
            if (!renameFileToBAK(outfname))
                return false;
            ofstream out(outfname);
            //ofstream outh(outhdrfname);
            ofstream outh; // TBD open if we process msg files (not yet supported)
//XXX            generateCpp(out, outh, tree, &symboltable);
            out.close();
            outh.close();
        }

        delete tree;

        if (errors->containsError())
            return false;
    }
    return true;
}


bool processListFile(const char *listfilename, bool istemplistfile, NEDErrorStore *errors)
{
    const int maxline=1024;
    char line[maxline];
    char olddir[1024] = "";

    if (opt_verbose) fprintf(stdout,"processing list file '%s'...\n",listfilename);

    ifstream in(listfilename, ios::in);
    if (in.fail())
    {
        fprintf(stderr,"nedtool: cannot open list file '%s'\n",listfilename);
        return false;
    }

    if (!istemplistfile)
    {
        // with @listfile, files should be relative to list file, so try cd into list file's directory
        // (with @@listfile, files are relative to the wd, so we don't cd)
        std::string dir, fnameonly;
        splitFileName(listfilename, dir, fnameonly);
        if (!getcwd(olddir,1024))
        {
            fprintf(stderr,"nedtool: cannot get the name of current directory\n");
            return false;
        }
        if (opt_verbose) fprintf(stdout,"changing into '%s'...\n",dir.c_str());
        if (chdir(dir.c_str()))
        {
            fprintf(stderr,"nedtool: cannot temporarily change to directory `%s' (does it exist?)\n", dir.c_str());
            return false;
        }
    }

    while (in.getline(line, maxline))
    {
        int len = in.gcount();
        if (line[len-1]=='\n')
            line[len-1] = '\0';
        const char *fname = line;
        if (fname[0]=='@')
        {
            bool istmp = (fname[1]=='@');
            if (!processListFile(fname+(istmp?2:1), istmp, errors))
            {
                in.close();
                return false;
            }
        }
        else if (fname[0] && fname[0]!='#')
        {
            if (!processFile(fname, errors))
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

    if (olddir[0])
    {
        if (opt_verbose) fprintf(stdout,"changing back to '%s'...\n",olddir);
        if (chdir(olddir))
        {
            fprintf(stderr,"nedtool: cannot change back to directory `%s'\n", olddir);
            return false;
        }
    }
    return true;
}


int main(int argc, char **argv)
{
    // print usage
    if (argc<2)
    {
        printUsage();
        return 0;
    }

    NEDErrorStore errorstore;
    NEDErrorStore *errors = &errorstore;
    errors->setPrintToStderr(true);

    // process options
    for (int i=1; i<argc; i++)
    {
        if (!strcmp(argv[i],"-x"))
        {
            opt_genxml = true;
        }
        else if (!strcmp(argv[i],"-n"))
        {
            opt_gensrc = true;
        }
        else if (!strcmp(argv[i],"-P"))
        {
            opt_gensrc = true;
            opt_inplace = true;
            opt_novalidation = true;
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
//XXX            importresolver.addImportPath(argv[i]);
        }
        else if (argv[i][0]=='-' && argv[i][1]=='I')
        {
//XXX            importresolver.addImportPath(argv[i]+2);
        }
        else if (!strcmp(argv[i],"-X"))
        {
            i++;
            if (i==argc) {
                fprintf(stderr,"nedtool: unexpected end of arguments after -X\n");
                return 1;
            }
            if (!strcmp(argv[i],"ned"))
                opt_nextfiletype = NED_FILE;
            else if (!strcmp(argv[i],"msg"))
                opt_nextfiletype = MSG_FILE;
            else if (!strcmp(argv[i],"xml"))
                opt_nextfiletype = XML_FILE;
            else if (!strcmp(argv[i],"off"))
                opt_nextfiletype = UNKNOWN_FILE;
            else {
                fprintf(stderr,"nedtool: unknown file type %s after -X\n",argv[i]);
                return 1;
            }
        }
        else if (!strcmp(argv[i],"-Q"))
        {
            opt_oldsyntax = true;
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
        else if (!strcmp(argv[i],"-k"))
        {
            opt_inplace = true;
        }
        else if (!strcmp(argv[i],"-u"))
        {
            opt_splitnedfiles = true;
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
            outputtree = new FilesNode;
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
            // treat @listfile and @@listfile differently
            bool istmp = (argv[i][1]=='@');
            if (!processListFile(argv[i]+(istmp?2:1), istmp, errors))
                return 1;
        }
        else
        {
            // process individual files on the command line
            //FIXME these checks get bypassed with list files!!!
            if (!opt_genxml && !opt_gensrc)
            {
                fprintf(stderr,"nedtool: generating C++ source not currently supported\n"); //XXX
                return 1;
            }
            if (opt_genxml && opt_gensrc)
            {
                fprintf(stderr,"nedtool: conflicting options -n (generate source) and -x (generate XML)\n");
                return 1;
            }
            if (opt_mergeoutput && opt_inplace)
            {
                fprintf(stderr,"nedtool: conflicting options -m (merge files) and -k (replace original file)\n");
                return 1;
            }
            if (opt_inplace && !opt_genxml && !opt_gensrc)
            {
                fprintf(stderr,"nedtool: conflicting options: -k (replace original file) needs -n (generate source) or -x (generate XML)\n");
                return 1;
            }
            if (opt_mergeoutput && !opt_genxml && !opt_gensrc)
            {
                fprintf(stderr,"nedtool: option -m not supported with C++ output\n");
                return 1;
            }
            if (opt_splitnedfiles && !opt_mergeoutput && !opt_inplace)
            {
                fprintf(stderr,"nedtool: option -u ignored because -k or -m is not specified\n"); //XXX not too logical
            }

#if SHELL_EXPANDS_WILDCARDS
            if (!processFile(argv[i], errors))
                return 1;
#else
            // we have to expand wildcards ourselves
            std::vector<std::string> filelist = FileGlobber(argv[i]).getFilenames();
            if (filelist.empty())
            {
                fprintf(stderr,"nedtool: not found: %s\n", argv[i]);
                return 1;
            }
            for (int i=0; i<filelist.size(); i++)
                if (!processFile(filelist[i].c_str(), errors))
                    return 1;
#endif
        }
    }

    if (opt_mergeoutput)
    {
        if (errors->containsError())
        {
            delete outputtree;
            return 1;
        }

        if (opt_splitnedfiles)
            NEDTools::splitToFiles(outputtree);

        const char *outfname;

        if (opt_outputfile)
            outfname = opt_outputfile;
        else if (opt_genxml)
            outfname = "out_n.xml";
        else if (opt_gensrc)
            outfname = "out_n.ned";
        else
            outfname = "out_n.cc";

        ofstream out(outfname);

        if (opt_genxml)
            generateXML(out, outputtree, opt_srcloc);
        else if (opt_gensrc)
            generateNED(out, outputtree, errors, opt_oldsyntax);
        else
            return 1; // mergeoutput with C++ output not supported
            // generateCpp(out, cout, outputtree);
        out.close();

        delete outputtree;

        if (errors->containsError())
            return 1;
    }

    return 0;
}



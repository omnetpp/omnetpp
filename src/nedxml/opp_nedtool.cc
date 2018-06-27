//==========================================================================
//  OPP_NEDTOOL.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contains: main() for opp_nedtool
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <cerrno>
#include <string>
#include <vector>

#include "common/fileglobber.h"
#include "common/fileutil.h"
#include "common/stringutil.h"
#include "common/ver.h"
#include "errorstore.h"
#include "omnetpp/platdep/platmisc.h"  // getcwd, chdir
#include "nedparser.h"
#include "exception.h"
#include "neddtdvalidator.h"
#include "nedsyntaxvalidator.h"
#include "nedcrossvalidator.h"
#include "nedgenerator.h"
#include "xmlgenerator.h"
#include "nedtools.h"
#include "xmlastparser.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

using std::ofstream;
using std::ifstream;
using std::ios;

// file types
enum { XML_FILE, NED_FILE, MSG_FILE, CPP_FILE, UNKNOWN_FILE };

// option variables
bool opt_genxml = false;           // -x
bool opt_gensrc = false;           // -n
bool opt_validateonly = false;     // -v
int opt_nextfiletype = UNKNOWN_FILE; // -T
const char *opt_suffix = nullptr;  // -s
bool opt_inplace = false;          // -k
bool opt_unparsedexpr = false;     // -e
bool opt_storesrc = false;         // -S
bool opt_novalidation = false;     // -y
bool opt_noimports = false;        // -z
bool opt_srcloc = false;           // -p
bool opt_mergeoutput = false;      // -m
bool opt_verbose = false;          // -V
const char *opt_outputfile = nullptr; // -o
bool opt_here = false;             // -h
bool opt_splitnedfiles = false;    // -u

FilesElement *outputtree;

//TODO turn code into a class, like scavetool

void printUsage()
{
    fprintf(stderr,
       "opp_nedtool -- part of " OMNETPP_PRODUCT ", (C) 2006-2018 Andras Varga, OpenSim Ltd.\n"
       "Version: " OMNETPP_VERSION_STR ", build: " OMNETPP_BUILDID ", edition: " OMNETPP_EDITION "\n"
       "\n"
       "Usage: opp_nedtool [options] <file1> <file2> ...\n"
       "Files may be given in a listfile as well, with the @listfile or @@listfile\n"
       "syntax (check the difference below.) By default, if neither -n nor -x is\n"
       "specified, opp_nedtool generates C++ source.\n"
       "  -x: generate XML (you may need -y, -e and -p as well)\n"
       "  -n: generate source (NED or MSG; you may need -y and -e as well)\n"
       "  -P: pretty-print; this is a shortcut for -n -k -y\n"
       "  -v: no output (only validate input)\n"
       "  -m: output is a single file (out_n.* by default, see also -o)\n"
       "  -o <filename>: output file name (don't use when processing multiple files)\n"
       "  -h  place output file into current directory\n"
       "  -I <dir>: add directory to NED include path\n"
       "  -s <suffix>: suffix for generated files\n"
       "  -T xml/ned/off: following files are XML or NED up to '-T off'\n"
       "  -k: with -n: replace original file and create backup (.bak). If input is a\n"
       "      single XML file created by 'opp_nedtool -m -x': replace original NED files\n"
       "  -u: with -m or -k: split NED files to one NED component per file\n" //XXX refine help text
       "  -e: do not parse expressions in NED input; expect unparsed expressions in XML\n"
       "  -y: skip semantic validation (implies -z, skip processing imports)\n"
       "  -z: skip processing imports\n"
       "  -S: with NED parsing: include source code of components in XML\n"
       "  -p: with -x: add source location info (src-loc attributes) to XML output\n"
       "  -V: verbose\n"
       "  @listfile: listfile should contain one file per line (@ or @@ listfiles\n"
       "      also accepted). Files are interpreted as relative to the listfile.\n"
       "      @ listfiles can be invoked from anywhere, with the same effect.\n"
       "  @@listfile: like @listfile, but contents is interpreted as relative to\n"
       "      the current working directory. @@ listfiles can be put anywhere,\n"
       "      including /tmp -- effect only depends on the working directory.\n"
    );
}

void createFileNameWithSuffix(char *outfname, const char *infname, const char *suffix)
{
    if (opt_here) {
        // remove directory part
        const char *s = infname+strlen(infname);
        while (s > infname && *s != '/' && *s != '\\')
            s--;
        if (*s == '/' || *s == '\\')
            s++;
        strcpy(outfname, s);
    }
    else {
        strcpy(outfname, infname);
    }

    // replace extension with suffix.
    char *s = outfname+strlen(outfname);
    while (s > outfname && *s != '/' && *s != '\\' && *s != '.')
        s--;
    if (*s != '.')
        s = outfname+strlen(outfname);
    strcpy(s, suffix);
}

bool renameFileToBAK(const char *fname)
{
    // returns false on failure, true if successfully renamed or no such file
    char bakfname[1024];
    createFileNameWithSuffix(bakfname, fname, ".bak");

    if (unlink(bakfname) != 0 && errno != ENOENT) {
        fprintf(stderr, "opp_nedtool: cannot remove old backup file %s, leaving file %s unchanged\n", bakfname, fname);
        return false;
    }
    if (rename(fname, bakfname) != 0 && errno != ENOENT) {
        fprintf(stderr, "opp_nedtool: cannot rename original %s to %s, leaving file unchanged\n", fname, bakfname);
        return false;
    }
    return true;
}

void generateSource(std::ostream& out, ASTNode *node, ErrorStore *e, int contentType)
{
    //TODO
    switch (contentType) {
    case NED_FILE: generateNed(out, node); break;
    }
}

bool processFile(const char *fname, ErrorStore *errors)
{
    if (opt_verbose)
        fprintf(stdout, "processing '%s'...\n", fname);

    ASTNode *tree = nullptr;

    try {
        // determine file type
        int ftype = opt_nextfiletype;
        if (ftype == UNKNOWN_FILE) {
            if (opp_stringendswith(fname, ".ned"))
                ftype = NED_FILE;
            else if (opp_stringendswith(fname, ".msg"))
                ftype = MSG_FILE;
            else if (opp_stringendswith(fname, ".xml"))
                ftype = XML_FILE;
            else
                ftype = NED_FILE;
        }

        if (ftype == MSG_FILE) {
            fprintf(stdout, "opp_nedtool: %s: error: use opp_msgtool for processing msg files\n", fname);
            return false;
        }

        // process input tree
        errors->clear();
        if (ftype == XML_FILE) {
            tree = parseXML(fname, errors);
        }
        else if (ftype == NED_FILE) {
            NedParser parser(errors);
            parser.setParseExpressions(!opt_unparsedexpr);
            parser.setStoreSource(opt_storesrc);
            tree = parser.parseNedFile(fname);
        }
        if (errors->containsError()) {
            delete tree;
            return false;
        }

        int contentType = (ftype==NED_FILE || ftype==MSG_FILE) ? ftype :
                (tree->getTagCode()==NED_NED_FILE || tree->getFirstChildWithTag(NED_NED_FILE)!=nullptr) ? NED_FILE : UNKNOWN_FILE;

        // DTD validation and additional syntax validation
        if (contentType == NED_FILE) {
            NedDtdValidator dtdvalidator(errors);
            dtdvalidator.validate(tree);
            if (errors->containsError()) {
                delete tree;
                return false;
            }

            NedSyntaxValidator syntaxvalidator(!opt_unparsedexpr, errors);
            syntaxvalidator.validate(tree);
            if (errors->containsError()) {
                delete tree;
                return false;
            }
        }

        if (opt_mergeoutput) {
            outputtree->appendChild(tree);
        }
        else if (!opt_validateonly) {
            char outfname[1024];
            char outhdrfname[1024];

            if (opt_inplace) {
                // won't be used if we're to split a single XML to several NED files
                strcpy(outfname, fname);
                strcpy(outhdrfname, "");  // unused
            }
            else if (opt_outputfile && (opt_genxml || opt_gensrc)) {
                strcpy(outfname, opt_outputfile);
                strcpy(outhdrfname, "");  // unused
            }
            else {
                // generate output file name
                const char *suffix = opt_suffix;
                if (!suffix) {
                    if (opt_genxml)
                        suffix = "_n.xml";
                    else if (opt_gensrc)
                        suffix = "_n.ned";
                    else
                        ; //TODO ensure this cannot happen
                }
                createFileNameWithSuffix(outfname, fname, suffix);
            }

            // TBD check output file for write errors!
            if (opt_genxml) {
                if (opt_inplace && !renameFileToBAK(outfname))
                    return false;
                ofstream out(outfname);
                if (out.fail())
                    throw opp_runtime_error("Cannot open '%s' for write", outfname);
                generateXML(out, tree, opt_srcloc);
                out.close();
                if (!out)
                    throw opp_runtime_error("Error writing '%s'", outfname);
            }
            else if (opt_inplace && opt_gensrc && (tree->getTagCode() == NED_FILES || tree->getTagCode() == NED_NED_FILE))
            {
                if (tree->getTagCode() == NED_NED_FILE) {
                    // wrap the tree into a FilesElement
                    ASTNode *file = tree;
                    tree = new FilesElement();
                    tree->appendChild(file);
                }

                if (opt_splitnedfiles)
                    NedTools::splitNedFiles((FilesElement *)tree);

                for (ASTNode *child = tree->getFirstChild(); child; child = child->getNextSibling()) {
                    // extract file name
                    if (child->getTagCode() == NED_NED_FILE)
                        strcpy(outfname, ((NedFileElement *)child)->getFilename());
                    else
                        continue;  // if there's anything else, ignore it

                    // generate the file
                    if (opt_inplace && !renameFileToBAK(outfname))
                        return false;
                    ofstream out(outfname);
                    if (out.fail())
                        throw opp_runtime_error("Cannot open '%s' for write", outfname);
                    generateSource(out, child, errors, contentType);
                    out.close();
                    if (!out)
                        throw opp_runtime_error("Error writing '%s'", outfname);
                }
            }
            else if (opt_gensrc) {
                if (opt_inplace && !renameFileToBAK(outfname))
                    return false;
                ofstream out(outfname);
                if (out.fail())
                    throw opp_runtime_error("Cannot open '%s' for write", outfname);
                generateSource(out, tree, errors, contentType);
                out.close();
                if (!out)
                    throw opp_runtime_error("Error writing '%s'", outfname);
            }
            else {
                Assert(!opt_gensrc && !opt_genxml);  // already handled above
                fprintf(stderr, "opp_nedtool: generating C++ source from %s files is not supported\n",
                        (ftype == NED_FILE ? "NED" : ftype == XML_FILE ? "XML" : "unknown"));
                delete tree;
                return false;
            }
            delete tree;

            if (errors->containsError())
                return false;
        }
    }
    catch (std::exception& e) {
        fprintf(stderr, "opp_nedtool: error: %s\n", e.what());
        delete tree;
        return false;
    }
    return true;
}

bool processListFile(const char *listfilename, bool istemplistfile, ErrorStore *errors)
{
    const int maxline = 1024;
    char line[maxline];
    char olddir[1024] = "";

    if (opt_verbose)
        fprintf(stdout, "processing list file '%s'...\n", listfilename);

    ifstream in(listfilename, ios::in);
    if (in.fail()) {
        fprintf(stderr, "opp_nedtool: cannot open list file '%s'\n", listfilename);
        return false;
    }

    if (!istemplistfile) {
        // with @listfile, files should be relative to list file, so try cd into list file's directory
        // (with @@listfile, files are relative to the wd, so we don't cd)
        std::string dir, fnameonly;
        splitFileName(listfilename, dir, fnameonly);
        if (!getcwd(olddir, 1024)) {
            fprintf(stderr, "opp_nedtool: cannot get the name of current directory\n");
            return false;
        }
        if (opt_verbose)
            fprintf(stdout, "changing into '%s'...\n", dir.c_str());
        if (chdir(dir.c_str())) {
            fprintf(stderr, "opp_nedtool: cannot temporarily change to directory '%s' (does it exist?)\n", dir.c_str());
            return false;
        }
    }

    while (in.getline(line, maxline)) {
        int len = in.gcount();
        if (line[len-1] == '\n')
            line[len-1] = '\0';
        const char *fname = line;
        if (fname[0] == '@') {
            bool istmp = (fname[1] == '@');
            if (!processListFile(fname+(istmp ? 2 : 1), istmp, errors)) {
                in.close();
                return false;
            }
        }
        else if (fname[0] && fname[0] != '#') {
            if (!processFile(fname, errors)) {
                in.close();
                return false;
            }
        }
    }

    if (in.bad()) {
        fprintf(stderr, "opp_nedtool: error reading list file '%s'\n", listfilename);
        return false;
    }
    in.close();

    if (olddir[0]) {
        if (opt_verbose)
            fprintf(stdout, "changing back to '%s'...\n", olddir);
        if (chdir(olddir)) {
            fprintf(stderr, "opp_nedtool: cannot change back to directory '%s'\n", olddir);
            return false;
        }
    }
    return true;
}

}  // namespace nedxml
}  // namespace omnetpp

using namespace omnetpp::nedxml;

int main(int argc, char **argv)
{
    // print usage
    if (argc < 2) {
        printUsage();
        return 0;
    }

    ErrorStore errorstore;
    ErrorStore *errors = &errorstore;
    errors->setPrintToStderr(true);

    // process options
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-x")) {
            opt_genxml = true;
        }
        else if (!strcmp(argv[i], "-n")) {
            opt_gensrc = true;
        }
        else if (!strcmp(argv[i], "-P")) {
            opt_gensrc = true;
            opt_inplace = true;
            opt_novalidation = true;
        }
        else if (!strcmp(argv[i], "-v")) {
            opt_validateonly = true;
        }
        else if (!strncmp(argv[i], "-T", 2)) {
            const char *arg = argv[i]+2;
            if (!*arg) {
                if (++i == argc) {
                    fprintf(stderr, "opp_nedtool: unexpected end of arguments after %s\n", argv[i-1]);
                    return 1;
                }
                arg = argv[i];
            }
            if (!strcmp(arg, "ned"))
                opt_nextfiletype = NED_FILE;
            else if (!strcmp(arg, "msg"))
                opt_nextfiletype = MSG_FILE;
            else if (!strcmp(arg, "xml"))
                opt_nextfiletype = XML_FILE;
            else if (!strcmp(arg, "off"))
                opt_nextfiletype = UNKNOWN_FILE;
            else {
                fprintf(stderr, "opp_nedtool: unknown file type %s after -T\n", arg);
                return 1;
            }
        }
        else if (!strcmp(argv[i], "-s")) {
            i++;
            if (i == argc) {
                fprintf(stderr, "opp_nedtool: unexpected end of arguments after -s\n");
                return 1;
            }
            opt_suffix = argv[i];
        }
        else if (!strcmp(argv[i], "-k")) {
            opt_inplace = true;
        }
        else if (!strcmp(argv[i], "-u")) {
            opt_splitnedfiles = true;
        }
        else if (!strcmp(argv[i], "-e")) {
            opt_unparsedexpr = true;
        }
        else if (!strcmp(argv[i], "-S")) {
            opt_storesrc = true;
        }
        else if (!strcmp(argv[i], "-y")) {
            opt_novalidation = true;
        }
        else if (!strcmp(argv[i], "-z")) {
            opt_noimports = true;
        }
        else if (!strcmp(argv[i], "-p")) {
            opt_srcloc = true;
        }
        else if (!strcmp(argv[i], "-m")) {
            opt_mergeoutput = true;
            outputtree = new FilesElement;
        }
        else if (!strcmp(argv[i], "-o")) {
            i++;
            if (i == argc) {
                fprintf(stderr, "opp_nedtool: unexpected end of arguments after -o\n");
                return 1;
            }
            opt_outputfile = argv[i];
        }
        else if (!strcmp(argv[i], "-V")) {
            opt_verbose = true;
        }
        else if (!strcmp(argv[i], "-h")) {
            opt_here = true;
        }
        else if (argv[i][0] == '-') {
            fprintf(stderr, "opp_nedtool: unknown option %s\n", argv[i]);
            return 1;
        }
        else if (argv[i][0] == '@') {
            // treat @listfile and @@listfile differently
            bool istmp = (argv[i][1] == '@');
            if (!processListFile(argv[i]+(istmp ? 2 : 1), istmp, errors))
                return 1;
        }
        else {
            // process individual files on the command line
            // FIXME these checks get bypassed with list files
            if (opt_genxml && opt_gensrc) {
                fprintf(stderr, "opp_nedtool: conflicting options -n (generate source) and -x (generate XML)\n");
                return 1;
            }
            if (opt_mergeoutput && opt_inplace) {
                fprintf(stderr, "opp_nedtool: conflicting options -m (merge files) and -k (replace original file)\n");
                return 1;
            }
            if (opt_inplace && !opt_genxml && !opt_gensrc) {
                fprintf(stderr, "opp_nedtool: conflicting options: -k (replace original file) needs -n (generate source) or -x (generate XML)\n");
                return 1;
            }
            if (opt_mergeoutput && !opt_genxml && !opt_gensrc) {
                fprintf(stderr, "opp_nedtool: option -m not supported with C++ output\n");
                return 1;
            }
            if (opt_splitnedfiles && !opt_mergeoutput && !opt_inplace) {
                fprintf(stderr, "opp_nedtool: option -u ignored because -k or -m is not specified\n");  // XXX not too logical
            }

#if SHELL_EXPANDS_WILDCARDS
            if (!processFile(argv[i], errors))
                return 1;
#else
            // we have to expand wildcards ourselves
            std::vector<std::string> filelist = FileGlobber(argv[i]).getFilenames();
            if (filelist.empty()) {
                fprintf(stderr, "opp_nedtool: not found: %s\n", argv[i]);
                return 1;
            }
            for (size_t i = 0; i < filelist.size(); i++)
                if (!processFile(filelist[i].c_str(), errors))
                    return 1;

#endif
        }

    }

    if (opt_mergeoutput) {
        if (errors->containsError()) {
            delete outputtree;
            return 1;
        }

        if (opt_splitnedfiles)
            NedTools::splitNedFiles(outputtree);

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
        if (out.fail())
            throw opp_runtime_error("Cannot open '%s' for write", outfname);

        if (opt_genxml)
            generateXML(out, outputtree, opt_srcloc);
        else if (opt_gensrc)
            generateSource(out, outputtree, errors, NED_FILE);
        else
            return 1;  // mergeoutput with C++ output not supported
        out.close();
        if (!out)
            throw opp_runtime_error("Error writing '%s'", outfname);

        delete outputtree;

        if (errors->containsError())
            return 1;
    }

    return 0;
}


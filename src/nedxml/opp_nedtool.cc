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
#include <memory>

#include "common/fileglobber.h"
#include "common/fileutil.h"
#include "common/stringutil.h"
#include "common/stlutil.h"
#include "common/formattedprinter.h"
#include "common/opp_ctype.h"
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
#include "opp_nedtool.h"

#if defined(__MINGW32__)
int _CRT_glob = 0;  // Turn off runtime file globbing support on MinGW. The shell already handles file globbing on the command line.
#endif

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

using std::ofstream;
using std::ifstream;
using std::ios;

std::vector<std::string> NedTool::expandNedFolder(const char *filename)
{
    if (isDirectory(filename))
        return collectFilesInDirectory(filename, true, ".ned");
    else
        return std::vector<std::string> { filename };
}

std::vector<std::string> NedTool::expandFileArg(const char *arg)
{
#if SHELL_EXPANDS_WILDCARDS
    return expandNedFolder(arg);
#else
    // we have to expand wildcards ourselves
    std::vector<std::string> expandedList = FileGlobber(arg).getFilenames();
    if (expandedList.empty())
        throw opp_runtime_error("not found: %s", arg);
    std::vector<std::string> result;
    for (std::string elem : expandedList)
        addAll(result, expandNedFolder(elem.c_str()));
    return result;
#endif
}

bool NedTool::fileLooksLikeXml(const char *filename)
{
    // return true if first non-whitespace character is "<"
    ifstream in(filename);
    unsigned char c = 0;
    while (in.good() && (c == 0 || opp_isspace(c)))
        in >> c;
    return in.good() && c == '<';
}

NedFileElement *NedTool::parseNedFile(const char *filename, bool opt_storesrc)
{
    if (opt_verbose)
        std::cout << "parsing " << filename << "\n";

    ErrorStore errors;
    errors.setPrintToStderr(true);
    ASTNode *tree;

    // process input tree
    NedParser parser(&errors);
    parser.setStoreSource(opt_storesrc);
    tree = parser.parseNedFile(filename);
    if (errors.containsError()) {
        delete tree;
        return nullptr;
    }

    NedDtdValidator dtdvalidator(&errors);
    dtdvalidator.validate(tree);
    if (errors.containsError()) {
        delete tree;
        return nullptr;
    }

    NedSyntaxValidator syntaxvalidator(&errors);
    syntaxvalidator.validate(tree);
    if (errors.containsError()) {
        delete tree;
        return nullptr;
    }

    if (!dynamic_cast<NedFileElement *>(tree))
        throw opp_runtime_error("internal error: NedFileElement expected as root");

    return static_cast<NedFileElement *>(tree);
}

ASTNode *NedTool::parseXmlFile(const char *filename)
{
    if (opt_verbose)
        std::cout << "loading " << filename << "\n";
    ErrorStore errors;
    errors.setPrintToStderr(true);
    return parseXML(filename, &errors);
}

FilesElement *NedTool::wrapIntoFilesElement(ASTNode *tree)
{
    if (!tree)
        return nullptr;
    else if (dynamic_cast<FilesElement*>(tree))
        return (FilesElement *)tree;
    else if (dynamic_cast<NedFileElement*>(tree)) {
        FilesElement *result = new FilesElement;
        result->appendChild(tree);
        return result;
    }
    else {
        delete tree;
        throw opp_runtime_error("root element of XML is expected to be <files> or <ned-file>");
    }
}

void NedTool::moveChildren(ASTNode *source, ASTNode *target)
{
    for (ASTNode *child = source->getFirstChild(); child; ) {
        ASTNode *nextChild = child->getNextSibling();
        source->removeChild(child);
        target->appendChild(child);
        child = nextChild;
    }
}

void NedTool::generateNedFile(const char *filename, NedFileElement *tree)
{
    if (opt_verbose)
        std::cout << "writing " << filename << "\n";

    ofstream out(filename);
    if (out.fail())
        throw opp_runtime_error("cannot open '%s' for write", filename);
    generateNed(out, tree);
    out.close();
    if (!out)
        throw opp_runtime_error("error writing '%s'", filename);
}

void NedTool::generateNedFiles(FilesElement *tree)
{
    for (NedFileElement *child = tree->getFirstNedFileChild(); child; child = child->getNextNedFileSibling())
        generateNedFile(child->getFilename(), child);
}

void NedTool::generateXmlFile(const char *filename, ASTNode *tree, bool srcloc)
{
    if (opt_verbose)
        std::cout << "writing " << filename << "\n";
    ofstream out(filename);
    if (out.fail())
        throw opp_runtime_error("cannot open '%s' for write", filename);
    generateXML(out, tree, srcloc);
    out.close();
    if (!out)
        throw opp_runtime_error("error writing '%s'", filename);
}

void NedTool::renameFileToBak(const char *filename)
{
    std::string bakfname = removeFileExtension(filename) + ".bak";
    if (opt_verbose)
        std::cout << "renaming " << filename << " to " << bakfname << "\n";
    if (unlink(bakfname.c_str()) != 0 && errno != ENOENT)
        throw opp_runtime_error("cannot remove old backup file %s, leaving file %s unchanged", bakfname.c_str(), filename);
    if (rename(filename, bakfname.c_str()) != 0 && errno != ENOENT)
        throw opp_runtime_error("cannot rename original %s to %s, leaving file unchanged", filename, bakfname.c_str());
}

void NedTool::helpCommand(int argc, char **argv)
{
    string page = argc==0 ? "options" : argv[0];
    printHelpPage(page);
}

void NedTool::printHelpPage(const std::string& page)
{
    FormattedPrinter help(cout);
    if (page == "options") {
        help.line("opp_nedtool -- part of " OMNETPP_PRODUCT ", (C) 2006-2018 OpenSim Ltd.");
        help.line("Version: " OMNETPP_VERSION_STR ", build: " OMNETPP_BUILDID ", edition: " OMNETPP_EDITION);
        help.line();
        help.para("Usage: opp_nedtool [<command>] [<options>] <files>...");
        help.para("Operations on (and related to) NED files.");
        help.line("Commands:");
        help.option("c, convert", "Convert NED files to XML form and vice versa, split up NED files to contain one NED type per file, etc.");
        help.option("p, prettyprint", "Format (pretty-print) NED files");
        help.option("v, validate", "Validate NED files");
        help.option("x, cpp", "Translate NED files into C++, to allow building self-contained simulation binaries that don't need separate NED files");
        help.option("h, help", "Print help text");
        help.line();
        help.para("The default command is 'validate'.");
        help.para("To get help, use opp_nedtool help <topic>. Available help topics: command names, 'makefrag'.");
    }
    else if (page == "h" || page == "help") {
        help.para("Usage: opp_nedtool help <topic>");
        help.para("Print help text on the given topic.");
    }
    else if (page == "c" || page == "convert") {
        help.para("Usage: opp_nedtool convert [<options>] <ned-folders-and-ned-or-xml-files>");
        help.para("This command essentially converts between NED and its abstract syntax tree (AST) "
                  "representation serialized in XML form. It is possible to export the AST of a NED file "
                  "(or files) to XML, and to regenerate the NED file(s) from said XML. The command "
                  "can also optionally perform additional tasks, such as splitting up NED files "
                  "containing multiple NED components to several NED files containing one component each.");
        help.para("An XML file may contain the ASTs of one or more NED files. The NED file names (with "
                  "their relative paths) are also part of the XML, so it is possible to regenerate "
                  "(and overwrite) the NED files the XML was produced from. The format of XML files is "
                  "described in the file <omnetpp>/doc/etc/ned2.dtd, and also in the manual. "
                  "The XML format has optional features, such as optional location info "
                  "(line/column ranges that AST nodes correspond to), and having expressions "
                  "in parsed or unparsed form. Command-line options exist to control these features "
                  "when translating NED to XML.");
        help.para("Unless specified otherwise, NED files specified on the command line are "
                  "converted to XML, and XML files are converted to NED. For the latter case "
                  "(XML-to-NED conversion), the names of NED files to be generated are taken from "
                  "the XML, i.e. the original NED files will be overwritten.");
        help.para("Specifying a folder as argument will cause all NED files in it to be processed. "
                  "To specify all XML files in a folder as input, use shell wildcards or the Unix "
                  "'find' command.");
        help.para("NED-to-NED conversion (which you can force with the --ned option) amounts to "
                  "parsing into AST and regenerating NED source from it. The net result is "
                  "essentially pretty-printing.");
        help.line("Options:");
        help.option("-n, --ned", "Force NED output");
        help.option("-x, --xml", "Force XML output");
        help.option("-m, --merge", "Output is a single file (out.ned or out.xml by default).");
        help.option("-o <filename>", "Output file name (don't use when processing multiple files)");
        help.option("-s, --split", "Split NED files to one NED component per file");
        help.option("-l, --srcloc", "Add source location info (src-loc attributes) to XML output");
        help.option("-t, --storesrc", "When converting NED to XML, include source code of components in XML output");
        help.option("-k, --bak", "Save backup of original files as .bak");
        help.option("-v", "Verbose");
        help.line();
    }
    else if (page == "p" || page == "prettyprint") {
        help.para("Usage: opp_nedtool prettyprint <ned-folders-and-files>");
        help.para("Format (pretty-print) NED files. The original files will be overwritten, optionally creating backups of the original files.");
        help.line("Options:");
        help.option("-k, --bak", "Save backup of original files as .bak");
        help.option("-v", "Verbose");
        help.line();
    }
    else if (page == "v" || page == "validate") {
        help.para("Usage: opp_nedtool validate [<options>] <ned-folders-and-files>");
        help.para("Perform syntax check and basic local validation of the NED files. "
                  "If no errors are found, the output is empty and the program will exit with zero exit code. "
                  "Otherwise, errors will be reported on the standard error, the program will return a nonzero exit code. "
                  "Specify -v (verbose) to see which files are being checked.");
        help.line("Options:");
        help.option("-v", "Verbose");
        help.line();
    }
    else if (page == "x" || page == "cpp") {
        help.para("Usage: opp_nedtool cpp [<options>] <ned-folders-and-files>");
        help.para("Generate C++ source that embeds the content of the specified NED files as "
                  "string constants. When the C++ source is compiled into a simulation binary, "
                  "the original NED files will no longer be required for running simulations. "
                  "Optional garbling, which prevents NED source code from being directly readable "
                  "inside the compiled binaries (and from being extracted using the 'strings' "
                  "command), is also available.");
        help.para("A makefrag file can be used to integrate the NED-to-C++ translation "
                  "into the build process. To see an example makefrag file, "
                  "view the 'makefrag' help topic:");
        help.line("$ opp_nedtool -h makefrag");
        help.line();
        help.para("Tip: When you try out NED file embedding on a simulation, "
                   "you may want to also prevent the simulation program from dynamically "
                   "loading NED files, to avoid confusion. You can do so by additionally "
                   "specifying the '-x.' option to the simulation. This option prevents "
                   "all NED files from being loaded.");
        help.line("Options:");
        help.option("-o <filename>", "Name of output file. Default: out_n.cc");
        help.option("-p, --garblephrase <string>", "Phrase for garbling the NED source. Default: none");
        help.option("-v", "Verbose");
        help.line();
    }
    else if (page == "makefrag") {
        help.line("----8<------------------------------------------------------------------------------");
        help.line("# Save this file as 'makefrag' into the folder where opp_makemake (or the IDE)");
        help.line("# generates the Makefile for your simulation binary, and re-generate the Makefile.");
        help.line("# Note: make sure to use a Tab character for the indented lines below, not 8 spaces.");
        help.line();
        help.line("# Change the following line to point to the root of your NED source tree.");
        help.line("NEDFOLDER = .");
        help.line();
        help.line("NEDFILES_CC = nedfiles_n.cc # File name must end in '_n.cc'");
        help.line("OBJS += $O/$(NEDFILES_CC:.cc=.o)");
        help.line();
        help.line("default: all");
        help.line();
        help.line("$(NEDFILES_CC): $(call opp_rwildcard, $(NEDFOLDER), *.ned)");
        help.line("\t$(Q) opp_nedtool cpp $(NEDFOLDER) -o $(NEDFILES_CC)");
        help.line();
        help.line("clean: clean-nedfilescc");
        help.line();
        help.line("clean-nedfilescc:");
        help.line("\t$(Q) rm -f $(NEDFILES_CC)");
        help.line("----8<------------------------------------------------------------------------------");
        help.line();
    }
    else {
        throw opp_runtime_error("no help topic '%s'", page.c_str());
    }
}

static ASTNode *maybeUnwrap(FilesElement *filesElement)
{
    return filesElement->getNumChildren() == 1 ? filesElement->getFirstChild() : filesElement;
}

void NedTool::convertCommand(int argc, char **argv)
{
    // process options
    bool opt_forcened = false;
    bool opt_forcexml = false;
    bool opt_mergeoutput = false;
    std::string opt_outputfile;
    bool opt_splitnedfiles = false;
    bool opt_srcloc = false;
    bool opt_storesrc = false;
    bool opt_makebackup = false;
    std::vector<std::string> inputFiles;

    for (int i = 0; i < argc; i++) {
        if (string(argv[i]) == "-n" || string(argv[i]) == "--ned")
            opt_forcened = true;
        else if (string(argv[i]) == "-x" || string(argv[i]) == "--xml")
            opt_forcexml = true;
        else if (string(argv[i]) == "-m" || string(argv[i]) == "--merge")
            opt_mergeoutput = true;
        else if (string(argv[i]) == "-o") {
            i++;
            if (i == argc)
                throw opp_runtime_error("unexpected end of arguments after -o");
            opt_outputfile = argv[i];
        }
        else if (string(argv[i]) == "-s" || string(argv[i]) == "--split")
            opt_splitnedfiles = true;
        else if (string(argv[i]) == "-l" || string(argv[i]) == "--srcloc")
            opt_srcloc = true;
        else if (string(argv[i]) == "-t" || string(argv[i]) == "--storesrc")
            opt_storesrc = true;
        else if (string(argv[i]) == "-k" || string(argv[i]) == "--bak")
            opt_makebackup = true;
        else if (string(argv[i]) == "-v")
            opt_verbose = true;
        else if (argv[i][0] == '-')
            throw opp_runtime_error("unknown option %s", argv[i]);
        else
            addAll(inputFiles, expandFileArg(argv[i]));
    }

    if (opt_forcened && opt_forcexml)
        throw opp_runtime_error("conflicting options --ned and --xml");

    if (opt_verbose)
        std::cout << "Converting " << inputFiles.size() << " file(s)\n";

    if (inputFiles.empty())
        std::cerr << "opp_nedtool: warning: no input files\n";

    FilesElement *mergedTree = opt_mergeoutput ? new FilesElement : nullptr;
    std::unique_ptr<FilesElement> dummy(mergedTree);

    int numSkipped = 0;
    for (std::string inputFile : inputFiles) {
        bool isNedFile = opp_stringendswith(inputFile.c_str(), ".ned") ? true :
                opp_stringendswith(inputFile.c_str(), ".xml") ? false :
                        !fileLooksLikeXml(inputFile.c_str());
        FilesElement *tree;
        if (isNedFile)
            tree = wrapIntoFilesElement(parseNedFile(inputFile.c_str(), opt_storesrc));
        else
            tree = wrapIntoFilesElement(parseXmlFile(inputFile.c_str()));

        if (tree == nullptr)
            numSkipped++;
        else if (opt_mergeoutput) {
            moveChildren(tree, mergedTree);
            delete tree;
        }
        else {
            std::unique_ptr<FilesElement> dummy(tree);

            if (opt_splitnedfiles)
                NedTools::splitNedFiles(tree);

            bool nedOutput = opt_forcened ? true : opt_forcexml ? false : !isNedFile;

            std::string outputFile = inputFile;
            if (isNedFile == nedOutput) {  // NED-to-NED or XML-to-XML
                if (opt_makebackup)
                    renameFileToBak(inputFile.c_str());
                else
                    removeFile(inputFile.c_str(), nullptr);
            }
            else {
                // change extension if needed
                outputFile += (nedOutput ? ".ned" : ".xml");
            }
            if (!nedOutput)
                generateXmlFile(outputFile.c_str(), maybeUnwrap(tree), opt_srcloc);
            else
                generateNedFiles(tree);
        }
    }

    if (opt_mergeoutput) {
        if (opt_splitnedfiles)
            NedTools::splitNedFiles(mergedTree);
        if (opt_forcened)
            throw opp_runtime_error("refusing to generate merged NED file");
        std::string outputFile = opt_outputfile.empty() ? "out.xml" : opt_outputfile;
        generateXmlFile(outputFile.c_str(), mergedTree, opt_srcloc);
    }

    if (numSkipped > 0)
        throw opp_runtime_error("error in %d file(s)", numSkipped);
}

void NedTool::prettyprintCommand(int argc, char **argv)
{
    // process options
    bool opt_makebackup = false;
    std::vector<std::string> nedfiles;

    for (int i = 0; i < argc; i++) {
        if (string(argv[i]) == "-k" || string(argv[i]) == "--bak")
            opt_makebackup = true;
        else if (string(argv[i]) == "-v")
            opt_verbose = true;
        else if (argv[i][0] == '-')
            throw opp_runtime_error("unknown option %s", argv[i]);
        else
            addAll(nedfiles, expandFileArg(argv[i]));
    }

    if (nedfiles.empty())
        std::cerr << "opp_nedtool: warning: no input files\n";

    if (opt_verbose)
        std::cout << "Pretty-printing " << nedfiles.size() << " file(s)\n";

    int numSkipped = 0;
    for (std::string nedfile : nedfiles) {
        NedFileElement *tree = parseNedFile(nedfile.c_str(), false);
        if (tree == nullptr)
            numSkipped++;
        else {
            if (opt_makebackup)
                renameFileToBak(nedfile.c_str());
            generateNedFile(nedfile.c_str(), tree);
            delete tree;
        }
    }
    if (numSkipped > 0)
        throw opp_runtime_error("error in %d file(s)", numSkipped);
}

void NedTool::validateCommand(int argc, char **argv)
{
    // process options
    std::vector<std::string> nedfiles;

    for (int i = 0; i < argc; i++) {
        if (string(argv[i]) == "-v")
            opt_verbose = true;
        else if (argv[i][0] == '-')
            throw opp_runtime_error("unknown option %s", argv[i]);
        else
            addAll(nedfiles, expandFileArg(argv[i]));
    }

    if (opt_verbose)
        std::cout << "Validating " << nedfiles.size() << " file(s)\n";

    if (nedfiles.empty())
        std::cerr << "opp_nedtool: Warning: no input files\n";

    int numFilesWithErrors = 0;
    for (std::string nedfile : nedfiles) {
        NedElement *tree = parseNedFile(nedfile.c_str(), false);
        if (tree == nullptr)
            numFilesWithErrors++;
        else
            delete tree;
    }

    if (numFilesWithErrors > 0)
        throw opp_runtime_error("error in %d file(s)", numFilesWithErrors);

    if (opt_verbose)
        cout << "Validation found no error\n";

}

void NedTool::generateCppCommand(int argc, char **argv)
{
    // process options
    std::string opt_cppfile = "out_n.cc";
    std::string opt_garblephrase;
    std::vector<std::string> fileArgs;

    for (int i = 0; i < argc; i++) {
        if (string(argv[i]) == "-o") {
            i++;
            if (i == argc)
                throw opp_runtime_error("unexpected end of arguments after -o");
            opt_cppfile = argv[i];
        }
        else if (string(argv[i]) == "-p" || string(argv[i]) == "--garblephrase") {
            i++;
            if (i == argc)
                throw opp_runtime_error("unexpected end of arguments after -p");
            opt_garblephrase = argv[i];
        }
        else if (string(argv[i]) == "-v")
            opt_verbose = true;
        else if (argv[i][0] == '-')
            throw opp_runtime_error("unknown option %s", argv[i]);
        else
            fileArgs.push_back(argv[i]);
    }

    // load NED files/folders
    std::vector<std::string> nedfiles;
    for (std::string arg : fileArgs)
        addAll(nedfiles, expandFileArg(arg.c_str()));

    if (nedfiles.empty())
        std::cerr << "opp_nedtool: Warning: no input files\n";

    if (opt_verbose)
        std::cout << "Converting to C++ " << nedfiles.size() << " file(s)\n";

    // basic syntax check on the files
    int numFilesWithErrors = 0;
    for (std::string nedfile : nedfiles) {
        NedElement *tree = parseNedFile(nedfile.c_str(), false);
        if (tree == nullptr)
            numFilesWithErrors++;
        else
            delete tree;
    }
    if (numFilesWithErrors > 0)
        throw opp_runtime_error("errors found in %d file(s), refusing to generate C++ source", numFilesWithErrors);

    // generate C++ source
    NedTools::generateCppSource(opt_cppfile.c_str(), nedfiles, opt_garblephrase.empty() ? nullptr : opt_garblephrase.c_str());
}

int NedTool::main(int argc, char **argv)
{
    if (argc < 2) {
        helpCommand(argc-1, argv+1);
        exit(0);
    }

    try {
        string command = argv[1];
        if (argc >= 3 && command[0] != '-' && (string(argv[2]) == "-h" || string(argv[2]) == "--help"))
            printHelpPage(command);
        else if (command == "c" || command == "convert")
            convertCommand(argc-2, argv+2);
        else if (command == "p" || command == "prettyprint")
            prettyprintCommand(argc-2, argv+2);
        else if (command == "v" || command == "validate")
            validateCommand(argc-2, argv+2);
        else if (command == "x" || command == "cpp")
            generateCppCommand(argc-2, argv+2);
        else if (command == "h" || command == "help" || command == "-h" || command == "--help")
            helpCommand(argc-2, argv+2);
        else if (command[0] == '-' || isFile(command.c_str()) || isDirectory(command.c_str())) // missing command
            validateCommand(argc-1, argv+1);
        else
            throw opp_runtime_error("unknown command or file name '%s'", command.c_str());
    }
    catch (exception& e) {
        cerr << "opp_nedtool: " << e.what() << endl;
        return 1;
    }
    return 0;
}

}  // namespace scave
}  // namespace omnetpp

using namespace omnetpp::nedxml;

int main(int argc, char **argv)
{
    NedTool nedTool;
    return nedTool.main(argc, argv);
}


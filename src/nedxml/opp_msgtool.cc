//==========================================================================
//  OPP_MSGTOOL.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contains: main() for opp_msgtool
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
#include "msgparser.h"
#include "exception.h"
#include "msgdtdvalidator.h"
#include "msgvalidator.h"
#include "msggenerator.h"
#include "msgcompiler.h"
#include "xmlgenerator.h"
#include "nedtools.h"
#include "xmlastparser.h"
#include "opp_msgtool.h"

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

std::vector<std::string> MsgTool::expandMsgFolder(const char *filename)
{
    if (isDirectory(filename))
        return collectFilesInDirectory(filename, true, ".msg");
    else
        return std::vector<std::string> { filename };
}

std::vector<std::string> MsgTool::expandFileArg(const char *arg)
{
#if SHELL_EXPANDS_WILDCARDS
    return expandMsgFolder(arg);
#else
    // we have to expand wildcards ourselves
    std::vector<std::string> expandedList = FileGlobber(arg).getFilenames();
    if (expandedList.empty())
        throw opp_runtime_error("not found: %s", arg);
    std::vector<std::string> result;
    for (std::string elem : expandedList)
        addAll(result, expandMsgFolder(elem.c_str()));
    return result;
#endif
}

bool MsgTool::fileLooksLikeXml(const char *filename)
{
    // return true if first non-whitespace character is "<"
    ifstream in(filename);
    unsigned char c = 0;
    while (in.good() && (c == 0 || opp_isspace(c)))
        in >> c;
    return in.good() && c == '<';
}

MsgFileElement *MsgTool::parseMsgFile(const char *filename, bool opt_storesrc)
{
    if (opt_verbose)
        std::cout << "parsing " << filename << "\n";

    ErrorStore errors;
    errors.setPrintToStderr(true);
    ASTNode *tree;

    // process input tree
    MsgParser parser(&errors);
    parser.setStoreSource(opt_storesrc);
    tree = parser.parseMsgFile(filename);
    if (errors.containsError()) {
        delete tree;
        return nullptr;
    }

    MsgDtdValidator dtdvalidator(&errors);
    dtdvalidator.validate(tree);
    if (errors.containsError()) {
        delete tree;
        return nullptr;
    }

    if (!dynamic_cast<MsgFileElement *>(tree))
        throw opp_runtime_error("internal error: MsgFileElement expected as root");

    return static_cast<MsgFileElement *>(tree);
}

ASTNode *MsgTool::parseXmlFile(const char *filename)
{
    if (opt_verbose)
        std::cout << "loading " << filename << "\n";
    ErrorStore errors;
    errors.setPrintToStderr(true);
    return parseXML(filename, &errors);
}

FilesElement *MsgTool::wrapIntoFilesElement(ASTNode *tree)
{
    if (!tree)
        return nullptr;
    else if (dynamic_cast<FilesElement*>(tree))
        return (FilesElement *)tree;
    else if (dynamic_cast<MsgFileElement*>(tree)) {
        FilesElement *result = new FilesElement;
        result->appendChild(tree);
        return result;
    }
    else {
        delete tree;
        throw opp_runtime_error("root element of XML is expected to be <files> or <msg-file>");
    }
}

void MsgTool::moveChildren(ASTNode *source, ASTNode *target)
{
    for (ASTNode *child = source->getFirstChild(); child; ) {
        ASTNode *nextChild = child->getNextSibling();
        source->removeChild(child);
        target->appendChild(child);
        child = nextChild;
    }
}

void MsgTool::generateMsgFile(const char *filename, MsgFileElement *tree)
{
    if (opt_verbose)
        std::cout << "writing " << filename << "\n";

    ofstream out(filename);
    if (out.fail())
        throw opp_runtime_error("cannot open '%s' for write", filename);
    generateMsg(out, tree);
    out.close();
    if (!out)
        throw opp_runtime_error("error writing '%s'", filename);
}

void MsgTool::generateMsgFiles(FilesElement *tree)
{
    for (MsgFileElement *child = (MsgFileElement*)tree->getFirstChildWithTag(MSG_MSG_FILE); child; child = (MsgFileElement*)child->getNextSiblingWithTag(MSG_MSG_FILE))
        generateMsgFile(child->getFilename(), child);
}

void MsgTool::generateXmlFile(const char *filename, ASTNode *tree, bool srcloc)
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

void MsgTool::renameFileToBak(const char *filename)
{
    std::string bakfname = removeFileExtension(filename) + ".bak";
    if (opt_verbose)
        std::cout << "renaming " << filename << " to " << bakfname << "\n";
    if (unlink(bakfname.c_str()) != 0 && errno != ENOENT)
        throw opp_runtime_error("cannot remove old backup file %s, leaving file %s unchanged", bakfname.c_str(), filename);
    if (rename(filename, bakfname.c_str()) != 0 && errno != ENOENT)
        throw opp_runtime_error("cannot rename original %s to %s, leaving file unchanged", filename, bakfname.c_str());
}

void MsgTool::generateDependencies(const char *depsfile, const char *msgfile, const char *hfile, const char *ccfile, const std::set<std::string>& dependencies, bool wantPhonytargets)
{
    bool useFileOutput = !opp_isempty(depsfile) && strcmp(depsfile, "-") != 0;

    if (opt_verbose) {
        if (useFileOutput)
            cout << "writing dependencies to " << depsfile << "\n";
        else
            cout << "writing dependencies to stdout\n";
    }

    std::ofstream fileStream;
    if (useFileOutput) {
        mkPath(directoryOf(depsfile).c_str());
        fileStream.open(depsfile);
        if (fileStream.fail())
            throw opp_runtime_error("Could not open '%s' for write", depsfile);
    }

    std::ostream& out = useFileOutput ? fileStream : std::cout;
    out << ccfile << " " << hfile << " :";
    out << " \\\n\t" << msgfile;
    for (const std::string& dep : dependencies)
        out << " \\\n\t" << dep;
    out << "\n";
    if (wantPhonytargets) {
        out << msgfile << ":\n";
        for (const std::string& dep : dependencies)
            out << dep << ":\n";
    }
    if (!out)
        throw opp_runtime_error("Error writing dependencies to '%s'", depsfile);
    if (useFileOutput)
        fileStream.close();
}


void MsgTool::helpCommand(int argc, char **argv)
{
    string page = argc==0 ? "options" : argv[0];
    printHelpPage(page);
}

void MsgTool::printHelpPage(const std::string& page)
{
    FormattedPrinter help(cout);
    if (page == "options") {
        help.line("opp_msgtool -- part of " OMNETPP_PRODUCT ", (C) 2006-2018 OpenSim Ltd.");
        help.line("Version: " OMNETPP_VERSION_STR ", build: " OMNETPP_BUILDID ", edition: " OMNETPP_EDITION);
        help.line();
        help.para("Usage: opp_msgtool [<command>] [<options>] <files>...");
        help.para("Message compiler and MSG file manipulation tool.");
        help.line("Commands:");
        help.option("x, cpp", "Generate C++ code from MSG files");
        help.option("p, prettyprint", "Format (pretty-print) MSG files");
        help.option("c, convert", "Convert MSG files to XML form and vice versa, etc.");
        help.option("v, validate", "Validate MSG files");
        help.option("h, help", "Print help text");
        help.line();
        help.para("The default command is 'cpp'.");
        help.para("To get help, use opp_msgtool help <topic>. Available help topics: command names, 'builtindefs'.");
    }
    else if (page == "h" || page == "help") {
        help.para("Usage: opp_msgtool help <topic>");
        help.para("Print help text on the given topic.");
    }
    else if (page == "c" || page == "convert") {
        help.para("Usage: opp_msgtool convert [<options>] <folders-and-msg-or-xml-files>");
        help.para("This command essentially converts between MSG and its abstract syntax tree (AST) "
                  "representation serialized in XML form. It is possible to export the AST of a MSG file "
                  "(or files) to XML, and to regenerate the MSG file(s) from said XML.");
        help.para("An XML file may contain the ASTs of one or more MSG files. The MSG file names (with "
                  "their relative paths) are also part of the XML, so it is possible to regenerate "
                  "(and overwrite) the MSG files the XML was produced from. The format of XML files is "
                  "described in the file <omnetpp>/doc/etc/msg2.dtd, and also in the manual. "
                  "The XML format has optional features, such as optional location info "
                  "(line/column ranges that AST nodes correspond to). Command-line options exist "
                  "to control these features when translating MSG to XML.");
        help.para("Unless specified otherwise, MSG files specified on the command line are "
                  "converted to XML, and XML files are converted to MSG. For the latter case "
                  "(XML-to-MSG conversion), the names of MSG files to be generated are taken from "
                  "the XML, i.e. the original MSG files will be overwritten.");
        help.para("Specifying a folder as argument will cause all MSG files in it to be processed. "
                  "To specify all XML files in a folder as input, use shell wildcards or the Unix "
                  "'find' command.");
        help.para("MSG-to-MSG conversion (which you can force with the --msg option) amounts to "
                  "parsing into AST and regenerating MSG source from it. The net result is "
                  "essentially pretty-printing.");
        help.line("Options:");
        help.option("-g, --msg", "Force MSG output");
        help.option("-x, --xml", "Force XML output");
        help.option("-m, --merge", "Output is a single file (out.msg or out.xml by default).");
        help.option("-o <filename>", "Output file name (don't use when processing multiple files)");
        help.option("-l, --srcloc", "Add source location info (src-loc attributes) to XML output");
        help.option("-t, --storesrc", "When converting MSG to XML, include source code of components in XML output");
        help.option("-k, --bak", "Save backup of original files as .bak");
        help.option("-v", "Verbose");
        help.line();
    }
    else if (page == "p" || page == "prettyprint") {
        help.para("Usage: opp_msgtool prettyprint <folders-and-msg-files>");
        help.para("Format (pretty-print) MSG files. The original files will be overwritten, optionally creating backups of the original files.");
        help.line("Options:");
        help.option("-k, --bak", "Save backup of original files as .bak");
        help.option("-v", "Verbose");
        help.line();
    }
    else if (page == "v" || page == "validate") {
        help.para("Usage: opp_msgtool validate [<options>] <folders-and-msg-files>");
        help.para("Perform syntax check and basic local validation of the MSG files. "
                  "If no errors are found, the output is empty and the program will exit with zero exit code. "
                  "Otherwise, errors will be reported on the standard error, the program will return a nonzero exit code. "
                  "Specify -v (verbose) to see which files are being checked.");
        help.line("Options:");
        help.option("-v", "Verbose");
        help.line();
    }
    else if (page == "x" || page == "cpp") {
        help.para("Usage: opp_msgtool cpp [<options>] <folders-and-msg-files>");
        help.para("Generate C++ source from MSG files.");
        help.line();
        help.line("Options:");
        help.option("--msg6", "This is the default and only mode in OMNeT++ 6.0 and later.");
        help.option("-I <dir>", "Add directory to MSG import path");
        help.option("-s <suffix>", "Suffix for generated C++ source files (default: '_m.cc')");
        help.option("-t <suffix>", "Suffix for generated C++ header files (default: '_m.h')");
        help.option("-P <symbol>", "Add dllexport/dllimport symbol to class declarations; if symbol "
                    "name ends in _API, boilerplate code to conditionally define "
                    "it as OPP_DLLEXPORT/OPP_DLLIMPORT is also generated");
        help.option("-MD", "Turn on dependency generation for MSG files; see also: -MF, -MP");
        help.option("-MF <file>", "Save dependencies into the specified file; when absent, "
                    "dependencies will be written to the standard output");
        help.option("-MP", "Add a phony target for each dependency other than the main file,"
                    "causing each to depend on nothing. These dummy rules work around errors "
                    "make gives if you remove header files without updating the Makefile.");
        help.option("-Xnc", "Do not generate classes, only descriptors (cf. @existingClass MSG property)");
        help.option("-Xnd", "Do not generate class descriptors (cf. @descriptor(false) MSG property)");
        help.option("-Xns", "Do not generate setters in class descriptors (cf. @descriptor(readonly) MSG property)");
        help.option("-v", "Verbose");
        help.line();
    }
    else if (page == "builtindefs") {
        std::cout << MsgCompiler::getBuiltinDefinitions();
    }
    else if (page == "latexdoc") {
        MsgCompilerOptions options;
        ErrorStore errors;
        MsgCompiler(options, &errors).printPropertiesLatexDocu(std::cout);
    }
    else {
        throw opp_runtime_error("no help topic '%s'", page.c_str());
    }
}

static ASTNode *maybeUnwrap(FilesElement *filesElement)
{
    return filesElement->getNumChildren() == 1 ? filesElement->getFirstChild() : filesElement;
}

void MsgTool::convertCommand(int argc, char **argv)
{
    // process options
    bool opt_forcemsg = false;
    bool opt_forcexml = false;
    bool opt_mergeoutput = false;
    std::string opt_outputfile;
    bool opt_srcloc = false;
    bool opt_storesrc = false;
    bool opt_makebackup = false;
    std::vector<std::string> inputFiles;

    for (int i = 0; i < argc; i++) {
        if (string(argv[i]) == "-g" || string(argv[i]) == "--msg")
            opt_forcemsg = true;
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

    if (opt_forcemsg && opt_forcexml)
        throw opp_runtime_error("conflicting options --msg and --xml");

    if (opt_verbose)
        std::cout << "Converting " << inputFiles.size() << " file(s)\n";

    if (inputFiles.empty())
        std::cerr << "opp_msgtool: warning: no input files\n";

    FilesElement *mergedTree = opt_mergeoutput ? new FilesElement : nullptr;
    std::unique_ptr<FilesElement> dummy(mergedTree);

    int numSkipped = 0;
    for (std::string inputFile : inputFiles) {
        bool isMsgFile = opp_stringendswith(inputFile.c_str(), ".msg") ? true :
                opp_stringendswith(inputFile.c_str(), ".xml") ? false :
                        !fileLooksLikeXml(inputFile.c_str());
        FilesElement *tree;
        if (isMsgFile)
            tree = wrapIntoFilesElement(parseMsgFile(inputFile.c_str(), opt_storesrc));
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

            bool msgOutput = opt_forcemsg ? true : opt_forcexml ? false : !isMsgFile;

            std::string outputFile = inputFile;
            if (isMsgFile == msgOutput) {  // MSG-to-MSG or XML-to-XML
                if (opt_makebackup)
                    renameFileToBak(inputFile.c_str());
                else
                    removeFile(inputFile.c_str(), nullptr);
            }
            else {
                // change extension if needed
                outputFile += (msgOutput ? ".msg" : ".xml");
            }
            if (!msgOutput)
                generateXmlFile(outputFile.c_str(), maybeUnwrap(tree), opt_srcloc);
            else
                generateMsgFiles(tree);
        }
    }

    if (opt_mergeoutput) {
        if (opt_forcemsg)
            throw opp_runtime_error("refusing to generate merged MSG file");
        std::string outputFile = opt_outputfile.empty() ? "out.xml" : opt_outputfile;
        generateXmlFile(outputFile.c_str(), mergedTree, opt_srcloc);
    }

    if (numSkipped > 0)
        throw opp_runtime_error("error in %d file(s)", numSkipped);
}

void MsgTool::prettyprintCommand(int argc, char **argv)
{
    // process options
    bool opt_makebackup = false;
    std::vector<std::string> msgfiles;

    for (int i = 0; i < argc; i++) {
        if (string(argv[i]) == "-k" || string(argv[i]) == "--bak")
            opt_makebackup = true;
        else if (string(argv[i]) == "-v")
            opt_verbose = true;
        else if (argv[i][0] == '-')
            throw opp_runtime_error("unknown option %s", argv[i]);
        else
            addAll(msgfiles, expandFileArg(argv[i]));
    }

    if (msgfiles.empty())
        std::cerr << "opp_msgtool: warning: no input files\n";

    if (opt_verbose)
        std::cout << "Pretty-printing " << msgfiles.size() << " file(s)\n";

    int numSkipped = 0;
    for (std::string msgfile : msgfiles) {
        MsgFileElement *tree = parseMsgFile(msgfile.c_str(), false);
        if (tree == nullptr)
            numSkipped++;
        else {
            if (opt_makebackup)
                renameFileToBak(msgfile.c_str());
            generateMsgFile(msgfile.c_str(), tree);
            delete tree;
        }
    }
    if (numSkipped > 0)
        throw opp_runtime_error("error in %d file(s)", numSkipped);
}

void MsgTool::validateCommand(int argc, char **argv)
{
    // process options
    std::vector<std::string> msgfiles;

    for (int i = 0; i < argc; i++) {
        if (string(argv[i]) == "-v")
            opt_verbose = true;
        else if (argv[i][0] == '-')
            throw opp_runtime_error("unknown option %s", argv[i]);
        else
            addAll(msgfiles, expandFileArg(argv[i]));
    }

    if (opt_verbose)
        std::cout << "Validating " << msgfiles.size() << " file(s)\n";

    if (msgfiles.empty())
        std::cerr << "opp_msgtool: Warning: no input files\n";

    int numFilesWithErrors = 0;
    for (std::string msgfile : msgfiles) {
        MsgElement *tree = parseMsgFile(msgfile.c_str(), false);
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

void MsgTool::generateCppCommand(int argc, char **argv)
{
    // process options
    const char *opt_suffix = nullptr;  // -s
    const char *opt_hdrsuffix = nullptr; // -t
    std::vector<std::string> opt_importpath; // -I
    bool opt_generatedependencies = false; // -MD
    std::string opt_dependenciesfile;  // -MF
    bool opt_phonytargets = false;     // -MP
    MsgCompilerOptions msg_options;
    std::vector<std::string> msgfiles;

    for (int i = 0; i < argc; i++) {
        if (!strncmp(argv[i], "-I", 2)) {
            const char *arg = argv[i]+2;
            if (!*arg) {
                if (++i == argc)
                    throw opp_runtime_error("unexpected end of arguments after %s", argv[i-1]);
                arg = argv[i];
            }
            opt_importpath.push_back(arg);
        }
        else if (!strcmp(argv[i], "-s")) {
            i++;
            if (i == argc)
                throw opp_runtime_error("unexpected end of arguments after -s");
            opt_suffix = argv[i];
        }
        else if (!strcmp(argv[i], "-t")) {
            i++;
            if (i == argc)
                throw opp_runtime_error("unexpected end of arguments after -t");
            opt_hdrsuffix = argv[i];
        }
        else if (!strncmp(argv[i], "-P", 2)) {
            if (argv[i][2])
                msg_options.exportDef = argv[i]+2;
            else {
                if (++i == argc)
                    throw opp_runtime_error("unexpected end of arguments after %s", argv[i-1]);
                msg_options.exportDef = argv[i];
            }
        }
        else if (!strcmp(argv[i], "--msg6")) {
            // this is the default and only mode since 6.0
        }
        else if (!strcmp(argv[i], "--msg4")) {
            throw opp_runtime_error("option is no longer supported: %s", argv[i]);
        }
        else if (!strcmp(argv[i], "-MD")) {
            opt_generatedependencies = true;
        }
        else if (!strcmp(argv[i], "-MF")) {
            if (++i == argc)
                throw opp_runtime_error("unexpected end of arguments after %s", argv[i-1]);
            opt_dependenciesfile = argv[i];
        }
        else if (!strcmp(argv[i], "-MP")) {
            opt_phonytargets = true;
        }
        else if (!strncmp(argv[i], "-X", 2)) {
            const char *arg = argv[i]+2;
            if (!*arg) {
                if (++i == argc)
                    throw opp_runtime_error("unexpected end of arguments after %s", argv[i-1]);
                arg = argv[i];
            }
            if (!strcmp(arg, "nc")) {
                msg_options.generateClasses = false;
            }
            else if (!strcmp(arg, "nd")) {
                msg_options.generateDescriptors = false;
            }
            else if (!strcmp(arg, "ns")) {
                msg_options.generateSettersInDescriptors = false;
            }
            else
                throw opp_runtime_error("unknown option -X %s", arg);
        }
        else if (string(argv[i]) == "-v")
            opt_verbose = true;
        else if (argv[i][0] == '-')
            throw opp_runtime_error("unknown option %s", argv[i]);
        else
            addAll(msgfiles, expandFileArg(argv[i]));
    }

    if (msgfiles.empty())
        std::cerr << "opp_msgtool: Warning: no input files\n";

    if (opt_verbose)
        std::cout << "Translating to C++ " << msgfiles.size() << " file(s)\n";

    int numFilesWithErrors = 0;
    for (std::string msgfile : msgfiles) {
        // parse
        MsgFileElement *tree = parseMsgFile(msgfile.c_str(), false);
        if (tree == nullptr) {
            numFilesWithErrors++;
            continue;
        }

        // generate output file names
        const char *suffix = opt_suffix;
        const char *hdrsuffix = opt_hdrsuffix;
        if (!suffix)
            suffix = "_m.cc";
        if (!hdrsuffix)
            hdrsuffix = "_m.h";
        std::string outccfname = removeFileExtension(msgfile.c_str()) + suffix;
        std::string outhfname = removeFileExtension(msgfile.c_str()) + hdrsuffix;

        // generate C++ code
        if (opt_verbose)
            cout << "writing " << outhfname << " and " << outccfname << "\n";
        ErrorStore errors;
        errors.setPrintToStderr(true);
        msg_options.importPath = opt_importpath;
        MsgCompiler generator(msg_options, &errors);
        std::set<std::string> dependencies;
        generator.generate(tree, outhfname.c_str(), outccfname.c_str(), dependencies);
        if (errors.containsError())
            numFilesWithErrors++;
        if (opt_generatedependencies)
            generateDependencies(opt_dependenciesfile.c_str(), msgfile.c_str(), outhfname.c_str(), outccfname.c_str(), dependencies, opt_phonytargets);
        delete tree;
    }

    if (numFilesWithErrors > 0)
        throw opp_runtime_error("error in %d file(s)", numFilesWithErrors);
}

int MsgTool::main(int argc, char **argv)
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
            generateCppCommand(argc-1, argv+1);
        else
            throw opp_runtime_error("unknown command or file name '%s'", command.c_str());
    }
    catch (exception& e) {
        cerr << "opp_msgtool: " << e.what() << endl;
        return 1;
    }
    return 0;
}

}  // namespace scave
}  // namespace omnetpp

using namespace omnetpp::nedxml;

int main(int argc, char **argv)
{
    MsgTool msgTool;
    return msgTool.main(argc, argv);
}


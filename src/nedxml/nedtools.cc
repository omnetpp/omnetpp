//==========================================================================
// nedtools.cc -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <fstream>

#include "common/fileutil.h"
#include "common/fileglobber.h"
#include "common/stringutil.h"
#include "common/opp_ctype.h"
#include "nedtools.h"

#include "errorstore.h"
#include "neddtdvalidator.h"
#include "msgdtdvalidator.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

void NedTools::repairNedAST(ASTNode *tree)
{
    while (true) {
        // try DTD validation, and find first problem
        ErrorStore errors;
        NedDtdValidator dtdValidator(&errors);
        dtdValidator.validate(tree);
        if (errors.empty())
            break;  // we're done
        ASTNode *errorNode = errors.errorContext(0);
        if (!errorNode)
            break;  // this shouldn't happen, but if it does we can't go on
        if (!errorNode->getParent())
            break;  // we can't help if root node is wrong

        // throw out problem node, and try again
        errorNode->getParent()->removeChild(errorNode);
    }
}

void NedTools::repairMsgAST(ASTNode *tree)
{
    while (true) {
        // try DTD validation, and find first problem
        ErrorStore errors;
        MsgDtdValidator dtdValidator(&errors);
        dtdValidator.validate(tree);
        if (errors.empty())
            break;  // we're done
        ASTNode *errorNode = errors.errorContext(0);
        if (!errorNode)
            break;  // this shouldn't happen, but if it does we can't go on
        if (!errorNode->getParent())
            break;  // we can't help if root node is wrong

        // throw out problem node, and try again
        errorNode->getParent()->removeChild(errorNode);
    }
}

inline bool isNedComponent(ASTNode *node)
{
    int type = node->getTagCode();
    return type == NED_SIMPLE_MODULE || type == NED_COMPOUND_MODULE || type == NED_CHANNEL ||
           type == NED_MODULE_INTERFACE || type == NED_CHANNEL_INTERFACE;
}

void NedTools::splitNedFiles(FilesElement *tree)
{
    FilesElement *tmpTree = new FilesElement();
    for (NedFileElement *fileNode = tree->getFirstNedFileChild(); fileNode; ) {

        // we'll generate new files into the directory of the original file
        std::string directory;
        std::string filename;
        splitFileName(fileNode->getFilename(), directory, filename);

        // go through NED components in the file, and create new NED files for them
        bool fileStripped = false;
        for (ASTNode *child = fileNode->getFirstChild(); child; ) {
            if (!isNedComponent(child)) {
                child = child->getNextSibling();
                continue;
            }

            // process NED component
            ASTNode *componentNode = child;
            const char *componentName = componentNode->getAttribute("name");

            // create new file for it
            NedFileElement *newFileNode = fileNode->dup();
            std::string newFileName = concatDirAndFile(directory.c_str(), componentName) + ".ned";
            newFileNode->setFilename(newFileName.c_str());
            tmpTree->appendChild(newFileNode);

            // copy comments, imports, etc. from old file
            for (ASTNode *child2 = fileNode->getFirstChild(); child2; child2 = child2->getNextSibling())
                if (!isNedComponent(child2))
                    newFileNode->appendChild(child2->dupTree());

            // move NED component into new file
            child = child->getNextSibling();  // adjust iterator
            fileNode->removeChild(componentNode);
            newFileNode->appendChild(componentNode);

            fileStripped = true;
        }

        // if components were stripped away from this file, discard the remains
        if (!fileStripped)
            fileNode = fileNode->getNextNedFileSibling();
        else {
            NedFileElement *strippedFileNode = fileNode;
            fileNode = strippedFileNode->getNextNedFileSibling();
            delete strippedFileNode;
        }
    }

    while (tmpTree->getFirstChild())
        tree->appendChild(tmpTree->removeChild(tmpTree->getFirstChild()));
}

void NedTools::printNedFileAsStringConstant(const char *symbol, const char *filename, const char *passphrase, std::ostream& out)
{
    std::ifstream infile(filename);
    std::string contents((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    if (!passphrase) {
        out << "static const char *" << symbol << " = R\"raw(" << contents << ")raw\";\n\n";
    }
    else {
        contents = opp_garble(contents, passphrase);
        out << "static const char " << symbol << "[] = {";
        for (size_t i = 0; i < contents.size(); i++) {
            if (i%20 == 0)
                out << "\n    ";
            out << (int)contents[i] << ",";
        }
        out << "};\n\n";
    }
}

void NedTools::generateCppSource(const char *cppfile, std::vector<std::string> nedfiles, const char *garblephrase)
{
    std::ofstream out(cppfile);
    if (out.fail())
        throw opp_runtime_error("Cannot open '%s' for write", cppfile);

    out << "//\n// generated file, do not edit\n//\n\n";
    out << "#include <omnetpp.h>\n\n";
    out << "using namespace omnetpp;\n\n";

    std::map<std::string,std::string> symbols;
    int i = 0;
    for (auto& nedfile : nedfiles) {
        std::stringstream ss;
        ss << "FILE" << (++i) << "_";
        for (char c : filenameOf(nedfile.c_str()))
            ss << (opp_isalnum(c) ? opp_toupper(c) : '_');
        ss << "_CONTENTS";
        std::string symbol = ss.str();

        printNedFileAsStringConstant(symbol.c_str(), nedfile.c_str(), garblephrase, out);
        symbols[nedfile] = symbol;
    }

    out << "EXECUTE_ON_STARTUP(\n";
    if (garblephrase)
        out << "    const char *passphrase = \"" << garblephrase << "\";\n";
    for (auto& filename : nedfiles) {
        out << "    embeddedNedFiles.push_back(EmbeddedNedFile(\"" << filename << "\", ";
        if (!garblephrase)
            out << symbols[filename];
        else {
            out << "std::string(" << symbols[filename] << ", sizeof(" << symbols[filename] << ")), passphrase";
        }
        out << "));\n";
    }
    out << ")\n";
    out.close();
    if (!out)
        throw opp_runtime_error("Error writing '%s'", cppfile);
}



}  // namespace nedxml
}  // namespace omnetpp


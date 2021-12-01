//=========================================================================
//  OPP_NEDTOOL.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2019 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_NEDXML_NEDTOOL_H
#define __OMNETPP_NEDXML_NEDTOOL_H

#include <string>
#include "nedxmldefs.h"

namespace omnetpp {
namespace nedxml {

class ASTNode;
class FilesElement;
class NedFileElement;

/**
 * Implements opp_nedtool.
 */
class NedTool
{
  protected:
    bool opt_verbose = false;
  protected:
    // utility methods
    std::vector<std::string> expandNedFolder(const char *fname);
    std::vector<std::string> expandFileArg(const char *arg);
    bool fileLooksLikeXml(const char *filename);
    NedFileElement *parseNedFile(const char *fname, bool opt_storesrc);
    ASTNode *parseXmlFile(const char *filename);
    FilesElement *wrapIntoFilesElement(ASTNode *tree);
    void moveChildren(ASTNode *source, ASTNode *target);
    void generateNedFile(const char *filename, NedFileElement *tree);
    void generateNedFiles(FilesElement *tree);
    void generateXmlFile(const char *filename, ASTNode *tree, bool srcloc);
    void renameFileToBak(const char *fname);

    // commands
    void helpCommand(int argc, char **argv);
    void printHelpPage(const std::string& page);
    void convertCommand(int argc, char **argv);
    void prettyprintCommand(int argc, char **argv);
    void validateCommand(int argc, char **argv);
    void generateCppCommand(int argc, char **argv);
  public:
    int main(int argc, char **argv);
};

}  // namespace nedxml
}  // namespace omnetpp

#endif

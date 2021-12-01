//=========================================================================
//  OPP_MSGTOOL.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2019 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_NEDXML_MSGTOOL_H
#define __OMNETPP_NEDXML_MSGTOOL_H

#include <string>
#include "nedxmldefs.h"

namespace omnetpp {
namespace nedxml {

class ASTNode;
class FilesElement;
class MsgFileElement;

/**
 * Implements opp_msgtool.
 */
class MsgTool
{
  protected:
    bool opt_verbose = false;
  protected:
    // utility methods
    std::vector<std::string> expandMsgFolder(const char *fname);
    std::vector<std::string> expandFileArg(const char *arg);
    bool fileLooksLikeXml(const char *filename);
    MsgFileElement *parseMsgFile(const char *fname, bool opt_storesrc);
    ASTNode *parseXmlFile(const char *filename);
    FilesElement *wrapIntoFilesElement(ASTNode *tree);
    void moveChildren(ASTNode *source, ASTNode *target);
    void generateMsgFile(const char *filename, MsgFileElement *tree);
    void generateMsgFiles(FilesElement *tree);
    void generateXmlFile(const char *filename, ASTNode *tree, bool srcloc);
    void generateDependencies(const char *depsfile, const char *msgfile, const char *hfile, const char *ccfile, const std::set<std::string>& dependencies, bool wantPhonytargets);
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

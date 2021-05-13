//==========================================================================
// nedtools.h -
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


#ifndef __OMNETPP_NEDXML_NEDTOOLS_H
#define __OMNETPP_NEDXML_NEDTOOLS_H

#include <string>
#include <vector>
#include "nedelements.h"

namespace omnetpp {
namespace nedxml {

/**
 * @brief Misc NED utilities
 *
 * @ingroup Validation
 */
class NEDXML_API NedTools
{
  private:
    static void doCollectNedFiles(std::vector<std::string>& result, const std::string& prefix);
    static void printNedFileAsStringConstant(const char *varname, const char *filename, const char *passphrase, std::ostream& out);

  public:
    /**
     * Tries to makes an ASTNode tree comply with the DTD by gradually
     * discarding elements that fail DTD validation. Success is not guaranteed.
     */
    static void repairNedAST(ASTNode *tree);

    /**
     * Tries to makes an ASTNode tree comply with the DTD by gradually
     * discarding elements that fail DTD validation. Success is not guaranteed.
     */
    static void repairMsgAST(ASTNode *tree);

    /**
     * Splits each NedFile under FilesElement to several files so that
     * every module or interface is placed into its own file.
     */
    static void splitNedFiles(FilesElement *tree);

    /**
     * Generate C++ source that embeds the content of the specified NED files,
     * and makes it available at runtime for simulations. The passphrase (optional)
     * causes the NED source to be garbled a little so that it not directly
     * readable inside the compiled binary.
     */
    static void generateCppSource(const char *cppfile, std::vector<std::string> nedfiles, const char *passphrase);

};

}  // namespace nedxml
}  // namespace omnetpp


#endif


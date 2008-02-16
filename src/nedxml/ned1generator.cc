//==========================================================================
//  NED1GENERATOR.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <string.h>
#include <string>
#include <sstream>
#include "ned1generator.h"
#include "stringutil.h"
#include "nedutil.h"

NAMESPACE_BEGIN

using std::ostream;

#define DEFAULTINDENT "            "

#define NED2FEATURE  "NED2 feature unsupported in NED1: "


void generateNED1(ostream& out, NEDElement *node, NEDErrorStore *e)
{
    NED1Generator nedgen(e);
    nedgen.generate(out, node, "");
}

//-----------------------------------------

#define OUT  (*outp)

NED1Generator::NED1Generator(NEDErrorStore *e)
{
    outp = NULL;
    indentsize = 4;
    errors = e;
}

NED1Generator::~NED1Generator()
{
}

void NED1Generator::setIndentSize(int indentsiz)
{
    indentsize = indentsiz;
}

void NED1Generator::generate(ostream& out, NEDElement *node, const char *indent)
{
    outp = &out;
    generateNedItem(node, indent, false);
    outp = NULL;
}

std::string NED1Generator::generate(NEDElement *node, const char *indent)
{
    std::stringstream ss;
    generate(ss, node, indent);
    return ss.str();
}

const char *NED1Generator::increaseIndent(const char *indent)
{
    // biggest possible indent: ~70 chars:
    static char spaces[] = "                                                                     ";

    // bump...
    int ilen = strlen(indent);
    if (ilen+indentsize <= (int)sizeof(spaces)-1)
        ilen+=indentsize;
    const char *newindent = spaces+sizeof(spaces)-1-ilen;
    return newindent;
}

const char *NED1Generator::decreaseIndent(const char *indent)
{
    return indent + indentsize;
}

//---------------------------------------------------------------------------

static bool _isNetwork(NEDElement *node)
{
    Assert(node->getTagCode()==NED_COMPOUND_MODULE || node->getTagCode()==NED_SIMPLE_MODULE);
    return NEDElementUtil::getLocalBoolProperty(node, "isNetwork");
}

//---------------------------------------------------------------------------

void NED1Generator::generateChildren(NEDElement *node, const char *indent, const char *arg)
{
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
        generateNedItem(child, indent, child==node->getLastChild(), arg);
}

void NED1Generator::generateChildrenWithType(NEDElement *node, int tagcode, const char *indent, const char *arg)
{
    // find last
    NEDElement *lastWithTag = NULL;
    for (NEDElement *child1=node->getFirstChild(); child1; child1=child1->getNextSibling())
        if (child1->getTagCode()==tagcode)
             lastWithTag = child1;

    // now the recursive call
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
        if (child->getTagCode()==tagcode)
            generateNedItem(child, indent, child==lastWithTag, arg);
}

static int isInVector(int a, int v[])
{
    for (int i=0; v[i]; i++)  // v[] is zero-terminated
        if (v[i]==a)
            return true;
    return false;
}

void NED1Generator::generateChildrenWithTypes(NEDElement *node, int tagcodes[], const char *indent, const char *arg)
{
    // find last
    NEDElement *lastWithTag = NULL;
    for (NEDElement *child1=node->getFirstChild(); child1; child1=child1->getNextSibling())
        if (isInVector(child1->getTagCode(), tagcodes))
             lastWithTag = child1;

    // now the recursive call
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
        if (isInVector(child->getTagCode(), tagcodes))
            generateNedItem(child, indent, child==lastWithTag, arg);
}

//---------------------------------------------------------------------------

void NED1Generator::printInheritance(NEDElement *node, const char *indent)
{
    // for network...endnetwork, print " : type", otherwise warn for any "extends" or "like"
    if (_isNetwork(node))
    {
        if (node->getNumChildrenWithTag(NED_INTERFACE_NAME)>0)
            errors->addWarning(node, NED2FEATURE "inheritance");
        NEDElement *extendsNode = node->getFirstChildWithTag(NED_EXTENDS);
        if (!extendsNode)
            errors->addWarning(node, "network must extend a module type");
        else
            OUT << " : " << ((ExtendsElement *)extendsNode)->getName();
    }
    else if (node->getFirstChildWithTag(NED_EXTENDS) || node->getFirstChildWithTag(NED_INTERFACE_NAME))
    {
        errors->addWarning(node, NED2FEATURE "inheritance");
    }
}

bool NED1Generator::hasExpression(NEDElement *node, const char *attr)
{
    if (!opp_isempty(node->getAttribute(attr)))
    {
        return true;
    }
    else
    {
        for (ExpressionElement *expr=(ExpressionElement *)node->getFirstChildWithTag(NED_EXPRESSION); expr; expr=expr->getNextExpressionSibling())
            if (!opp_isempty(expr->getTarget()) && !strcmp(expr->getTarget(),attr))
                return true;
        return false;
    }
}

void NED1Generator::printExpression(NEDElement *node, const char *attr, const char *indent)
{
    if (!opp_isempty(node->getAttribute(attr)))
    {
        OUT << node->getAttribute(attr);
    }
    else
    {
        for (ExpressionElement *expr=(ExpressionElement *)node->getFirstChildWithTag(NED_EXPRESSION); expr; expr=expr->getNextExpressionSibling())
            if (!opp_isempty(expr->getTarget()) && !strcmp(expr->getTarget(),attr))
                generateNedItem(expr, indent, false, NULL);
    }
}

void NED1Generator::printOptVector(NEDElement *node, const char *attr, const char *indent)
{
    if (hasExpression(node,attr))
    {
        OUT << "[";
        printExpression(node,attr,indent);
        OUT << "]";
    }
}

//---------------------------------------------------------------------------

static const char *getComment(NEDElement *node, const char *locId)
{
    CommentElement *comment = (CommentElement *)node->getFirstChildWithAttribute(NED_COMMENT, "locid", locId);
    return (comment && !opp_isempty(comment->getContent())) ? comment->getContent() : NULL;
}

static std::string formatComment(const char *comment, const char *indent, const char *defaultValue)
{
    if (!comment || !comment[0])
        return defaultValue;

    // indent each line of comment; also ensure that if last line contains
    // a comment (//), it gets terminated by newline. If indent==NULL,
    // keep original indent
    std::string ret;
    const char *curLine = comment;
    while (curLine[0])
    {
        const char *nextLine = strchr(curLine,'\n');
        if (!nextLine)
            nextLine = curLine + strlen(curLine);
        const char *commentStart = strstr(curLine, "//");
        if (!commentStart || commentStart>=nextLine)
            ret += "\n"; // no comment in that line -- just add newline
        else if (!indent)
            ret += std::string(curLine, nextLine) + "\n"; // use original indent
        else
            ret += indent + std::string(commentStart, nextLine) + "\n"; // indent the comment

        curLine = nextLine[0] ? nextLine+1 : nextLine; // +1: skip newline
    }
    return ret;
}

std::string NED1Generator::concatInnerComments(NEDElement *node)
{
    std::string ret;
    for (NEDElement *child=node->getFirstChildWithTag(NED_COMMENT); child; child = child->getNextSiblingWithTag(NED_COMMENT))
    {
        CommentElement *comment = (CommentElement *)child;
        if (!strcmp(comment->getLocid(), "inner"))
            ret += comment->getContent();
    }
    return ret;
}

std::string NED1Generator::getBannerComment(NEDElement *node, const char *indent)
{
    const char *comment = getComment(node, "banner");
    std::string innerComments = concatInnerComments(node);
    return formatComment(comment, indent, "") + formatComment(innerComments.c_str(), indent, "");
}

std::string NED1Generator::getRightComment(NEDElement *node)
{
    const char *comment = getComment(node, "right");
    return formatComment(comment, NULL, "\n");
}

std::string NED1Generator::getInlineRightComment(NEDElement *node)
{
    const char *comment = getComment(node, "right");
    return formatComment(comment, NULL, " ");
}

std::string NED1Generator::getTrailingComment(NEDElement *node)
{
    const char *comment = getComment(node, "trailing");
    return formatComment(comment, NULL, "\n");
}

//---------------------------------------------------------------------------

void NED1Generator::doNedfiles(FilesElement *node, const char *indent, bool, const char *)
{
    generateChildren(node, indent);
}

void NED1Generator::doNedfile(NedFileElement *node, const char *indent, bool, const char *)
{
    OUT << getBannerComment(node, indent);
    generateChildren(node, indent);
}

void NED1Generator::doImport(ImportElement *node, const char *indent, bool islast, const char *)
{
    // In NED2 style, it would be just these 2 lines:
    //OUT << getBannerComment(node, indent);
    //OUT << indent << "import \"" << node->getFilename() << "\";" << getRightComment(node);

    // but we want to merge all imports under a single "import" keyword
    if (!node->getPrevSibling() || node->getPrevSibling()->getTagCode()!=NED_IMPORT)
    {
        OUT << indent << "import\n";
    }

    OUT << getBannerComment(node, indent);
    bool isLastImport = !node->getNextSibling() || node->getNextSibling()->getTagCode()!=NED_IMPORT;
    OUT << increaseIndent(indent) << "\"" << node->getImportSpec() << "\"";
    OUT << (isLastImport ? ";" : ",") << getRightComment(node);
}

void NED1Generator::doPropertyDecl(PropertyDeclElement *node, const char *indent, bool islast, const char *)
{
    errors->addWarning(node, NED2FEATURE "properties");
}

void NED1Generator::doExtends(ExtendsElement *node, const char *indent, bool islast, const char *sep)
{
    INTERNAL_ERROR0(node,"should never come here");
}

void NED1Generator::doInterfaceName(InterfaceNameElement *node, const char *indent, bool islast, const char *sep)
{
    INTERNAL_ERROR0(node,"should never come here");
}

void NED1Generator::doSimpleModule(SimpleModuleElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "simple " << node->getName();
    printInheritance(node, indent);
    OUT << getRightComment(node);

    generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));
    generateChildrenWithType(node, NED_GATES, increaseIndent(indent));

    OUT << indent << "endsimple" << getTrailingComment(node);
}

void NED1Generator::doModuleInterface(ModuleInterfaceElement *node, const char *indent, bool islast, const char *)
{
    errors->addWarning(node, NED2FEATURE "interfaces");
}

void NED1Generator::doCompoundModule(CompoundModuleElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << (_isNetwork(node) ? "network" : "module") << " " << node->getName();
    printInheritance(node, indent);
    OUT << getRightComment(node);

    generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));
    generateChildrenWithType(node, NED_GATES, increaseIndent(indent));
    generateChildrenWithType(node, NED_TYPES, increaseIndent(indent));
    generateChildrenWithType(node, NED_SUBMODULES, increaseIndent(indent));
    generateChildrenWithType(node, NED_CONNECTIONS, increaseIndent(indent));

    PropertyElement *displayProp;
    std::string dispstr = getDisplayStringOf(node, displayProp);
    if (!dispstr.empty())
    {
        try {
            dispstr = DisplayStringUtil::toOldBackgroundDisplayString(opp_parsequotedstr(dispstr.c_str()).c_str());
            OUT << getBannerComment(displayProp, increaseIndent(indent));
            OUT << increaseIndent(indent) << "display: " << opp_quotestr(dispstr.c_str()) << ";" << getRightComment(displayProp);
        }
        catch (std::exception& e) {
            errors->addWarning(node, opp_stringf("error converting display string: %s", e.what()).c_str());
        }
    }

    OUT << indent << (_isNetwork(node) ? "endnetwork" : "endmodule") << getTrailingComment(node);
}

void NED1Generator::doChannelInterface(ChannelInterfaceElement *node, const char *indent, bool islast, const char *)
{
    errors->addWarning(node, NED2FEATURE "channel interfaces");
}

void NED1Generator::doChannel(ChannelElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "channel ";
    OUT << node->getName();
    printInheritance(node, indent);
    OUT << getRightComment(node);

    generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));

    OUT << indent << "endchannel" << getTrailingComment(node);
}

void NED1Generator::doParameters(ParametersElement *node, const char *indent, bool islast, const char *)
{
    // in NED-1, parameters followed different syntaxes at different places
    int parentTag = node->getParent()->getTagCode();
    if (parentTag==NED_SUBMODULE || _isNetwork(node->getParent()))
        doSubstParameters(node, indent);
    else if (parentTag==NED_SIMPLE_MODULE || parentTag==NED_COMPOUND_MODULE)
        doModuleParameters(node, indent);
    else if (parentTag==NED_CHANNEL)
        doChannelParameters(node, indent);
    else if (parentTag==NED_CHANNEL_SPEC)
        doConnectionAttributes(node, indent);
    else
        INTERNAL_ERROR0(node,"unexpected parameters section");
}

void NED1Generator::doModuleParameters(ParametersElement *node, const char *indent)
{
    OUT << getBannerComment(node, indent);
    if (node->getFirstChildWithTag(NED_PARAM))
        OUT << indent << "parameters:" << getRightComment(node);

    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
    {
        int childTag = child->getTagCode();
        if (childTag==NED_COMMENT)
            ; // ignore whitespace
        else if (childTag==NED_PROPERTY)
            doProperty((PropertyElement *)child, increaseIndent(indent), false, NULL);
        else if (childTag==NED_PARAM)
            doModuleParam((ParamElement *)child, increaseIndent(indent), child->getNextSiblingWithTag(NED_PARAM)==NULL, NULL);
        else if (childTag==NED_PATTERN)
            errors->addWarning(node, NED2FEATURE "assigment by pattern matching");
        else
            INTERNAL_ERROR0(node,"unexpected element");
    }
}

void NED1Generator::doSubstParameters(ParametersElement *node, const char *indent)
{
    OUT << getBannerComment(node, indent);
    if (node->getFirstChildWithTag(NED_PARAM))
        OUT << indent << "parameters:" << getRightComment(node);

    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
    {
        int childTag = child->getTagCode();
        if (childTag==NED_COMMENT)
            ; //ignore
        else if (childTag==NED_PROPERTY)
            doProperty((PropertyElement *)child, increaseIndent(indent), false, NULL);
        else if (childTag==NED_PARAM)
            doSubstParam((ParamElement *)child, increaseIndent(indent), child->getNextSiblingWithTag(NED_PARAM)==NULL, NULL);
        else if (childTag==NED_PATTERN)
            errors->addWarning(node, NED2FEATURE "assignment by pattern matching");
        else
            INTERNAL_ERROR0(node,"unexpected element");
    }
}

void NED1Generator::doChannelParameters(ParametersElement *node, const char *indent)
{
    // only "delay", "error", "datarate" parameters need to be recognized
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
    {
        int childTag = child->getTagCode();
        if (childTag==NED_COMMENT)
            ; //ignore
        else if (childTag==NED_PROPERTY)
            doProperty((PropertyElement *)child, indent, false, NULL);
        else if (childTag==NED_PARAM)
            doChannelParam((ParamElement *)child, indent);
        else if (childTag==NED_PATTERN)
            errors->addWarning(node, NED2FEATURE "assignment by pattern matching");
        else
            INTERNAL_ERROR0(node,"unexpected element");
    }
}

void NED1Generator::doConnectionAttributes(ParametersElement *node, const char *indent)
{
    // only "delay", "error", "datarate" parameters need to be recognized
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
    {
        int childTag = child->getTagCode();
        if (childTag==NED_COMMENT)
            ; //ignore
        else if (childTag==NED_PROPERTY)
            doProperty((PropertyElement *)child, indent, false, NULL);
        else if (childTag==NED_PARAM)
            doChannelParam((ParamElement *)child, NULL);
        else if (childTag==NED_PATTERN)
            errors->addWarning(node, NED2FEATURE "patterns");
        else
            INTERNAL_ERROR0(node,"unexpected element");
    }
}

void NED1Generator::doChannelParam(ParamElement *node, const char *indent)
{
    // this is used both in channel definitions and in connections
    OUT << getBannerComment(node, indent);
    const char *name = node->getName();
    if (strcmp(name, "delay")==0 || strcmp(name, "error")==0 || strcmp(name, "datarate")==0)
    {
        // indent==NULL means no indent and no new line at end (but a space at front)
        OUT << (indent ? indent : " ");
        OUT << node->getName() << " ";
        printExpression(node, "value", indent);
        if (indent)
            OUT << ";" << getRightComment(node);
    }
    else
    {
        errors->addWarning(node, NED2FEATURE "channel parameters other than delay, error and datarate");
    }
}

void NED1Generator::doParam(ParamElement *node, const char *indent, bool islast, const char *)
{
    // we use doModuleParam() and doSubstParam() instead
    // doParameters() ensures we never get here
    INTERNAL_ERROR0(node, "should never get here");
}

void NED1Generator::doModuleParam(ParamElement *node, const char *indent, bool islast, const char *)
{
    const char *parType = NULL;
    switch (node->getType())
    {
        case NED_PARTYPE_NONE:   break;
        case NED_PARTYPE_DOUBLE: case NED_PARTYPE_INT:
                                 parType = node->getIsVolatile() ? "numeric" : "numeric const"; break;
        case NED_PARTYPE_STRING: parType = "string"; break;
        case NED_PARTYPE_BOOL:   parType = "bool"; break;
        case NED_PARTYPE_XML:    parType = "xml"; break;
        default: INTERNAL_ERROR0(node, "wrong type");
    }

    OUT << getBannerComment(node, indent);
    OUT << indent << node->getName();
    if (parType!=NULL)
        OUT << ": " << parType;

    if (hasExpression(node,"value"))
        errors->addWarning(node, NED2FEATURE "assignment in parameter declaration");

    const char *subindent = indent ? increaseIndent(indent) : DEFAULTINDENT;
    generateChildrenWithType(node, NED_PROPERTY, subindent, " ");

    OUT << (islast ? ";" : ",") << getRightComment(node);
}

const char *NED1Generator::getPromptTextOf(ParamElement *param)
{
    PropertyElement *promptProp = (PropertyElement *)param->getFirstChildWithAttribute(NED_PROPERTY, "name", "prompt");
    if (!promptProp)
        return NULL;
    PropertyKeyElement *propKey = (PropertyKeyElement *)promptProp->getFirstChildWithAttribute(NED_PROPERTY_KEY, "name", "");
    if (!propKey)
        return NULL;
    LiteralElement *literal = (LiteralElement *)propKey->getFirstChildWithTag(NED_LITERAL);
    if (!literal)
        return NULL;
    return literal->getText(); //FIXME use value if text is not present!!!! (see doLiteral())
}

void NED1Generator::doSubstParam(ParamElement *node, const char *indent, bool islast, const char *)
{
    if (node->getType()!=NED_PARTYPE_NONE)
        errors->addWarning(node, NED2FEATURE "defining new parameter for a submodule");

    OUT << getBannerComment(node, indent);
    OUT << indent << node->getName() << " = ";

    if (!hasExpression(node, "value"))
    {
        OUT << "input";
    }
    else if (!node->getIsDefault())
    {
        printExpression(node, "value", indent);
    }
    else
    {
        OUT << "input(";
        printExpression(node, "value", indent);
        const char *prompt = getPromptTextOf(node);
        if (prompt)
            OUT << ", " << prompt;
        OUT << ")";
    }

    generateChildrenWithType(node, NED_PROPERTY, increaseIndent(indent), " ");

    OUT << (islast ? ";" : ",") << getRightComment(node);
}

void NED1Generator::doPattern(PatternElement *node, const char *indent, bool islast, const char *)
{
    errors->addWarning(node, NED2FEATURE "assignment by pattern matching");
}

void NED1Generator::doProperty(PropertyElement *node, const char *indent, bool islast, const char *sep)
{
    // issue a warning, except for those few accepted occurrences of @display and @prompt
    // note: no code needs to be generated here, that is done separately
    if (strcmp(node->getName(), "display")==0)
    {
        // must be submodule->parameters->property, module->parameters->property or
        // connection->chanspec->parameters->property
        NEDElement *grandparent = node->getParent() ? node->getParent()->getParent() : NULL;
        if (!grandparent || (grandparent->getTagCode()!=NED_SUBMODULE && grandparent->getTagCode()!=NED_COMPOUND_MODULE && grandparent->getTagCode()!=NED_CHANNEL_SPEC))
            errors->addWarning(node, NED2FEATURE "@display may occur on submodules, connections and compound modules only");
    }
    else if (strcmp(node->getName(), "prompt")==0)
    {
        int parentTag = node->getParent()->getTagCode();
        if (parentTag!=NED_PARAM)
            errors->addWarning(node, NED2FEATURE "@prompt may occur in submodule parameter assigments and networks only");
    }
    else
        errors->addWarning(node, NED2FEATURE "property (except @display and @prompt)");
}

void NED1Generator::doPropertyKey(PropertyKeyElement *node, const char *indent, bool islast, const char *sep)
{
    INTERNAL_ERROR0(node,"should never come here");
}

void NED1Generator::doGates(GatesElement *node, const char *indent, bool islast, const char *)
{
    // in NED-1, distingish between "gates" and "gatesizes"
    int parentTag = node->getParent()->getTagCode();
    if (parentTag==NED_SUBMODULE)
        doSubmoduleGatesizes(node, indent);
    else if (parentTag==NED_SIMPLE_MODULE || parentTag==NED_COMPOUND_MODULE)
        doModuleGates(node, indent);
    else
        INTERNAL_ERROR0(node,"unexpected gates section");
}

void NED1Generator::doModuleGates(GatesElement *node, const char *indent)
{
    OUT << getBannerComment(node, indent);
    if (node->getFirstChildWithTag(NED_GATE))
        OUT << indent << "gates:" << getRightComment(node);

    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
    {
        int childTag = child->getTagCode();
        if (childTag==NED_COMMENT)
            ; // ignore whitespace
        else if (childTag==NED_GATE)
            doModuleGate((GateElement *)child, increaseIndent(indent), child->getNextSiblingWithTag(NED_GATE)==NULL, NULL);
        else
            INTERNAL_ERROR0(node,"unexpected element");
    }
}

void NED1Generator::doSubmoduleGatesizes(GatesElement *node, const char *indent)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "gatesizes:" << getRightComment(node);

    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
    {
        int childTag = child->getTagCode();
        if (childTag==NED_COMMENT)
            ; //ignore
        else if (childTag==NED_GATE)
            doGatesize((GateElement *)child, increaseIndent(indent), child->getNextSiblingWithTag(NED_GATE)==NULL, NULL);
        else
            INTERNAL_ERROR0(node,"unexpected element");
    }
}

void NED1Generator::doGate(GateElement *node, const char *indent, bool islast, const char *)
{
    // we use doModuleGate()/doGatesize() instead
    // doGates() ensures we never get here
    INTERNAL_ERROR0(node, "should never get here");
}

void NED1Generator::doModuleGate(GateElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent;
    switch (node->getType())
    {
        case NED_GATETYPE_INPUT:  OUT << "in: "; break;
        case NED_GATETYPE_OUTPUT: OUT << "out: "; break;
        case NED_GATETYPE_INOUT:  errors->addWarning(node, NED2FEATURE "inout gate"); break;
        case NED_GATETYPE_NONE:   errors->addWarning(node, NED2FEATURE "missing gate type"); break;
        default: INTERNAL_ERROR0(node, "wrong type");
    }
    OUT << node->getName();
    if (node->getIsVector())
        OUT << "[]";
    if (hasExpression(node, "vector-size"))
        errors->addWarning(node, NED2FEATURE "gate vector size in gate declaration");
    generateChildrenWithType(node, NED_PROPERTY, increaseIndent(indent), " ");
    OUT << ";" << getRightComment(node);
}

void NED1Generator::doGatesize(GateElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent;
    if (node->getType()!=NED_GATETYPE_NONE)
        errors->addWarning(node, NED2FEATURE "declaring new gates for submodules");
    OUT << node->getName();
    if (!hasExpression(node, "vector-size"))
        errors->addWarning(node, NED2FEATURE "missing gate size");
    printOptVector(node, "vector-size",indent);

    generateChildrenWithType(node, NED_PROPERTY, increaseIndent(indent), " ");
    OUT << (islast ? ";" : ",") << getRightComment(node);
}

void NED1Generator::doTypes(TypesElement *node, const char *indent, bool islast, const char *)
{
    errors->addWarning(node, NED2FEATURE "inner type");
}

void NED1Generator::doSubmodules(SubmodulesElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "submodules:" << getRightComment(node);
    generateChildren(node, increaseIndent(indent));
}

void NED1Generator::doSubmodule(SubmoduleElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << node->getName() << ": ";

    if (!opp_isempty(node->getLikeType()))
    {
        // "like" version
        printExpression(node, "like-param", indent); // this (incidentally) also works if like-param contains a property (ie. starts with "@")
        printOptVector(node, "vector-size",indent);
        OUT << " like " << node->getLikeType();
    }
    else
    {
        // "like"-less
        OUT << node->getType();
        printOptVector(node, "vector-size",indent);
    }
    OUT << ";" << getRightComment(node);

    generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));
    generateChildrenWithType(node, NED_GATES, increaseIndent(indent));

    PropertyElement *displayProp;
    std::string dispstr = getDisplayStringOf(node, displayProp);
    if (!dispstr.empty())
    {
        try {
            dispstr = DisplayStringUtil::toOldSubmoduleDisplayString(opp_parsequotedstr(dispstr.c_str()).c_str());
            OUT << getBannerComment(displayProp, increaseIndent(indent));
            OUT << increaseIndent(indent) << "display: " << opp_quotestr(dispstr.c_str()) << ";" << getRightComment(displayProp);
        }
        catch (std::exception& e) {
            errors->addWarning(node, opp_stringf("error converting display string: %s", e.what()).c_str());
        }
    }
}

std::string NED1Generator::getDisplayStringOf(NEDElement *node, PropertyElement *&outDisplayProp)
{
    // node must be a module, submodule or a connection-spec
    outDisplayProp = NULL;
    ParametersElement *parameters = (ParametersElement *)node->getFirstChildWithTag(NED_PARAMETERS);
    if (!parameters)
        return "";
    PropertyElement *displayProp = (PropertyElement *)parameters->getFirstChildWithAttribute(NED_PROPERTY, "name", "display");
    if (!displayProp)
        return "";
    PropertyKeyElement *propKey = (PropertyKeyElement *)displayProp->getFirstChildWithAttribute(NED_PROPERTY_KEY, "name", "");
    if (!propKey)
        return "";
    LiteralElement *literal = (LiteralElement *)propKey->getFirstChildWithTag(NED_LITERAL);
    if (!literal)
        return "";
    outDisplayProp = displayProp;
    return !opp_isempty(literal->getText()) ? literal->getText() : opp_quotestr(literal->getValue());
}

void NED1Generator::doConnections(ConnectionsElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    if (node->getAllowUnconnected())
        OUT << indent << "connections nocheck:" << getRightComment(node);
    else
        OUT << indent << "connections:" << getRightComment(node);
    generateChildren(node, increaseIndent(indent));
}

void NED1Generator::doConnection(ConnectionElement *node, const char *indent, bool islast, const char *)
{
    //  direction
    const char *arrow;
    bool srcfirst;
    switch (node->getArrowDirection())
    {
        case NED_ARROWDIR_L2R:   arrow = " -->"; srcfirst = true; break;
        case NED_ARROWDIR_R2L:   arrow = " <--"; srcfirst = false; break;
        case NED_ARROWDIR_BIDIR: errors->addWarning(node, NED2FEATURE "two-way connection");
                                 arrow = " <-->"; srcfirst = true; break;
        default: INTERNAL_ERROR0(node, "wrong arrow-dir");
    }

    OUT << getBannerComment(node, indent);

    // print src
    OUT << indent;
    if (srcfirst)
        printConnectionGate(node, node->getSrcModule(), "src-module-index", node->getSrcGate(), "src-gate-index", node->getSrcGatePlusplus(), node->getSrcGateSubg(), indent);
    else
        printConnectionGate(node, node->getDestModule(), "dest-module-index", node->getDestGate(), "dest-gate-index", node->getDestGatePlusplus(), node->getDestGateSubg(), indent);

    // arrow
    OUT << arrow;

    // print channel attributes
    if (node->getFirstChildWithTag(NED_CHANNEL_SPEC))
    {
        generateChildrenWithType(node, NED_CHANNEL_SPEC, indent, arrow);
    }

    // print dest
    OUT << " ";
    if (srcfirst)
        printConnectionGate(node, node->getDestModule(), "dest-module-index", node->getDestGate(), "dest-gate-index", node->getDestGatePlusplus(), node->getDestGateSubg(), indent);
    else
        printConnectionGate(node, node->getSrcModule(), "src-module-index", node->getSrcGate(), "src-gate-index", node->getSrcGatePlusplus(), node->getSrcGateSubg(), indent);

    if (node->getFirstChildWithTag(NED_LOOP))
        errors->addWarning(node, NED2FEATURE "per-connection `for'");

    if (node->getNumChildrenWithTag(NED_CONDITION)>1)
        errors->addWarning(node, NED2FEATURE "more than one `if' per-connection");

    NEDElement *condition = node->getFirstChildWithTag(NED_CONDITION);
    if (condition)
        doCondition((ConditionElement *)condition, indent, false, NULL);

    // display string
    NEDElement *chanSpecNode = node->getFirstChildWithTag(NED_CHANNEL_SPEC);
    if (chanSpecNode)
    {
        PropertyElement *dummy;
        std::string dispstr = getDisplayStringOf(chanSpecNode, dummy);
        if (!dispstr.empty())
        {
            try {
                dispstr = DisplayStringUtil::toOldConnectionDisplayString(opp_parsequotedstr(dispstr.c_str()).c_str());
                OUT << " display " << opp_quotestr(dispstr.c_str());
            }
            catch (std::exception& e) {
                errors->addWarning(node, opp_stringf("error converting display string: %s", e.what()).c_str());
            }
        }
    }

    OUT << ";" << getRightComment(node);
}

void NED1Generator::doChannelSpec(ChannelSpecElement *node, const char *indent, bool islast, const char *arrow)
{
    NEDElement *params = node->getFirstChildWithTag(NED_PARAMETERS);
    bool hasParams = params && params->getFirstChildWithTag(NED_PARAM);

    if (!opp_isempty(node->getLikeType()))
    {
        errors->addWarning(node, NED2FEATURE "channel `like'");
    }
    else if (!opp_isempty(node->getType()))
    {
        // concrete channel type
        OUT << " " << node->getType() << arrow;

        if (hasParams)
            errors->addWarning(node, NED2FEATURE "channel spec with parameters");
    }
    else if (node->getFirstChildWithTag(NED_PARAMETERS))
    {
        generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));
        if (hasParams)
            OUT << arrow;
    }
}

void NED1Generator::doConnectionGroup(ConnectionGroupElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "for ";
    generateChildrenWithType(node, NED_LOOP, increaseIndent(indent), ", ");
    OUT << " do" << getRightComment(node);

    generateChildrenWithType(node, NED_CONNECTION, increaseIndent(indent));

    OUT << indent << "endfor;" << getTrailingComment(node);
}

void NED1Generator::doLoop(LoopElement *node, const char *indent, bool islast, const char *sep)
{
    OUT << node->getParamName() << "=";
    printExpression(node, "from-value",indent);
    OUT << "..";
    printExpression(node, "to-value",indent);

    if (!islast)
        OUT << (sep ? sep : "");
}

void NED1Generator::doCondition(ConditionElement *node, const char *indent, bool islast, const char *sep)
{
    OUT << " if ";
    printExpression(node, "condition",indent);
    if (!islast)
        OUT << (sep ? sep : "");
}

void NED1Generator::printConnectionGate(NEDElement *conn, const char *modname, const char *modindexattr,
                                        const char *gatename, const char *gateindexattr, bool isplusplus,
                                        int gatesubg, const char *indent)
{
    if (!opp_isempty(modname)) {
        OUT << modname;
        printOptVector(conn, modindexattr,indent);
        OUT << ".";
    }

    OUT << gatename;
    if (isplusplus)
        OUT << "++";
    else
        printOptVector(conn, gateindexattr,indent);

    if (gatesubg!=NED_SUBGATE_NONE)
        errors->addWarning(conn, NED2FEATURE "subgate of inout gate");
}

void NED1Generator::doExpression(ExpressionElement *node, const char *indent, bool islast, const char *)
{
    generateChildren(node,indent);
}

int NED1Generator::getOperatorPriority(const char *op, int args)
{
    //
    // this method should always contain the same precendence rules as ebnf.y
    //

    if (args==3)
    {
        // %left '?' ':'
        if (!strcmp(op,"?:")) return 1;
    }

    if (args==2)
    {
        // %left AND OR XOR
        if (!strcmp(op,"&&")) return 2;
        if (!strcmp(op,"||")) return 2;
        if (!strcmp(op,"##")) return 2;

        // %left EQ NE GT GE LS LE
        if (!strcmp(op,"==")) return 3;
        if (!strcmp(op,"!=")) return 3;
        if (!strcmp(op,"<"))  return 3;
        if (!strcmp(op,">"))  return 3;
        if (!strcmp(op,"<=")) return 3;
        if (!strcmp(op,">=")) return 3;

        // %left BIN_AND BIN_OR BIN_XOR
        if (!strcmp(op,"&"))  return 4;
        if (!strcmp(op,"|"))  return 4;
        if (!strcmp(op,"#"))  return 4;

        // %left SHIFT_LEFT SHIFT_RIGHT
        if (!strcmp(op,"<<")) return 5;
        if (!strcmp(op,">>")) return 5;

        // %left PLUS MIN
        if (!strcmp(op,"+"))  return 6;
        if (!strcmp(op,"-"))  return 6;

        // %left MUL DIV MOD
        if (!strcmp(op,"*"))  return 7;
        if (!strcmp(op,"/"))  return 7;
        if (!strcmp(op,"%"))  return 7;

        // %right EXP
        if (!strcmp(op,"^"))  return 8;
    }

    if (args==1)
    {
        // %left UMIN NOT BIN_COMPL
        if (!strcmp(op,"-"))  return 9;
        if (!strcmp(op,"!"))  return 9;
        if (!strcmp(op,"~"))  return 9;
    }
    INTERNAL_ERROR1(NULL, "getOperatorPriority(): unknown operator '%s'", op);
    return -1;
}

bool NED1Generator::isOperatorLeftAssoc(const char *op)
{
    // only exponentiation is right assoc, all others are left assoc
    if (!strcmp(op,"^")) return false;
    return true;
}

void NED1Generator::doOperator(OperatorElement *node, const char *indent, bool islast, const char *)
{
    NEDElement *op1 = node->getFirstChild();
    NEDElement *op2 = op1 ? op1->getNextSibling() : NULL;
    NEDElement *op3 = op2 ? op2->getNextSibling() : NULL;

    if (!op2)
    {
        // unary
        OUT << node->getName();
        generateNedItem(op1,indent,false,NULL);
    }
    else if (!op3)
    {
        // binary
        int prio = getOperatorPriority(node->getName(), 2);
        //bool leftassoc = isOperatorLeftAssoc(node->getName());

        bool needsParen = false;
        bool spacious = (prio<=2);  // we want spaces around &&, ||, ##

        NEDElement *parent = node->getParent();
        if (parent && parent->getTagCode()==NED_OPERATOR) {
            OperatorElement *parentop = (OperatorElement *)parent;
            int parentprio = getOperatorPriority(parentop->getName(),2 ); //FIXME 2 ???
            if (parentprio>prio) {
                needsParen = true;
            } else if (parentprio==prio) {
                // TBD can be refined (make use of associativity & precedence rules)
                needsParen = true;
            }
        }

        if (needsParen) OUT << "(";
        generateNedItem(op1,indent,false,NULL);
        if (spacious)
            OUT << " " << node->getName() << " ";
        else
            OUT << node->getName();
        generateNedItem(op2,indent,false,NULL);
        if (needsParen) OUT << ")";
    }
    else
    {
        // tertiary
        bool needsParen = true; // TBD can be refined
        bool spacious = true; // TBD can be refined

        if (needsParen) OUT << "(";
        generateNedItem(op1,indent,false,NULL);
        OUT << (spacious ? " ? " : "?");
        generateNedItem(op2,indent,false,NULL);
        OUT << (spacious ? " : " : ":");
        generateNedItem(op3,indent,false,NULL);
        if (needsParen) OUT << ")";
    }
}

void NED1Generator::doFunction(FunctionElement *node, const char *indent, bool islast, const char *)
{
    NEDElement *op1 = node->getFirstChild();
    NEDElement *op2 = op1 ? op1->getNextSibling() : NULL;
    NEDElement *op3 = op2 ? op2->getNextSibling() : NULL;

    if (!strcmp(node->getName(), "index")) {
        OUT << node->getName();  // 'index' doesn't need parentheses
        return;
    }

    OUT << node->getName() << "(";
    if (op1) {
        generateNedItem(op1,indent,false,NULL);
    }
    if (op2) {
        OUT << ", ";
        generateNedItem(op2,indent,false,NULL);
    }
    if (op3) {
        OUT << ", ";
        generateNedItem(op3,indent,false,NULL);
    }
    OUT << ")";
}

void NED1Generator::doIdent(IdentElement *node, const char *indent, bool islast, const char *)
{
    if (!opp_isempty(node->getModule())) {
        OUT << node->getModule();
        printOptVector(node, "module-index", indent);
        OUT << ".";
    }

    OUT << node->getName();
}

void NED1Generator::doLiteral(LiteralElement *node, const char *indent, bool islast, const char *)
{
    if (!opp_isempty(node->getText()))
    {
        OUT << node->getText();
    }
    else
    {
        // fallback: when original text is not present, use value
        if (node->getType()==NED_CONST_STRING)
            OUT << opp_quotestr(node->getValue());
        else
            OUT << node->getValue();
    }
}

//---------------------------------------------------------------------------

void NED1Generator::generateNedItem(NEDElement *node, const char *indent, bool islast, const char *arg)
{
    // dispatch
    int tagcode = node->getTagCode();
    switch (tagcode)
    {
        case NED_FILES:
            doNedfiles((FilesElement *)node, indent, islast, arg); break;
        case NED_NED_FILE:
            doNedfile((NedFileElement *)node, indent, islast, arg); break;
        case NED_IMPORT:
            doImport((ImportElement *)node, indent, islast, arg); break;
        case NED_PROPERTY_DECL:
            doPropertyDecl((PropertyDeclElement *)node, indent, islast, arg); break;
        case NED_EXTENDS:
            doExtends((ExtendsElement *)node, indent, islast, arg); break;
        case NED_INTERFACE_NAME:
            doInterfaceName((InterfaceNameElement *)node, indent, islast, arg); break;
        case NED_SIMPLE_MODULE:
            doSimpleModule((SimpleModuleElement *)node, indent, islast, arg); break;
        case NED_MODULE_INTERFACE:
            doModuleInterface((ModuleInterfaceElement *)node, indent, islast, arg); break;
        case NED_COMPOUND_MODULE:
            doCompoundModule((CompoundModuleElement *)node, indent, islast, arg); break;
        case NED_CHANNEL_INTERFACE:
            doChannelInterface((ChannelInterfaceElement *)node, indent, islast, arg); break;
        case NED_CHANNEL:
            doChannel((ChannelElement *)node, indent, islast, arg); break;
        case NED_PARAMETERS:
            doParameters((ParametersElement *)node, indent, islast, arg); break;
        case NED_PARAM:
            doParam((ParamElement *)node, indent, islast, arg); break;
        case NED_PATTERN:
            doPattern((PatternElement *)node, indent, islast, arg); break;
        case NED_PROPERTY:
            doProperty((PropertyElement *)node, indent, islast, arg); break;
        case NED_PROPERTY_KEY:
            doPropertyKey((PropertyKeyElement *)node, indent, islast, arg); break;
        case NED_GATES:
            doGates((GatesElement *)node, indent, islast, arg); break;
        case NED_GATE:
            doGate((GateElement *)node, indent, islast, arg); break;
        case NED_TYPES:
            doTypes((TypesElement *)node, indent, islast, arg); break;
        case NED_SUBMODULES:
            doSubmodules((SubmodulesElement *)node, indent, islast, arg); break;
        case NED_SUBMODULE:
            doSubmodule((SubmoduleElement *)node, indent, islast, arg); break;
        case NED_CONNECTIONS:
            doConnections((ConnectionsElement *)node, indent, islast, arg); break;
        case NED_CONNECTION:
            doConnection((ConnectionElement *)node, indent, islast, arg); break;
        case NED_CHANNEL_SPEC:
            doChannelSpec((ChannelSpecElement *)node, indent, islast, arg); break;
        case NED_CONNECTION_GROUP:
            doConnectionGroup((ConnectionGroupElement *)node, indent, islast, arg); break;
        case NED_LOOP:
            doLoop((LoopElement *)node, indent, islast, arg); break;
        case NED_CONDITION:
            doCondition((ConditionElement *)node, indent, islast, arg); break;
        case NED_EXPRESSION:
            doExpression((ExpressionElement *)node, indent, islast, arg); break;
        case NED_OPERATOR:
            doOperator((OperatorElement *)node, indent, islast, arg); break;
        case NED_FUNCTION:
            doFunction((FunctionElement *)node, indent, islast, arg); break;
        case NED_IDENT:
            doIdent((IdentElement *)node, indent, islast, arg); break;
        case NED_LITERAL:
            doLiteral((LiteralElement *)node, indent, islast, arg); break;
        default:
            ;//XXX: add back this line: INTERNAL_ERROR1(node, "generateNedItem(): unknown tag '%s'", node->getTagName());
    }
}

NAMESPACE_END



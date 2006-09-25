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
#include "nederror.h"

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

inline bool strnotnull(const char *s)
{
    return s && s[0];
}

static bool _hasSiblingBefore(NEDElement *node, int searchTag, int stopTag)
{
    // true if: node itself is searchTag, or has searchTag before stopTag
    // (and false if node is immediately a stopTag)
    for (NEDElement *rest=node; rest && rest->getTagCode()!=stopTag; rest=rest->getNextSibling())
         if (rest->getTagCode()==searchTag)
             return true;
    return false;
}

static bool _isNetworkNode(NEDElement *node)
{
    return (node->getTagCode()==NED_COMPOUND_MODULE && ((CompoundModuleNode *)node)->getIsNetwork())
        || (node->getTagCode()==NED_SIMPLE_MODULE && ((SimpleModuleNode *)node)->getIsNetwork());
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
    if (_isNetworkNode(node))
    {
        if (node->getNumChildrenWithTag(NED_INTERFACE_NAME)>0)
            errors->add(node, ERRCAT_WARNING, NED2FEATURE "inheritance");
        NEDElement *extendsNode = node->getFirstChildWithTag(NED_EXTENDS);
        if (!extendsNode)
            errors->add(node, ERRCAT_WARNING, "network must extend a module type");
        else
            OUT << " : " << ((ExtendsNode *)extendsNode)->getName();
    }
    else if (node->getFirstChildWithTag(NED_EXTENDS) || node->getFirstChildWithTag(NED_INTERFACE_NAME))
    {
        errors->add(node, ERRCAT_WARNING, NED2FEATURE "inheritance");
    }
}

bool NED1Generator::hasExpression(NEDElement *node, const char *attr)
{
    if (strnotnull(node->getAttribute(attr)))
    {
        return true;
    }
    else
    {
        for (ExpressionNode *expr=(ExpressionNode *)node->getFirstChildWithTag(NED_EXPRESSION); expr; expr=expr->getNextExpressionNodeSibling())
            if (strnotnull(expr->getTarget()) && !strcmp(expr->getTarget(),attr))
                return true;
        return false;
    }
}

void NED1Generator::printExpression(NEDElement *node, const char *attr, const char *indent)
{
    if (strnotnull(node->getAttribute(attr)))
    {
        OUT << node->getAttribute(attr);
    }
    else
    {
        for (ExpressionNode *expr=(ExpressionNode *)node->getFirstChildWithTag(NED_EXPRESSION); expr; expr=expr->getNextExpressionNodeSibling())
            if (strnotnull(expr->getTarget()) && !strcmp(expr->getTarget(),attr))
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

void NED1Generator::appendBannerComment(const char *comment, const char *indent)
{
    if (!strnotnull(comment))
        return;

    OUT << comment;
}

void NED1Generator::appendRightComment(const char *comment, const char *indent)
{
    if (!strnotnull(comment))
        return;

    OUT << comment;
}

void NED1Generator::appendInlineRightComment(const char *comment, const char *indent)
{
    if (!strnotnull(comment))
        return;

    OUT << comment;
}

void NED1Generator::appendTrailingComment(const char *comment, const char *indent)
{
    if (!strnotnull(comment))
        return;

    OUT << comment;
}

//---------------------------------------------------------------------------

void NED1Generator::doNedfiles(FilesNode *node, const char *indent, bool, const char *)
{
    generateChildren(node, indent);
}

void NED1Generator::doNedfile(NedFileNode *node, const char *indent, bool, const char *)
{
    //XXX appendBannerComment(node->getBannerComment(), indent);
    generateChildren(node, indent);
}

void NED1Generator::doImport(ImportNode *node, const char *indent, bool islast, const char *)
{
    //XXX appendBannerComment(node->getBannerComment(), indent);
    OUT << indent << "import \"" << node->getFilename() << "\";" << "\n";
}

void NED1Generator::doPropertyDecl(PropertyDeclNode *node, const char *indent, bool islast, const char *)
{
    errors->add(node, ERRCAT_WARNING, NED2FEATURE "properties");
}

void NED1Generator::doExtends(ExtendsNode *node, const char *indent, bool islast, const char *sep)
{
    INTERNAL_ERROR0(node,"should never come here");
}

void NED1Generator::doInterfaceName(InterfaceNameNode *node, const char *indent, bool islast, const char *sep)
{
    INTERNAL_ERROR0(node,"should never come here");
}

void NED1Generator::doSimpleModule(SimpleModuleNode *node, const char *indent, bool islast, const char *)
{
    OUT << indent << "simple " << node->getName();
    printInheritance(node, indent);
    OUT << "\n";

    generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));
    generateChildrenWithType(node, NED_GATES, increaseIndent(indent));

    OUT << indent << "endsimple\n\n";
}

void NED1Generator::doModuleInterface(ModuleInterfaceNode *node, const char *indent, bool islast, const char *)
{
    errors->add(node, ERRCAT_WARNING, NED2FEATURE "interfaces");
}

void NED1Generator::doCompoundModule(CompoundModuleNode *node, const char *indent, bool islast, const char *)
{
    OUT << indent << (node->getIsNetwork() ? "network" : "module") << " " << node->getName();
    printInheritance(node, indent);
    OUT << "\n";

    generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));
    generateChildrenWithType(node, NED_GATES, increaseIndent(indent));
    generateChildrenWithType(node, NED_TYPES, increaseIndent(indent));
    generateChildrenWithType(node, NED_SUBMODULES, increaseIndent(indent));
    generateChildrenWithType(node, NED_CONNECTIONS, increaseIndent(indent));

    const char *dispstr = getDisplayStringOf(node);
    if (dispstr)
        OUT << increaseIndent(indent) << "display: " << dispstr << ";\n";

    OUT << indent << (node->getIsNetwork() ? "endnetwork" : "endmodule") << "\n\n";
}

void NED1Generator::doChannelInterface(ChannelInterfaceNode *node, const char *indent, bool islast, const char *)
{
    errors->add(node, ERRCAT_WARNING, NED2FEATURE "channel interfaces");
}

void NED1Generator::doChannel(ChannelNode *node, const char *indent, bool islast, const char *)
{
    OUT << indent << "channel ";
    if (node->getIsWithcppclass())
        errors->add(node, ERRCAT_WARNING, NED2FEATURE "channel withcppclass");
    OUT << node->getName();
    printInheritance(node, indent);
    OUT << "\n";

    generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));

    OUT << indent << "endchannel\n\n";
}

void NED1Generator::doParameters(ParametersNode *node, const char *indent, bool islast, const char *)
{
    // in NED-1, parameters followed different syntaxes at different places
    int parentTag = node->getParent()->getTagCode();
    if (parentTag==NED_SUBMODULE || _isNetworkNode(node->getParent()))
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

void NED1Generator::doModuleParameters(ParametersNode *node, const char *indent)
{
    if (node->getFirstChildWithTag(NED_PARAM))
        OUT << indent << "parameters:\n";

    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
    {
        int childTag = child->getTagCode();
        if (childTag==NED_COMMENT)
            ; // ignore whitespace
        else if (childTag==NED_PROPERTY)
            doProperty((PropertyNode *)child, increaseIndent(indent), false, NULL);
        else if (childTag==NED_PARAM)
            doModuleParam((ParamNode *)child, increaseIndent(indent), child->getNextSiblingWithTag(NED_PARAM)==NULL, NULL);
        else if (childTag==NED_PATTERN)
            errors->add(node, ERRCAT_WARNING, NED2FEATURE "assigment by pattern matching");
        else if (childTag==NED_PARAM_GROUP)
            errors->add(node, ERRCAT_WARNING, NED2FEATURE "parameter group");
        else
            INTERNAL_ERROR0(node,"unexpected element");
    }
}

void NED1Generator::doSubstParameters(ParametersNode *node, const char *indent)
{
    doSubstParamGroup(node, indent);
}

void NED1Generator::doSubstParamGroup(NEDElement *node, const char *indent)
{
    // node may be ParametersNode or ParamGroupNode

    // print "parameters:" if there's a parameter until the next paramgroup (or at all)
    if (_hasSiblingBefore(node->getFirstChild(), NED_PARAM, NED_PARAM_GROUP))
    {
        // print "parameters" or "parameters if"
        if (node->getFirstChildWithTag(NED_CONDITION)==NULL)
        {
            OUT << indent << "parameters:\n";
        }
        else
        {
            OUT << indent << "parameters ";
            generateChildrenWithType(node, NED_CONDITION, increaseIndent(indent));
            OUT << ":\n";
        }
    }

    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
    {
        int childTag = child->getTagCode();
        if (childTag==NED_COMMENT || childTag==NED_CONDITION)
            ; //ignore
        else if (childTag==NED_PROPERTY)
            doProperty((PropertyNode *)child, increaseIndent(indent), false, NULL);
        else if (childTag==NED_PARAM)
            doSubstParam((ParamNode *)child, increaseIndent(indent), !_hasSiblingBefore(child->getNextSibling(), NED_PARAM, NED_PARAM_GROUP), NULL);
        else if (childTag==NED_PATTERN)
            errors->add(node, ERRCAT_WARNING, NED2FEATURE "assignment by pattern matching");
        else if (childTag==NED_PARAM_GROUP)
        {
            doSubstParamGroup(child, indent);

            // if there is more NED_PARAM until the next NED_PARAM_GROUP or end,
            // print "parameters:" (to return to unconditional scope)
            if (_hasSiblingBefore(child->getNextSibling(), NED_PARAM, NED_PARAM_GROUP))
                OUT << indent << "parameters:\n";
        }
        else
            INTERNAL_ERROR0(node,"unexpected element");
    }
}

void NED1Generator::doChannelParameters(ParametersNode *node, const char *indent)
{
    // only "delay", "error", "datarate" parameters need to be recognized
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
    {
        int childTag = child->getTagCode();
        if (childTag==NED_COMMENT)
            ; //ignore
        else if (childTag==NED_PROPERTY)
            doProperty((PropertyNode *)child, indent, false, NULL);
        else if (childTag==NED_PARAM)
            doChannelParam((ParamNode *)child, indent);
        else if (childTag==NED_PATTERN)
            errors->add(node, ERRCAT_WARNING, NED2FEATURE "assignment by pattern matching");
        else if (childTag==NED_PARAM_GROUP)
            errors->add(node, ERRCAT_WARNING, NED2FEATURE "parameter group");
        else
            INTERNAL_ERROR0(node,"unexpected element");
    }
}

void NED1Generator::doConnectionAttributes(ParametersNode *node, const char *indent)
{
    // only "delay", "error", "datarate" parameters need to be recognized
    //FIXME "display" property is to be handled elsewhere
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
    {
        int childTag = child->getTagCode();
        if (childTag==NED_COMMENT)
            ; //ignore
        else if (childTag==NED_PROPERTY)
            doProperty((PropertyNode *)child, indent, false, NULL);
        else if (childTag==NED_PARAM)
            doChannelParam((ParamNode *)child, NULL);
        else if (childTag==NED_PATTERN)
            errors->add(node, ERRCAT_WARNING, NED2FEATURE "patterns");
        else if (childTag==NED_PARAM_GROUP)
            errors->add(node, ERRCAT_WARNING, NED2FEATURE "parameter group");
        else
            INTERNAL_ERROR0(node,"unexpected element");
    }
}

void NED1Generator::doChannelParam(ParamNode *node, const char *indent)
{
    const char *name = node->getName();
    if (strcmp(name, "delay")==0 || strcmp(name, "error")==0 || strcmp(name, "datarate")==0)
    {
        // indent==NULL means no indent and no new line at end (but a space at front)
        OUT << (indent ? indent : " ");
        OUT << node->getName() << " ";
        printExpression(node, "value", indent);
        OUT << (indent ? "\n" : "");
    }
    else
    {
        errors->add(node, ERRCAT_WARNING, NED2FEATURE "channel parameters other than delay, error and datarate");
    }
}

void NED1Generator::doParamGroup(ParamGroupNode *node, const char *indent, bool islast, const char *)
{
    // we use doSubstParamGroup() instead, NED-1 doesn't allow groups elsewhere
    // doParameters() ensures we never get here
    INTERNAL_ERROR0(node, "should never get here");
}

void NED1Generator::doParam(ParamNode *node, const char *indent, bool islast, const char *)
{
    // we use doModuleParam() and doSubstParam() instead
    // doParameters() ensures we never get here
    INTERNAL_ERROR0(node, "should never get here");
}

void NED1Generator::doModuleParam(ParamNode *node, const char *indent, bool islast, const char *)
{
    const char *parType = NULL;
    switch (node->getType())
    {
        case NED_PARTYPE_NONE:   break;
        case NED_PARTYPE_DOUBLE: case NED_PARTYPE_INT:
                                 parType = node->getIsFunction() ? "numeric" : "numeric const"; break;
        case NED_PARTYPE_STRING: parType = "string"; break;
        case NED_PARTYPE_BOOL:   parType = "bool"; break;
        case NED_PARTYPE_XML:    parType = "xml"; break;
        default: INTERNAL_ERROR0(node, "wrong type");
    }

    OUT << indent << node->getName();
    if (parType!=NULL)
        OUT << ": " << parType;

    if (hasExpression(node,"value"))
        errors->add(node, ERRCAT_WARNING, NED2FEATURE "assignment in parameter declaration");

    const char *subindent = indent ? increaseIndent(indent) : DEFAULTINDENT;
    generateChildrenWithType(node, NED_PROPERTY, subindent, " ");
    if (node->getFirstChildWithTag(NED_CONDITION))
        errors->add(node, ERRCAT_WARNING, "conditional parameter assignments for NED-1 are not supported");

    OUT << (islast ? ";\n" : ",\n");
}

void NED1Generator::doSubstParam(ParamNode *node, const char *indent, bool islast, const char *)
{
    if (node->getType()!=NED_PARTYPE_NONE)
        errors->add(node, ERRCAT_WARNING, NED2FEATURE "defining new parameter for a submodule");

    OUT << indent << node->getName() << " = ";
    printExpression(node, "value", indent);

    generateChildrenWithType(node, NED_PROPERTY, increaseIndent(indent), " ");
    if (node->getFirstChildWithTag(NED_CONDITION))
        errors->add(node, ERRCAT_WARNING, NED2FEATURE "conditional parameter assignment");

    OUT << (islast ? ";\n" : ",\n");
}

void NED1Generator::doPattern(PatternNode *node, const char *indent, bool islast, const char *)
{
    errors->add(node, ERRCAT_WARNING, NED2FEATURE "assignment by pattern matching");
}

void NED1Generator::doProperty(PropertyNode *node, const char *indent, bool islast, const char *sep)
{
    // only @display is recognized, but it needs to be printed at a different place
    //FIXME but gates, parameters etc cannot have @display property!!!
    //FIXME but parameters can have @prompt etc.
    if (strcmp(node->getName(), "display")!=0)
        errors->add(node, ERRCAT_WARNING, NED2FEATURE "property");
}

void NED1Generator::doPropertyKey(PropertyKeyNode *node, const char *indent, bool islast, const char *sep)
{
    INTERNAL_ERROR0(node,"should never come here");
}

void NED1Generator::doGates(GatesNode *node, const char *indent, bool islast, const char *)
{
    // in NED-1, distingish between "gates" and "gatesizes"
    int parentTag = node->getParent()->getTagCode();
    if (parentTag==NED_SUBMODULE)
        doSubmoduleGatesizes(node, indent);
    else if (parentTag==NED_SIMPLE_MODULE || parentTag==NED_COMPOUND_MODULE)
        doModuleGates(node, indent);
    else
        INTERNAL_ERROR0(node,"unexpected gates section");

    //OUT << indent << "gates:\n";
    //generateChildren(node, increaseIndent(indent));
}

void NED1Generator::doModuleGates(GatesNode *node, const char *indent)
{
    if (node->getFirstChildWithTag(NED_GATE))
        OUT << indent << "gates:\n";

    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
    {
        int childTag = child->getTagCode();
        if (childTag==NED_COMMENT)
            ; // ignore whitespace
        else if (childTag==NED_GATE)
            doModuleGate((GateNode *)child, increaseIndent(indent), child->getNextSiblingWithTag(NED_GATE)==NULL, NULL);
        else if (childTag==NED_GATE_GROUP)
            errors->add(node, ERRCAT_WARNING, NED2FEATURE "gate group");
        else
            INTERNAL_ERROR0(node,"unexpected element");
    }
}

void NED1Generator::doGateGroup(GateGroupNode *node, const char *indent, bool islast, const char *)
{
    // we use doSubmoduleGatesizes() instead, NED-1 doesn't allow groups elsewhere
    // doGates() ensures we never get here
    INTERNAL_ERROR0(node, "should never get here");
}

void NED1Generator::doSubmoduleGatesizes(GatesNode *node, const char *indent)
{
    doGatesizesGroup(node, indent);
}

void NED1Generator::doGatesizesGroup(NEDElement *node, const char *indent)
{
    // node may be GatesNode or GateGroupNode

    // print "gates:" if there's a gate until the next gategroup (or at all)
    if (_hasSiblingBefore(node->getFirstChild(), NED_GATE, NED_GATE_GROUP))
    {
        // print "gates" or "gates if"
        if (node->getFirstChildWithTag(NED_CONDITION)==NULL)
        {
            OUT << indent << "gatesizes:\n";
        }
        else
        {
            OUT << indent << "gatesizes ";
            generateChildrenWithType(node, NED_CONDITION, increaseIndent(indent));
            OUT << ":\n";
        }
    }

    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
    {
        int childTag = child->getTagCode();
        if (childTag==NED_COMMENT || childTag==NED_CONDITION)
            ; //ignore
        else if (childTag==NED_GATE)
            doGatesize((GateNode *)child, increaseIndent(indent), !_hasSiblingBefore(child->getNextSibling(), NED_GATE, NED_GATE_GROUP), NULL);
        else if (childTag==NED_GATE_GROUP)
        {
            doGatesizesGroup(child, indent);

            // if there is more NED_GATE until the next NED_GATE_GROUP or end,
            // print "gatesizes:" (to return to unconditional scope)
            if (_hasSiblingBefore(child->getNextSibling(), NED_GATE, NED_GATE_GROUP))
                OUT << indent << "gatesizes:\n";
        }
        else
            INTERNAL_ERROR0(node,"unexpected element");
    }
}

void NED1Generator::doGate(GateNode *node, const char *indent, bool islast, const char *)
{
    // we use doModuleGate()/doGatesize() instead
    // doGates() ensures we never get here
    INTERNAL_ERROR0(node, "should never get here");
}

void NED1Generator::doModuleGate(GateNode *node, const char *indent, bool islast, const char *)
{
    OUT << indent;
    switch (node->getType())
    {
        case NED_GATETYPE_INPUT:  OUT << "in: "; break;
        case NED_GATETYPE_OUTPUT: OUT << "out: "; break;
        case NED_GATETYPE_INOUT:  errors->add(node, ERRCAT_WARNING, NED2FEATURE "inout gate"); break;
        case NED_GATETYPE_NONE:   errors->add(node, ERRCAT_WARNING, NED2FEATURE "missing gate type"); break;
        default: INTERNAL_ERROR0(node, "wrong type");
    }
    OUT << node->getName();
    if (node->getIsVector())
        OUT << "[]";
    if (hasExpression(node, "vector-size"))
        errors->add(node, ERRCAT_WARNING, NED2FEATURE "gate vector size in gate declaration");
    generateChildrenWithType(node, NED_PROPERTY, increaseIndent(indent), " ");
    if (node->getFirstChildWithTag(NED_CONDITION))
        errors->add(node, ERRCAT_WARNING, NED2FEATURE "conditional gate declaration");
    OUT << ";\n";
}

void NED1Generator::doGatesize(GateNode *node, const char *indent, bool islast, const char *)
{
    OUT << indent;
    if (node->getType()!=NED_GATETYPE_NONE)
        errors->add(node, ERRCAT_WARNING, NED2FEATURE "declaring new gates for submodules");
    OUT << node->getName();
    if (!hasExpression(node, "vector-size"))
        errors->add(node, ERRCAT_WARNING, NED2FEATURE "missing gate size");
    printOptVector(node, "vector-size",indent);

    generateChildrenWithType(node, NED_PROPERTY, increaseIndent(indent), " ");
    generateChildrenWithType(node, NED_CONDITION, increaseIndent(indent));
    OUT << (islast ? ";\n" : ",\n");
}

void NED1Generator::doTypes(TypesNode *node, const char *indent, bool islast, const char *)
{
    errors->add(node, ERRCAT_WARNING, NED2FEATURE "inner type");
}

void NED1Generator::doSubmodules(SubmodulesNode *node, const char *indent, bool islast, const char *)
{
    OUT << indent << "submodules:\n";
    generateChildren(node, increaseIndent(indent));
}

void NED1Generator::doSubmodule(SubmoduleNode *node, const char *indent, bool islast, const char *)
{
    OUT << indent << node->getName() << ": ";

    if (node->getLikeAny() || strnotnull(node->getLikeType()))
    {
        // "like" version
        printExpression(node, "like-param", indent); // this (incidentally) also works if like-param contains a property (ie. starts with "@")
        printOptVector(node, "vector-size",indent);

        if (strnotnull(node->getLikeType()))
            OUT << " like " << node->getLikeType();
        if (node->getLikeAny())
            errors->add(node, ERRCAT_WARNING, NED2FEATURE "`like *'");
    }
    else
    {
        // "like"-less
        OUT << node->getType();
        printOptVector(node, "vector-size",indent);
    }
    OUT << ";\n";

    generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));
    generateChildrenWithType(node, NED_GATES, increaseIndent(indent));

    const char *dispstr = getDisplayStringOf(node);
    if (dispstr)
        OUT << increaseIndent(indent) << "display: " << dispstr << ";\n";
}

const char *NED1Generator::getDisplayStringOf(NEDElement *node)
{
    // node must be a module, submodule or a connection-spec
    ParametersNode *parameters = (ParametersNode *)node->getFirstChildWithTag(NED_PARAMETERS);
    if (!parameters)
        return NULL;
    PropertyNode *displayProp = (PropertyNode *)parameters->getFirstChildWithAttribute(NED_PROPERTY, "name", "display");
    if (!displayProp)
        return NULL;
    PropertyKeyNode *propKey = (PropertyKeyNode *)displayProp->getFirstChildWithAttribute(NED_PROPERTY_KEY, "key", "");
    if (!propKey)
        return NULL;
    LiteralNode *literal = (LiteralNode *)propKey->getFirstChildWithTag(NED_LITERAL);
    if (!literal)
        return NULL;
    return literal->getText();
}

void NED1Generator::doConnections(ConnectionsNode *node, const char *indent, bool islast, const char *)
{
    if (node->getAllowUnconnected()) {
        OUT << indent << "connections nocheck:\n";
    } else {
        OUT << indent << "connections:\n";
    }
    generateChildren(node, increaseIndent(indent));
}

void NED1Generator::doConnection(ConnectionNode *node, const char *indent, bool islast, const char *)
{
    //  direction
    const char *arrow;
    bool srcfirst;
    switch (node->getArrowDirection())
    {
        case NED_ARROWDIR_L2R:   arrow = " -->"; srcfirst = true; break;
        case NED_ARROWDIR_R2L:   arrow = " <--"; srcfirst = false; break;
        case NED_ARROWDIR_BIDIR: errors->add(node, ERRCAT_WARNING, NED2FEATURE "two-way connection");
                                 arrow = " <-->"; srcfirst = true; break;
        default: INTERNAL_ERROR0(node, "wrong arrow-dir");
    }

    // print src
    OUT << indent;
    if (srcfirst)
        printGate(node, node->getSrcModule(), "src-module-index", node->getSrcGate(), "src-gate-index", node->getSrcGatePlusplus(), node->getSrcGateSubg(), indent);
    else
        printGate(node, node->getDestModule(), "dest-module-index", node->getDestGate(), "dest-gate-index", node->getDestGatePlusplus(), node->getDestGateSubg(), indent);

    // arrow
    OUT << arrow;

    // print channel attributes
    if (node->getFirstChildWithTag(NED_CHANNEL_SPEC))
    {
        generateChildrenWithType(node, NED_CHANNEL_SPEC, indent);
        OUT << arrow;
    }

    // print dest
    OUT << " ";
    if (srcfirst)
        printGate(node, node->getDestModule(), "dest-module-index", node->getDestGate(), "dest-gate-index", node->getDestGatePlusplus(), node->getDestGateSubg(), indent);
    else
        printGate(node, node->getSrcModule(), "src-module-index", node->getSrcGate(), "src-gate-index", node->getSrcGatePlusplus(), node->getSrcGateSubg(), indent);

    if (node->getFirstChildWithTag(NED_LOOP))
        errors->add(node, ERRCAT_WARNING, NED2FEATURE "per-connection `for'");

    if (node->getNumChildrenWithTag(NED_CONDITION)>1)
        errors->add(node, ERRCAT_WARNING, NED2FEATURE "more than one `if' per-connection");

    NEDElement *condition = node->getFirstChildWithTag(NED_CONDITION);
    if (condition)
        doCondition((ConditionNode *)condition, indent, false, NULL);

    // display string
    NEDElement *chanSpecNode = node->getFirstChildWithTag(NED_CHANNEL_SPEC);
    if (chanSpecNode)
    {
        const char *dispstr = getDisplayStringOf(chanSpecNode);
        if (dispstr)
            OUT << " display " << dispstr;
    }

    OUT << ";\n";
}

void NED1Generator::doChannelSpec(ChannelSpecNode *node, const char *indent, bool islast, const char *)
{
    if (node->getLikeAny() || strnotnull(node->getLikeType()))
    {
        errors->add(node, ERRCAT_WARNING, NED2FEATURE "channel `like'");
    }
    else if (strnotnull(node->getType()))
    {
        // concrete channel type
        OUT << " " << node->getType();
    }

    if (node->getFirstChildWithTag(NED_PARAMETERS))
    {
        generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));
    }
}

void NED1Generator::doConnectionGroup(ConnectionGroupNode *node, const char *indent, bool islast, const char *)
{
    OUT << indent << "for ";
    generateChildrenWithType(node, NED_LOOP, increaseIndent(indent), ", ");
    OUT << " do\n";

    generateChildrenWithType(node, NED_CONNECTION, increaseIndent(indent));

    OUT << indent << "endfor;\n";
}

void NED1Generator::doLoop(LoopNode *node, const char *indent, bool islast, const char *sep)
{
    OUT << node->getParamName() << "=";
    printExpression(node, "from-value",indent);
    OUT << "..";
    printExpression(node, "to-value",indent);

    if (!islast)
        OUT << sep;
}

void NED1Generator::doCondition(ConditionNode *node, const char *indent, bool islast, const char *sep)
{
    OUT << "if ";
    printExpression(node, "condition",indent);
    if (!islast)
        OUT << sep;
}

void NED1Generator::printGate(NEDElement *conn, const char *modname, const char *modindexattr,
                             const char *gatename, const char *gateindexattr, bool isplusplus,
                             int gatesubg, const char *indent)
{
    if (strnotnull(modname)) {
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
        errors->add(conn, ERRCAT_WARNING, NED2FEATURE "subgate of inout gate");
}

void NED1Generator::doExpression(ExpressionNode *node, const char *indent, bool islast, const char *)
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

void NED1Generator::doOperator(OperatorNode *node, const char *indent, bool islast, const char *)
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
            OperatorNode *parentop = (OperatorNode *)parent;
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
        generateNedItem(op1,indent,false,NULL);
        OUT << (spacious ? " : " : ":");
        generateNedItem(op1,indent,false,NULL);
        if (needsParen) OUT << ")";
    }
}

void NED1Generator::doFunction(FunctionNode *node, const char *indent, bool islast, const char *)
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

void NED1Generator::doIdent(IdentNode *node, const char *indent, bool islast, const char *)
{
    if (strnotnull(node->getModule())) {
        OUT << node->getModule();
        printOptVector(node, "module-index", indent);
        OUT << ".";
    }

    OUT << node->getName();
}

void NED1Generator::doLiteral(LiteralNode *node, const char *indent, bool islast, const char *)
{
    if (strnotnull(node->getText()))
    {
        OUT << node->getText();
    }
    else
    {
        // fallback: when original text is not present, use value
        bool isstring = (node->getType()==NED_CONST_STRING);
        if (isstring) OUT << "\"";
        OUT << node->getValue();
        if (isstring) OUT << "\"";
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
            doNedfiles((FilesNode *)node, indent, islast, arg); break;
        case NED_NED_FILE:
            doNedfile((NedFileNode *)node, indent, islast, arg); break;
        case NED_IMPORT:
            doImport((ImportNode *)node, indent, islast, arg); break;
        case NED_PROPERTY_DECL:
            doPropertyDecl((PropertyDeclNode *)node, indent, islast, arg); break;
        case NED_EXTENDS:
            doExtends((ExtendsNode *)node, indent, islast, arg); break;
        case NED_INTERFACE_NAME:
            doInterfaceName((InterfaceNameNode *)node, indent, islast, arg); break;
        case NED_SIMPLE_MODULE:
            doSimpleModule((SimpleModuleNode *)node, indent, islast, arg); break;
        case NED_MODULE_INTERFACE:
            doModuleInterface((ModuleInterfaceNode *)node, indent, islast, arg); break;
        case NED_COMPOUND_MODULE:
            doCompoundModule((CompoundModuleNode *)node, indent, islast, arg); break;
        case NED_CHANNEL_INTERFACE:
            doChannelInterface((ChannelInterfaceNode *)node, indent, islast, arg); break;
        case NED_CHANNEL:
            doChannel((ChannelNode *)node, indent, islast, arg); break;
        case NED_PARAMETERS:
            doParameters((ParametersNode *)node, indent, islast, arg); break;
        case NED_PARAM_GROUP:
            doParamGroup((ParamGroupNode *)node, indent, islast, arg); break;
        case NED_PARAM:
            doParam((ParamNode *)node, indent, islast, arg); break;
        case NED_PATTERN:
            doPattern((PatternNode *)node, indent, islast, arg); break;
        case NED_PROPERTY:
            doProperty((PropertyNode *)node, indent, islast, arg); break;
        case NED_PROPERTY_KEY:
            doPropertyKey((PropertyKeyNode *)node, indent, islast, arg); break;
        case NED_GATES:
            doGates((GatesNode *)node, indent, islast, arg); break;
        case NED_GATE_GROUP:
            doGateGroup((GateGroupNode *)node, indent, islast, arg); break;
        case NED_GATE:
            doGate((GateNode *)node, indent, islast, arg); break;
        case NED_TYPES:
            doTypes((TypesNode *)node, indent, islast, arg); break;
        case NED_SUBMODULES:
            doSubmodules((SubmodulesNode *)node, indent, islast, arg); break;
        case NED_SUBMODULE:
            doSubmodule((SubmoduleNode *)node, indent, islast, arg); break;
        case NED_CONNECTIONS:
            doConnections((ConnectionsNode *)node, indent, islast, arg); break;
        case NED_CONNECTION:
            doConnection((ConnectionNode *)node, indent, islast, arg); break;
        case NED_CHANNEL_SPEC:
            doChannelSpec((ChannelSpecNode *)node, indent, islast, arg); break;
        case NED_CONNECTION_GROUP:
            doConnectionGroup((ConnectionGroupNode *)node, indent, islast, arg); break;
        case NED_LOOP:
            doLoop((LoopNode *)node, indent, islast, arg); break;
        case NED_CONDITION:
            doCondition((ConditionNode *)node, indent, islast, arg); break;
        case NED_EXPRESSION:
            doExpression((ExpressionNode *)node, indent, islast, arg); break;
        case NED_OPERATOR:
            doOperator((OperatorNode *)node, indent, islast, arg); break;
        case NED_FUNCTION:
            doFunction((FunctionNode *)node, indent, islast, arg); break;
        case NED_IDENT:
            doIdent((IdentNode *)node, indent, islast, arg); break;
        case NED_LITERAL:
            doLiteral((LiteralNode *)node, indent, islast, arg); break;
        default:
            ;//XXX: add back this line: INTERNAL_ERROR1(node, "generateNedItem(): unknown tag '%s'", node->getTagName());
    }
}



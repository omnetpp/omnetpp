//==========================================================================
// Part of the OMNeT++/OMNEST Discrete Event Simulation System
//
// Generated from ned.dtd by dtdclassgen.pl
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

//
// THIS IS A GENERATED FILE, DO NOT EDIT!
//


#ifndef __NEDELEMENTS_H
#define __NEDELEMENTS_H

#include "nedelement.h"

class NedFilesNode;
class NedFileNode;
class ImportNode;
class ImportedFileNode;
class ChannelNode;
class ChannelAttrNode;
class NetworkNode;
class SimpleModuleNode;
class CompoundModuleNode;
class ParamsNode;
class ParamNode;
class GatesNode;
class GateNode;
class MachinesNode;
class MachineNode;
class SubmodulesNode;
class SubmoduleNode;
class SubstparamsNode;
class SubstparamNode;
class GatesizesNode;
class GatesizeNode;
class SubstmachinesNode;
class SubstmachineNode;
class ConnectionsNode;
class ConnectionNode;
class ConnAttrNode;
class ForLoopNode;
class LoopVarNode;
class DisplayStringNode;
class ExpressionNode;
class OperatorNode;
class FunctionNode;
class ParamRefNode;
class IdentNode;
class ConstNode;
class CplusplusNode;
class StructDeclNode;
class ClassDeclNode;
class MessageDeclNode;
class EnumDeclNode;
class EnumNode;
class EnumFieldsNode;
class EnumFieldNode;
class MessageNode;
class ClassNode;
class StructNode;
class FieldsNode;
class FieldNode;
class PropertiesNode;
class PropertyNode;
class UnknownNode;


/**
 * Tag codes
 * 
 * @ingroup Data
 */
enum NEDElementCode {
    NED_NED_FILES,
    NED_NED_FILE,
    NED_IMPORT,
    NED_IMPORTED_FILE,
    NED_CHANNEL,
    NED_CHANNEL_ATTR,
    NED_NETWORK,
    NED_SIMPLE_MODULE,
    NED_COMPOUND_MODULE,
    NED_PARAMS,
    NED_PARAM,
    NED_GATES,
    NED_GATE,
    NED_MACHINES,
    NED_MACHINE,
    NED_SUBMODULES,
    NED_SUBMODULE,
    NED_SUBSTPARAMS,
    NED_SUBSTPARAM,
    NED_GATESIZES,
    NED_GATESIZE,
    NED_SUBSTMACHINES,
    NED_SUBSTMACHINE,
    NED_CONNECTIONS,
    NED_CONNECTION,
    NED_CONN_ATTR,
    NED_FOR_LOOP,
    NED_LOOP_VAR,
    NED_DISPLAY_STRING,
    NED_EXPRESSION,
    NED_OPERATOR,
    NED_FUNCTION,
    NED_PARAM_REF,
    NED_IDENT,
    NED_CONST,
    NED_CPLUSPLUS,
    NED_STRUCT_DECL,
    NED_CLASS_DECL,
    NED_MESSAGE_DECL,
    NED_ENUM_DECL,
    NED_ENUM,
    NED_ENUM_FIELDS,
    NED_ENUM_FIELD,
    NED_MESSAGE,
    NED_CLASS,
    NED_STRUCT,
    NED_FIELDS,
    NED_FIELD,
    NED_PROPERTIES,
    NED_PROPERTY,
    NED_UNKNOWN
};

enum {NED_GATEDIR_INPUT, NED_GATEDIR_OUTPUT};
enum {NED_ARROWDIR_LEFT, NED_ARROWDIR_RIGHT};
enum {NED_CONST_BOOL, NED_CONST_INT, NED_CONST_REAL, NED_CONST_STRING, NED_CONST_TIME};

/**
 * GENERATED CLASS. Represents the &lt;ned-files&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT ned-files ((ned-files|ned-file)*)>
 * 
 * </pre>
 * 
 * @ingroup Data
 */
class NedFilesNode : public NEDElement
{
  private:
  public:
    /** @name Constructors, destructor */
    //@{
    NedFilesNode() {applyDefaults();}
    NedFilesNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~NedFilesNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "ned-files";}
    virtual int getTagCode() const {return NED_NED_FILES;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{

    virtual NedFilesNode *getNextNedFilesNodeSibling() const;
    virtual NedFilesNode *getFirstNedFilesChild() const;
    virtual NedFileNode *getFirstNedFileChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;ned-file&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT ned-file ((import|channel|simple-module|compound-module|network|
 *                      cplusplus|struct-decl|class-decl|message-decl|enum-decl|
 *                      enum|message|class|struct)*)>
 * <!ATTLIST ned-file
 *      filename            CDATA     #IMPLIED
 *      source-code         CDATA     #IMPLIED
 *      preferred-indent    CDATA     "4"
 *      banner-comment      CDATA     #IMPLIED >
 * </pre>
 * 
 * @ingroup Data
 */
class NedFileNode : public NEDElement
{
  private:
    std::string filename;
    std::string sourceCode;
    std::string preferredIndent;
    std::string bannerComment;
  public:
    /** @name Constructors, destructor */
    //@{
    NedFileNode() {applyDefaults();}
    NedFileNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~NedFileNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "ned-file";}
    virtual int getTagCode() const {return NED_NED_FILE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getFilename() const  {return filename.c_str();}
    void setFilename(const char * val)  {filename = val;}
    const char * getSourceCode() const  {return sourceCode.c_str();}
    void setSourceCode(const char * val)  {sourceCode = val;}
    const char * getPreferredIndent() const  {return preferredIndent.c_str();}
    void setPreferredIndent(const char * val)  {preferredIndent = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}

    virtual NedFileNode *getNextNedFileNodeSibling() const;
    virtual ImportNode *getFirstImportChild() const;
    virtual ChannelNode *getFirstChannelChild() const;
    virtual SimpleModuleNode *getFirstSimpleModuleChild() const;
    virtual CompoundModuleNode *getFirstCompoundModuleChild() const;
    virtual NetworkNode *getFirstNetworkChild() const;
    virtual CplusplusNode *getFirstCplusplusChild() const;
    virtual StructDeclNode *getFirstStructDeclChild() const;
    virtual ClassDeclNode *getFirstClassDeclChild() const;
    virtual MessageDeclNode *getFirstMessageDeclChild() const;
    virtual EnumDeclNode *getFirstEnumDeclChild() const;
    virtual EnumNode *getFirstEnumChild() const;
    virtual MessageNode *getFirstMessageChild() const;
    virtual ClassNode *getFirstClassChild() const;
    virtual StructNode *getFirstStructChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;import&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT import (imported-file*)>
 * <!ATTLIST import
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class ImportNode : public NEDElement
{
  private:
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    ImportNode() {applyDefaults();}
    ImportNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ImportNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "import";}
    virtual int getTagCode() const {return NED_IMPORT;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual ImportNode *getNextImportNodeSibling() const;
    virtual ImportedFileNode *getFirstImportedFileChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;imported-file&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT imported-file EMPTY>
 * <!ATTLIST imported-file
 *      filename            CDATA     #REQUIRED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class ImportedFileNode : public NEDElement
{
  private:
    std::string filename;
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    ImportedFileNode() {applyDefaults();}
    ImportedFileNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ImportedFileNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "imported-file";}
    virtual int getTagCode() const {return NED_IMPORTED_FILE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getFilename() const  {return filename.c_str();}
    void setFilename(const char * val)  {filename = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual ImportedFileNode *getNextImportedFileNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;channel&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT channel (channel-attr*, display-string?)>
 * <!ATTLIST channel
 *      name                NMTOKEN   #REQUIRED
 *      source-code         CDATA     #IMPLIED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;"
 *      trailing-comment    CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class ChannelNode : public NEDElement
{
  private:
    std::string name;
    std::string sourceCode;
    std::string bannerComment;
    std::string rightComment;
    std::string trailingComment;
  public:
    /** @name Constructors, destructor */
    //@{
    ChannelNode() {applyDefaults();}
    ChannelNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ChannelNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "channel";}
    virtual int getTagCode() const {return NED_CHANNEL;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getSourceCode() const  {return sourceCode.c_str();}
    void setSourceCode(const char * val)  {sourceCode = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}
    const char * getTrailingComment() const  {return trailingComment.c_str();}
    void setTrailingComment(const char * val)  {trailingComment = val;}

    virtual ChannelNode *getNextChannelNodeSibling() const;
    virtual ChannelAttrNode *getFirstChannelAttrChild() const;
    virtual DisplayStringNode *getFirstDisplayStringChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;channel-attr&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT channel-attr (expression?)>
 * <!ATTLIST channel-attr
 *      name                NMTOKEN   #REQUIRED
 *      value               CDATA     #IMPLIED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class ChannelAttrNode : public NEDElement
{
  private:
    std::string name;
    std::string value;
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    ChannelAttrNode() {applyDefaults();}
    ChannelAttrNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ChannelAttrNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "channel-attr";}
    virtual int getTagCode() const {return NED_CHANNEL_ATTR;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getValue() const  {return value.c_str();}
    void setValue(const char * val)  {value = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual ChannelAttrNode *getNextChannelAttrNodeSibling() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;network&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT network (substmachines?,substparams?)>
 * <!ATTLIST network
 *      name                NMTOKEN   #REQUIRED
 *      type-name           NMTOKEN   #REQUIRED
 *      source-code         CDATA     #IMPLIED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;"
 *      trailing-comment    CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class NetworkNode : public NEDElement
{
  private:
    std::string name;
    std::string typeName;
    std::string sourceCode;
    std::string bannerComment;
    std::string rightComment;
    std::string trailingComment;
  public:
    /** @name Constructors, destructor */
    //@{
    NetworkNode() {applyDefaults();}
    NetworkNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~NetworkNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "network";}
    virtual int getTagCode() const {return NED_NETWORK;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getTypeName() const  {return typeName.c_str();}
    void setTypeName(const char * val)  {typeName = val;}
    const char * getSourceCode() const  {return sourceCode.c_str();}
    void setSourceCode(const char * val)  {sourceCode = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}
    const char * getTrailingComment() const  {return trailingComment.c_str();}
    void setTrailingComment(const char * val)  {trailingComment = val;}

    virtual NetworkNode *getNextNetworkNodeSibling() const;
    virtual SubstmachinesNode *getFirstSubstmachinesChild() const;
    virtual SubstparamsNode *getFirstSubstparamsChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;simple-module&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT simple-module (machines?, params?, gates?, display-string?)>
 * <!ATTLIST simple-module
 *      name                NMTOKEN   #REQUIRED
 *      source-code         CDATA     #IMPLIED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;"
 *      trailing-comment    CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class SimpleModuleNode : public NEDElement
{
  private:
    std::string name;
    std::string sourceCode;
    std::string bannerComment;
    std::string rightComment;
    std::string trailingComment;
  public:
    /** @name Constructors, destructor */
    //@{
    SimpleModuleNode() {applyDefaults();}
    SimpleModuleNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~SimpleModuleNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "simple-module";}
    virtual int getTagCode() const {return NED_SIMPLE_MODULE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getSourceCode() const  {return sourceCode.c_str();}
    void setSourceCode(const char * val)  {sourceCode = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}
    const char * getTrailingComment() const  {return trailingComment.c_str();}
    void setTrailingComment(const char * val)  {trailingComment = val;}

    virtual SimpleModuleNode *getNextSimpleModuleNodeSibling() const;
    virtual MachinesNode *getFirstMachinesChild() const;
    virtual ParamsNode *getFirstParamsChild() const;
    virtual GatesNode *getFirstGatesChild() const;
    virtual DisplayStringNode *getFirstDisplayStringChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;compound-module&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT compound-module (machines?, params?, gates?,
 *                   submodules?, connections?, display-string?)>
 * <!ATTLIST compound-module
 *      name                NMTOKEN   #REQUIRED
 *      source-code         CDATA     #IMPLIED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;"
 *      trailing-comment    CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class CompoundModuleNode : public NEDElement
{
  private:
    std::string name;
    std::string sourceCode;
    std::string bannerComment;
    std::string rightComment;
    std::string trailingComment;
  public:
    /** @name Constructors, destructor */
    //@{
    CompoundModuleNode() {applyDefaults();}
    CompoundModuleNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~CompoundModuleNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "compound-module";}
    virtual int getTagCode() const {return NED_COMPOUND_MODULE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getSourceCode() const  {return sourceCode.c_str();}
    void setSourceCode(const char * val)  {sourceCode = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}
    const char * getTrailingComment() const  {return trailingComment.c_str();}
    void setTrailingComment(const char * val)  {trailingComment = val;}

    virtual CompoundModuleNode *getNextCompoundModuleNodeSibling() const;
    virtual MachinesNode *getFirstMachinesChild() const;
    virtual ParamsNode *getFirstParamsChild() const;
    virtual GatesNode *getFirstGatesChild() const;
    virtual SubmodulesNode *getFirstSubmodulesChild() const;
    virtual ConnectionsNode *getFirstConnectionsChild() const;
    virtual DisplayStringNode *getFirstDisplayStringChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;params&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT params (param*)>
 * <!ATTLIST params
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class ParamsNode : public NEDElement
{
  private:
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    ParamsNode() {applyDefaults();}
    ParamsNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ParamsNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "params";}
    virtual int getTagCode() const {return NED_PARAMS;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual ParamsNode *getNextParamsNodeSibling() const;
    virtual ParamNode *getFirstParamChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;param&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT param EMPTY>
 * <!ATTLIST param
 *      name                NMTOKEN   #REQUIRED
 *      data-type           CDATA     "numeric"
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class ParamNode : public NEDElement
{
  private:
    std::string name;
    std::string dataType;
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    ParamNode() {applyDefaults();}
    ParamNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ParamNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "param";}
    virtual int getTagCode() const {return NED_PARAM;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getDataType() const  {return dataType.c_str();}
    void setDataType(const char * val)  {dataType = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual ParamNode *getNextParamNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;gates&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT gates (gate*)>
 * <!ATTLIST gates
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class GatesNode : public NEDElement
{
  private:
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    GatesNode() {applyDefaults();}
    GatesNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~GatesNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "gates";}
    virtual int getTagCode() const {return NED_GATES;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual GatesNode *getNextGatesNodeSibling() const;
    virtual GateNode *getFirstGateChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;gate&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT gate EMPTY>
 * <!ATTLIST gate
 *      name                NMTOKEN   #REQUIRED
 *      direction      (input|output) #REQUIRED
 *      is-vector       (true|false)  "false"
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class GateNode : public NEDElement
{
  private:
    std::string name;
    int direction;
    bool isVector;
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    GateNode() {applyDefaults();}
    GateNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~GateNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "gate";}
    virtual int getTagCode() const {return NED_GATE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    int getDirection() const  {return direction;}
    void setDirection(int val)  {direction = val;}
    bool getIsVector() const  {return isVector;}
    void setIsVector(bool val)  {isVector = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual GateNode *getNextGateNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;machines&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT machines (machine*)>
 * <!ATTLIST machines
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class MachinesNode : public NEDElement
{
  private:
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    MachinesNode() {applyDefaults();}
    MachinesNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~MachinesNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "machines";}
    virtual int getTagCode() const {return NED_MACHINES;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual MachinesNode *getNextMachinesNodeSibling() const;
    virtual MachineNode *getFirstMachineChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;machine&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT machine EMPTY>
 * <!ATTLIST machine
 *      name                NMTOKEN   #REQUIRED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class MachineNode : public NEDElement
{
  private:
    std::string name;
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    MachineNode() {applyDefaults();}
    MachineNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~MachineNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "machine";}
    virtual int getTagCode() const {return NED_MACHINE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual MachineNode *getNextMachineNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;submodules&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT submodules (submodule*)>
 * <!ATTLIST submodules
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class SubmodulesNode : public NEDElement
{
  private:
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    SubmodulesNode() {applyDefaults();}
    SubmodulesNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~SubmodulesNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "submodules";}
    virtual int getTagCode() const {return NED_SUBMODULES;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual SubmodulesNode *getNextSubmodulesNodeSibling() const;
    virtual SubmoduleNode *getFirstSubmoduleChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;submodule&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT submodule (expression?, substmachines*, substparams*,
 *                      gatesizes*, display-string?)>
 * <!ATTLIST submodule
 *      name                NMTOKEN   #REQUIRED
 *      type-name           NMTOKEN   #REQUIRED
 *      like-param          NMTOKEN   #IMPLIED
 *      vector-size         CDATA     #IMPLIED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class SubmoduleNode : public NEDElement
{
  private:
    std::string name;
    std::string typeName;
    std::string likeParam;
    std::string vectorSize;
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    SubmoduleNode() {applyDefaults();}
    SubmoduleNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~SubmoduleNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "submodule";}
    virtual int getTagCode() const {return NED_SUBMODULE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getTypeName() const  {return typeName.c_str();}
    void setTypeName(const char * val)  {typeName = val;}
    const char * getLikeParam() const  {return likeParam.c_str();}
    void setLikeParam(const char * val)  {likeParam = val;}
    const char * getVectorSize() const  {return vectorSize.c_str();}
    void setVectorSize(const char * val)  {vectorSize = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual SubmoduleNode *getNextSubmoduleNodeSibling() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    virtual SubstmachinesNode *getFirstSubstmachinesChild() const;
    virtual SubstparamsNode *getFirstSubstparamsChild() const;
    virtual GatesizesNode *getFirstGatesizesChild() const;
    virtual DisplayStringNode *getFirstDisplayStringChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;substparams&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT substparams (expression?, substparam*)>
 * <!ATTLIST substparams
 *      condition           CDATA     #IMPLIED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class SubstparamsNode : public NEDElement
{
  private:
    std::string condition;
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    SubstparamsNode() {applyDefaults();}
    SubstparamsNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~SubstparamsNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "substparams";}
    virtual int getTagCode() const {return NED_SUBSTPARAMS;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getCondition() const  {return condition.c_str();}
    void setCondition(const char * val)  {condition = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual SubstparamsNode *getNextSubstparamsNodeSibling() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    virtual SubstparamNode *getFirstSubstparamChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;substparam&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT substparam (expression?)>
 * <!ATTLIST substparam
 *      name                NMTOKEN   #REQUIRED
 *      value               CDATA     #IMPLIED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class SubstparamNode : public NEDElement
{
  private:
    std::string name;
    std::string value;
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    SubstparamNode() {applyDefaults();}
    SubstparamNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~SubstparamNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "substparam";}
    virtual int getTagCode() const {return NED_SUBSTPARAM;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getValue() const  {return value.c_str();}
    void setValue(const char * val)  {value = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual SubstparamNode *getNextSubstparamNodeSibling() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;gatesizes&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT gatesizes (expression?, gatesize*)>
 * <!ATTLIST gatesizes
 *      condition           CDATA     #IMPLIED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class GatesizesNode : public NEDElement
{
  private:
    std::string condition;
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    GatesizesNode() {applyDefaults();}
    GatesizesNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~GatesizesNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "gatesizes";}
    virtual int getTagCode() const {return NED_GATESIZES;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getCondition() const  {return condition.c_str();}
    void setCondition(const char * val)  {condition = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual GatesizesNode *getNextGatesizesNodeSibling() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    virtual GatesizeNode *getFirstGatesizeChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;gatesize&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT gatesize (expression?)>
 * <!ATTLIST gatesize
 *      name                NMTOKEN   #REQUIRED
 *      vector-size         CDATA     #IMPLIED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class GatesizeNode : public NEDElement
{
  private:
    std::string name;
    std::string vectorSize;
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    GatesizeNode() {applyDefaults();}
    GatesizeNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~GatesizeNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "gatesize";}
    virtual int getTagCode() const {return NED_GATESIZE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getVectorSize() const  {return vectorSize.c_str();}
    void setVectorSize(const char * val)  {vectorSize = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual GatesizeNode *getNextGatesizeNodeSibling() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;substmachines&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT substmachines (expression?, substmachine*)>
 * <!ATTLIST substmachines
 *      condition           CDATA     #IMPLIED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class SubstmachinesNode : public NEDElement
{
  private:
    std::string condition;
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    SubstmachinesNode() {applyDefaults();}
    SubstmachinesNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~SubstmachinesNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "substmachines";}
    virtual int getTagCode() const {return NED_SUBSTMACHINES;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getCondition() const  {return condition.c_str();}
    void setCondition(const char * val)  {condition = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual SubstmachinesNode *getNextSubstmachinesNodeSibling() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    virtual SubstmachineNode *getFirstSubstmachineChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;substmachine&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT substmachine EMPTY>
 * <!ATTLIST substmachine
 *      name                NMTOKEN   #REQUIRED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class SubstmachineNode : public NEDElement
{
  private:
    std::string name;
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    SubstmachineNode() {applyDefaults();}
    SubstmachineNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~SubstmachineNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "substmachine";}
    virtual int getTagCode() const {return NED_SUBSTMACHINE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual SubstmachineNode *getNextSubstmachineNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;connections&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT connections ((connection|for-loop)*)>
 * <!ATTLIST connections
 *      check-unconnected (true|false) "true"
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class ConnectionsNode : public NEDElement
{
  private:
    bool checkUnconnected;
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    ConnectionsNode() {applyDefaults();}
    ConnectionsNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ConnectionsNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "connections";}
    virtual int getTagCode() const {return NED_CONNECTIONS;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    bool getCheckUnconnected() const  {return checkUnconnected;}
    void setCheckUnconnected(bool val)  {checkUnconnected = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual ConnectionsNode *getNextConnectionsNodeSibling() const;
    virtual ConnectionNode *getFirstConnectionChild() const;
    virtual ForLoopNode *getFirstForLoopChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;connection&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT connection ((expression|conn-attr|display-string)*)>
 * <!ATTLIST connection
 *      condition           CDATA     #IMPLIED
 *      src-module          NMTOKEN   #IMPLIED
 *      src-module-index    CDATA     #IMPLIED
 *      src-gate            NMTOKEN   #REQUIRED
 *      src-gate-plusplus (true|false) "false"
 *      src-gate-index      CDATA     #IMPLIED
 *      dest-module         NMTOKEN   #IMPLIED
 *      dest-module-index   CDATA     #IMPLIED
 *      dest-gate           NMTOKEN   #REQUIRED
 *      dest-gate-plusplus (true|false) "false"
 *      dest-gate-index     CDATA     #IMPLIED
 *      arrow-direction  (left|right) "right"
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class ConnectionNode : public NEDElement
{
  private:
    std::string condition;
    std::string srcModule;
    std::string srcModuleIndex;
    std::string srcGate;
    bool srcGatePlusplus;
    std::string srcGateIndex;
    std::string destModule;
    std::string destModuleIndex;
    std::string destGate;
    bool destGatePlusplus;
    std::string destGateIndex;
    int arrowDirection;
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    ConnectionNode() {applyDefaults();}
    ConnectionNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ConnectionNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "connection";}
    virtual int getTagCode() const {return NED_CONNECTION;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getCondition() const  {return condition.c_str();}
    void setCondition(const char * val)  {condition = val;}
    const char * getSrcModule() const  {return srcModule.c_str();}
    void setSrcModule(const char * val)  {srcModule = val;}
    const char * getSrcModuleIndex() const  {return srcModuleIndex.c_str();}
    void setSrcModuleIndex(const char * val)  {srcModuleIndex = val;}
    const char * getSrcGate() const  {return srcGate.c_str();}
    void setSrcGate(const char * val)  {srcGate = val;}
    bool getSrcGatePlusplus() const  {return srcGatePlusplus;}
    void setSrcGatePlusplus(bool val)  {srcGatePlusplus = val;}
    const char * getSrcGateIndex() const  {return srcGateIndex.c_str();}
    void setSrcGateIndex(const char * val)  {srcGateIndex = val;}
    const char * getDestModule() const  {return destModule.c_str();}
    void setDestModule(const char * val)  {destModule = val;}
    const char * getDestModuleIndex() const  {return destModuleIndex.c_str();}
    void setDestModuleIndex(const char * val)  {destModuleIndex = val;}
    const char * getDestGate() const  {return destGate.c_str();}
    void setDestGate(const char * val)  {destGate = val;}
    bool getDestGatePlusplus() const  {return destGatePlusplus;}
    void setDestGatePlusplus(bool val)  {destGatePlusplus = val;}
    const char * getDestGateIndex() const  {return destGateIndex.c_str();}
    void setDestGateIndex(const char * val)  {destGateIndex = val;}
    int getArrowDirection() const  {return arrowDirection;}
    void setArrowDirection(int val)  {arrowDirection = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual ConnectionNode *getNextConnectionNodeSibling() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    virtual ConnAttrNode *getFirstConnAttrChild() const;
    virtual DisplayStringNode *getFirstDisplayStringChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;conn-attr&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT conn-attr (expression*)>
 * <!ATTLIST conn-attr
 *      name                NMTOKEN   #REQUIRED
 *      value               CDATA     #IMPLIED
 *      right-comment       CDATA     "" >
 * </pre>
 * 
 * @ingroup Data
 */
class ConnAttrNode : public NEDElement
{
  private:
    std::string name;
    std::string value;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    ConnAttrNode() {applyDefaults();}
    ConnAttrNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ConnAttrNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "conn-attr";}
    virtual int getTagCode() const {return NED_CONN_ATTR;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getValue() const  {return value.c_str();}
    void setValue(const char * val)  {value = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual ConnAttrNode *getNextConnAttrNodeSibling() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;for-loop&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT for-loop (loop-var+,connection*)>
 * <!ATTLIST for-loop
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;"
 *      trailing-comment    CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class ForLoopNode : public NEDElement
{
  private:
    std::string bannerComment;
    std::string rightComment;
    std::string trailingComment;
  public:
    /** @name Constructors, destructor */
    //@{
    ForLoopNode() {applyDefaults();}
    ForLoopNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ForLoopNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "for-loop";}
    virtual int getTagCode() const {return NED_FOR_LOOP;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}
    const char * getTrailingComment() const  {return trailingComment.c_str();}
    void setTrailingComment(const char * val)  {trailingComment = val;}

    virtual ForLoopNode *getNextForLoopNodeSibling() const;
    virtual LoopVarNode *getFirstLoopVarChild() const;
    virtual ConnectionNode *getFirstConnectionChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;loop-var&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT loop-var (expression*)>
 * <!ATTLIST loop-var
 *      param-name          NMTOKEN   #REQUIRED
 *      from-value          CDATA     #IMPLIED
 *      to-value            CDATA     #IMPLIED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class LoopVarNode : public NEDElement
{
  private:
    std::string paramName;
    std::string fromValue;
    std::string toValue;
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    LoopVarNode() {applyDefaults();}
    LoopVarNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~LoopVarNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "loop-var";}
    virtual int getTagCode() const {return NED_LOOP_VAR;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getParamName() const  {return paramName.c_str();}
    void setParamName(const char * val)  {paramName = val;}
    const char * getFromValue() const  {return fromValue.c_str();}
    void setFromValue(const char * val)  {fromValue = val;}
    const char * getToValue() const  {return toValue.c_str();}
    void setToValue(const char * val)  {toValue = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual LoopVarNode *getNextLoopVarNodeSibling() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;display-string&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT display-string EMPTY>
 * <!ATTLIST display-string
 *      value               CDATA     #IMPLIED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class DisplayStringNode : public NEDElement
{
  private:
    std::string value;
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    DisplayStringNode() {applyDefaults();}
    DisplayStringNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~DisplayStringNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "display-string";}
    virtual int getTagCode() const {return NED_DISPLAY_STRING;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getValue() const  {return value.c_str();}
    void setValue(const char * val)  {value = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual DisplayStringNode *getNextDisplayStringNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;expression&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT expression ((operator|function|param-ref|ident|const))>
 * <!ATTLIST expression
 *      target              CDATA     #IMPLIED >
 * </pre>
 * 
 * @ingroup Data
 */
class ExpressionNode : public NEDElement
{
  private:
    std::string target;
  public:
    /** @name Constructors, destructor */
    //@{
    ExpressionNode() {applyDefaults();}
    ExpressionNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ExpressionNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "expression";}
    virtual int getTagCode() const {return NED_EXPRESSION;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getTarget() const  {return target.c_str();}
    void setTarget(const char * val)  {target = val;}

    virtual ExpressionNode *getNextExpressionNodeSibling() const;
    virtual OperatorNode *getFirstOperatorChild() const;
    virtual FunctionNode *getFirstFunctionChild() const;
    virtual ParamRefNode *getFirstParamRefChild() const;
    virtual IdentNode *getFirstIdentChild() const;
    virtual ConstNode *getFirstConstChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;operator&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT operator ((operator|function|param-ref|ident|const)+)>
 * <!ATTLIST operator
 *      name                CDATA     #REQUIRED >
 * </pre>
 * 
 * @ingroup Data
 */
class OperatorNode : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    OperatorNode() {applyDefaults();}
    OperatorNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~OperatorNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "operator";}
    virtual int getTagCode() const {return NED_OPERATOR;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual OperatorNode *getNextOperatorNodeSibling() const;
    virtual OperatorNode *getFirstOperatorChild() const;
    virtual FunctionNode *getFirstFunctionChild() const;
    virtual ParamRefNode *getFirstParamRefChild() const;
    virtual IdentNode *getFirstIdentChild() const;
    virtual ConstNode *getFirstConstChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;function&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT function ((operator|function|param-ref|ident|const)*)>
 * <!ATTLIST function
 *      name                NMTOKEN   #REQUIRED >
 * </pre>
 * 
 * @ingroup Data
 */
class FunctionNode : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    FunctionNode() {applyDefaults();}
    FunctionNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~FunctionNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "function";}
    virtual int getTagCode() const {return NED_FUNCTION;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual FunctionNode *getNextFunctionNodeSibling() const;
    virtual OperatorNode *getFirstOperatorChild() const;
    virtual FunctionNode *getFirstFunctionChild() const;
    virtual ParamRefNode *getFirstParamRefChild() const;
    virtual IdentNode *getFirstIdentChild() const;
    virtual ConstNode *getFirstConstChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;param-ref&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT param-ref (expression*)>
 * <!ATTLIST param-ref
 *      module              CDATA     #IMPLIED
 *      module-index        CDATA     #IMPLIED
 *      param-name          NMTOKEN   #REQUIRED
 *      param-index         CDATA     #IMPLIED
 *      is-ref           (true|false) "false"
 *      is-ancestor      (true|false) "false" >
 * </pre>
 * 
 * @ingroup Data
 */
class ParamRefNode : public NEDElement
{
  private:
    std::string module;
    std::string moduleIndex;
    std::string paramName;
    std::string paramIndex;
    bool isRef;
    bool isAncestor;
  public:
    /** @name Constructors, destructor */
    //@{
    ParamRefNode() {applyDefaults();}
    ParamRefNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ParamRefNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "param-ref";}
    virtual int getTagCode() const {return NED_PARAM_REF;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getModule() const  {return module.c_str();}
    void setModule(const char * val)  {module = val;}
    const char * getModuleIndex() const  {return moduleIndex.c_str();}
    void setModuleIndex(const char * val)  {moduleIndex = val;}
    const char * getParamName() const  {return paramName.c_str();}
    void setParamName(const char * val)  {paramName = val;}
    const char * getParamIndex() const  {return paramIndex.c_str();}
    void setParamIndex(const char * val)  {paramIndex = val;}
    bool getIsRef() const  {return isRef;}
    void setIsRef(bool val)  {isRef = val;}
    bool getIsAncestor() const  {return isAncestor;}
    void setIsAncestor(bool val)  {isAncestor = val;}

    virtual ParamRefNode *getNextParamRefNodeSibling() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;ident&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT ident EMPTY>
 * <!ATTLIST ident
 *      name                NMTOKEN   #REQUIRED >
 * </pre>
 * 
 * @ingroup Data
 */
class IdentNode : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    IdentNode() {applyDefaults();}
    IdentNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~IdentNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "ident";}
    virtual int getTagCode() const {return NED_IDENT;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual IdentNode *getNextIdentNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;const&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT const EMPTY>
 * <!ATTLIST const
 *      type  (bool|int|real|string|time)  #REQUIRED
 *      text                CDATA     #IMPLIED
 *      value               CDATA     #IMPLIED>
 * </pre>
 * 
 * @ingroup Data
 */
class ConstNode : public NEDElement
{
  private:
    int type;
    std::string text;
    std::string value;
  public:
    /** @name Constructors, destructor */
    //@{
    ConstNode() {applyDefaults();}
    ConstNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ConstNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "const";}
    virtual int getTagCode() const {return NED_CONST;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    int getType() const  {return type;}
    void setType(int val)  {type = val;}
    const char * getText() const  {return text.c_str();}
    void setText(const char * val)  {text = val;}
    const char * getValue() const  {return value.c_str();}
    void setValue(const char * val)  {value = val;}

    virtual ConstNode *getNextConstNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;cplusplus&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT cplusplus EMPTY>
 * <!ATTLIST cplusplus
 *      body                CDATA     #REQUIRED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;"
 *      trailing-comment    CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class CplusplusNode : public NEDElement
{
  private:
    std::string body;
    std::string bannerComment;
    std::string rightComment;
    std::string trailingComment;
  public:
    /** @name Constructors, destructor */
    //@{
    CplusplusNode() {applyDefaults();}
    CplusplusNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~CplusplusNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "cplusplus";}
    virtual int getTagCode() const {return NED_CPLUSPLUS;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getBody() const  {return body.c_str();}
    void setBody(const char * val)  {body = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}
    const char * getTrailingComment() const  {return trailingComment.c_str();}
    void setTrailingComment(const char * val)  {trailingComment = val;}

    virtual CplusplusNode *getNextCplusplusNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;struct-decl&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT struct-decl EMPTY>
 * <!ATTLIST struct-decl
 *      name                NMTOKEN   #REQUIRED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;"
 *      trailing-comment    CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class StructDeclNode : public NEDElement
{
  private:
    std::string name;
    std::string bannerComment;
    std::string rightComment;
    std::string trailingComment;
  public:
    /** @name Constructors, destructor */
    //@{
    StructDeclNode() {applyDefaults();}
    StructDeclNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~StructDeclNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "struct-decl";}
    virtual int getTagCode() const {return NED_STRUCT_DECL;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}
    const char * getTrailingComment() const  {return trailingComment.c_str();}
    void setTrailingComment(const char * val)  {trailingComment = val;}

    virtual StructDeclNode *getNextStructDeclNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;class-decl&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT class-decl EMPTY>
 * <!ATTLIST class-decl
 *      name                NMTOKEN   #REQUIRED
 *      is-cobject      (true|false)  "false"
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;"
 *      trailing-comment    CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class ClassDeclNode : public NEDElement
{
  private:
    std::string name;
    bool isCobject;
    std::string bannerComment;
    std::string rightComment;
    std::string trailingComment;
  public:
    /** @name Constructors, destructor */
    //@{
    ClassDeclNode() {applyDefaults();}
    ClassDeclNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ClassDeclNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "class-decl";}
    virtual int getTagCode() const {return NED_CLASS_DECL;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    bool getIsCobject() const  {return isCobject;}
    void setIsCobject(bool val)  {isCobject = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}
    const char * getTrailingComment() const  {return trailingComment.c_str();}
    void setTrailingComment(const char * val)  {trailingComment = val;}

    virtual ClassDeclNode *getNextClassDeclNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;message-decl&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT message-decl EMPTY>
 * <!ATTLIST message-decl
 *      name                NMTOKEN   #REQUIRED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;"
 *      trailing-comment    CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class MessageDeclNode : public NEDElement
{
  private:
    std::string name;
    std::string bannerComment;
    std::string rightComment;
    std::string trailingComment;
  public:
    /** @name Constructors, destructor */
    //@{
    MessageDeclNode() {applyDefaults();}
    MessageDeclNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~MessageDeclNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "message-decl";}
    virtual int getTagCode() const {return NED_MESSAGE_DECL;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}
    const char * getTrailingComment() const  {return trailingComment.c_str();}
    void setTrailingComment(const char * val)  {trailingComment = val;}

    virtual MessageDeclNode *getNextMessageDeclNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;enum-decl&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT enum-decl EMPTY>
 * <!ATTLIST enum-decl
 *      name                NMTOKEN   #REQUIRED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;"
 *      trailing-comment    CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class EnumDeclNode : public NEDElement
{
  private:
    std::string name;
    std::string bannerComment;
    std::string rightComment;
    std::string trailingComment;
  public:
    /** @name Constructors, destructor */
    //@{
    EnumDeclNode() {applyDefaults();}
    EnumDeclNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~EnumDeclNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "enum-decl";}
    virtual int getTagCode() const {return NED_ENUM_DECL;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}
    const char * getTrailingComment() const  {return trailingComment.c_str();}
    void setTrailingComment(const char * val)  {trailingComment = val;}

    virtual EnumDeclNode *getNextEnumDeclNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;enum&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT enum (enum-fields?)>
 * <!ATTLIST enum
 *      name                NMTOKEN   #REQUIRED
 *      extends-name        NMTOKEN   #IMPLIED
 *      source-code         CDATA     #IMPLIED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;"
 *      trailing-comment    CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class EnumNode : public NEDElement
{
  private:
    std::string name;
    std::string extendsName;
    std::string sourceCode;
    std::string bannerComment;
    std::string rightComment;
    std::string trailingComment;
  public:
    /** @name Constructors, destructor */
    //@{
    EnumNode() {applyDefaults();}
    EnumNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~EnumNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "enum";}
    virtual int getTagCode() const {return NED_ENUM;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getExtendsName() const  {return extendsName.c_str();}
    void setExtendsName(const char * val)  {extendsName = val;}
    const char * getSourceCode() const  {return sourceCode.c_str();}
    void setSourceCode(const char * val)  {sourceCode = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}
    const char * getTrailingComment() const  {return trailingComment.c_str();}
    void setTrailingComment(const char * val)  {trailingComment = val;}

    virtual EnumNode *getNextEnumNodeSibling() const;
    virtual EnumFieldsNode *getFirstEnumFieldsChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;enum-fields&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT enum-fields (enum-field*)>
 * <!ATTLIST enum-fields
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class EnumFieldsNode : public NEDElement
{
  private:
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    EnumFieldsNode() {applyDefaults();}
    EnumFieldsNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~EnumFieldsNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "enum-fields";}
    virtual int getTagCode() const {return NED_ENUM_FIELDS;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual EnumFieldsNode *getNextEnumFieldsNodeSibling() const;
    virtual EnumFieldNode *getFirstEnumFieldChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;enum-field&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT enum-field EMPTY>
 * <!ATTLIST enum-field
 *      name                NMTOKEN   #REQUIRED
 *      value               CDATA     #IMPLIED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class EnumFieldNode : public NEDElement
{
  private:
    std::string name;
    std::string value;
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    EnumFieldNode() {applyDefaults();}
    EnumFieldNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~EnumFieldNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "enum-field";}
    virtual int getTagCode() const {return NED_ENUM_FIELD;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getValue() const  {return value.c_str();}
    void setValue(const char * val)  {value = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual EnumFieldNode *getNextEnumFieldNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;message&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT message (properties?,fields?)>
 * <!ATTLIST message
 *      name                NMTOKEN   #REQUIRED
 *      extends-name        NMTOKEN   #IMPLIED
 *      source-code         CDATA     #IMPLIED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;"
 *      trailing-comment    CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class MessageNode : public NEDElement
{
  private:
    std::string name;
    std::string extendsName;
    std::string sourceCode;
    std::string bannerComment;
    std::string rightComment;
    std::string trailingComment;
  public:
    /** @name Constructors, destructor */
    //@{
    MessageNode() {applyDefaults();}
    MessageNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~MessageNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "message";}
    virtual int getTagCode() const {return NED_MESSAGE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getExtendsName() const  {return extendsName.c_str();}
    void setExtendsName(const char * val)  {extendsName = val;}
    const char * getSourceCode() const  {return sourceCode.c_str();}
    void setSourceCode(const char * val)  {sourceCode = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}
    const char * getTrailingComment() const  {return trailingComment.c_str();}
    void setTrailingComment(const char * val)  {trailingComment = val;}

    virtual MessageNode *getNextMessageNodeSibling() const;
    virtual PropertiesNode *getFirstPropertiesChild() const;
    virtual FieldsNode *getFirstFieldsChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;class&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT class (properties?,fields?)>
 * <!ATTLIST class
 *      name                NMTOKEN   #REQUIRED
 *      extends-name        NMTOKEN   #IMPLIED
 *      source-code         CDATA     #IMPLIED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;"
 *      trailing-comment    CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class ClassNode : public NEDElement
{
  private:
    std::string name;
    std::string extendsName;
    std::string sourceCode;
    std::string bannerComment;
    std::string rightComment;
    std::string trailingComment;
  public:
    /** @name Constructors, destructor */
    //@{
    ClassNode() {applyDefaults();}
    ClassNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ClassNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "class";}
    virtual int getTagCode() const {return NED_CLASS;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getExtendsName() const  {return extendsName.c_str();}
    void setExtendsName(const char * val)  {extendsName = val;}
    const char * getSourceCode() const  {return sourceCode.c_str();}
    void setSourceCode(const char * val)  {sourceCode = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}
    const char * getTrailingComment() const  {return trailingComment.c_str();}
    void setTrailingComment(const char * val)  {trailingComment = val;}

    virtual ClassNode *getNextClassNodeSibling() const;
    virtual PropertiesNode *getFirstPropertiesChild() const;
    virtual FieldsNode *getFirstFieldsChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;struct&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT struct (properties?,fields?)>
 * <!ATTLIST struct
 *      name                NMTOKEN   #REQUIRED
 *      extends-name        NMTOKEN   #IMPLIED
 *      source-code         CDATA     #IMPLIED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;"
 *      trailing-comment    CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class StructNode : public NEDElement
{
  private:
    std::string name;
    std::string extendsName;
    std::string sourceCode;
    std::string bannerComment;
    std::string rightComment;
    std::string trailingComment;
  public:
    /** @name Constructors, destructor */
    //@{
    StructNode() {applyDefaults();}
    StructNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~StructNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "struct";}
    virtual int getTagCode() const {return NED_STRUCT;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getExtendsName() const  {return extendsName.c_str();}
    void setExtendsName(const char * val)  {extendsName = val;}
    const char * getSourceCode() const  {return sourceCode.c_str();}
    void setSourceCode(const char * val)  {sourceCode = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}
    const char * getTrailingComment() const  {return trailingComment.c_str();}
    void setTrailingComment(const char * val)  {trailingComment = val;}

    virtual StructNode *getNextStructNodeSibling() const;
    virtual PropertiesNode *getFirstPropertiesChild() const;
    virtual FieldsNode *getFirstFieldsChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;fields&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT fields (field*)>
 * <!ATTLIST fields
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class FieldsNode : public NEDElement
{
  private:
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    FieldsNode() {applyDefaults();}
    FieldsNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~FieldsNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "fields";}
    virtual int getTagCode() const {return NED_FIELDS;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual FieldsNode *getNextFieldsNodeSibling() const;
    virtual FieldNode *getFirstFieldChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;field&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT field EMPTY>
 * <!ATTLIST field
 *      name                NMTOKEN   #REQUIRED
 *      data-type           CDATA     #IMPLIED
 *      is-abstract     (true|false)  "false"
 *      is-readonly     (true|false)  "false"
 *      is-vector       (true|false)  "false"
 *      vector-size         CDATA     #IMPLIED
 *      enum-name           NMTOKEN   #IMPLIED
 *      default-value       CDATA     #IMPLIED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class FieldNode : public NEDElement
{
  private:
    std::string name;
    std::string dataType;
    bool isAbstract;
    bool isReadonly;
    bool isVector;
    std::string vectorSize;
    std::string enumName;
    std::string defaultValue;
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    FieldNode() {applyDefaults();}
    FieldNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~FieldNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "field";}
    virtual int getTagCode() const {return NED_FIELD;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getDataType() const  {return dataType.c_str();}
    void setDataType(const char * val)  {dataType = val;}
    bool getIsAbstract() const  {return isAbstract;}
    void setIsAbstract(bool val)  {isAbstract = val;}
    bool getIsReadonly() const  {return isReadonly;}
    void setIsReadonly(bool val)  {isReadonly = val;}
    bool getIsVector() const  {return isVector;}
    void setIsVector(bool val)  {isVector = val;}
    const char * getVectorSize() const  {return vectorSize.c_str();}
    void setVectorSize(const char * val)  {vectorSize = val;}
    const char * getEnumName() const  {return enumName.c_str();}
    void setEnumName(const char * val)  {enumName = val;}
    const char * getDefaultValue() const  {return defaultValue.c_str();}
    void setDefaultValue(const char * val)  {defaultValue = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual FieldNode *getNextFieldNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;properties&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT properties (property*)>
 * <!ATTLIST properties
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class PropertiesNode : public NEDElement
{
  private:
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    PropertiesNode() {applyDefaults();}
    PropertiesNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~PropertiesNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "properties";}
    virtual int getTagCode() const {return NED_PROPERTIES;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual PropertiesNode *getNextPropertiesNodeSibling() const;
    virtual PropertyNode *getFirstPropertyChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;property&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT property EMPTY>
 * <!ATTLIST property
 *      name                NMTOKEN   #REQUIRED
 *      value               CDATA     #IMPLIED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class PropertyNode : public NEDElement
{
  private:
    std::string name;
    std::string value;
    std::string bannerComment;
    std::string rightComment;
  public:
    /** @name Constructors, destructor */
    //@{
    PropertyNode() {applyDefaults();}
    PropertyNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~PropertyNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "property";}
    virtual int getTagCode() const {return NED_PROPERTY;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getValue() const  {return value.c_str();}
    void setValue(const char * val)  {value = val;}
    const char * getBannerComment() const  {return bannerComment.c_str();}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment.c_str();}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual PropertyNode *getNextPropertyNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;unknown&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT unknown        ANY>
 * <!ATTLIST unknown
 *      element             NMTOKEN   #REQUIRED>
 * </pre>
 * 
 * @ingroup Data
 */
class UnknownNode : public NEDElement
{
  private:
    std::string element;
  public:
    /** @name Constructors, destructor */
    //@{
    UnknownNode() {applyDefaults();}
    UnknownNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~UnknownNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "unknown";}
    virtual int getTagCode() const {return NED_UNKNOWN;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getElement() const  {return element.c_str();}
    void setElement(const char * val)  {element = val;}

    virtual UnknownNode *getNextUnknownNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Factory for NEDElement subclasses.
 * 
 * @ingroup Data
 */
class NEDElementFactory
{
  private:
    static NEDElementFactory *f;
    // ctor is private, because only one instance is allowed
    NEDElementFactory() {}
  public:
    /** Destructor */
    virtual ~NEDElementFactory() {}
    /** Returns factory instance */
    static NEDElementFactory *getInstance();
    /** Creates NEDElement subclass which corresponds to tagname */
    virtual NEDElement *createNodeWithTag(const char *tagname);
    /** Creates NEDElement subclass which corresponds to tagcode */
    virtual NEDElement *createNodeWithTag(int tagcode);
};

#endif


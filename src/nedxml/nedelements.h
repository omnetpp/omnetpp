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

NAMESPACE_BEGIN

class FilesNode;
class NedFileNode;
class CommentNode;
class PackageNode;
class ImportNode;
class PropertyDeclNode;
class ExtendsNode;
class InterfaceNameNode;
class SimpleModuleNode;
class ModuleInterfaceNode;
class CompoundModuleNode;
class ChannelInterfaceNode;
class ChannelNode;
class ParametersNode;
class ParamNode;
class PatternNode;
class PropertyNode;
class PropertyKeyNode;
class GatesNode;
class GateNode;
class TypesNode;
class SubmodulesNode;
class SubmoduleNode;
class ConnectionsNode;
class ConnectionNode;
class ChannelSpecNode;
class ConnectionGroupNode;
class LoopNode;
class ConditionNode;
class ExpressionNode;
class OperatorNode;
class FunctionNode;
class IdentNode;
class LiteralNode;
class MsgFileNode;
class NamespaceNode;
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
class FieldNode;
class UnknownNode;


/**
 * Tag codes
 * 
 * @ingroup Data
 */
enum NEDElementCode {
    NED_NULL = 0,  // 0 is reserved
    NED_FILES,
    NED_NED_FILE,
    NED_COMMENT,
    NED_PACKAGE,
    NED_IMPORT,
    NED_PROPERTY_DECL,
    NED_EXTENDS,
    NED_INTERFACE_NAME,
    NED_SIMPLE_MODULE,
    NED_MODULE_INTERFACE,
    NED_COMPOUND_MODULE,
    NED_CHANNEL_INTERFACE,
    NED_CHANNEL,
    NED_PARAMETERS,
    NED_PARAM,
    NED_PATTERN,
    NED_PROPERTY,
    NED_PROPERTY_KEY,
    NED_GATES,
    NED_GATE,
    NED_TYPES,
    NED_SUBMODULES,
    NED_SUBMODULE,
    NED_CONNECTIONS,
    NED_CONNECTION,
    NED_CHANNEL_SPEC,
    NED_CONNECTION_GROUP,
    NED_LOOP,
    NED_CONDITION,
    NED_EXPRESSION,
    NED_OPERATOR,
    NED_FUNCTION,
    NED_IDENT,
    NED_LITERAL,
    NED_MSG_FILE,
    NED_NAMESPACE,
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
    NED_FIELD,
    NED_UNKNOWN
};

// Note: zero *must* be a valid value for all enums, because that gets set in the ctor if there's not default
enum {NED_GATETYPE_NONE, NED_GATETYPE_INPUT, NED_GATETYPE_OUTPUT, NED_GATETYPE_INOUT};
enum {NED_ARROWDIR_R2L, NED_ARROWDIR_L2R, NED_ARROWDIR_BIDIR};
enum {NED_PARTYPE_NONE, NED_PARTYPE_DOUBLE, NED_PARTYPE_INT, NED_PARTYPE_STRING, NED_PARTYPE_BOOL, NED_PARTYPE_XML};
enum {NED_CONST_DOUBLE, NED_CONST_INT, NED_CONST_STRING, NED_CONST_BOOL, NED_CONST_SPEC};
enum {NED_SUBGATE_NONE, NED_SUBGATE_I, NED_SUBGATE_O};

/**
 * GENERATED CLASS. Represents the &lt;files&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT files ((ned-file|msg-file)*)>
 * 
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API FilesNode : public NEDElement
{
  private:
  public:
    /** @name Constructors, destructor */
    //@{
    FilesNode();
    FilesNode(NEDElement *parent);
    virtual ~FilesNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "files";}
    virtual int getTagCode() const {return NED_FILES;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    virtual FilesNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{

    virtual FilesNode *getNextFilesNodeSibling() const;
    virtual NedFileNode *getFirstNedFileChild() const;
    virtual MsgFileNode *getFirstMsgFileChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;ned-file&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT ned-file (comment*, (package|import|property-decl|property|channel|
 *                     channel-interface|simple-module|compound-module|module-interface)*)>
 * <!ATTLIST ned-file
 *      filename           CDATA     #REQUIRED
 *      version            CDATA     "2">
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API NedFileNode : public NEDElement
{
  private:
    std::string filename;
    std::string version;
  public:
    /** @name Constructors, destructor */
    //@{
    NedFileNode();
    NedFileNode(NEDElement *parent);
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
    virtual NedFileNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getFilename() const  {return filename.c_str();}
    void setFilename(const char * val)  {filename = val;}
    const char * getVersion() const  {return version.c_str();}
    void setVersion(const char * val)  {version = val;}

    virtual NedFileNode *getNextNedFileNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual PackageNode *getFirstPackageChild() const;
    virtual ImportNode *getFirstImportChild() const;
    virtual PropertyDeclNode *getFirstPropertyDeclChild() const;
    virtual PropertyNode *getFirstPropertyChild() const;
    virtual ChannelNode *getFirstChannelChild() const;
    virtual ChannelInterfaceNode *getFirstChannelInterfaceChild() const;
    virtual SimpleModuleNode *getFirstSimpleModuleChild() const;
    virtual CompoundModuleNode *getFirstCompoundModuleChild() const;
    virtual ModuleInterfaceNode *getFirstModuleInterfaceChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;comment&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT comment EMPTY>
 * <!ATTLIST comment
 *      locid              NMTOKEN   #REQUIRED
 *      content            CDATA     #IMPLIED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API CommentNode : public NEDElement
{
  private:
    std::string locid;
    std::string content;
  public:
    /** @name Constructors, destructor */
    //@{
    CommentNode();
    CommentNode(NEDElement *parent);
    virtual ~CommentNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "comment";}
    virtual int getTagCode() const {return NED_COMMENT;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    virtual CommentNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getLocid() const  {return locid.c_str();}
    void setLocid(const char * val)  {locid = val;}
    const char * getContent() const  {return content.c_str();}
    void setContent(const char * val)  {content = val;}

    virtual CommentNode *getNextCommentNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;package&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT package (comment*)>
 * <!ATTLIST package
 *      name               CDATA     #REQUIRED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API PackageNode : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    PackageNode();
    PackageNode(NEDElement *parent);
    virtual ~PackageNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "package";}
    virtual int getTagCode() const {return NED_PACKAGE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    virtual PackageNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual PackageNode *getNextPackageNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;import&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT import (comment*)>
 * <!ATTLIST import
 *      import-spec        CDATA     #REQUIRED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API ImportNode : public NEDElement
{
  private:
    std::string importSpec;
  public:
    /** @name Constructors, destructor */
    //@{
    ImportNode();
    ImportNode(NEDElement *parent);
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
    virtual ImportNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getImportSpec() const  {return importSpec.c_str();}
    void setImportSpec(const char * val)  {importSpec = val;}

    virtual ImportNode *getNextImportNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;property-decl&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT property-decl (comment*, property-key*, property*)>
 * <!ATTLIST property-decl
 *      name               NMTOKEN   #REQUIRED
 *      is-array           (true|false) "false">
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API PropertyDeclNode : public NEDElement
{
  private:
    std::string name;
    bool isArray;
  public:
    /** @name Constructors, destructor */
    //@{
    PropertyDeclNode();
    PropertyDeclNode(NEDElement *parent);
    virtual ~PropertyDeclNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "property-decl";}
    virtual int getTagCode() const {return NED_PROPERTY_DECL;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    virtual PropertyDeclNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    bool getIsArray() const  {return isArray;}
    void setIsArray(bool val)  {isArray = val;}

    virtual PropertyDeclNode *getNextPropertyDeclNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual PropertyKeyNode *getFirstPropertyKeyChild() const;
    virtual PropertyNode *getFirstPropertyChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;extends&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT extends (comment*)>
 * <!ATTLIST extends
 *      name               CDATA     #REQUIRED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API ExtendsNode : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    ExtendsNode();
    ExtendsNode(NEDElement *parent);
    virtual ~ExtendsNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "extends";}
    virtual int getTagCode() const {return NED_EXTENDS;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    virtual ExtendsNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual ExtendsNode *getNextExtendsNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;interface-name&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT interface-name (comment*)>
 * <!ATTLIST interface-name
 *      name               CDATA     #REQUIRED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API InterfaceNameNode : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    InterfaceNameNode();
    InterfaceNameNode(NEDElement *parent);
    virtual ~InterfaceNameNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "interface-name";}
    virtual int getTagCode() const {return NED_INTERFACE_NAME;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    virtual InterfaceNameNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual InterfaceNameNode *getNextInterfaceNameNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;simple-module&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT simple-module (comment*, extends?, interface-name*, parameters?, gates?)>
 * <!ATTLIST simple-module
 *      name               NMTOKEN   #REQUIRED
 *      is-network         (true|false) "false">
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API SimpleModuleNode : public NEDElement
{
  private:
    std::string name;
    bool isNetwork;
  public:
    /** @name Constructors, destructor */
    //@{
    SimpleModuleNode();
    SimpleModuleNode(NEDElement *parent);
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
    virtual SimpleModuleNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    bool getIsNetwork() const  {return isNetwork;}
    void setIsNetwork(bool val)  {isNetwork = val;}

    virtual SimpleModuleNode *getNextSimpleModuleNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual ExtendsNode *getFirstExtendsChild() const;
    virtual InterfaceNameNode *getFirstInterfaceNameChild() const;
    virtual ParametersNode *getFirstParametersChild() const;
    virtual GatesNode *getFirstGatesChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;module-interface&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT module-interface (comment*, extends*, parameters?, gates?)>
 * <!ATTLIST module-interface
 *      name               NMTOKEN   #REQUIRED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API ModuleInterfaceNode : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    ModuleInterfaceNode();
    ModuleInterfaceNode(NEDElement *parent);
    virtual ~ModuleInterfaceNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "module-interface";}
    virtual int getTagCode() const {return NED_MODULE_INTERFACE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    virtual ModuleInterfaceNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual ModuleInterfaceNode *getNextModuleInterfaceNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual ExtendsNode *getFirstExtendsChild() const;
    virtual ParametersNode *getFirstParametersChild() const;
    virtual GatesNode *getFirstGatesChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;compound-module&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT compound-module (comment*, extends?, interface-name*,
 *                            parameters?, gates?, types?, submodules?, connections?)>
 * <!ATTLIST compound-module
 *      name               NMTOKEN   #REQUIRED
 *      is-network         (true|false) "false">
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API CompoundModuleNode : public NEDElement
{
  private:
    std::string name;
    bool isNetwork;
  public:
    /** @name Constructors, destructor */
    //@{
    CompoundModuleNode();
    CompoundModuleNode(NEDElement *parent);
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
    virtual CompoundModuleNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    bool getIsNetwork() const  {return isNetwork;}
    void setIsNetwork(bool val)  {isNetwork = val;}

    virtual CompoundModuleNode *getNextCompoundModuleNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual ExtendsNode *getFirstExtendsChild() const;
    virtual InterfaceNameNode *getFirstInterfaceNameChild() const;
    virtual ParametersNode *getFirstParametersChild() const;
    virtual GatesNode *getFirstGatesChild() const;
    virtual TypesNode *getFirstTypesChild() const;
    virtual SubmodulesNode *getFirstSubmodulesChild() const;
    virtual ConnectionsNode *getFirstConnectionsChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;channel-interface&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT channel-interface (comment*, extends*, parameters?)>
 * <!ATTLIST channel-interface
 *      name                NMTOKEN   #REQUIRED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API ChannelInterfaceNode : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    ChannelInterfaceNode();
    ChannelInterfaceNode(NEDElement *parent);
    virtual ~ChannelInterfaceNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "channel-interface";}
    virtual int getTagCode() const {return NED_CHANNEL_INTERFACE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    virtual ChannelInterfaceNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual ChannelInterfaceNode *getNextChannelInterfaceNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual ExtendsNode *getFirstExtendsChild() const;
    virtual ParametersNode *getFirstParametersChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;channel&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT channel (comment*, extends?, interface-name*, parameters?)>
 * <!ATTLIST channel
 *      name                NMTOKEN   #REQUIRED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API ChannelNode : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    ChannelNode();
    ChannelNode(NEDElement *parent);
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
    virtual ChannelNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual ChannelNode *getNextChannelNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual ExtendsNode *getFirstExtendsChild() const;
    virtual InterfaceNameNode *getFirstInterfaceNameChild() const;
    virtual ParametersNode *getFirstParametersChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;parameters&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT parameters (comment*, (property|param|pattern)*)>
 * <!ATTLIST parameters
 *     is-implicit         (true|false)  "false">
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API ParametersNode : public NEDElement
{
  private:
    bool isImplicit;
  public:
    /** @name Constructors, destructor */
    //@{
    ParametersNode();
    ParametersNode(NEDElement *parent);
    virtual ~ParametersNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "parameters";}
    virtual int getTagCode() const {return NED_PARAMETERS;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    virtual ParametersNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    bool getIsImplicit() const  {return isImplicit;}
    void setIsImplicit(bool val)  {isImplicit = val;}

    virtual ParametersNode *getNextParametersNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual PropertyNode *getFirstPropertyChild() const;
    virtual ParamNode *getFirstParamChild() const;
    virtual PatternNode *getFirstPatternChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;param&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT param (comment*, expression?, property*)>
 * <!ATTLIST param
 *      type               (double|int|string|bool|xml) #IMPLIED
 *      is-volatile        (true|false)  "false"
 *      name               NMTOKEN   #REQUIRED
 *      value              CDATA     #IMPLIED
 *      is-default         (true|false)  "false">
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API ParamNode : public NEDElement
{
  private:
    int type;
    bool isVolatile;
    std::string name;
    std::string value;
    bool isDefault;
  public:
    /** @name Constructors, destructor */
    //@{
    ParamNode();
    ParamNode(NEDElement *parent);
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
    virtual ParamNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    int getType() const  {return type;}
    void setType(int val);
    bool getIsVolatile() const  {return isVolatile;}
    void setIsVolatile(bool val)  {isVolatile = val;}
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getValue() const  {return value.c_str();}
    void setValue(const char * val)  {value = val;}
    bool getIsDefault() const  {return isDefault;}
    void setIsDefault(bool val)  {isDefault = val;}

    virtual ParamNode *getNextParamNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    virtual PropertyNode *getFirstPropertyChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;pattern&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT pattern (comment*, expression?, property*)>
 * <!ATTLIST pattern
 *      pattern            CDATA     #REQUIRED
 *      value              CDATA     #IMPLIED
 *      is-default         (true|false)  "false">
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API PatternNode : public NEDElement
{
  private:
    std::string pattern;
    std::string value;
    bool isDefault;
  public:
    /** @name Constructors, destructor */
    //@{
    PatternNode();
    PatternNode(NEDElement *parent);
    virtual ~PatternNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "pattern";}
    virtual int getTagCode() const {return NED_PATTERN;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    virtual PatternNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getPattern() const  {return pattern.c_str();}
    void setPattern(const char * val)  {pattern = val;}
    const char * getValue() const  {return value.c_str();}
    void setValue(const char * val)  {value = val;}
    bool getIsDefault() const  {return isDefault;}
    void setIsDefault(bool val)  {isDefault = val;}

    virtual PatternNode *getNextPatternNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    virtual PropertyNode *getFirstPropertyChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;property&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT property (comment*, property-key*)>
 * <!ATTLIST property
 *      is-implicit        (true|false) "false"
 *      name               NMTOKEN   #REQUIRED
 *      index              NMTOKEN   #IMPLIED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API PropertyNode : public NEDElement
{
  private:
    bool isImplicit;
    std::string name;
    std::string index;
  public:
    /** @name Constructors, destructor */
    //@{
    PropertyNode();
    PropertyNode(NEDElement *parent);
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
    virtual PropertyNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    bool getIsImplicit() const  {return isImplicit;}
    void setIsImplicit(bool val)  {isImplicit = val;}
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getIndex() const  {return index.c_str();}
    void setIndex(const char * val)  {index = val;}

    virtual PropertyNode *getNextPropertyNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual PropertyKeyNode *getFirstPropertyKeyChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;property-key&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT property-key (comment*, literal*)>
 * <!ATTLIST property-key
 *      name               CDATA     #IMPLIED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API PropertyKeyNode : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    PropertyKeyNode();
    PropertyKeyNode(NEDElement *parent);
    virtual ~PropertyKeyNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "property-key";}
    virtual int getTagCode() const {return NED_PROPERTY_KEY;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    virtual PropertyKeyNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual PropertyKeyNode *getNextPropertyKeyNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual LiteralNode *getFirstLiteralChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;gates&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT gates (comment*, gate*)>
 * 
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API GatesNode : public NEDElement
{
  private:
  public:
    /** @name Constructors, destructor */
    //@{
    GatesNode();
    GatesNode(NEDElement *parent);
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
    virtual GatesNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{

    virtual GatesNode *getNextGatesNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual GateNode *getFirstGateChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;gate&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT gate (comment*, expression?, property*)>
 * <!ATTLIST gate
 *      name               NMTOKEN   #REQUIRED
 *      type               (input|output|inout) #IMPLIED
 *      is-vector          (true|false) "false"
 *      vector-size        CDATA     #IMPLIED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API GateNode : public NEDElement
{
  private:
    std::string name;
    int type;
    bool isVector;
    std::string vectorSize;
  public:
    /** @name Constructors, destructor */
    //@{
    GateNode();
    GateNode(NEDElement *parent);
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
    virtual GateNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    int getType() const  {return type;}
    void setType(int val);
    bool getIsVector() const  {return isVector;}
    void setIsVector(bool val)  {isVector = val;}
    const char * getVectorSize() const  {return vectorSize.c_str();}
    void setVectorSize(const char * val)  {vectorSize = val;}

    virtual GateNode *getNextGateNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    virtual PropertyNode *getFirstPropertyChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;types&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT types (comment*, (channel|channel-interface|simple-module|
 *                                compound-module|module-interface)*)>
 * 
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API TypesNode : public NEDElement
{
  private:
  public:
    /** @name Constructors, destructor */
    //@{
    TypesNode();
    TypesNode(NEDElement *parent);
    virtual ~TypesNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "types";}
    virtual int getTagCode() const {return NED_TYPES;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    virtual TypesNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{

    virtual TypesNode *getNextTypesNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual ChannelNode *getFirstChannelChild() const;
    virtual ChannelInterfaceNode *getFirstChannelInterfaceChild() const;
    virtual SimpleModuleNode *getFirstSimpleModuleChild() const;
    virtual CompoundModuleNode *getFirstCompoundModuleChild() const;
    virtual ModuleInterfaceNode *getFirstModuleInterfaceChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;submodules&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT submodules (comment*, submodule*)>
 * 
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API SubmodulesNode : public NEDElement
{
  private:
  public:
    /** @name Constructors, destructor */
    //@{
    SubmodulesNode();
    SubmodulesNode(NEDElement *parent);
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
    virtual SubmodulesNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{

    virtual SubmodulesNode *getNextSubmodulesNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual SubmoduleNode *getFirstSubmoduleChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;submodule&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT submodule (comment*, expression*, parameters?, gates?)>
 * <!ATTLIST submodule
 *      name               NMTOKEN   #REQUIRED
 *      type               CDATA     #IMPLIED
 *      like-type          CDATA     #IMPLIED
 *      like-param         CDATA     #IMPLIED
 *      vector-size        CDATA     #IMPLIED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API SubmoduleNode : public NEDElement
{
  private:
    std::string name;
    std::string type;
    std::string likeType;
    std::string likeParam;
    std::string vectorSize;
  public:
    /** @name Constructors, destructor */
    //@{
    SubmoduleNode();
    SubmoduleNode(NEDElement *parent);
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
    virtual SubmoduleNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getType() const  {return type.c_str();}
    void setType(const char * val)  {type = val;}
    const char * getLikeType() const  {return likeType.c_str();}
    void setLikeType(const char * val)  {likeType = val;}
    const char * getLikeParam() const  {return likeParam.c_str();}
    void setLikeParam(const char * val)  {likeParam = val;}
    const char * getVectorSize() const  {return vectorSize.c_str();}
    void setVectorSize(const char * val)  {vectorSize = val;}

    virtual SubmoduleNode *getNextSubmoduleNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    virtual ParametersNode *getFirstParametersChild() const;
    virtual GatesNode *getFirstGatesChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;connections&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT connections (comment*, (connection|connection-group)*)>
 * <!ATTLIST connections
 *      allow-unconnected (true|false) "false">
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API ConnectionsNode : public NEDElement
{
  private:
    bool allowUnconnected;
  public:
    /** @name Constructors, destructor */
    //@{
    ConnectionsNode();
    ConnectionsNode(NEDElement *parent);
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
    virtual ConnectionsNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    bool getAllowUnconnected() const  {return allowUnconnected;}
    void setAllowUnconnected(bool val)  {allowUnconnected = val;}

    virtual ConnectionsNode *getNextConnectionsNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual ConnectionNode *getFirstConnectionChild() const;
    virtual ConnectionGroupNode *getFirstConnectionGroupChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;connection&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT connection (comment*, expression*, channel-spec?, (loop|condition)*)>
 * <!ATTLIST connection
 *      src-module          NMTOKEN   #IMPLIED
 *      src-module-index    CDATA     #IMPLIED
 *      src-gate            NMTOKEN   #REQUIRED
 *      src-gate-plusplus  (true|false) "false"
 *      src-gate-index      CDATA     #IMPLIED
 *      src-gate-subg       (i|o)     #IMPLIED
 *      dest-module         NMTOKEN   #IMPLIED
 *      dest-module-index   CDATA     #IMPLIED
 *      dest-gate           NMTOKEN   #REQUIRED
 *      dest-gate-plusplus (true|false) "false"
 *      dest-gate-index     CDATA     #IMPLIED
 *      dest-gate-subg      (i|o)     #IMPLIED
 *      arrow-direction    (l2r|r2l|bidir) #REQUIRED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API ConnectionNode : public NEDElement
{
  private:
    std::string srcModule;
    std::string srcModuleIndex;
    std::string srcGate;
    bool srcGatePlusplus;
    std::string srcGateIndex;
    int srcGateSubg;
    std::string destModule;
    std::string destModuleIndex;
    std::string destGate;
    bool destGatePlusplus;
    std::string destGateIndex;
    int destGateSubg;
    int arrowDirection;
  public:
    /** @name Constructors, destructor */
    //@{
    ConnectionNode();
    ConnectionNode(NEDElement *parent);
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
    virtual ConnectionNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
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
    int getSrcGateSubg() const  {return srcGateSubg;}
    void setSrcGateSubg(int val);
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
    int getDestGateSubg() const  {return destGateSubg;}
    void setDestGateSubg(int val);
    int getArrowDirection() const  {return arrowDirection;}
    void setArrowDirection(int val);

    virtual ConnectionNode *getNextConnectionNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    virtual ChannelSpecNode *getFirstChannelSpecChild() const;
    virtual LoopNode *getFirstLoopChild() const;
    virtual ConditionNode *getFirstConditionChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;channel-spec&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT channel-spec (comment*, expression*, parameters?)>
 * <!ATTLIST channel-spec
 *      type               CDATA     #IMPLIED
 *      like-type          CDATA     #IMPLIED
 *      like-param         CDATA     #IMPLIED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API ChannelSpecNode : public NEDElement
{
  private:
    std::string type;
    std::string likeType;
    std::string likeParam;
  public:
    /** @name Constructors, destructor */
    //@{
    ChannelSpecNode();
    ChannelSpecNode(NEDElement *parent);
    virtual ~ChannelSpecNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "channel-spec";}
    virtual int getTagCode() const {return NED_CHANNEL_SPEC;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    virtual ChannelSpecNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getType() const  {return type.c_str();}
    void setType(const char * val)  {type = val;}
    const char * getLikeType() const  {return likeType.c_str();}
    void setLikeType(const char * val)  {likeType = val;}
    const char * getLikeParam() const  {return likeParam.c_str();}
    void setLikeParam(const char * val)  {likeParam = val;}

    virtual ChannelSpecNode *getNextChannelSpecNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    virtual ParametersNode *getFirstParametersChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;connection-group&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT connection-group (comment*, (loop|condition)*, connection*)>
 * 
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API ConnectionGroupNode : public NEDElement
{
  private:
  public:
    /** @name Constructors, destructor */
    //@{
    ConnectionGroupNode();
    ConnectionGroupNode(NEDElement *parent);
    virtual ~ConnectionGroupNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "connection-group";}
    virtual int getTagCode() const {return NED_CONNECTION_GROUP;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    virtual ConnectionGroupNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{

    virtual ConnectionGroupNode *getNextConnectionGroupNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual LoopNode *getFirstLoopChild() const;
    virtual ConditionNode *getFirstConditionChild() const;
    virtual ConnectionNode *getFirstConnectionChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;loop&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT loop (comment*, expression*)>
 * <!ATTLIST loop
 *      param-name          NMTOKEN   #REQUIRED
 *      from-value          CDATA     #IMPLIED
 *      to-value            CDATA     #IMPLIED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API LoopNode : public NEDElement
{
  private:
    std::string paramName;
    std::string fromValue;
    std::string toValue;
  public:
    /** @name Constructors, destructor */
    //@{
    LoopNode();
    LoopNode(NEDElement *parent);
    virtual ~LoopNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "loop";}
    virtual int getTagCode() const {return NED_LOOP;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    virtual LoopNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getParamName() const  {return paramName.c_str();}
    void setParamName(const char * val)  {paramName = val;}
    const char * getFromValue() const  {return fromValue.c_str();}
    void setFromValue(const char * val)  {fromValue = val;}
    const char * getToValue() const  {return toValue.c_str();}
    void setToValue(const char * val)  {toValue = val;}

    virtual LoopNode *getNextLoopNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;condition&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT condition (comment*, expression?)>
 * <!ATTLIST condition
 *      condition           CDATA     #IMPLIED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API ConditionNode : public NEDElement
{
  private:
    std::string condition;
  public:
    /** @name Constructors, destructor */
    //@{
    ConditionNode();
    ConditionNode(NEDElement *parent);
    virtual ~ConditionNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "condition";}
    virtual int getTagCode() const {return NED_CONDITION;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    virtual ConditionNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getCondition() const  {return condition.c_str();}
    void setCondition(const char * val)  {condition = val;}

    virtual ConditionNode *getNextConditionNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;expression&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT expression (comment*, (operator|function|ident|literal))>
 * <!ATTLIST expression
 *      target              CDATA     #IMPLIED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API ExpressionNode : public NEDElement
{
  private:
    std::string target;
  public:
    /** @name Constructors, destructor */
    //@{
    ExpressionNode();
    ExpressionNode(NEDElement *parent);
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
    virtual ExpressionNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getTarget() const  {return target.c_str();}
    void setTarget(const char * val)  {target = val;}

    virtual ExpressionNode *getNextExpressionNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual OperatorNode *getFirstOperatorChild() const;
    virtual FunctionNode *getFirstFunctionChild() const;
    virtual IdentNode *getFirstIdentChild() const;
    virtual LiteralNode *getFirstLiteralChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;operator&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT operator (comment*, (operator|function|ident|literal)+)>
 * <!ATTLIST operator
 *      name                CDATA     #REQUIRED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API OperatorNode : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    OperatorNode();
    OperatorNode(NEDElement *parent);
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
    virtual OperatorNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual OperatorNode *getNextOperatorNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual OperatorNode *getFirstOperatorChild() const;
    virtual FunctionNode *getFirstFunctionChild() const;
    virtual IdentNode *getFirstIdentChild() const;
    virtual LiteralNode *getFirstLiteralChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;function&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT function (comment*, (operator|function|ident|literal)*)>
 * <!ATTLIST function
 *      name                NMTOKEN   #REQUIRED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API FunctionNode : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    FunctionNode();
    FunctionNode(NEDElement *parent);
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
    virtual FunctionNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual FunctionNode *getNextFunctionNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual OperatorNode *getFirstOperatorChild() const;
    virtual FunctionNode *getFirstFunctionChild() const;
    virtual IdentNode *getFirstIdentChild() const;
    virtual LiteralNode *getFirstLiteralChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;ident&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT ident (comment*, (operator|function|ident|literal)?)>
 * <!ATTLIST ident
 *      module              CDATA     #IMPLIED
 *      name                NMTOKEN   #REQUIRED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API IdentNode : public NEDElement
{
  private:
    std::string module;
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    IdentNode();
    IdentNode(NEDElement *parent);
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
    virtual IdentNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getModule() const  {return module.c_str();}
    void setModule(const char * val)  {module = val;}
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual IdentNode *getNextIdentNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual OperatorNode *getFirstOperatorChild() const;
    virtual FunctionNode *getFirstFunctionChild() const;
    virtual IdentNode *getFirstIdentChild() const;
    virtual LiteralNode *getFirstLiteralChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;literal&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT literal (comment*)>
 * <!ATTLIST literal
 *      type  (double|int|string|bool|spec)  #REQUIRED
 *      unit                CDATA     #IMPLIED
 *      text                CDATA     #IMPLIED
 *      value               CDATA     #IMPLIED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API LiteralNode : public NEDElement
{
  private:
    int type;
    std::string unit;
    std::string text;
    std::string value;
  public:
    /** @name Constructors, destructor */
    //@{
    LiteralNode();
    LiteralNode(NEDElement *parent);
    virtual ~LiteralNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "literal";}
    virtual int getTagCode() const {return NED_LITERAL;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    virtual LiteralNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    int getType() const  {return type;}
    void setType(int val);
    const char * getUnit() const  {return unit.c_str();}
    void setUnit(const char * val)  {unit = val;}
    const char * getText() const  {return text.c_str();}
    void setText(const char * val)  {text = val;}
    const char * getValue() const  {return value.c_str();}
    void setValue(const char * val)  {value = val;}

    virtual LiteralNode *getNextLiteralNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;msg-file&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT msg-file (comment*, (namespace|property-decl|property|cplusplus|struct-decl|class-decl|message-decl|enum-decl|
 *                      enum|message|class|struct)*)>
 * <!ATTLIST msg-file
 *      filename            CDATA     #IMPLIED
 *      version             CDATA     "2">
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API MsgFileNode : public NEDElement
{
  private:
    std::string filename;
    std::string version;
  public:
    /** @name Constructors, destructor */
    //@{
    MsgFileNode();
    MsgFileNode(NEDElement *parent);
    virtual ~MsgFileNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "msg-file";}
    virtual int getTagCode() const {return NED_MSG_FILE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    virtual MsgFileNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getFilename() const  {return filename.c_str();}
    void setFilename(const char * val)  {filename = val;}
    const char * getVersion() const  {return version.c_str();}
    void setVersion(const char * val)  {version = val;}

    virtual MsgFileNode *getNextMsgFileNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual NamespaceNode *getFirstNamespaceChild() const;
    virtual PropertyDeclNode *getFirstPropertyDeclChild() const;
    virtual PropertyNode *getFirstPropertyChild() const;
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
 * GENERATED CLASS. Represents the &lt;namespace&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT namespace (comment*)>
 * <!ATTLIST namespace
 *      name                NMTOKEN   #REQUIRED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API NamespaceNode : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    NamespaceNode();
    NamespaceNode(NEDElement *parent);
    virtual ~NamespaceNode() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "namespace";}
    virtual int getTagCode() const {return NED_NAMESPACE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual void setAttribute(int k, const char *val);
    virtual const char *getAttributeDefault(int k) const;
    virtual NamespaceNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual NamespaceNode *getNextNamespaceNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;cplusplus&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT cplusplus (comment*)>
 * <!ATTLIST cplusplus
 *      body                CDATA     #REQUIRED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API CplusplusNode : public NEDElement
{
  private:
    std::string body;
  public:
    /** @name Constructors, destructor */
    //@{
    CplusplusNode();
    CplusplusNode(NEDElement *parent);
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
    virtual CplusplusNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getBody() const  {return body.c_str();}
    void setBody(const char * val)  {body = val;}

    virtual CplusplusNode *getNextCplusplusNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;struct-decl&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT struct-decl (comment*)>
 * <!ATTLIST struct-decl
 *      name                NMTOKEN   #REQUIRED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API StructDeclNode : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    StructDeclNode();
    StructDeclNode(NEDElement *parent);
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
    virtual StructDeclNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual StructDeclNode *getNextStructDeclNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;class-decl&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT class-decl (comment*)>
 * <!ATTLIST class-decl
 *      name                NMTOKEN   #REQUIRED
 *      is-cobject      (true|false)  "false">
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API ClassDeclNode : public NEDElement
{
  private:
    std::string name;
    bool isCobject;
  public:
    /** @name Constructors, destructor */
    //@{
    ClassDeclNode();
    ClassDeclNode(NEDElement *parent);
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
    virtual ClassDeclNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    bool getIsCobject() const  {return isCobject;}
    void setIsCobject(bool val)  {isCobject = val;}

    virtual ClassDeclNode *getNextClassDeclNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;message-decl&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT message-decl (comment*)>
 * <!ATTLIST message-decl
 *      name                NMTOKEN   #REQUIRED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API MessageDeclNode : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    MessageDeclNode();
    MessageDeclNode(NEDElement *parent);
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
    virtual MessageDeclNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual MessageDeclNode *getNextMessageDeclNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;enum-decl&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT enum-decl (comment*)>
 * <!ATTLIST enum-decl
 *      name                NMTOKEN   #REQUIRED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API EnumDeclNode : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    EnumDeclNode();
    EnumDeclNode(NEDElement *parent);
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
    virtual EnumDeclNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual EnumDeclNode *getNextEnumDeclNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;enum&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT enum (comment*, enum-fields?)>
 * <!ATTLIST enum
 *      name                NMTOKEN   #REQUIRED
 *      extends-name        NMTOKEN   #IMPLIED
 *      source-code         CDATA     #IMPLIED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API EnumNode : public NEDElement
{
  private:
    std::string name;
    std::string extendsName;
    std::string sourceCode;
  public:
    /** @name Constructors, destructor */
    //@{
    EnumNode();
    EnumNode(NEDElement *parent);
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
    virtual EnumNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getExtendsName() const  {return extendsName.c_str();}
    void setExtendsName(const char * val)  {extendsName = val;}
    const char * getSourceCode() const  {return sourceCode.c_str();}
    void setSourceCode(const char * val)  {sourceCode = val;}

    virtual EnumNode *getNextEnumNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual EnumFieldsNode *getFirstEnumFieldsChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;enum-fields&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT enum-fields (comment*, enum-field*)>
 * 
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API EnumFieldsNode : public NEDElement
{
  private:
  public:
    /** @name Constructors, destructor */
    //@{
    EnumFieldsNode();
    EnumFieldsNode(NEDElement *parent);
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
    virtual EnumFieldsNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{

    virtual EnumFieldsNode *getNextEnumFieldsNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual EnumFieldNode *getFirstEnumFieldChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;enum-field&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT enum-field (comment*)>
 * <!ATTLIST enum-field
 *      name                NMTOKEN   #REQUIRED
 *      value               CDATA     #IMPLIED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API EnumFieldNode : public NEDElement
{
  private:
    std::string name;
    std::string value;
  public:
    /** @name Constructors, destructor */
    //@{
    EnumFieldNode();
    EnumFieldNode(NEDElement *parent);
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
    virtual EnumFieldNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getValue() const  {return value.c_str();}
    void setValue(const char * val)  {value = val;}

    virtual EnumFieldNode *getNextEnumFieldNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;message&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT message (comment*, (property|field)*)>
 * <!ATTLIST message
 *      name                NMTOKEN   #REQUIRED
 *      extends-name        NMTOKEN   #IMPLIED
 *      source-code         CDATA     #IMPLIED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API MessageNode : public NEDElement
{
  private:
    std::string name;
    std::string extendsName;
    std::string sourceCode;
  public:
    /** @name Constructors, destructor */
    //@{
    MessageNode();
    MessageNode(NEDElement *parent);
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
    virtual MessageNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getExtendsName() const  {return extendsName.c_str();}
    void setExtendsName(const char * val)  {extendsName = val;}
    const char * getSourceCode() const  {return sourceCode.c_str();}
    void setSourceCode(const char * val)  {sourceCode = val;}

    virtual MessageNode *getNextMessageNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual PropertyNode *getFirstPropertyChild() const;
    virtual FieldNode *getFirstFieldChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;class&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT class (comment*, (property|field)*)>
 * <!ATTLIST class
 *      name                NMTOKEN   #REQUIRED
 *      extends-name        NMTOKEN   #IMPLIED
 *      source-code         CDATA     #IMPLIED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API ClassNode : public NEDElement
{
  private:
    std::string name;
    std::string extendsName;
    std::string sourceCode;
  public:
    /** @name Constructors, destructor */
    //@{
    ClassNode();
    ClassNode(NEDElement *parent);
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
    virtual ClassNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getExtendsName() const  {return extendsName.c_str();}
    void setExtendsName(const char * val)  {extendsName = val;}
    const char * getSourceCode() const  {return sourceCode.c_str();}
    void setSourceCode(const char * val)  {sourceCode = val;}

    virtual ClassNode *getNextClassNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual PropertyNode *getFirstPropertyChild() const;
    virtual FieldNode *getFirstFieldChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;struct&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT struct (comment*, (property|field)*)>
 * <!ATTLIST struct
 *      name                NMTOKEN   #REQUIRED
 *      extends-name        NMTOKEN   #IMPLIED
 *      source-code         CDATA     #IMPLIED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API StructNode : public NEDElement
{
  private:
    std::string name;
    std::string extendsName;
    std::string sourceCode;
  public:
    /** @name Constructors, destructor */
    //@{
    StructNode();
    StructNode(NEDElement *parent);
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
    virtual StructNode *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getExtendsName() const  {return extendsName.c_str();}
    void setExtendsName(const char * val)  {extendsName = val;}
    const char * getSourceCode() const  {return sourceCode.c_str();}
    void setSourceCode(const char * val)  {sourceCode = val;}

    virtual StructNode *getNextStructNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    virtual PropertyNode *getFirstPropertyChild() const;
    virtual FieldNode *getFirstFieldChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;field&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT field (comment*)>
 * <!ATTLIST field
 *      name                NMTOKEN   #REQUIRED
 *      data-type           CDATA     #IMPLIED
 *      is-abstract     (true|false)  "false"
 *      is-readonly     (true|false)  "false"
 *      is-vector       (true|false)  "false"
 *      vector-size         CDATA     #IMPLIED
 *      enum-name           NMTOKEN   #IMPLIED
 *      default-value       CDATA     #IMPLIED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API FieldNode : public NEDElement
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
  public:
    /** @name Constructors, destructor */
    //@{
    FieldNode();
    FieldNode(NEDElement *parent);
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
    virtual FieldNode *dup() const;
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

    virtual FieldNode *getNextFieldNodeSibling() const;
    virtual CommentNode *getFirstCommentChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;unknown&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT unknown        ANY>
 * <!ATTLIST unknown
 *      element             CDATA     #REQUIRED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API UnknownNode : public NEDElement
{
  private:
    std::string element;
  public:
    /** @name Constructors, destructor */
    //@{
    UnknownNode();
    UnknownNode(NEDElement *parent);
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
    virtual UnknownNode *dup() const;
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
class NEDXML_API NEDElementFactory
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

NAMESPACE_END

#endif


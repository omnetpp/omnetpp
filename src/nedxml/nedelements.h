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

class FilesElement;
class NedFileElement;
class CommentElement;
class PackageElement;
class ImportElement;
class PropertyDeclElement;
class ExtendsElement;
class InterfaceNameElement;
class SimpleModuleElement;
class ModuleInterfaceElement;
class CompoundModuleElement;
class ChannelInterfaceElement;
class ChannelElement;
class ParametersElement;
class ParamElement;
class PatternElement;
class PropertyElement;
class PropertyKeyElement;
class GatesElement;
class GateElement;
class TypesElement;
class SubmodulesElement;
class SubmoduleElement;
class ConnectionsElement;
class ConnectionElement;
class ChannelSpecElement;
class ConnectionGroupElement;
class LoopElement;
class ConditionElement;
class ExpressionElement;
class OperatorElement;
class FunctionElement;
class IdentElement;
class LiteralElement;
class MsgFileElement;
class NamespaceElement;
class CplusplusElement;
class StructDeclElement;
class ClassDeclElement;
class MessageDeclElement;
class EnumDeclElement;
class EnumElement;
class EnumFieldsElement;
class EnumFieldElement;
class MessageElement;
class ClassElement;
class StructElement;
class FieldElement;
class UnknownElement;


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
class NEDXML_API FilesElement : public NEDElement
{
  private:
  public:
    /** @name Constructors, destructor */
    //@{
    FilesElement();
    FilesElement(NEDElement *parent);
    virtual ~FilesElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "files";}
    virtual int getTagCode() const {return NED_FILES;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual FilesElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{

    virtual FilesElement *getNextFilesSibling() const;
    virtual NedFileElement *getFirstNedFileChild() const;
    virtual MsgFileElement *getFirstMsgFileChild() const;
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
class NEDXML_API NedFileElement : public NEDElement
{
  private:
    std::string filename;
    std::string version;
  public:
    /** @name Constructors, destructor */
    //@{
    NedFileElement();
    NedFileElement(NEDElement *parent);
    virtual ~NedFileElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "ned-file";}
    virtual int getTagCode() const {return NED_NED_FILE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual NedFileElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getFilename() const  {return filename.c_str();}
    void setFilename(const char * val)  {filename = val;}
    const char * getVersion() const  {return version.c_str();}
    void setVersion(const char * val)  {version = val;}

    virtual NedFileElement *getNextNedFileSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual PackageElement *getFirstPackageChild() const;
    virtual ImportElement *getFirstImportChild() const;
    virtual PropertyDeclElement *getFirstPropertyDeclChild() const;
    virtual PropertyElement *getFirstPropertyChild() const;
    virtual ChannelElement *getFirstChannelChild() const;
    virtual ChannelInterfaceElement *getFirstChannelInterfaceChild() const;
    virtual SimpleModuleElement *getFirstSimpleModuleChild() const;
    virtual CompoundModuleElement *getFirstCompoundModuleChild() const;
    virtual ModuleInterfaceElement *getFirstModuleInterfaceChild() const;
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
class NEDXML_API CommentElement : public NEDElement
{
  private:
    std::string locid;
    std::string content;
  public:
    /** @name Constructors, destructor */
    //@{
    CommentElement();
    CommentElement(NEDElement *parent);
    virtual ~CommentElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "comment";}
    virtual int getTagCode() const {return NED_COMMENT;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual CommentElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getLocid() const  {return locid.c_str();}
    void setLocid(const char * val)  {locid = val;}
    const char * getContent() const  {return content.c_str();}
    void setContent(const char * val)  {content = val;}

    virtual CommentElement *getNextCommentSibling() const;
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
class NEDXML_API PackageElement : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    PackageElement();
    PackageElement(NEDElement *parent);
    virtual ~PackageElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "package";}
    virtual int getTagCode() const {return NED_PACKAGE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual PackageElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual PackageElement *getNextPackageSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
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
class NEDXML_API ImportElement : public NEDElement
{
  private:
    std::string importSpec;
  public:
    /** @name Constructors, destructor */
    //@{
    ImportElement();
    ImportElement(NEDElement *parent);
    virtual ~ImportElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "import";}
    virtual int getTagCode() const {return NED_IMPORT;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual ImportElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getImportSpec() const  {return importSpec.c_str();}
    void setImportSpec(const char * val)  {importSpec = val;}

    virtual ImportElement *getNextImportSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
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
class NEDXML_API PropertyDeclElement : public NEDElement
{
  private:
    std::string name;
    bool isArray;
  public:
    /** @name Constructors, destructor */
    //@{
    PropertyDeclElement();
    PropertyDeclElement(NEDElement *parent);
    virtual ~PropertyDeclElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "property-decl";}
    virtual int getTagCode() const {return NED_PROPERTY_DECL;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual PropertyDeclElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    bool getIsArray() const  {return isArray;}
    void setIsArray(bool val)  {isArray = val;}

    virtual PropertyDeclElement *getNextPropertyDeclSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual PropertyKeyElement *getFirstPropertyKeyChild() const;
    virtual PropertyElement *getFirstPropertyChild() const;
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
class NEDXML_API ExtendsElement : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    ExtendsElement();
    ExtendsElement(NEDElement *parent);
    virtual ~ExtendsElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "extends";}
    virtual int getTagCode() const {return NED_EXTENDS;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual ExtendsElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual ExtendsElement *getNextExtendsSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
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
class NEDXML_API InterfaceNameElement : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    InterfaceNameElement();
    InterfaceNameElement(NEDElement *parent);
    virtual ~InterfaceNameElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "interface-name";}
    virtual int getTagCode() const {return NED_INTERFACE_NAME;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual InterfaceNameElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual InterfaceNameElement *getNextInterfaceNameSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;simple-module&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT simple-module (comment*, extends?, interface-name*, parameters?, gates?)>
 * <!ATTLIST simple-module
 *      name               NMTOKEN   #REQUIRED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API SimpleModuleElement : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    SimpleModuleElement();
    SimpleModuleElement(NEDElement *parent);
    virtual ~SimpleModuleElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "simple-module";}
    virtual int getTagCode() const {return NED_SIMPLE_MODULE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual SimpleModuleElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual SimpleModuleElement *getNextSimpleModuleSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual ExtendsElement *getFirstExtendsChild() const;
    virtual InterfaceNameElement *getFirstInterfaceNameChild() const;
    virtual ParametersElement *getFirstParametersChild() const;
    virtual GatesElement *getFirstGatesChild() const;
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
class NEDXML_API ModuleInterfaceElement : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    ModuleInterfaceElement();
    ModuleInterfaceElement(NEDElement *parent);
    virtual ~ModuleInterfaceElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "module-interface";}
    virtual int getTagCode() const {return NED_MODULE_INTERFACE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual ModuleInterfaceElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual ModuleInterfaceElement *getNextModuleInterfaceSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual ExtendsElement *getFirstExtendsChild() const;
    virtual ParametersElement *getFirstParametersChild() const;
    virtual GatesElement *getFirstGatesChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;compound-module&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT compound-module (comment*, extends?, interface-name*,
 *                            parameters?, gates?, types?, submodules?, connections?)>
 * <!ATTLIST compound-module
 *      name               NMTOKEN   #REQUIRED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API CompoundModuleElement : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    CompoundModuleElement();
    CompoundModuleElement(NEDElement *parent);
    virtual ~CompoundModuleElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "compound-module";}
    virtual int getTagCode() const {return NED_COMPOUND_MODULE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual CompoundModuleElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual CompoundModuleElement *getNextCompoundModuleSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual ExtendsElement *getFirstExtendsChild() const;
    virtual InterfaceNameElement *getFirstInterfaceNameChild() const;
    virtual ParametersElement *getFirstParametersChild() const;
    virtual GatesElement *getFirstGatesChild() const;
    virtual TypesElement *getFirstTypesChild() const;
    virtual SubmodulesElement *getFirstSubmodulesChild() const;
    virtual ConnectionsElement *getFirstConnectionsChild() const;
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
class NEDXML_API ChannelInterfaceElement : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    ChannelInterfaceElement();
    ChannelInterfaceElement(NEDElement *parent);
    virtual ~ChannelInterfaceElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "channel-interface";}
    virtual int getTagCode() const {return NED_CHANNEL_INTERFACE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual ChannelInterfaceElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual ChannelInterfaceElement *getNextChannelInterfaceSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual ExtendsElement *getFirstExtendsChild() const;
    virtual ParametersElement *getFirstParametersChild() const;
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
class NEDXML_API ChannelElement : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    ChannelElement();
    ChannelElement(NEDElement *parent);
    virtual ~ChannelElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "channel";}
    virtual int getTagCode() const {return NED_CHANNEL;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual ChannelElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual ChannelElement *getNextChannelSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual ExtendsElement *getFirstExtendsChild() const;
    virtual InterfaceNameElement *getFirstInterfaceNameChild() const;
    virtual ParametersElement *getFirstParametersChild() const;
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
class NEDXML_API ParametersElement : public NEDElement
{
  private:
    bool isImplicit;
  public:
    /** @name Constructors, destructor */
    //@{
    ParametersElement();
    ParametersElement(NEDElement *parent);
    virtual ~ParametersElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "parameters";}
    virtual int getTagCode() const {return NED_PARAMETERS;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual ParametersElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    bool getIsImplicit() const  {return isImplicit;}
    void setIsImplicit(bool val)  {isImplicit = val;}

    virtual ParametersElement *getNextParametersSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual PropertyElement *getFirstPropertyChild() const;
    virtual ParamElement *getFirstParamChild() const;
    virtual PatternElement *getFirstPatternChild() const;
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
class NEDXML_API ParamElement : public NEDElement
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
    ParamElement();
    ParamElement(NEDElement *parent);
    virtual ~ParamElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "param";}
    virtual int getTagCode() const {return NED_PARAM;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual ParamElement *dup() const;
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

    virtual ParamElement *getNextParamSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual ExpressionElement *getFirstExpressionChild() const;
    virtual PropertyElement *getFirstPropertyChild() const;
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
class NEDXML_API PatternElement : public NEDElement
{
  private:
    std::string pattern;
    std::string value;
    bool isDefault;
  public:
    /** @name Constructors, destructor */
    //@{
    PatternElement();
    PatternElement(NEDElement *parent);
    virtual ~PatternElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "pattern";}
    virtual int getTagCode() const {return NED_PATTERN;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual PatternElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getPattern() const  {return pattern.c_str();}
    void setPattern(const char * val)  {pattern = val;}
    const char * getValue() const  {return value.c_str();}
    void setValue(const char * val)  {value = val;}
    bool getIsDefault() const  {return isDefault;}
    void setIsDefault(bool val)  {isDefault = val;}

    virtual PatternElement *getNextPatternSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual ExpressionElement *getFirstExpressionChild() const;
    virtual PropertyElement *getFirstPropertyChild() const;
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
class NEDXML_API PropertyElement : public NEDElement
{
  private:
    bool isImplicit;
    std::string name;
    std::string index;
  public:
    /** @name Constructors, destructor */
    //@{
    PropertyElement();
    PropertyElement(NEDElement *parent);
    virtual ~PropertyElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "property";}
    virtual int getTagCode() const {return NED_PROPERTY;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual PropertyElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    bool getIsImplicit() const  {return isImplicit;}
    void setIsImplicit(bool val)  {isImplicit = val;}
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getIndex() const  {return index.c_str();}
    void setIndex(const char * val)  {index = val;}

    virtual PropertyElement *getNextPropertySibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual PropertyKeyElement *getFirstPropertyKeyChild() const;
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
class NEDXML_API PropertyKeyElement : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    PropertyKeyElement();
    PropertyKeyElement(NEDElement *parent);
    virtual ~PropertyKeyElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "property-key";}
    virtual int getTagCode() const {return NED_PROPERTY_KEY;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual PropertyKeyElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual PropertyKeyElement *getNextPropertyKeySibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual LiteralElement *getFirstLiteralChild() const;
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
class NEDXML_API GatesElement : public NEDElement
{
  private:
  public:
    /** @name Constructors, destructor */
    //@{
    GatesElement();
    GatesElement(NEDElement *parent);
    virtual ~GatesElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "gates";}
    virtual int getTagCode() const {return NED_GATES;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual GatesElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{

    virtual GatesElement *getNextGatesSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual GateElement *getFirstGateChild() const;
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
class NEDXML_API GateElement : public NEDElement
{
  private:
    std::string name;
    int type;
    bool isVector;
    std::string vectorSize;
  public:
    /** @name Constructors, destructor */
    //@{
    GateElement();
    GateElement(NEDElement *parent);
    virtual ~GateElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "gate";}
    virtual int getTagCode() const {return NED_GATE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual GateElement *dup() const;
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

    virtual GateElement *getNextGateSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual ExpressionElement *getFirstExpressionChild() const;
    virtual PropertyElement *getFirstPropertyChild() const;
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
class NEDXML_API TypesElement : public NEDElement
{
  private:
  public:
    /** @name Constructors, destructor */
    //@{
    TypesElement();
    TypesElement(NEDElement *parent);
    virtual ~TypesElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "types";}
    virtual int getTagCode() const {return NED_TYPES;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual TypesElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{

    virtual TypesElement *getNextTypesSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual ChannelElement *getFirstChannelChild() const;
    virtual ChannelInterfaceElement *getFirstChannelInterfaceChild() const;
    virtual SimpleModuleElement *getFirstSimpleModuleChild() const;
    virtual CompoundModuleElement *getFirstCompoundModuleChild() const;
    virtual ModuleInterfaceElement *getFirstModuleInterfaceChild() const;
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
class NEDXML_API SubmodulesElement : public NEDElement
{
  private:
  public:
    /** @name Constructors, destructor */
    //@{
    SubmodulesElement();
    SubmodulesElement(NEDElement *parent);
    virtual ~SubmodulesElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "submodules";}
    virtual int getTagCode() const {return NED_SUBMODULES;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual SubmodulesElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{

    virtual SubmodulesElement *getNextSubmodulesSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual SubmoduleElement *getFirstSubmoduleChild() const;
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
class NEDXML_API SubmoduleElement : public NEDElement
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
    SubmoduleElement();
    SubmoduleElement(NEDElement *parent);
    virtual ~SubmoduleElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "submodule";}
    virtual int getTagCode() const {return NED_SUBMODULE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual SubmoduleElement *dup() const;
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

    virtual SubmoduleElement *getNextSubmoduleSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual ExpressionElement *getFirstExpressionChild() const;
    virtual ParametersElement *getFirstParametersChild() const;
    virtual GatesElement *getFirstGatesChild() const;
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
class NEDXML_API ConnectionsElement : public NEDElement
{
  private:
    bool allowUnconnected;
  public:
    /** @name Constructors, destructor */
    //@{
    ConnectionsElement();
    ConnectionsElement(NEDElement *parent);
    virtual ~ConnectionsElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "connections";}
    virtual int getTagCode() const {return NED_CONNECTIONS;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual ConnectionsElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    bool getAllowUnconnected() const  {return allowUnconnected;}
    void setAllowUnconnected(bool val)  {allowUnconnected = val;}

    virtual ConnectionsElement *getNextConnectionsSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual ConnectionElement *getFirstConnectionChild() const;
    virtual ConnectionGroupElement *getFirstConnectionGroupChild() const;
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
class NEDXML_API ConnectionElement : public NEDElement
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
    ConnectionElement();
    ConnectionElement(NEDElement *parent);
    virtual ~ConnectionElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "connection";}
    virtual int getTagCode() const {return NED_CONNECTION;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual ConnectionElement *dup() const;
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

    virtual ConnectionElement *getNextConnectionSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual ExpressionElement *getFirstExpressionChild() const;
    virtual ChannelSpecElement *getFirstChannelSpecChild() const;
    virtual LoopElement *getFirstLoopChild() const;
    virtual ConditionElement *getFirstConditionChild() const;
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
class NEDXML_API ChannelSpecElement : public NEDElement
{
  private:
    std::string type;
    std::string likeType;
    std::string likeParam;
  public:
    /** @name Constructors, destructor */
    //@{
    ChannelSpecElement();
    ChannelSpecElement(NEDElement *parent);
    virtual ~ChannelSpecElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "channel-spec";}
    virtual int getTagCode() const {return NED_CHANNEL_SPEC;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual ChannelSpecElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getType() const  {return type.c_str();}
    void setType(const char * val)  {type = val;}
    const char * getLikeType() const  {return likeType.c_str();}
    void setLikeType(const char * val)  {likeType = val;}
    const char * getLikeParam() const  {return likeParam.c_str();}
    void setLikeParam(const char * val)  {likeParam = val;}

    virtual ChannelSpecElement *getNextChannelSpecSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual ExpressionElement *getFirstExpressionChild() const;
    virtual ParametersElement *getFirstParametersChild() const;
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
class NEDXML_API ConnectionGroupElement : public NEDElement
{
  private:
  public:
    /** @name Constructors, destructor */
    //@{
    ConnectionGroupElement();
    ConnectionGroupElement(NEDElement *parent);
    virtual ~ConnectionGroupElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "connection-group";}
    virtual int getTagCode() const {return NED_CONNECTION_GROUP;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual ConnectionGroupElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{

    virtual ConnectionGroupElement *getNextConnectionGroupSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual LoopElement *getFirstLoopChild() const;
    virtual ConditionElement *getFirstConditionChild() const;
    virtual ConnectionElement *getFirstConnectionChild() const;
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
class NEDXML_API LoopElement : public NEDElement
{
  private:
    std::string paramName;
    std::string fromValue;
    std::string toValue;
  public:
    /** @name Constructors, destructor */
    //@{
    LoopElement();
    LoopElement(NEDElement *parent);
    virtual ~LoopElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "loop";}
    virtual int getTagCode() const {return NED_LOOP;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual LoopElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getParamName() const  {return paramName.c_str();}
    void setParamName(const char * val)  {paramName = val;}
    const char * getFromValue() const  {return fromValue.c_str();}
    void setFromValue(const char * val)  {fromValue = val;}
    const char * getToValue() const  {return toValue.c_str();}
    void setToValue(const char * val)  {toValue = val;}

    virtual LoopElement *getNextLoopSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual ExpressionElement *getFirstExpressionChild() const;
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
class NEDXML_API ConditionElement : public NEDElement
{
  private:
    std::string condition;
  public:
    /** @name Constructors, destructor */
    //@{
    ConditionElement();
    ConditionElement(NEDElement *parent);
    virtual ~ConditionElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "condition";}
    virtual int getTagCode() const {return NED_CONDITION;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual ConditionElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getCondition() const  {return condition.c_str();}
    void setCondition(const char * val)  {condition = val;}

    virtual ConditionElement *getNextConditionSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual ExpressionElement *getFirstExpressionChild() const;
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
class NEDXML_API ExpressionElement : public NEDElement
{
  private:
    std::string target;
  public:
    /** @name Constructors, destructor */
    //@{
    ExpressionElement();
    ExpressionElement(NEDElement *parent);
    virtual ~ExpressionElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "expression";}
    virtual int getTagCode() const {return NED_EXPRESSION;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual ExpressionElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getTarget() const  {return target.c_str();}
    void setTarget(const char * val)  {target = val;}

    virtual ExpressionElement *getNextExpressionSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual OperatorElement *getFirstOperatorChild() const;
    virtual FunctionElement *getFirstFunctionChild() const;
    virtual IdentElement *getFirstIdentChild() const;
    virtual LiteralElement *getFirstLiteralChild() const;
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
class NEDXML_API OperatorElement : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    OperatorElement();
    OperatorElement(NEDElement *parent);
    virtual ~OperatorElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "operator";}
    virtual int getTagCode() const {return NED_OPERATOR;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual OperatorElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual OperatorElement *getNextOperatorSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual OperatorElement *getFirstOperatorChild() const;
    virtual FunctionElement *getFirstFunctionChild() const;
    virtual IdentElement *getFirstIdentChild() const;
    virtual LiteralElement *getFirstLiteralChild() const;
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
class NEDXML_API FunctionElement : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    FunctionElement();
    FunctionElement(NEDElement *parent);
    virtual ~FunctionElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "function";}
    virtual int getTagCode() const {return NED_FUNCTION;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual FunctionElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual FunctionElement *getNextFunctionSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual OperatorElement *getFirstOperatorChild() const;
    virtual FunctionElement *getFirstFunctionChild() const;
    virtual IdentElement *getFirstIdentChild() const;
    virtual LiteralElement *getFirstLiteralChild() const;
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
class NEDXML_API IdentElement : public NEDElement
{
  private:
    std::string module;
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    IdentElement();
    IdentElement(NEDElement *parent);
    virtual ~IdentElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "ident";}
    virtual int getTagCode() const {return NED_IDENT;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual IdentElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getModule() const  {return module.c_str();}
    void setModule(const char * val)  {module = val;}
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual IdentElement *getNextIdentSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual OperatorElement *getFirstOperatorChild() const;
    virtual FunctionElement *getFirstFunctionChild() const;
    virtual IdentElement *getFirstIdentChild() const;
    virtual LiteralElement *getFirstLiteralChild() const;
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
class NEDXML_API LiteralElement : public NEDElement
{
  private:
    int type;
    std::string unit;
    std::string text;
    std::string value;
  public:
    /** @name Constructors, destructor */
    //@{
    LiteralElement();
    LiteralElement(NEDElement *parent);
    virtual ~LiteralElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "literal";}
    virtual int getTagCode() const {return NED_LITERAL;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual LiteralElement *dup() const;
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

    virtual LiteralElement *getNextLiteralSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
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
class NEDXML_API MsgFileElement : public NEDElement
{
  private:
    std::string filename;
    std::string version;
  public:
    /** @name Constructors, destructor */
    //@{
    MsgFileElement();
    MsgFileElement(NEDElement *parent);
    virtual ~MsgFileElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "msg-file";}
    virtual int getTagCode() const {return NED_MSG_FILE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual MsgFileElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getFilename() const  {return filename.c_str();}
    void setFilename(const char * val)  {filename = val;}
    const char * getVersion() const  {return version.c_str();}
    void setVersion(const char * val)  {version = val;}

    virtual MsgFileElement *getNextMsgFileSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual NamespaceElement *getFirstNamespaceChild() const;
    virtual PropertyDeclElement *getFirstPropertyDeclChild() const;
    virtual PropertyElement *getFirstPropertyChild() const;
    virtual CplusplusElement *getFirstCplusplusChild() const;
    virtual StructDeclElement *getFirstStructDeclChild() const;
    virtual ClassDeclElement *getFirstClassDeclChild() const;
    virtual MessageDeclElement *getFirstMessageDeclChild() const;
    virtual EnumDeclElement *getFirstEnumDeclChild() const;
    virtual EnumElement *getFirstEnumChild() const;
    virtual MessageElement *getFirstMessageChild() const;
    virtual ClassElement *getFirstClassChild() const;
    virtual StructElement *getFirstStructChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the &lt;namespace&gt; XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT namespace (comment*)>
 * <!ATTLIST namespace
 *      name                CDATA     #REQUIRED>
 * </pre>
 * 
 * @ingroup Data
 */
class NEDXML_API NamespaceElement : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    NamespaceElement();
    NamespaceElement(NEDElement *parent);
    virtual ~NamespaceElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "namespace";}
    virtual int getTagCode() const {return NED_NAMESPACE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual NamespaceElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual NamespaceElement *getNextNamespaceSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
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
class NEDXML_API CplusplusElement : public NEDElement
{
  private:
    std::string body;
  public:
    /** @name Constructors, destructor */
    //@{
    CplusplusElement();
    CplusplusElement(NEDElement *parent);
    virtual ~CplusplusElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "cplusplus";}
    virtual int getTagCode() const {return NED_CPLUSPLUS;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual CplusplusElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getBody() const  {return body.c_str();}
    void setBody(const char * val)  {body = val;}

    virtual CplusplusElement *getNextCplusplusSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
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
class NEDXML_API StructDeclElement : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    StructDeclElement();
    StructDeclElement(NEDElement *parent);
    virtual ~StructDeclElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "struct-decl";}
    virtual int getTagCode() const {return NED_STRUCT_DECL;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual StructDeclElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual StructDeclElement *getNextStructDeclSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
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
class NEDXML_API ClassDeclElement : public NEDElement
{
  private:
    std::string name;
    bool isCobject;
  public:
    /** @name Constructors, destructor */
    //@{
    ClassDeclElement();
    ClassDeclElement(NEDElement *parent);
    virtual ~ClassDeclElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "class-decl";}
    virtual int getTagCode() const {return NED_CLASS_DECL;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual ClassDeclElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    bool getIsCobject() const  {return isCobject;}
    void setIsCobject(bool val)  {isCobject = val;}

    virtual ClassDeclElement *getNextClassDeclSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
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
class NEDXML_API MessageDeclElement : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    MessageDeclElement();
    MessageDeclElement(NEDElement *parent);
    virtual ~MessageDeclElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "message-decl";}
    virtual int getTagCode() const {return NED_MESSAGE_DECL;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual MessageDeclElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual MessageDeclElement *getNextMessageDeclSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
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
class NEDXML_API EnumDeclElement : public NEDElement
{
  private:
    std::string name;
  public:
    /** @name Constructors, destructor */
    //@{
    EnumDeclElement();
    EnumDeclElement(NEDElement *parent);
    virtual ~EnumDeclElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "enum-decl";}
    virtual int getTagCode() const {return NED_ENUM_DECL;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual EnumDeclElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}

    virtual EnumDeclElement *getNextEnumDeclSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
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
class NEDXML_API EnumElement : public NEDElement
{
  private:
    std::string name;
    std::string extendsName;
    std::string sourceCode;
  public:
    /** @name Constructors, destructor */
    //@{
    EnumElement();
    EnumElement(NEDElement *parent);
    virtual ~EnumElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "enum";}
    virtual int getTagCode() const {return NED_ENUM;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual EnumElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getExtendsName() const  {return extendsName.c_str();}
    void setExtendsName(const char * val)  {extendsName = val;}
    const char * getSourceCode() const  {return sourceCode.c_str();}
    void setSourceCode(const char * val)  {sourceCode = val;}

    virtual EnumElement *getNextEnumSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual EnumFieldsElement *getFirstEnumFieldsChild() const;
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
class NEDXML_API EnumFieldsElement : public NEDElement
{
  private:
  public:
    /** @name Constructors, destructor */
    //@{
    EnumFieldsElement();
    EnumFieldsElement(NEDElement *parent);
    virtual ~EnumFieldsElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "enum-fields";}
    virtual int getTagCode() const {return NED_ENUM_FIELDS;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual EnumFieldsElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{

    virtual EnumFieldsElement *getNextEnumFieldsSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual EnumFieldElement *getFirstEnumFieldChild() const;
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
class NEDXML_API EnumFieldElement : public NEDElement
{
  private:
    std::string name;
    std::string value;
  public:
    /** @name Constructors, destructor */
    //@{
    EnumFieldElement();
    EnumFieldElement(NEDElement *parent);
    virtual ~EnumFieldElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "enum-field";}
    virtual int getTagCode() const {return NED_ENUM_FIELD;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual EnumFieldElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getValue() const  {return value.c_str();}
    void setValue(const char * val)  {value = val;}

    virtual EnumFieldElement *getNextEnumFieldSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
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
class NEDXML_API MessageElement : public NEDElement
{
  private:
    std::string name;
    std::string extendsName;
    std::string sourceCode;
  public:
    /** @name Constructors, destructor */
    //@{
    MessageElement();
    MessageElement(NEDElement *parent);
    virtual ~MessageElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "message";}
    virtual int getTagCode() const {return NED_MESSAGE;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual MessageElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getExtendsName() const  {return extendsName.c_str();}
    void setExtendsName(const char * val)  {extendsName = val;}
    const char * getSourceCode() const  {return sourceCode.c_str();}
    void setSourceCode(const char * val)  {sourceCode = val;}

    virtual MessageElement *getNextMessageSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual PropertyElement *getFirstPropertyChild() const;
    virtual FieldElement *getFirstFieldChild() const;
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
class NEDXML_API ClassElement : public NEDElement
{
  private:
    std::string name;
    std::string extendsName;
    std::string sourceCode;
  public:
    /** @name Constructors, destructor */
    //@{
    ClassElement();
    ClassElement(NEDElement *parent);
    virtual ~ClassElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "class";}
    virtual int getTagCode() const {return NED_CLASS;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual ClassElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getExtendsName() const  {return extendsName.c_str();}
    void setExtendsName(const char * val)  {extendsName = val;}
    const char * getSourceCode() const  {return sourceCode.c_str();}
    void setSourceCode(const char * val)  {sourceCode = val;}

    virtual ClassElement *getNextClassSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual PropertyElement *getFirstPropertyChild() const;
    virtual FieldElement *getFirstFieldChild() const;
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
class NEDXML_API StructElement : public NEDElement
{
  private:
    std::string name;
    std::string extendsName;
    std::string sourceCode;
  public:
    /** @name Constructors, destructor */
    //@{
    StructElement();
    StructElement(NEDElement *parent);
    virtual ~StructElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "struct";}
    virtual int getTagCode() const {return NED_STRUCT;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual StructElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getName() const  {return name.c_str();}
    void setName(const char * val)  {name = val;}
    const char * getExtendsName() const  {return extendsName.c_str();}
    void setExtendsName(const char * val)  {extendsName = val;}
    const char * getSourceCode() const  {return sourceCode.c_str();}
    void setSourceCode(const char * val)  {sourceCode = val;}

    virtual StructElement *getNextStructSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
    virtual PropertyElement *getFirstPropertyChild() const;
    virtual FieldElement *getFirstFieldChild() const;
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
class NEDXML_API FieldElement : public NEDElement
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
    FieldElement();
    FieldElement(NEDElement *parent);
    virtual ~FieldElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "field";}
    virtual int getTagCode() const {return NED_FIELD;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual FieldElement *dup() const;
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

    virtual FieldElement *getNextFieldSibling() const;
    virtual CommentElement *getFirstCommentChild() const;
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
class NEDXML_API UnknownElement : public NEDElement
{
  private:
    std::string element;
  public:
    /** @name Constructors, destructor */
    //@{
    UnknownElement();
    UnknownElement(NEDElement *parent);
    virtual ~UnknownElement() {}
    //@}

    /** @name Redefined NEDElement methods, incl. generic access to attributes */
    //@{
    virtual const char *getTagName() const {return "unknown";}
    virtual int getTagCode() const {return NED_UNKNOWN;}
    virtual int getNumAttributes() const;
    virtual const char *getAttributeName(int k) const;
    virtual const char *getAttribute(int k) const;
    virtual const char *getAttribute(const char *name) const {return NEDElement::getAttribute(name);} // needed because of a C++ language quirk
    virtual void setAttribute(int k, const char *val);
    virtual void setAttribute(const char *name, const char *val) {NEDElement::setAttribute(name, val);} // ditto
    virtual const char *getAttributeDefault(int k) const;
    virtual UnknownElement *dup() const;
    //@}

    /** @name Typed access to attributes, children and siblings */
    //@{
    const char * getElement() const  {return element.c_str();}
    void setElement(const char * val)  {element = val;}

    virtual UnknownElement *getNextUnknownSibling() const;
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
    virtual NEDElement *createElementWithTag(const char *tagname);
    /** Creates NEDElement subclass which corresponds to tagcode */
    virtual NEDElement *createElementWithTag(int tagcode);
};

NAMESPACE_END

#endif


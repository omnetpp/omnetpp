//==========================================================================
// Part of the OMNeT++ Discrete System Simulation System
//
// GENERATED FILE -- DO NOT EDIT!
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

// *** THIS IS A GENERATED FILE, HAND-EDITING IT IS USELESS! ***

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
    NED_UNKNOWN,
};

enum {NED_GATEDIR_INPUT, NED_GATEDIR_OUTPUT};
enum {NED_ARROWDIR_LEFT, NED_ARROWDIR_RIGHT};
enum {NED_CONST_BOOL, NED_CONST_INT, NED_CONST_REAL, NED_CONST_STRING, NED_CONST_TIME};

/**
 * GENERATED CLASS. Represents the <ned-files> XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT ned-files ((ned-files|ned-file)*)>
 * <!ATTLIST ned-files >
 * </pre>
 * 
 * @ingroup Data
 */
class NedFilesNode : public NEDElement
{
  private:
  public:
    NedFilesNode() {applyDefaults();}
    NedFilesNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~NedFilesNode() {}
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
 * GENERATED CLASS. Represents the <ned-file> XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT ned-file ((import|channel|simple-module|compound-module|network)*)>
 * <!ATTLIST ned-file
 *      filename            CDATA     #IMPLIED
 *      preferred-indent    CDATA     "4"
 *      banner-comment      CDATA     #IMPLIED >
 * </pre>
 * 
 * @ingroup Data
 */
class NedFileNode : public NEDElement
{
  private:
    NEDString filename;
    NEDString preferredIndent;
    NEDString bannerComment;
  public:
    NedFileNode() {applyDefaults();}
    NedFileNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~NedFileNode() {}
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
    const char * getFilename() const  {return filename;}
    void setFilename(const char * val)  {filename = val;}
    const char * getPreferredIndent() const  {return preferredIndent;}
    void setPreferredIndent(const char * val)  {preferredIndent = val;}
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}

    virtual NedFileNode *getNextNedFileNodeSibling() const;
    virtual ImportNode *getFirstImportChild() const;
    virtual ChannelNode *getFirstChannelChild() const;
    virtual SimpleModuleNode *getFirstSimpleModuleChild() const;
    virtual CompoundModuleNode *getFirstCompoundModuleChild() const;
    virtual NetworkNode *getFirstNetworkChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <import> XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT import (imported-file*)>
 * <!ATTLIST import
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;"
 *      trailing-comment    CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class ImportNode : public NEDElement
{
  private:
    NEDString bannerComment;
    NEDString rightComment;
    NEDString trailingComment;
  public:
    ImportNode() {applyDefaults();}
    ImportNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ImportNode() {}
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
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}
    const char * getTrailingComment() const  {return trailingComment;}
    void setTrailingComment(const char * val)  {trailingComment = val;}

    virtual ImportNode *getNextImportNodeSibling() const;
    virtual ImportedFileNode *getFirstImportedFileChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <imported-file> XML element in memory. DTD declaration:
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
    NEDString filename;
    NEDString bannerComment;
    NEDString rightComment;
  public:
    ImportedFileNode() {applyDefaults();}
    ImportedFileNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ImportedFileNode() {}
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
    const char * getFilename() const  {return filename;}
    void setFilename(const char * val)  {filename = val;}
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual ImportedFileNode *getNextImportedFileNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <channel> XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT channel (channel-attr*, display-string?)>
 * <!ATTLIST channel
 *      name                NMTOKEN   #REQUIRED
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
    NEDString name;
    NEDString bannerComment;
    NEDString rightComment;
    NEDString trailingComment;
  public:
    ChannelNode() {applyDefaults();}
    ChannelNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ChannelNode() {}
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
    const char * getName() const  {return name;}
    void setName(const char * val)  {name = val;}
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}
    const char * getTrailingComment() const  {return trailingComment;}
    void setTrailingComment(const char * val)  {trailingComment = val;}

    virtual ChannelNode *getNextChannelNodeSibling() const;
    virtual ChannelAttrNode *getFirstChannelAttrChild() const;
    virtual DisplayStringNode *getFirstDisplayStringChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <channel-attr> XML element in memory. DTD declaration:
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
    NEDString name;
    NEDString value;
    NEDString bannerComment;
    NEDString rightComment;
  public:
    ChannelAttrNode() {applyDefaults();}
    ChannelAttrNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ChannelAttrNode() {}
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
    const char * getName() const  {return name;}
    void setName(const char * val)  {name = val;}
    const char * getValue() const  {return value;}
    void setValue(const char * val)  {value = val;}
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual ChannelAttrNode *getNextChannelAttrNodeSibling() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <network> XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT network (substmachines?,substparams?)>
 * <!ATTLIST network
 *      name                NMTOKEN   #REQUIRED
 *      type-name           NMTOKEN   #REQUIRED
 *      like-name           NMTOKEN   #IMPLIED
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
    NEDString name;
    NEDString typeName;
    NEDString likeName;
    NEDString bannerComment;
    NEDString rightComment;
    NEDString trailingComment;
  public:
    NetworkNode() {applyDefaults();}
    NetworkNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~NetworkNode() {}
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
    const char * getName() const  {return name;}
    void setName(const char * val)  {name = val;}
    const char * getTypeName() const  {return typeName;}
    void setTypeName(const char * val)  {typeName = val;}
    const char * getLikeName() const  {return likeName;}
    void setLikeName(const char * val)  {likeName = val;}
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}
    const char * getTrailingComment() const  {return trailingComment;}
    void setTrailingComment(const char * val)  {trailingComment = val;}

    virtual NetworkNode *getNextNetworkNodeSibling() const;
    virtual SubstmachinesNode *getFirstSubstmachinesChild() const;
    virtual SubstparamsNode *getFirstSubstparamsChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <simple-module> XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT simple-module (machines?, params?, gates?, display-string?)>
 * <!ATTLIST simple-module
 *      name                NMTOKEN   #REQUIRED
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
    NEDString name;
    NEDString bannerComment;
    NEDString rightComment;
    NEDString trailingComment;
  public:
    SimpleModuleNode() {applyDefaults();}
    SimpleModuleNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~SimpleModuleNode() {}
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
    const char * getName() const  {return name;}
    void setName(const char * val)  {name = val;}
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}
    const char * getTrailingComment() const  {return trailingComment;}
    void setTrailingComment(const char * val)  {trailingComment = val;}

    virtual SimpleModuleNode *getNextSimpleModuleNodeSibling() const;
    virtual MachinesNode *getFirstMachinesChild() const;
    virtual ParamsNode *getFirstParamsChild() const;
    virtual GatesNode *getFirstGatesChild() const;
    virtual DisplayStringNode *getFirstDisplayStringChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <compound-module> XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT compound-module (machines?, params?, gates?,
 *                   submodules?, connections?, display-string?)>
 * <!ATTLIST compound-module
 *      name                NMTOKEN   #REQUIRED
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
    NEDString name;
    NEDString bannerComment;
    NEDString rightComment;
    NEDString trailingComment;
  public:
    CompoundModuleNode() {applyDefaults();}
    CompoundModuleNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~CompoundModuleNode() {}
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
    const char * getName() const  {return name;}
    void setName(const char * val)  {name = val;}
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}
    const char * getTrailingComment() const  {return trailingComment;}
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
 * GENERATED CLASS. Represents the <params> XML element in memory. DTD declaration:
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
    NEDString bannerComment;
    NEDString rightComment;
  public:
    ParamsNode() {applyDefaults();}
    ParamsNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ParamsNode() {}
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
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual ParamsNode *getNextParamsNodeSibling() const;
    virtual ParamNode *getFirstParamChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <param> XML element in memory. DTD declaration:
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
    NEDString name;
    NEDString dataType;
    NEDString bannerComment;
    NEDString rightComment;
  public:
    ParamNode() {applyDefaults();}
    ParamNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ParamNode() {}
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
    const char * getName() const  {return name;}
    void setName(const char * val)  {name = val;}
    const char * getDataType() const  {return dataType;}
    void setDataType(const char * val)  {dataType = val;}
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual ParamNode *getNextParamNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <gates> XML element in memory. DTD declaration:
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
    NEDString bannerComment;
    NEDString rightComment;
  public:
    GatesNode() {applyDefaults();}
    GatesNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~GatesNode() {}
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
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual GatesNode *getNextGatesNodeSibling() const;
    virtual GateNode *getFirstGateChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <gate> XML element in memory. DTD declaration:
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
    NEDString name;
    int direction;
    bool isVector;
    NEDString bannerComment;
    NEDString rightComment;
  public:
    GateNode() {applyDefaults();}
    GateNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~GateNode() {}
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
    const char * getName() const  {return name;}
    void setName(const char * val)  {name = val;}
    int getDirection() const  {return direction;}
    void setDirection(int val)  {direction = val;}
    bool getIsVector() const  {return isVector;}
    void setIsVector(bool val)  {isVector = val;}
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual GateNode *getNextGateNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <machines> XML element in memory. DTD declaration:
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
    NEDString bannerComment;
    NEDString rightComment;
  public:
    MachinesNode() {applyDefaults();}
    MachinesNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~MachinesNode() {}
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
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual MachinesNode *getNextMachinesNodeSibling() const;
    virtual MachineNode *getFirstMachineChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <machine> XML element in memory. DTD declaration:
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
    NEDString name;
    NEDString bannerComment;
    NEDString rightComment;
  public:
    MachineNode() {applyDefaults();}
    MachineNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~MachineNode() {}
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
    const char * getName() const  {return name;}
    void setName(const char * val)  {name = val;}
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual MachineNode *getNextMachineNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <submodules> XML element in memory. DTD declaration:
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
    NEDString bannerComment;
    NEDString rightComment;
  public:
    SubmodulesNode() {applyDefaults();}
    SubmodulesNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~SubmodulesNode() {}
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
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual SubmodulesNode *getNextSubmodulesNodeSibling() const;
    virtual SubmoduleNode *getFirstSubmoduleChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <submodule> XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT submodule (expression?, substmachines*, substparams*,
 *                      gatesizes*, display-string?)>
 * <!ATTLIST submodule
 *      name                NMTOKEN   #REQUIRED
 *      type-name           NMTOKEN   #REQUIRED
 *      like-name           NMTOKEN   #IMPLIED
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
    NEDString name;
    NEDString typeName;
    NEDString likeName;
    NEDString vectorSize;
    NEDString bannerComment;
    NEDString rightComment;
  public:
    SubmoduleNode() {applyDefaults();}
    SubmoduleNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~SubmoduleNode() {}
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
    const char * getName() const  {return name;}
    void setName(const char * val)  {name = val;}
    const char * getTypeName() const  {return typeName;}
    void setTypeName(const char * val)  {typeName = val;}
    const char * getLikeName() const  {return likeName;}
    void setLikeName(const char * val)  {likeName = val;}
    const char * getVectorSize() const  {return vectorSize;}
    void setVectorSize(const char * val)  {vectorSize = val;}
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
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
 * GENERATED CLASS. Represents the <substparams> XML element in memory. DTD declaration:
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
    NEDString condition;
    NEDString bannerComment;
    NEDString rightComment;
  public:
    SubstparamsNode() {applyDefaults();}
    SubstparamsNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~SubstparamsNode() {}
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
    const char * getCondition() const  {return condition;}
    void setCondition(const char * val)  {condition = val;}
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual SubstparamsNode *getNextSubstparamsNodeSibling() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    virtual SubstparamNode *getFirstSubstparamChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <substparam> XML element in memory. DTD declaration:
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
    NEDString name;
    NEDString value;
    NEDString bannerComment;
    NEDString rightComment;
  public:
    SubstparamNode() {applyDefaults();}
    SubstparamNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~SubstparamNode() {}
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
    const char * getName() const  {return name;}
    void setName(const char * val)  {name = val;}
    const char * getValue() const  {return value;}
    void setValue(const char * val)  {value = val;}
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual SubstparamNode *getNextSubstparamNodeSibling() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <gatesizes> XML element in memory. DTD declaration:
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
    NEDString condition;
    NEDString bannerComment;
    NEDString rightComment;
  public:
    GatesizesNode() {applyDefaults();}
    GatesizesNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~GatesizesNode() {}
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
    const char * getCondition() const  {return condition;}
    void setCondition(const char * val)  {condition = val;}
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual GatesizesNode *getNextGatesizesNodeSibling() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    virtual GatesizeNode *getFirstGatesizeChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <gatesize> XML element in memory. DTD declaration:
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
    NEDString name;
    NEDString vectorSize;
    NEDString bannerComment;
    NEDString rightComment;
  public:
    GatesizeNode() {applyDefaults();}
    GatesizeNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~GatesizeNode() {}
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
    const char * getName() const  {return name;}
    void setName(const char * val)  {name = val;}
    const char * getVectorSize() const  {return vectorSize;}
    void setVectorSize(const char * val)  {vectorSize = val;}
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual GatesizeNode *getNextGatesizeNodeSibling() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <substmachines> XML element in memory. DTD declaration:
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
    NEDString condition;
    NEDString bannerComment;
    NEDString rightComment;
  public:
    SubstmachinesNode() {applyDefaults();}
    SubstmachinesNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~SubstmachinesNode() {}
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
    const char * getCondition() const  {return condition;}
    void setCondition(const char * val)  {condition = val;}
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual SubstmachinesNode *getNextSubstmachinesNodeSibling() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    virtual SubstmachineNode *getFirstSubstmachineChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <substmachine> XML element in memory. DTD declaration:
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
    NEDString name;
    NEDString bannerComment;
    NEDString rightComment;
  public:
    SubstmachineNode() {applyDefaults();}
    SubstmachineNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~SubstmachineNode() {}
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
    const char * getName() const  {return name;}
    void setName(const char * val)  {name = val;}
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual SubstmachineNode *getNextSubstmachineNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <connections> XML element in memory. DTD declaration:
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
    NEDString bannerComment;
    NEDString rightComment;
  public:
    ConnectionsNode() {applyDefaults();}
    ConnectionsNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ConnectionsNode() {}
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
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual ConnectionsNode *getNextConnectionsNodeSibling() const;
    virtual ConnectionNode *getFirstConnectionChild() const;
    virtual ForLoopNode *getFirstForLoopChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <connection> XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT connection ((expression|conn-attr|display-string)*)>
 * <!ATTLIST connection
 *      condition           CDATA     #IMPLIED
 *      src-module          NMTOKEN   #IMPLIED
 *      src-module-index    CDATA     #IMPLIED
 *      src-gate            NMTOKEN   #REQUIRED
 *      src-gate-index      CDATA     #IMPLIED
 *      dest-module         NMTOKEN   #IMPLIED
 *      dest-module-index   CDATA     #IMPLIED
 *      dest-gate           NMTOKEN   #REQUIRED
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
    NEDString condition;
    NEDString srcModule;
    NEDString srcModuleIndex;
    NEDString srcGate;
    NEDString srcGateIndex;
    NEDString destModule;
    NEDString destModuleIndex;
    NEDString destGate;
    NEDString destGateIndex;
    int arrowDirection;
    NEDString bannerComment;
    NEDString rightComment;
  public:
    ConnectionNode() {applyDefaults();}
    ConnectionNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ConnectionNode() {}
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
    const char * getCondition() const  {return condition;}
    void setCondition(const char * val)  {condition = val;}
    const char * getSrcModule() const  {return srcModule;}
    void setSrcModule(const char * val)  {srcModule = val;}
    const char * getSrcModuleIndex() const  {return srcModuleIndex;}
    void setSrcModuleIndex(const char * val)  {srcModuleIndex = val;}
    const char * getSrcGate() const  {return srcGate;}
    void setSrcGate(const char * val)  {srcGate = val;}
    const char * getSrcGateIndex() const  {return srcGateIndex;}
    void setSrcGateIndex(const char * val)  {srcGateIndex = val;}
    const char * getDestModule() const  {return destModule;}
    void setDestModule(const char * val)  {destModule = val;}
    const char * getDestModuleIndex() const  {return destModuleIndex;}
    void setDestModuleIndex(const char * val)  {destModuleIndex = val;}
    const char * getDestGate() const  {return destGate;}
    void setDestGate(const char * val)  {destGate = val;}
    const char * getDestGateIndex() const  {return destGateIndex;}
    void setDestGateIndex(const char * val)  {destGateIndex = val;}
    int getArrowDirection() const  {return arrowDirection;}
    void setArrowDirection(int val)  {arrowDirection = val;}
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual ConnectionNode *getNextConnectionNodeSibling() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    virtual ConnAttrNode *getFirstConnAttrChild() const;
    virtual DisplayStringNode *getFirstDisplayStringChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <conn-attr> XML element in memory. DTD declaration:
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
    NEDString name;
    NEDString value;
    NEDString rightComment;
  public:
    ConnAttrNode() {applyDefaults();}
    ConnAttrNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ConnAttrNode() {}
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
    const char * getName() const  {return name;}
    void setName(const char * val)  {name = val;}
    const char * getValue() const  {return value;}
    void setValue(const char * val)  {value = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual ConnAttrNode *getNextConnAttrNodeSibling() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <for-loop> XML element in memory. DTD declaration:
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
    NEDString bannerComment;
    NEDString rightComment;
    NEDString trailingComment;
  public:
    ForLoopNode() {applyDefaults();}
    ForLoopNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ForLoopNode() {}
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
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}
    const char * getTrailingComment() const  {return trailingComment;}
    void setTrailingComment(const char * val)  {trailingComment = val;}

    virtual ForLoopNode *getNextForLoopNodeSibling() const;
    virtual LoopVarNode *getFirstLoopVarChild() const;
    virtual ConnectionNode *getFirstConnectionChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <loop-var> XML element in memory. DTD declaration:
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
    NEDString paramName;
    NEDString fromValue;
    NEDString toValue;
    NEDString bannerComment;
    NEDString rightComment;
  public:
    LoopVarNode() {applyDefaults();}
    LoopVarNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~LoopVarNode() {}
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
    const char * getParamName() const  {return paramName;}
    void setParamName(const char * val)  {paramName = val;}
    const char * getFromValue() const  {return fromValue;}
    void setFromValue(const char * val)  {fromValue = val;}
    const char * getToValue() const  {return toValue;}
    void setToValue(const char * val)  {toValue = val;}
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual LoopVarNode *getNextLoopVarNodeSibling() const;
    virtual ExpressionNode *getFirstExpressionChild() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <display-string> XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT display-string EMPTY>
 * <!ATTLIST display-string
 *      value               CDATA     #REQUIRED
 *      banner-comment      CDATA     #IMPLIED
 *      right-comment       CDATA     "&#10;" >
 * </pre>
 * 
 * @ingroup Data
 */
class DisplayStringNode : public NEDElement
{
  private:
    NEDString value;
    NEDString bannerComment;
    NEDString rightComment;
  public:
    DisplayStringNode() {applyDefaults();}
    DisplayStringNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~DisplayStringNode() {}
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
    const char * getValue() const  {return value;}
    void setValue(const char * val)  {value = val;}
    const char * getBannerComment() const  {return bannerComment;}
    void setBannerComment(const char * val)  {bannerComment = val;}
    const char * getRightComment() const  {return rightComment;}
    void setRightComment(const char * val)  {rightComment = val;}

    virtual DisplayStringNode *getNextDisplayStringNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <expression> XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT expression ((operator|function|param-ref|ident|const))>
 * <!ATTLIST expression
 *      target              NMTOKEN   #IMPLIED >
 * </pre>
 * 
 * @ingroup Data
 */
class ExpressionNode : public NEDElement
{
  private:
    NEDString target;
  public:
    ExpressionNode() {applyDefaults();}
    ExpressionNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ExpressionNode() {}
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
    const char * getTarget() const  {return target;}
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
 * GENERATED CLASS. Represents the <operator> XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT operator ((operator|function|param-ref|ident|const)+)>
 * <!ATTLIST operator
 *      name                NMTOKEN   #REQUIRED >
 * </pre>
 * 
 * @ingroup Data
 */
class OperatorNode : public NEDElement
{
  private:
    NEDString name;
  public:
    OperatorNode() {applyDefaults();}
    OperatorNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~OperatorNode() {}
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
    const char * getName() const  {return name;}
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
 * GENERATED CLASS. Represents the <function> XML element in memory. DTD declaration:
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
    NEDString name;
  public:
    FunctionNode() {applyDefaults();}
    FunctionNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~FunctionNode() {}
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
    const char * getName() const  {return name;}
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
 * GENERATED CLASS. Represents the <param-ref> XML element in memory. DTD declaration:
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
    NEDString module;
    NEDString moduleIndex;
    NEDString paramName;
    NEDString paramIndex;
    bool isRef;
    bool isAncestor;
  public:
    ParamRefNode() {applyDefaults();}
    ParamRefNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ParamRefNode() {}
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
    const char * getModule() const  {return module;}
    void setModule(const char * val)  {module = val;}
    const char * getModuleIndex() const  {return moduleIndex;}
    void setModuleIndex(const char * val)  {moduleIndex = val;}
    const char * getParamName() const  {return paramName;}
    void setParamName(const char * val)  {paramName = val;}
    const char * getParamIndex() const  {return paramIndex;}
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
 * GENERATED CLASS. Represents the <ident> XML element in memory. DTD declaration:
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
    NEDString name;
  public:
    IdentNode() {applyDefaults();}
    IdentNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~IdentNode() {}
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
    const char * getName() const  {return name;}
    void setName(const char * val)  {name = val;}

    virtual IdentNode *getNextIdentNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <const> XML element in memory. DTD declaration:
 * 
 * <pre>
 * <!ELEMENT const EMPTY>
 * <!ATTLIST const
 *      type  (bool|int|real|string|time)  #REQUIRED
 *      text                CDATA     #IMPLIED
 *      value               CDATA     #REQUIRED >
 * </pre>
 * 
 * @ingroup Data
 */
class ConstNode : public NEDElement
{
  private:
    int type;
    NEDString text;
    NEDString value;
  public:
    ConstNode() {applyDefaults();}
    ConstNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~ConstNode() {}
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
    const char * getText() const  {return text;}
    void setText(const char * val)  {text = val;}
    const char * getValue() const  {return value;}
    void setValue(const char * val)  {value = val;}

    virtual ConstNode *getNextConstNodeSibling() const;
    //@}
};

/**
 * GENERATED CLASS. Represents the <unknown> XML element in memory. DTD declaration:
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
    NEDString element;
  public:
    UnknownNode() {applyDefaults();}
    UnknownNode(NEDElement *parent) : NEDElement(parent) {applyDefaults();}
    virtual ~UnknownNode() {}
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
    const char * getElement() const  {return element;}
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
  protected:
    static NEDElementFactory *f;
  public:
    static NEDElementFactory *getInstance();
    virtual NEDElement *createNodeWithTag(const char *tagname);
    virtual NEDElement *createNodeWithTag(int tagcode);
};

#endif


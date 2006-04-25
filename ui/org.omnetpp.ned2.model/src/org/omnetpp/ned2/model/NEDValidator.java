package org.omnetpp.ned2.model;

import org.omnetpp.ned2.model.pojo.AbstractNEDValidator;
import org.omnetpp.ned2.model.pojo.ChannelInterfaceNode;
import org.omnetpp.ned2.model.pojo.ChannelNode;
import org.omnetpp.ned2.model.pojo.ChannelSpecNode;
import org.omnetpp.ned2.model.pojo.ClassDeclNode;
import org.omnetpp.ned2.model.pojo.ClassNode;
import org.omnetpp.ned2.model.pojo.CompoundModuleNode;
import org.omnetpp.ned2.model.pojo.ConditionNode;
import org.omnetpp.ned2.model.pojo.ConnectionGroupNode;
import org.omnetpp.ned2.model.pojo.ConnectionNode;
import org.omnetpp.ned2.model.pojo.ConnectionsNode;
import org.omnetpp.ned2.model.pojo.CplusplusNode;
import org.omnetpp.ned2.model.pojo.EnumDeclNode;
import org.omnetpp.ned2.model.pojo.EnumFieldNode;
import org.omnetpp.ned2.model.pojo.EnumFieldsNode;
import org.omnetpp.ned2.model.pojo.EnumNode;
import org.omnetpp.ned2.model.pojo.ExpressionNode;
import org.omnetpp.ned2.model.pojo.ExtendsNode;
import org.omnetpp.ned2.model.pojo.FieldNode;
import org.omnetpp.ned2.model.pojo.FieldsNode;
import org.omnetpp.ned2.model.pojo.FilesNode;
import org.omnetpp.ned2.model.pojo.FunctionNode;
import org.omnetpp.ned2.model.pojo.GateGroupNode;
import org.omnetpp.ned2.model.pojo.GateNode;
import org.omnetpp.ned2.model.pojo.GatesNode;
import org.omnetpp.ned2.model.pojo.IdentNode;
import org.omnetpp.ned2.model.pojo.ImportNode;
import org.omnetpp.ned2.model.pojo.InterfaceNameNode;
import org.omnetpp.ned2.model.pojo.LiteralNode;
import org.omnetpp.ned2.model.pojo.LoopNode;
import org.omnetpp.ned2.model.pojo.MessageDeclNode;
import org.omnetpp.ned2.model.pojo.MessageNode;
import org.omnetpp.ned2.model.pojo.ModuleInterfaceNode;
import org.omnetpp.ned2.model.pojo.MsgFileNode;
import org.omnetpp.ned2.model.pojo.MsgpropertyNode;
import org.omnetpp.ned2.model.pojo.NedFileNode;
import org.omnetpp.ned2.model.pojo.OperatorNode;
import org.omnetpp.ned2.model.pojo.ParamGroupNode;
import org.omnetpp.ned2.model.pojo.ParamNode;
import org.omnetpp.ned2.model.pojo.ParametersNode;
import org.omnetpp.ned2.model.pojo.PatternNode;
import org.omnetpp.ned2.model.pojo.PropertiesNode;
import org.omnetpp.ned2.model.pojo.PropertyDeclNode;
import org.omnetpp.ned2.model.pojo.PropertyKeyNode;
import org.omnetpp.ned2.model.pojo.PropertyNode;
import org.omnetpp.ned2.model.pojo.SimpleModuleNode;
import org.omnetpp.ned2.model.pojo.StructDeclNode;
import org.omnetpp.ned2.model.pojo.StructNode;
import org.omnetpp.ned2.model.pojo.SubmoduleNode;
import org.omnetpp.ned2.model.pojo.SubmodulesNode;
import org.omnetpp.ned2.model.pojo.TypesNode;
import org.omnetpp.ned2.model.pojo.UnknownNode;
import org.omnetpp.ned2.model.pojo.WhereNode;
import org.omnetpp.ned2.model.pojo.WhitespaceNode;

/**
 * Validates consistency of NED files.
 * 
 * @author andras
 */
public class NEDValidator extends AbstractNEDValidator {

    protected void validateElement(FilesNode node) {}

    protected void validateElement(NedFileNode node) {}

    protected void validateElement(WhitespaceNode node) {}

    protected void validateElement(ImportNode node) {}

    protected void validateElement(PropertyDeclNode node) {}

    protected void validateElement(ExtendsNode node) {}

    protected void validateElement(InterfaceNameNode node) {}

    protected void validateElement(SimpleModuleNode node) {}

    protected void validateElement(ModuleInterfaceNode node) {}

    protected void validateElement(CompoundModuleNode node) {}

    protected void validateElement(ChannelInterfaceNode node) {}

    protected void validateElement(ChannelNode node) {}

    protected void validateElement(ParametersNode node) {}

    protected void validateElement(ParamGroupNode node) {}

    protected void validateElement(ParamNode node) {}

    protected void validateElement(PatternNode node) {}

    protected void validateElement(PropertyNode node) {}

    protected void validateElement(PropertyKeyNode node) {}

    protected void validateElement(GatesNode node) {}

    protected void validateElement(GateGroupNode node) {}

    protected void validateElement(GateNode node) {}

    protected void validateElement(TypesNode node) {}

    protected void validateElement(SubmodulesNode node) {}

    protected void validateElement(SubmoduleNode node) {}

    protected void validateElement(ConnectionsNode node) {}

    protected void validateElement(ConnectionNode node) {}

    protected void validateElement(ChannelSpecNode node) {}

    protected void validateElement(ConnectionGroupNode node) {}

    protected void validateElement(WhereNode node) {}

    protected void validateElement(LoopNode node) {}

    protected void validateElement(ConditionNode node) {}

    protected void validateElement(ExpressionNode node) {}

    protected void validateElement(OperatorNode node) {}

    protected void validateElement(FunctionNode node) {}

    protected void validateElement(IdentNode node) {}

    protected void validateElement(LiteralNode node) {}

    protected void validateElement(MsgFileNode node) {}

    protected void validateElement(CplusplusNode node) {}

    protected void validateElement(StructDeclNode node) {}

    protected void validateElement(ClassDeclNode node) {}

    protected void validateElement(MessageDeclNode node) {}

    protected void validateElement(EnumDeclNode node) {}

    protected void validateElement(EnumNode node) {}

    protected void validateElement(EnumFieldsNode node) {}

    protected void validateElement(EnumFieldNode node) {}

    protected void validateElement(MessageNode node) {}

    protected void validateElement(ClassNode node) {}

    protected void validateElement(StructNode node) {}

    protected void validateElement(FieldsNode node) {}

    protected void validateElement(FieldNode node) {}

    protected void validateElement(PropertiesNode node) {}

    protected void validateElement(MsgpropertyNode node) {}

    protected void validateElement(UnknownNode node) {}
}

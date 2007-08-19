package org.omnetpp.ned.core;

import java.util.HashMap;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.NEDElementConstants;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.interfaces.INedTypeNode;
import org.omnetpp.ned.model.pojo.AbstractNEDValidator;
import org.omnetpp.ned.model.pojo.ChannelInterfaceNode;
import org.omnetpp.ned.model.pojo.ChannelNode;
import org.omnetpp.ned.model.pojo.ChannelSpecNode;
import org.omnetpp.ned.model.pojo.ClassDeclNode;
import org.omnetpp.ned.model.pojo.ClassNode;
import org.omnetpp.ned.model.pojo.CommentNode;
import org.omnetpp.ned.model.pojo.CompoundModuleNode;
import org.omnetpp.ned.model.pojo.ConditionNode;
import org.omnetpp.ned.model.pojo.ConnectionGroupNode;
import org.omnetpp.ned.model.pojo.ConnectionNode;
import org.omnetpp.ned.model.pojo.ConnectionsNode;
import org.omnetpp.ned.model.pojo.CplusplusNode;
import org.omnetpp.ned.model.pojo.EnumDeclNode;
import org.omnetpp.ned.model.pojo.EnumFieldNode;
import org.omnetpp.ned.model.pojo.EnumFieldsNode;
import org.omnetpp.ned.model.pojo.EnumNode;
import org.omnetpp.ned.model.pojo.ExpressionNode;
import org.omnetpp.ned.model.pojo.ExtendsNode;
import org.omnetpp.ned.model.pojo.FieldNode;
import org.omnetpp.ned.model.pojo.FieldsNode;
import org.omnetpp.ned.model.pojo.FilesNode;
import org.omnetpp.ned.model.pojo.FunctionNode;
import org.omnetpp.ned.model.pojo.GateNode;
import org.omnetpp.ned.model.pojo.GatesNode;
import org.omnetpp.ned.model.pojo.IdentNode;
import org.omnetpp.ned.model.pojo.ImportNode;
import org.omnetpp.ned.model.pojo.InterfaceNameNode;
import org.omnetpp.ned.model.pojo.LiteralNode;
import org.omnetpp.ned.model.pojo.LoopNode;
import org.omnetpp.ned.model.pojo.MessageDeclNode;
import org.omnetpp.ned.model.pojo.MessageNode;
import org.omnetpp.ned.model.pojo.ModuleInterfaceNode;
import org.omnetpp.ned.model.pojo.MsgFileNode;
import org.omnetpp.ned.model.pojo.MsgpropertyNode;
import org.omnetpp.ned.model.pojo.NedFileNode;
import org.omnetpp.ned.model.pojo.OperatorNode;
import org.omnetpp.ned.model.pojo.ParamNode;
import org.omnetpp.ned.model.pojo.ParametersNode;
import org.omnetpp.ned.model.pojo.PatternNode;
import org.omnetpp.ned.model.pojo.PropertiesNode;
import org.omnetpp.ned.model.pojo.PropertyDeclNode;
import org.omnetpp.ned.model.pojo.PropertyKeyNode;
import org.omnetpp.ned.model.pojo.PropertyNode;
import org.omnetpp.ned.model.pojo.SimpleModuleNode;
import org.omnetpp.ned.model.pojo.StructDeclNode;
import org.omnetpp.ned.model.pojo.StructNode;
import org.omnetpp.ned.model.pojo.SubmoduleNode;
import org.omnetpp.ned.model.pojo.SubmodulesNode;
import org.omnetpp.ned.model.pojo.TypesNode;
import org.omnetpp.ned.model.pojo.UnknownNode;

/**
 * Validates consistency of NED files.
 *
 * @author andras
 */
//
// FIXME validation of embedded types!!!!
//
public class NEDValidator extends AbstractNEDValidator implements NEDElementConstants {

	INEDTypeResolver resolver;

	INEDErrorStore errors;

	// the component currently being validated
	INEDElement componentNode;

	// non-null while we're validating a submodule
	SubmoduleNode submoduleNode;
	INEDTypeInfo submoduleType; // null for the "like *" case(!); valid while submoduleNode!=null

	// non-null while we're validating a channelspec of a connection
	ChannelSpecNode channelSpecNode;
	INEDTypeInfo channelSpecType; // may be null; valid while channelSpecNode!=null

	// members of the component currently being validated
	HashMap<String, INEDElement> members = new HashMap<String, INEDElement>();

	// contents of the "types:" section of the component currently being validated
	HashMap<String, INEDTypeInfo> innerTypes = new HashMap<String, INEDTypeInfo>();

	public NEDValidator(INEDTypeResolver resolver, INEDErrorStore errors) {
		this.resolver = resolver;
		this.errors = errors;
	}

	@Override
	public void validate(INEDElement node) {
		validateElement(node);
	}

	protected void validateChildren(INEDElement node) {
		for (INEDElement child : node)
			validate(child);
	}

	@Override
    protected void validateElement(FilesNode node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(NedFileNode node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(CommentNode node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(ImportNode node) {
		//TODO check if file exists?
		validateChildren(node);
	}

	@Override
    protected void validateElement(PropertyDeclNode node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(ExtendsNode node) {
		Assert.isTrue(componentNode!=null);

		// referenced component must exist and must be the same type as this one
		String name = node.getName();
		INEDTypeInfo e = resolver.getComponent(name);
		if (e == null) {
			errors.add(node, "no such component: '" + name+"'");
			return;
		}
		int thisType = componentNode.getTagCode();
		int extendsType = e.getNEDElement().getTagCode();
		if (thisType != extendsType) {
			errors.add(node, "'"+name+"' is not a "+componentNode.getTagName());
			return;
		}
		//XXX enforce channel inheritance rules, wrt "withcppclass" keyword

		// if all OK, add inherited members to our member list
		for (String memberName : e.getMembers().keySet()) {
			if (members.containsKey(memberName))
				errors.add(node, "conflict: '"+memberName+"' occurs in multiple base interfaces");
			else
			    members.put(memberName, e.getMembers().get(memberName));
		}

		// then process children
		validateChildren(node);
	}

	@Override
    protected void validateElement(InterfaceNameNode node) {
		// nothing to do here: compliance to "like" interfaces will be checked
		// after we finished validating the component
		validateChildren(node);
	}

	@Override
    protected void validateElement(SimpleModuleNode node) {
		doValidateComponent(node);
	}

	@Override
    protected void validateElement(ModuleInterfaceNode node) {
		doValidateComponent(node);
	}

	@Override
    protected void validateElement(CompoundModuleNode node) {
		doValidateComponent(node);
	}

	@Override
    protected void validateElement(ChannelInterfaceNode node) {
		doValidateComponent(node);
	}

	@Override
    protected void validateElement(ChannelNode node) {
		//XXX check: exactly one of "extends" and "withcppclass" must be present!!!
		doValidateComponent(node);
	}

	/* utility method */
	protected void doValidateComponent(INEDElement node) {
        // init
		componentNode = node;
		Assert.isTrue(members.isEmpty());
		Assert.isTrue(innerTypes.isEmpty());

		// do the work
		validateChildren(node);
		//XXX check compliance to "like" interfaces

		// clean up
		componentNode = null;
		members.clear();
		innerTypes.clear();
	}

	@Override
    protected void validateElement(ParametersNode node) {
		validateChildren(node);
	}

    // FIXME inner types should be checked if they are already defined
    // global types are overridden by the local inner type definition
	@Override
    protected void validateElement(ParamNode node) {
		// structural, not checked by the DTD

		// parameter definitions
		String parname = node.getName();
		if (node.getType()!=NED_PARTYPE_NONE) {
			// check definitions: allowed here at all?
			if (submoduleNode!=null) {
				errors.add(node, "'"+parname+"': new parameters can only be defined on a module type, but not per submodule");
				return;
			}
			if (channelSpecNode!=null) {
				errors.add(node, "'"+parname+"': new channel parameters can only be defined on a channel type, but not per connection");
				return;
			}

			// param must NOT exist yet
			if (members.containsKey(parname)) {
				errors.add(node, "'"+parname+"': already defined at "+members.get(parname).getSourceLocation()); // and may not be a parameter at all...
				return;
			}
			members.put(parname, node);
		}

		// check assignments: the param must exist already, find definition
		ParamNode decl = null;
		if (submoduleNode!=null) {
			// inside a submodule's definition
			if (submoduleType==null) {
				errors.add(node, "cannot assign parameters of a submodule of unknown type");
				return;
			}
			decl = (ParamNode) submoduleType.getParamDeclarations().get(parname);
			if (decl==null) {
				errors.add(node, "'"+parname+"': type '"+submoduleType.getName()+"' has no such parameter");
				return;
			}
		}
		else if (channelSpecNode!=null) {
			// inside a connection's channel spec
			if (channelSpecType==null) {
				errors.add(node, "cannot assign parameters of a channel of unknown type");
				return;
			}
			decl = (ParamNode) channelSpecType.getParamDeclarations().get(parname);
			if (decl==null) {
				errors.add(node, "'"+parname+"': type '"+channelSpecType.getName()+"' has no such parameter");
				return;
			}
		}
		else {
			// global "parameters" section of type
			if (!members.containsKey(parname)) {
				errors.add(node, "'"+parname+"': undefined parameter");
				return;
			}
			decl = (ParamNode)members.get(parname);
		}

		//XXX: check expression matches type in the declaration

		validateChildren(node);
	}

	@Override
    protected void validateElement(PatternNode node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(PropertyNode node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(PropertyKeyNode node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(GatesNode node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(GateNode node) {
		// gate definitions
		String gatename = node.getName();
		if (node.getType()!=NED_GATETYPE_NONE) {
			// check definitions: allowed here at all?
			if (submoduleNode!=null) {
				errors.add(node, "'"+gatename+"': new gates can only be defined on a module type, but not per submodule");
				return;
			}

			// gate must NOT exist already
			if (members.containsKey(gatename)) {
				errors.add(node, "'"+gatename+"': already defined at "+members.get(gatename).getSourceLocation()); // and may not be a parameter at all...
				return;
			}
			members.put(gatename, node);
		}

		// for further checks: the gate must exist already, find definition
		GateNode decl = null;
		if (submoduleNode!=null) {
			// inside a submodule's definition
			if (submoduleType==null) {
				errors.add(node, "cannot configure gates of a submodule of unknown type");
				return;
			}
			decl = (GateNode) submoduleType.getGateDeclarations().get(gatename);
			if (decl==null) {
				errors.add(node, "'"+gatename+"': type '"+submoduleType.getName()+"' has no such gate");
				return;
			}
		}
		else {
			// global "gates" section of module
			if (!members.containsKey(gatename)) {
				errors.add(node, "'"+gatename+"': undefined gate");
				return;
			}
			decl = (GateNode)members.get(gatename);
		}

		// check vector/non vector stuff
        if (decl.getIsVector() && !node.getIsVector()) {
			errors.add(node, "missing []: '"+gatename+"' was declared as a vector gate at "+decl.getSourceLocation());
			return;
        }
        if (!decl.getIsVector() && node.getIsVector()) {
			errors.add(node, "'"+gatename+"' was declared as a non-vector gate at "+decl.getSourceLocation());
			return;
        }
		validateChildren(node);
	}

	@Override
    protected void validateElement(TypesNode node) {
		for (INEDElement child : node) {
			NEDValidator validator = new NEDValidator(resolver, errors);
			switch (child.getTagCode()) {
				case NED_COMMENT:
					break;
				case NED_SIMPLE_MODULE:
				case NED_MODULE_INTERFACE:
				case NED_COMPOUND_MODULE:
				case NED_CHANNEL_INTERFACE:
				case NED_CHANNEL:
					validator.validate(child);
					String name = child.getAttribute("name");
					innerTypes.put(name, resolver.wrapNEDElement((INedTypeNode)child));
					members.put(name, child);
					break;
				default:
					Assert.isTrue(false, "unexpected element type: "+child.getTagName());
			}
		}
	}

	@Override
    protected void validateElement(SubmodulesNode node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(SubmoduleNode node) {
		// find submodule type
		String name = node.getName();
		String typeName = node.getType();
		String likeTypeName = node.getLikeType();
		if (typeName!=null && !typeName.equals("")) {
			// normal case
			submoduleType = resolveTypeName(typeName);
			if (submoduleType == null) {
				errors.add(node, "'"+typeName+"': no such module type");
				return;
			}
			int typeTag = submoduleType.getNEDElement().getTagCode();
			if (typeTag!=NED_SIMPLE_MODULE && typeTag!=NED_COMPOUND_MODULE) {
				errors.add(node, "'"+typeName+"' is not a module type");
				return;
			}
		}
		else if ("*".equals(likeTypeName)) {
			// unchecked "like"...
			submoduleType = null;
		}
		else if (likeTypeName!=null && !likeTypeName.equals("")) {
			// "like" case
			submoduleType = resolveTypeName(likeTypeName);
			if (submoduleType == null) {
				errors.add(node, "'"+likeTypeName+"': no such module or interface type");
				return;
			}
			int typeTag = submoduleType.getNEDElement().getTagCode();
			if (typeTag!=NED_SIMPLE_MODULE && typeTag!=NED_COMPOUND_MODULE && typeTag!=NED_MODULE_INTERFACE) {
				errors.add(node, "'"+typeName+"' is not a module or interface type");
				return;
			}
		}
		else {
			// XXXX   the "<> like *" case comes here but it should NOT!!!
			errors.add(node, "no type info for '"+name+"'");  // should never happen
			return;
		}

		// validate contents
		submoduleNode = node;
		validateChildren(node);
		submoduleNode = null;
	}

	protected INEDTypeInfo resolveTypeName(String typeName) {
		INEDTypeInfo component = innerTypes.get(typeName);
		if (component!=null)
			return component;
		return resolver.getComponent(typeName);
	}

	@Override
    protected void validateElement(ConnectionsNode node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(ConnectionNode node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(ChannelSpecNode node) {
		// find channel type
		String typeName = node.getType();
		String likeTypeName = node.getLikeType();
		if (typeName!=null && !typeName.equals("")) {
			// normal case
			channelSpecType = resolveTypeName(typeName);
			if (channelSpecType == null) {
				errors.add(node, "'"+typeName+"': no such channel type");
				return;
			}
			int typeTag = channelSpecType.getNEDElement().getTagCode();
			if (typeTag!=NED_CHANNEL) {
				errors.add(node, "'"+typeName+"' is not a channel type");
				return;
			}
		}
		else if ("*".equals(likeTypeName)) {
			// unchecked "like"...
			channelSpecType = null;
		}
		else if (likeTypeName!=null && !likeTypeName.equals("")) {
			// "like" case
			channelSpecType = resolver.getComponent(likeTypeName);
			if (channelSpecType == null) {
				errors.add(node, "'"+likeTypeName+"': no such channel or channel interface type");
				return;
			}
			int typeTag = channelSpecType.getNEDElement().getTagCode();
			if (typeTag!=NED_CHANNEL && typeTag!=NED_CHANNEL_INTERFACE) {
				errors.add(node, "'"+typeName+"' is not a channel or channel interface type");
				return;
			}
		}
		else {
			// fallback: type is BasicChannel
			channelSpecType = resolver.getComponent("cBasicChannel");
			Assert.isTrue(channelSpecType!=null);
		}

		// validate contents
		channelSpecNode = node;
		validateChildren(node);
		channelSpecNode = null;
	}

	@Override
    protected void validateElement(ConnectionGroupNode node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(LoopNode node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(ConditionNode node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(ExpressionNode node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(OperatorNode node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(FunctionNode node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(IdentNode node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(LiteralNode node) {
		validateChildren(node);
	}

	/*------MSG----------------------------------------------------*/

	@Override
    protected void validateElement(MsgFileNode node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(CplusplusNode node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(StructDeclNode node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(ClassDeclNode node) {
		validateChildren(node);
	}

	protected void validateElement(MessageDeclNode node) {
		validateChildren(node);
	}

	protected void validateElement(EnumDeclNode node) {
		validateChildren(node);
	}

	protected void validateElement(EnumNode node) {
		validateChildren(node);
	}

	protected void validateElement(EnumFieldsNode node) {
		validateChildren(node);
	}

	protected void validateElement(EnumFieldNode node) {
		validateChildren(node);
	}

	protected void validateElement(MessageNode node) {
		validateChildren(node);
	}

	protected void validateElement(ClassNode node) {
		validateChildren(node);
	}

	protected void validateElement(StructNode node) {
		validateChildren(node);
	}

	protected void validateElement(FieldsNode node) {
		validateChildren(node);
	}

	protected void validateElement(FieldNode node) {
		validateChildren(node);
	}

	protected void validateElement(PropertiesNode node) {
		validateChildren(node);
	}

	protected void validateElement(MsgpropertyNode node) {
		validateChildren(node);
	}

	protected void validateElement(UnknownNode node) {
		validateChildren(node);
	}
}

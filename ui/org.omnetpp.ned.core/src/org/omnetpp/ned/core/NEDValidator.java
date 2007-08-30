package org.omnetpp.ned.core;

import java.util.HashMap;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.INEDErrorStore;
import org.omnetpp.ned.model.ex.ChannelElementEx;
import org.omnetpp.ned.model.ex.ChannelInterfaceElementEx;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.GateElementEx;
import org.omnetpp.ned.model.ex.ModuleInterfaceElementEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.ex.SimpleModuleElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IHasGates;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.pojo.ChannelSpecElement;
import org.omnetpp.ned.model.pojo.ClassDeclElement;
import org.omnetpp.ned.model.pojo.ClassElement;
import org.omnetpp.ned.model.pojo.CommentElement;
import org.omnetpp.ned.model.pojo.ConditionElement;
import org.omnetpp.ned.model.pojo.ConnectionGroupElement;
import org.omnetpp.ned.model.pojo.ConnectionsElement;
import org.omnetpp.ned.model.pojo.CplusplusElement;
import org.omnetpp.ned.model.pojo.EnumDeclElement;
import org.omnetpp.ned.model.pojo.EnumElement;
import org.omnetpp.ned.model.pojo.EnumFieldElement;
import org.omnetpp.ned.model.pojo.EnumFieldsElement;
import org.omnetpp.ned.model.pojo.ExpressionElement;
import org.omnetpp.ned.model.pojo.ExtendsElement;
import org.omnetpp.ned.model.pojo.FieldElement;
import org.omnetpp.ned.model.pojo.FieldsElement;
import org.omnetpp.ned.model.pojo.FilesElement;
import org.omnetpp.ned.model.pojo.FunctionElement;
import org.omnetpp.ned.model.pojo.GateElement;
import org.omnetpp.ned.model.pojo.GatesElement;
import org.omnetpp.ned.model.pojo.IdentElement;
import org.omnetpp.ned.model.pojo.ImportElement;
import org.omnetpp.ned.model.pojo.InterfaceNameElement;
import org.omnetpp.ned.model.pojo.LiteralElement;
import org.omnetpp.ned.model.pojo.LoopElement;
import org.omnetpp.ned.model.pojo.MessageDeclElement;
import org.omnetpp.ned.model.pojo.MessageElement;
import org.omnetpp.ned.model.pojo.MsgFileElement;
import org.omnetpp.ned.model.pojo.MsgpropertyElement;
import org.omnetpp.ned.model.pojo.OperatorElement;
import org.omnetpp.ned.model.pojo.ParamElement;
import org.omnetpp.ned.model.pojo.ParametersElement;
import org.omnetpp.ned.model.pojo.PatternElement;
import org.omnetpp.ned.model.pojo.PropertiesElement;
import org.omnetpp.ned.model.pojo.PropertyDeclElement;
import org.omnetpp.ned.model.pojo.PropertyElement;
import org.omnetpp.ned.model.pojo.PropertyKeyElement;
import org.omnetpp.ned.model.pojo.StructDeclElement;
import org.omnetpp.ned.model.pojo.StructElement;
import org.omnetpp.ned.model.pojo.SubmodulesElement;
import org.omnetpp.ned.model.pojo.TypesElement;
import org.omnetpp.ned.model.pojo.UnknownElement;

/**
 * Validates consistency of NED files.
 *
 * This code assumes UNPARSED expressions, and consequently, it doesn't validate expressions at all.
 *
 * @author andras
 */
//FIXME todo: validation of embedded types!!!!
//FIXME should be re-though -- it very much under-uses INedTypeInfo!!!
//FIXME asap: validate extends chain (cycles!!) validate 2 submods with the same name! etc 
public class NEDValidator extends AbstractNEDValidatorEx {

	INEDTypeResolver resolver;

	INEDErrorStore errors;

	// the component currently being validated
	INedTypeElement componentNode;

	// non-null while we're validating a submodule
	SubmoduleElementEx submoduleNode;
	INEDTypeInfo submoduleType; // may be null; valid while submoduleNode!=null

	// non-null while we're validating a channel spec of a connection
	ChannelSpecElement channelSpecElement;
	INEDTypeInfo channelSpecType; // may be null; valid while channelSpecElement!=null

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
    protected void validateElement(FilesElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(NedFileElementEx node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(CommentElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(ImportElement node) {
		//TODO check if file exists?
		validateChildren(node);
	}

	@Override
    protected void validateElement(PropertyDeclElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(ExtendsElement node) {
		Assert.isTrue(componentNode!=null);

		// referenced component must exist and must be the same type as this one
		String name = node.getName();
		INEDTypeInfo e = resolver.getComponent(name);
		if (e == null) {
			errors.addError(node, "no such component: '" + name+"'");
			return;
		}
		int thisType = componentNode.getTagCode();
		int extendsType = e.getNEDElement().getTagCode();
		if (thisType != extendsType) {
			errors.addError(node, "'"+name+"' is not a "+componentNode.getReadableTagName());
			return;
		}
		//XXX enforce channel inheritance rules, wrt "withcppclass" keyword

		// if all OK, add inherited members to our member list
		for (String memberName : e.getMembers().keySet()) {
			if (members.containsKey(memberName))
				errors.addError(node, "conflict: '"+memberName+"' occurs in multiple base interfaces");
			else
			    members.put(memberName, e.getMembers().get(memberName));
		}

		// then process children
		validateChildren(node);
	}

	@Override
    protected void validateElement(InterfaceNameElement node) {
		// nothing to do here: compliance to "like" interfaces will be checked
		// after we finished validating the component
		validateChildren(node);
	}

	@Override
    protected void validateElement(SimpleModuleElementEx node) {
		doValidateComponent(node);
	}

	@Override
    protected void validateElement(ModuleInterfaceElementEx node) {
		doValidateComponent(node);
	}

	@Override
    protected void validateElement(CompoundModuleElementEx node) {
		doValidateComponent(node);
	}

	@Override
    protected void validateElement(ChannelInterfaceElementEx node) {
		doValidateComponent(node);
	}

	@Override
    protected void validateElement(ChannelElementEx node) {
		//XXX check: exactly one of "extends" and "withcppclass" must be present!!!
		doValidateComponent(node);
	}

	/* utility method */
	protected void doValidateComponent(INedTypeElement node) {
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
    protected void validateElement(ParametersElement node) {
		validateChildren(node);
	}

    // FIXME inner types should be checked if they are already defined
    // global types are overridden by the local inner type definition
	@Override
    protected void validateElement(ParamElement node) {
		// structural, not checked by the DTD

		// parameter definitions
		String parname = node.getName();
		if (node.getType()!=NED_PARTYPE_NONE) {
			// check definitions: allowed here at all?
			if (submoduleNode!=null) {
				errors.addError(node, "'"+parname+"': new parameters can only be defined on a module type, but not per submodule");
				return;
			}
			if (channelSpecElement!=null) {
				errors.addError(node, "'"+parname+"': new channel parameters can only be defined on a channel type, but not per connection");
				return;
			}

			// param must NOT exist yet
			if (members.containsKey(parname)) {
				errors.addError(node, "'"+parname+"': already defined at "+members.get(parname).getSourceLocation()); // and may not be a parameter at all...
				return;
			}
			members.put(parname, node);
		}

		// check assignments: the param must exist already, find definition
		ParamElement decl = null;
		if (submoduleNode!=null) {
			// inside a submodule's definition
			if (submoduleType==null) {
				errors.addError(node, "cannot assign parameters of a submodule of unknown type");
				return;
			}
			decl = submoduleType.getParamDeclarations().get(parname);
			if (decl==null) {
				errors.addError(node, "'"+parname+"': type '"+submoduleType.getName()+"' has no such parameter");
				return;
			}
		}
		else if (channelSpecElement!=null) {
			// inside a connection's channel spec
			if (channelSpecType==null) {
				errors.addError(node, "cannot assign parameters of a channel of unknown type");
				return;
			}
			decl = channelSpecType.getParamDeclarations().get(parname);
			if (decl==null) {
				errors.addError(node, "'"+parname+"': type '"+channelSpecType.getName()+"' has no such parameter");
				return;
			}
		}
		else {
			// global "parameters" section of type
			if (!members.containsKey(parname)) {
				errors.addError(node, "'"+parname+"': undefined parameter");
				return;
			}
			decl = (ParamElement)members.get(parname);
		}

		//XXX: check expression matches type in the declaration

		validateChildren(node);
	}

	@Override
    protected void validateElement(PatternElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(PropertyElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(PropertyKeyElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(GatesElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(GateElement node) {
		// gate definitions
		String gatename = node.getName();
		if (node.getType()!=NED_GATETYPE_NONE) {
			// check definitions: allowed here at all?
			if (submoduleNode!=null) {
				errors.addError(node, "'"+gatename+"': new gates can only be defined on a module type, but not per submodule");
				return;
			}

			// gate must NOT exist already
			if (members.containsKey(gatename)) {
				errors.addError(node, "'"+gatename+"': already defined at "+members.get(gatename).getSourceLocation()); // and may not be a parameter at all...
				return;
			}
			members.put(gatename, node);
		}

		// for further checks: the gate must exist already, find definition
		GateElement decl = null;
		if (submoduleNode!=null) {
			// inside a submodule's definition
			if (submoduleType==null) {
				errors.addError(node, "cannot configure gates of a submodule of unknown type");
				return;
			}
			decl = submoduleType.getGateDeclarations().get(gatename);
			if (decl==null) {
				errors.addError(node, "'"+gatename+"': type '"+submoduleType.getName()+"' has no such gate");
				return;
			}
		}
		else {
			// global "gates" section of module
			if (!members.containsKey(gatename)) {
				errors.addError(node, "'"+gatename+"': undefined gate");
				return;
			}
			decl = (GateElement)members.get(gatename);
		}

		// check vector/non vector stuff
        if (decl.getIsVector() && !node.getIsVector()) {
			errors.addError(node, "missing []: '"+gatename+"' was declared as a vector gate at "+decl.getSourceLocation());
			return;
        }
        if (!decl.getIsVector() && node.getIsVector()) {
			errors.addError(node, "'"+gatename+"' was declared as a non-vector gate at "+decl.getSourceLocation());
			return;
        }
		validateChildren(node);
	}

	@Override
    protected void validateElement(TypesElement node) {
		for (INEDElement child : node) {
			if (child instanceof INedTypeElement) {
				INedTypeElement typeElement = (INedTypeElement)child;
				new NEDValidator(resolver, errors).validate(child);
				String name = typeElement.getName();
				innerTypes.put(name, typeElement.getNEDTypeInfo()); //FIXME typeInfo already stores this
				members.put(name, child);
			}
		}
	}

	@Override
    protected void validateElement(SubmodulesElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(SubmoduleElementEx node) {
		// find submodule type
		String name = node.getName();
		String typeName = node.getType();
		String likeTypeName = node.getLikeType();
		if (typeName!=null && !typeName.equals("")) {
			// normal case
			submoduleType = resolveTypeName(typeName);
			if (submoduleType == null) {
				errors.addError(node, "'"+typeName+"': no such module type");
				return;
			}
			int typeTag = submoduleType.getNEDElement().getTagCode();
			if (typeTag!=NED_SIMPLE_MODULE && typeTag!=NED_COMPOUND_MODULE) {
				errors.addError(node, "'"+typeName+"' is not a module type");
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
				errors.addError(node, "'"+likeTypeName+"': no such module or interface type");
				return;
			}
			int typeTag = submoduleType.getNEDElement().getTagCode();
			if (typeTag!=NED_SIMPLE_MODULE && typeTag!=NED_COMPOUND_MODULE && typeTag!=NED_MODULE_INTERFACE) {
				errors.addError(node, "'"+typeName+"' is not a module or interface type");
				return;
			}
		}
		else {
			// XXXX   the "<> like *" case comes here but it should NOT!!!
			errors.addError(node, "no type info for '"+name+"'");  // should never happen
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
    protected void validateElement(ConnectionsElement node) {
		validateChildren(node);
	}

	protected void checkGate(IHasGates module, String gateName, ConnectionElementEx conn, boolean doSrcGate) {
	    // check gate direction, check if vector
		int subgate = doSrcGate ? conn.getSrcGateSubg() : conn.getDestGateSubg();
		String gateIndex = doSrcGate ? conn.getSrcGateIndex() : conn.getDestGateIndex();
		boolean gatePlusPlus  = doSrcGate ? conn.getSrcGatePlusplus() : conn.getDestGatePlusplus();
		boolean gateUsedAsVector = gatePlusPlus || StringUtils.isNotEmpty(gateIndex);
		
		// gate is vector / not vector
		String prefix = doSrcGate ? "wrong source gate: " : "wrong destination gate: ";
		GateElementEx gate = module.getGateDeclarations().get(gateName);
		if (gateUsedAsVector && !gate.getIsVector())
	        errors.addError(conn, prefix + "extra gate index or '++' ('"+gateName+"' is not a vector gate)");
	    else if (!gateUsedAsVector && gate.getIsVector())
	    	errors.addError(conn, prefix + "missing gate index ('"+gateName+"' is a vector gate)");

		// check subgate notation
		if (gate.getType() != NED_GATETYPE_INOUT && subgate != NED_SUBGATE_NONE)
			errors.addError(conn, prefix + "$i/$o syntax only allowed for inout gates ('"+gateName+"' is an "+getGateTypeAsString(gate.getType())+" gate)");
		else {
			// check gate direction
			int gateDir = subgate==NED_SUBGATE_I ? NED_GATETYPE_INPUT : subgate==NED_SUBGATE_O ? NED_GATETYPE_OUTPUT : gate.getType(); 

			boolean connectedToParent = StringUtils.isEmpty(doSrcGate ? conn.getSrcModule() : conn.getDestModule());
			int expectedGateDir;
			if (conn.getArrowDirection()==NED_ARROWDIR_BIDIR)
				expectedGateDir = NED_GATETYPE_INOUT;
			else 
				expectedGateDir = (doSrcGate==connectedToParent) ? NED_GATETYPE_INPUT : NED_GATETYPE_OUTPUT;

			if (gateDir != expectedGateDir) {
				String gateDirString = getGateTypeAsString(gateDir);
				String expectedGateDirString = getGateTypeAsString(expectedGateDir);
				String fullGateName = subgate==NED_SUBGATE_I ? (gateName+"$i") : subgate==NED_SUBGATE_O ? (gateName+"$o") : gateName; 
				errors.addError(conn, prefix + expectedGateDirString+" gate expected but '"+fullGateName+"' is an "+gateDirString+" gate");
			}
		}
	}

	private static String getGateTypeAsString(int gateDir) {
		return gateDir==NED_GATETYPE_INPUT ? "input" : gateDir==NED_GATETYPE_OUTPUT ? "output" : "inout";
	}

	protected void validateConnGate(ConnectionElementEx conn, boolean doSrcGate) {
		String submodName = doSrcGate ? conn.getSrcModule() : conn.getDestModule();
		String submodIndex = doSrcGate ? conn.getSrcModuleIndex() : conn.getDestModuleIndex();
		String gateName = doSrcGate ? conn.getSrcGate() : conn.getDestGate();
		boolean hasSubmodIndex = StringUtils.isNotEmpty(submodIndex);
		
		CompoundModuleElementEx compoundModule = (CompoundModuleElementEx) componentNode;
	    String prefix = doSrcGate ? "wrong source gate: " : "wrong destination gate: ";
	    if (StringUtils.isEmpty(submodName)) {
	        // connected to parent module: check such gate is declared
	    	GateElementEx gate = compoundModule.getGateDeclarations().get(gateName);
	    	if (gate == null)
	            errors.addError(conn, prefix + "compound module has no gate named '"+gateName+"'");
	        else
            	checkGate(compoundModule, gateName, conn, doSrcGate);
	    }
	    else {
	        // check such submodule is declared
	    	SubmoduleElementEx submodule = ((CompoundModuleElementEx)componentNode).getSubmoduleByName(submodName);
	    	if (submodule == null)
	            errors.addError(conn, prefix + "no such submodule: '"+submodName+"'");
	        else {
	            boolean isSubmodVector = StringUtils.isNotEmpty(submodule.getVectorSize());
	            if (hasSubmodIndex && !isSubmodVector)
	                errors.addError(conn, prefix + "extra submodule index ('"+submodName+"' is not a vector submodule)");
	            else if (!hasSubmodIndex && isSubmodVector)
	                errors.addError(conn, prefix + "missing submodule index ('"+submodName+"' is a vector submodule)");

	            // check gate
	            GateElementEx gate = submodule.getGateDeclarations().get(gateName);
	            if (gate == null)
	            	errors.addError(conn, prefix + "submodule '"+submodName+"' has no gate named '"+gateName+"'");
	            else
	            	checkGate(submodule, gateName, conn, doSrcGate);
	        }
	    }
	}

	@Override
	protected void validateElement(ConnectionElementEx node)
	{
		// make sure submodule and gate names are valid, gate direction is OK
		// and that gates & modules are really vector (or really not)
		Assert.isTrue(componentNode instanceof CompoundModuleElementEx);
	    validateConnGate(node, true);
	    validateConnGate(node, false);

	    //FIXME validate channel
	    validateChildren(node);
	}

	@Override
    protected void validateElement(ChannelSpecElement node) {
		// find channel type
		String typeName = node.getType();
		String likeTypeName = node.getLikeType();
		if (typeName!=null && !typeName.equals("")) {
			// normal case
			channelSpecType = resolveTypeName(typeName);
			if (channelSpecType == null) {
				errors.addError(node, "'"+typeName+"': no such channel type");
				return;
			}
			int typeTag = channelSpecType.getNEDElement().getTagCode();
			if (typeTag!=NED_CHANNEL) {
				errors.addError(node, "'"+typeName+"' is not a channel type");
				return;
			}
		}
		else if (likeTypeName!=null && !likeTypeName.equals("")) {
			// "like" case
			channelSpecType = resolver.getComponent(likeTypeName);
			if (channelSpecType == null) {
				errors.addError(node, "'"+likeTypeName+"': no such channel or channel interface type");
				return;
			}
			int typeTag = channelSpecType.getNEDElement().getTagCode();
			if (typeTag!=NED_CHANNEL && typeTag!=NED_CHANNEL_INTERFACE) {
				errors.addError(node, "'"+typeName+"' is not a channel or channel interface type");
				return;
			}
		}
		else {
			// fallback: type is BasicChannel
			channelSpecType = resolver.getComponent("cBasicChannel");
			Assert.isTrue(channelSpecType!=null);
		}

		// validate contents
		channelSpecElement = node;
		validateChildren(node);
		channelSpecElement = null;
	}

	@Override
    protected void validateElement(ConnectionGroupElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(LoopElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(ConditionElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(ExpressionElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(OperatorElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(FunctionElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(IdentElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(LiteralElement node) {
		validateChildren(node);
	}

	/*------MSG----------------------------------------------------*/

	@Override
    protected void validateElement(MsgFileElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(CplusplusElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(StructDeclElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(ClassDeclElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(MessageDeclElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(EnumDeclElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(EnumElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(EnumFieldsElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(EnumFieldElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(MessageElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(ClassElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(StructElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(FieldsElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(FieldElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(PropertiesElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(MsgpropertyElement node) {
		validateChildren(node);
	}

	@Override
    protected void validateElement(UnknownElement node) {
		validateChildren(node);
	}
}

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.core;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Set;
import java.util.Vector;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.UnitConversion;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.INedErrorStore;
import org.omnetpp.ned.model.ex.ChannelElementEx;
import org.omnetpp.ned.model.ex.ChannelInterfaceElementEx;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.GateElementEx;
import org.omnetpp.ned.model.ex.ModuleInterfaceElementEx;
import org.omnetpp.ned.model.ex.NedElementUtilEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.ex.PropertyElementEx;
import org.omnetpp.ned.model.ex.SimpleModuleElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IHasGates;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.IModuleTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;
import org.omnetpp.ned.model.pojo.ChannelInterfaceElement;
import org.omnetpp.ned.model.pojo.ClassDeclElement;
import org.omnetpp.ned.model.pojo.ClassElement;
import org.omnetpp.ned.model.pojo.CommentElement;
import org.omnetpp.ned.model.pojo.ConditionElement;
import org.omnetpp.ned.model.pojo.ConnectionElement;
import org.omnetpp.ned.model.pojo.ConnectionGroupElement;
import org.omnetpp.ned.model.pojo.ConnectionsElement;
import org.omnetpp.ned.model.pojo.CplusplusElement;
import org.omnetpp.ned.model.pojo.EnumDeclElement;
import org.omnetpp.ned.model.pojo.EnumElement;
import org.omnetpp.ned.model.pojo.EnumFieldElement;
import org.omnetpp.ned.model.pojo.ExpressionElement;
import org.omnetpp.ned.model.pojo.ExtendsElement;
import org.omnetpp.ned.model.pojo.FieldElement;
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
import org.omnetpp.ned.model.pojo.ModuleInterfaceElement;
import org.omnetpp.ned.model.pojo.MsgFileElement;
import org.omnetpp.ned.model.pojo.NamespaceElement;
import org.omnetpp.ned.model.pojo.OperatorElement;
import org.omnetpp.ned.model.pojo.PackageElement;
import org.omnetpp.ned.model.pojo.PacketDeclElement;
import org.omnetpp.ned.model.pojo.PacketElement;
import org.omnetpp.ned.model.pojo.ParamElement;
import org.omnetpp.ned.model.pojo.ParametersElement;
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
//FIXME check that a module/channel satisfies the interfaces it implements!
//FIXME finish validator functions! e.g. turn on expression parsing
//FIXME todo: validation of embedded types!!!!
//FIXME should be re-though -- it very much under-uses INedTypeInfo!!!
//FIXME asap: validate extends chain (cycles!!)
//FIXME validate 2 submods with the same name! etc
//FIXME validate imports (what if there're clashes)
//FIXME validate like-expr: must be a string parameter, etc!
//FIXME detect inheritance cycles
public class NedValidator extends AbstractNedValidatorEx {

    private static final String DEFAULT_CHANNEL_TYPE = "ned.DatarateChannel";

    private INedTypeResolver resolver;

    private INedErrorStore errors;

    // the project in whose context fully qualified type names should be visible
    private IProject contextProject;

    // the component currently being validated
    private INedTypeElement componentNode;

    // non-null while we're validating a submodule
    private SubmoduleElementEx submoduleNode;
    private INedTypeInfo submoduleType; // may be null; valid while submoduleNode!=null

    // non-null while we're validating the type of a connection
    private ConnectionElement connectionElement;
    private INedTypeInfo connectionType; // may be null; valid while connectionElement!=null

    // members of the component currently being validated
    private HashMap<String, INedElement> members = new HashMap<String, INedElement>();

    // contents of the "types:" section of the component currently being validated
    private HashMap<String, INedTypeInfo> innerTypes = new HashMap<String, INedTypeInfo>();


    /**
     * Constructor.
     */
    public NedValidator(INedTypeResolver resolver, IProject context, INedErrorStore errors) {
        this.resolver = resolver;
        this.contextProject = context;
        this.errors = errors;
    }

    @Override
    public void validate(INedElement node) {
        validateElement(node);
    }

    protected void validateChildren(INedElement node) {
        for (INedElement child : node)
            validate(child);
    }

    @Override
    protected void validateElement(FilesElement node) {
        validateChildren(node);
    }

    @Override
    protected void validateElement(NedFileElementEx node) {
        // check it's in the right package
        IFile file = resolver.getNedFile(node);
        String expectedPackage = resolver.getExpectedPackageFor(file);
        String declaredPackage = StringUtils.nullToEmpty(node.getPackage());

        if (expectedPackage != null && !expectedPackage.equals(declaredPackage)) {
            INedElement errorNode = node.getFirstPackageChild()!=null ? node.getFirstPackageChild() : node;
            errors.addError(errorNode, "declared package \""+declaredPackage+"\" does not match expected package \"" + expectedPackage +"\"");
        }

        validateChildren(node);
    }

    @Override
    protected void validateElement(CommentElement node) {
        validateChildren(node);
    }

    @Override
    protected void validateElement(PackageElement node) {
        validateChildren(node);
    }

    @Override
    protected void validateElement(ImportElement node) {
        //XXX check clashing imports!
        String name = node.getImportSpec();
        if (!name.contains("*")) {
            // not a wildcard import: must match a type
            if (resolver.getToplevelNedType(name, contextProject) == null)
                errors.addError(node, "imported NED type not found: '" + name+"'");
        }
        else {
            // wildcard import: check if it matches anything
            String regex = NedElementUtilEx.importToRegex(name);
            boolean found = false;
            for (String qualifiedName : resolver.getToplevelNedTypeQNames(contextProject))
                if (qualifiedName.matches(regex))
                    {found = true; break;}
            if (!found)
                errors.addWarning(node, "import does not match any NED type: '" + name+"'");
        }
        validateChildren(node);
    }

    @Override
    protected void validateElement(PropertyDeclElement node) {
        validateChildren(node);
    }

    @Override
    protected void validateElement(ExtendsElement node) {
        Assert.isTrue(componentNode!=null);

        //FIXME detect cycles, etc

        // referenced component must exist and must be the same type as this one
        String name = node.getName();
        INedTypeInfo e = resolver.lookupNedType(name, componentNode.getParentLookupContext());
        if (e == null) {
            errors.addError(node, "no such component: '" + name+"'");
            return;
        }
        int thisType = componentNode.getTagCode();
        int extendsType = e.getNedElement().getTagCode();

        boolean moduleExtendsSimple = (thisType == NED_COMPOUND_MODULE) && (extendsType == NED_SIMPLE_MODULE);
        if (thisType != extendsType && !moduleExtendsSimple) {
            errors.addError(node, "a "+componentNode.getReadableTagName()+" cannot extend a "+e.getNedElement().getReadableTagName()+" ('"+name+"')");
            return;
        }

        // if all OK, add inherited members to our member list
        for (String memberName : e.getMembers().keySet()) {
            INedElement member = e.getMembers().get(memberName);
            if (members.containsKey(memberName)) {
                if (members.get(memberName) != member)
                    errors.addError(node, "conflict: '"+memberName+"' occurs in multiple base interfaces");
            }
            else
                members.put(memberName, member);
        }

        // validate xxx
        Set<String> inheritedSubmodules = e.getSubmodules().keySet();
        INedElement section = componentNode.getFirstChildWithTag(NED_SUBMODULES);
        if (section != null) {
            for (INedElement submodule : section) {
                if (submodule instanceof IHasName && submodule.getTagCode()==NED_SUBMODULE) {
                    String submoduleName = ((IHasName)submodule).getName();
                    if (inheritedSubmodules.contains(submoduleName))
                        errors.addError(submodule, "conflict: '"+submoduleName+"' is already defined as an inherited submodule");
                }
            }
        }

        // then process children
        validateChildren(node);
    }

    @Override
    protected void validateElement(InterfaceNameElement node) {
        Assert.isTrue(componentNode!=null);

        //FIXME detect cycles, etc

        // referenced component must exist and must be the same type as this one
        String name = node.getName();
        INedTypeInfo e = resolver.lookupNedType(name, componentNode.getParentLookupContext());
        if (e == null) {
            errors.addError(node, "no such interface: '" + name+"'");
            return;
        }

        boolean isModule = componentNode instanceof IModuleTypeElement || componentNode instanceof ModuleInterfaceElement;
        if (isModule && !(e.getNedElement() instanceof ModuleInterfaceElement)) {
            errors.addError(node, "'"+name+"' is not a module interface");
            return;
        }
        if (!isModule && !(e.getNedElement() instanceof ChannelInterfaceElement)) {
            errors.addError(node, "'"+name+"' is not a channel interface");
            return;
        }

//TODO
//        // if all OK, add inherited members to our member list
//        for (String memberName : e.getMembers().keySet()) {
//            if (members.containsKey(memberName))
//                errors.addError(node, "conflict: '"+memberName+"' occurs in multiple base interfaces");
//            else
//                members.put(memberName, e.getMembers().get(memberName));
//        }

        //TODO check compliance to interface (somewhere, not necessarily in the function)

        // then process children
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
            if (connectionElement!=null) {
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
        if (submoduleNode!=null) {
            // inside a submodule's definition
            if (submoduleType==null) {
                errors.addError(node, "cannot assign parameters of a submodule of unknown type");
                return;
            }

            Object[] result = ParamUtil.findMatchingParamDeclarationRecursively(submoduleNode, parname, contextProject);
            ParamElementEx paramDeclaration = (ParamElementEx)result[0];

            if (paramDeclaration == null) {
                String message = "'"+parname+"': type '"+submoduleType.getName()+"' has no such parameter";

                if (node.getIsPattern())
                    errors.addWarning(node, message);
                else
                    errors.addError(node, message);

                return;
            }
            else validateParamAssignment(result, (ParamElementEx)node, paramDeclaration);
        }
        else if (connectionElement!=null) {
            // inside a connection's channel spec
            if (connectionType==null) {
                errors.addError(node, "cannot assign parameters of a channel of unknown type");
                return;
            }

            if (connectionType.getParamDeclarations().get(parname) == null) {
                errors.addError(node, "'"+parname+"': type '"+connectionType.getName()+"' has no such parameter");
                return;
            }
        }
        else {
            Object[] result = ParamUtil.findMatchingParamDeclarationRecursively(componentNode.getNedTypeInfo(), parname, contextProject);
            ParamElementEx paramDeclaration = (ParamElementEx)result[0];

            // global "parameters" section of type
            if (paramDeclaration == null) {
                String message = "'"+parname+"': undefined parameter";

                if (node.getIsPattern())
                    errors.addWarning(node, message);
                else
                    errors.addError(node, message);

                return;
            }
            else validateParamAssignment(result, (ParamElementEx)node, paramDeclaration);
        }

        //XXX: check expression matches type in the declaration

        validateChildren(node);
    }

    @SuppressWarnings("unchecked")
    protected void validateParamAssignment(Object[] result, ParamElementEx paramAssignment, ParamElementEx paramDeclaration) {
        ArrayList<ParamElementEx> paramAssignments = ParamUtil.findParamAssignmentsForParamDeclaration((Vector<INedTypeInfo>)result[1], (Vector<ISubmoduleOrConnection>)result[2], paramDeclaration);
        validateParamValue(paramDeclaration, paramAssignment);

        if (paramAssignments.indexOf(paramAssignment) == -1) {
            for (ParamElementEx otherParamAssignment : paramAssignments) {
                if (otherParamAssignment.getIsDefault()) {
                    errors.addWarning(paramAssignment, "Unused parameter assignment");
                    return;
                }
            }

            if (!StringUtils.isEmpty(paramAssignment.getValue()) && paramAssignments.indexOf(paramAssignment) == -1)
                errors.addError(paramAssignment, "Cannot override already fixed parameter value");
        }
    }

    protected void validateParamValue(ParamElementEx paramDeclaration, ParamElementEx paramAssignment) {
        // determine assignment's data type
        int assignmentType = -1;
        String assignmentUnit = null;
        String assignmentValue = paramAssignment.getValue();
        if (assignmentValue == null)
            assignmentValue = "";

        if (assignmentValue.equals("true") || assignmentValue.equals("false"))
            assignmentType = NED_PARTYPE_BOOL;
        else if (assignmentValue.startsWith("\""))
            assignmentType = NED_PARTYPE_STRING;
        else if (assignmentValue.startsWith("xmldoc(") || assignmentValue.startsWith("xml("))
            assignmentType = NED_PARTYPE_XML;
        else {
            try {
                assignmentUnit = UnitConversion.parseQuantityForUnit(assignmentValue); // throws exception if not a quantity
                Assert.isNotNull(assignmentUnit);
            }
            catch (RuntimeException e) {
                // void
            }

            if (assignmentUnit != null)
                assignmentType = NED_PARTYPE_DOUBLE;
        }

        // determine declaraion's unit
        PropertyElementEx unitProperty = paramDeclaration.getLocalProperties().get("unit");
        String declarationUnit = unitProperty == null ? "" : StringUtils.nullToEmpty(unitProperty.getSimpleValue());

        // provided we could figure out the assignment's data type, check it's the same as declaration's data type
        int declarationType = paramDeclaration.getType();
        int tmpType = declarationType == NED_PARTYPE_INT ? NED_PARTYPE_DOUBLE : declarationType; // replace "int" with "double"

        if (assignmentType != -1 && assignmentType != tmpType) {
            String typeName = paramDeclaration.getAttribute(ParamElement.ATT_TYPE);
            errors.addError(paramAssignment, "Wrong data type: " + typeName + " expected");
        }

        // if value is numeric, check units
        if (assignmentUnit != null) {
            try {
                UnitConversion.parseQuantity(assignmentValue, declarationUnit); // throws exception on incompatible units
            }
            catch (RuntimeException e) {
                errors.addError(paramAssignment, e.getMessage());
            }
        }
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
        for (INedElement child : node) {
            if (child instanceof INedTypeElement) {
                INedTypeElement typeElement = (INedTypeElement)child;
                new NedValidator(resolver, contextProject, errors).validate(child);
                String name = typeElement.getName();
                innerTypes.put(name, typeElement.getNedTypeInfo()); //FIXME typeInfo already stores this
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
        CompoundModuleElementEx compoundModule = (CompoundModuleElementEx)componentNode;
        if (StringUtils.isNotEmpty(typeName)) {
            // normal case
            submoduleType = resolver.lookupNedType(typeName, compoundModule);
            if (submoduleType == null)
                errors.addError(node, "'"+typeName+"': no such module type");
            else if (!(submoduleType.getNedElement() instanceof IModuleTypeElement))
                errors.addError(node, "'"+typeName+"' is not a module type");
        }
        else if (StringUtils.isNotEmpty(likeTypeName)) {
            // "like" case
            submoduleType = resolver.lookupNedType(likeTypeName, compoundModule);
            if (submoduleType == null)
                errors.addError(node, "'"+likeTypeName+"': no such module interface type");
            else if (!(submoduleType.getNedElement() instanceof ModuleInterfaceElementEx))
                errors.addError(node, "'"+likeTypeName+"' is not a module interface type");
        }
        else {
            errors.addError(node, "no type info for '"+name+"'");  // should never happen
            return;
        }

        // validate contents
        submoduleNode = node;
        validateChildren(node);
        submoduleNode = null;
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
            if (conn.getIsBidirectional())
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

        // find channel type
        String typeName = node.getType();
        String likeTypeName = node.getLikeType();
        CompoundModuleElementEx compoundModule = (CompoundModuleElementEx)componentNode;
        if (StringUtils.isNotEmpty(typeName)) {
            // normal case
            connectionType = resolver.lookupNedType(typeName, compoundModule);
            if (connectionType == null)
                errors.addError(node, "'"+typeName+"': no such channel type");
            else if (!(connectionType.getNedElement() instanceof ChannelElementEx))
                errors.addError(node, "'"+typeName+"' is not a channel type");
        }
        else if (StringUtils.isNotEmpty(likeTypeName)) {
            // "like" case
            connectionType = resolver.lookupNedType(likeTypeName, compoundModule);
            if (connectionType == null)
                errors.addError(node, "'"+likeTypeName+"': no such channel or channel interface type");
            else if (!(connectionType.getNedElement() instanceof ChannelInterfaceElementEx))
                errors.addError(node, "'"+likeTypeName+"' is not a channel interface type");
        }
        else {
            // fallback: type is DatarateChannel
            connectionType = resolver.getToplevelNedType(DEFAULT_CHANNEL_TYPE, contextProject);
            Assert.isTrue(connectionType!=null);
        }

        // validate contents
        connectionElement = node;
        validateChildren(node);
        connectionElement = null;
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
    protected void validateElement(NamespaceElement node) {
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
    protected void validateElement(PacketDeclElement node) {
    }

    @Override
    protected void validateElement(PacketElement node) {
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
    protected void validateElement(FieldElement node) {
        validateChildren(node);
    }

    @Override
    protected void validateElement(UnknownElement node) {
        validateChildren(node);
    }
}

/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.model.ned2.emf.util;

import java.util.List;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;

import org.omnetpp.model.ned2.emf.*;

/**
 * <!-- begin-user-doc -->
 * The <b>Switch</b> for the model's inheritance hierarchy.
 * It supports the call {@link #doSwitch(EObject) doSwitch(object)}
 * to invoke the <code>caseXXX</code> method for each class of the model,
 * starting with the actual class of the object
 * and proceeding up the inheritance hierarchy
 * until a non-null result is returned,
 * which is the result of the switch.
 * <!-- end-user-doc -->
 * @see org.omnetpp.model.ned2.emf.Ned2Package
 * @generated
 */
public class Ned2Switch {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String copyright = "";

	/**
	 * The cached model package
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected static Ned2Package modelPackage;

	/**
	 * Creates an instance of the switch.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Ned2Switch() {
		if (modelPackage == null) {
			modelPackage = Ned2Package.eINSTANCE;
		}
	}

	/**
	 * Calls <code>caseXXX</code> for each class of the model until one returns a non null result; it yields that result.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the first non-null result returned by a <code>caseXXX</code> call.
	 * @generated
	 */
	public Object doSwitch(EObject theEObject) {
		return doSwitch(theEObject.eClass(), theEObject);
	}

	/**
	 * Calls <code>caseXXX</code> for each class of the model until one returns a non null result; it yields that result.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the first non-null result returned by a <code>caseXXX</code> call.
	 * @generated
	 */
	protected Object doSwitch(EClass theEClass, EObject theEObject) {
		if (theEClass.eContainer() == modelPackage) {
			return doSwitch(theEClass.getClassifierID(), theEObject);
		} else {
			List eSuperTypes = theEClass.getESuperTypes();
			return eSuperTypes.isEmpty() ? defaultCase(theEObject) : doSwitch(
					(EClass) eSuperTypes.get(0), theEObject);
		}
	}

	/**
	 * Calls <code>caseXXX</code> for each class of the model until one returns a non null result; it yields that result.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the first non-null result returned by a <code>caseXXX</code> call.
	 * @generated
	 */
	protected Object doSwitch(int classifierID, EObject theEObject) {
		switch (classifierID) {
		case Ned2Package.CHANNEL: {
			Channel channel = (Channel) theEObject;
			Object result = caseChannel(channel);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.CHANNEL_INTERFACE: {
			ChannelInterface channelInterface = (ChannelInterface) theEObject;
			Object result = caseChannelInterface(channelInterface);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.COMPOUND_MODULE: {
			CompoundModule compoundModule = (CompoundModule) theEObject;
			Object result = caseCompoundModule(compoundModule);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.CONDITION: {
			Condition condition = (Condition) theEObject;
			Object result = caseCondition(condition);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.CONNECTION: {
			Connection connection = (Connection) theEObject;
			Object result = caseConnection(connection);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.CONNECTION_GROUP: {
			ConnectionGroup connectionGroup = (ConnectionGroup) theEObject;
			Object result = caseConnectionGroup(connectionGroup);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.CONNECTIONS: {
			Connections connections = (Connections) theEObject;
			Object result = caseConnections(connections);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.CONST: {
			Const const_ = (Const) theEObject;
			Object result = caseConst(const_);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.EXPRESSION: {
			Expression expression = (Expression) theEObject;
			Object result = caseExpression(expression);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.EXTENDS: {
			Extends extends_ = (Extends) theEObject;
			Object result = caseExtends(extends_);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.FILES: {
			Files files = (Files) theEObject;
			Object result = caseFiles(files);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.FUNCTION: {
			Function function = (Function) theEObject;
			Object result = caseFunction(function);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.GATE: {
			Gate gate = (Gate) theEObject;
			Object result = caseGate(gate);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.GATE_GROUP: {
			GateGroup gateGroup = (GateGroup) theEObject;
			Object result = caseGateGroup(gateGroup);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.GATES: {
			Gates gates = (Gates) theEObject;
			Object result = caseGates(gates);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.IMPORT: {
			Import import_ = (Import) theEObject;
			Object result = caseImport(import_);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.INTERFACE_NAME: {
			InterfaceName interfaceName = (InterfaceName) theEObject;
			Object result = caseInterfaceName(interfaceName);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.KEY_VALUE: {
			KeyValue keyValue = (KeyValue) theEObject;
			Object result = caseKeyValue(keyValue);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.LOOP: {
			Loop loop = (Loop) theEObject;
			Object result = caseLoop(loop);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.MODULE_INTERFACE: {
			ModuleInterface moduleInterface = (ModuleInterface) theEObject;
			Object result = caseModuleInterface(moduleInterface);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.NED_FILE: {
			NedFile nedFile = (NedFile) theEObject;
			Object result = caseNedFile(nedFile);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.OPERATOR: {
			Operator operator = (Operator) theEObject;
			Object result = caseOperator(operator);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.PARAM: {
			Param param = (Param) theEObject;
			Object result = caseParam(param);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.PARAMETERS: {
			Parameters parameters = (Parameters) theEObject;
			Object result = caseParameters(parameters);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.PARAM_GROUP: {
			ParamGroup paramGroup = (ParamGroup) theEObject;
			Object result = caseParamGroup(paramGroup);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.PROPERTY: {
			Property property = (Property) theEObject;
			Object result = caseProperty(property);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.PROPERTYDEF: {
			Propertydef propertydef = (Propertydef) theEObject;
			Object result = casePropertydef(propertydef);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.REF: {
			Ref ref = (Ref) theEObject;
			Object result = caseRef(ref);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.SIMPLE_MODULE: {
			SimpleModule simpleModule = (SimpleModule) theEObject;
			Object result = caseSimpleModule(simpleModule);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.SUBMODULE: {
			Submodule submodule = (Submodule) theEObject;
			Object result = caseSubmodule(submodule);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.SUBMODULES: {
			Submodules submodules = (Submodules) theEObject;
			Object result = caseSubmodules(submodules);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.TYPES: {
			Types types = (Types) theEObject;
			Object result = caseTypes(types);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		case Ned2Package.WHITESPACE: {
			Whitespace whitespace = (Whitespace) theEObject;
			Object result = caseWhitespace(whitespace);
			if (result == null)
				result = defaultCase(theEObject);
			return result;
		}
		default:
			return defaultCase(theEObject);
		}
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Channel</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Channel</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseChannel(Channel object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Channel Interface</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Channel Interface</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseChannelInterface(ChannelInterface object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Compound Module</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Compound Module</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseCompoundModule(CompoundModule object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Condition</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Condition</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseCondition(Condition object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Connection</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Connection</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseConnection(Connection object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Connection Group</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Connection Group</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseConnectionGroup(ConnectionGroup object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Connections</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Connections</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseConnections(Connections object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Const</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Const</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseConst(Const object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Expression</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Expression</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseExpression(Expression object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Extends</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Extends</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseExtends(Extends object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Files</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Files</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseFiles(Files object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Function</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Function</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseFunction(Function object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Gate</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Gate</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseGate(Gate object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Gate Group</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Gate Group</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseGateGroup(GateGroup object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Gates</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Gates</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseGates(Gates object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Import</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Import</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseImport(Import object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Interface Name</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Interface Name</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseInterfaceName(InterfaceName object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Key Value</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Key Value</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseKeyValue(KeyValue object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Loop</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Loop</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseLoop(Loop object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Module Interface</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Module Interface</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseModuleInterface(ModuleInterface object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Ned File</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Ned File</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseNedFile(NedFile object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Operator</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Operator</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseOperator(Operator object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Param</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Param</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseParam(Param object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Parameters</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Parameters</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseParameters(Parameters object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Param Group</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Param Group</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseParamGroup(ParamGroup object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Property</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Property</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseProperty(Property object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Propertydef</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Propertydef</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object casePropertydef(Propertydef object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Ref</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Ref</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseRef(Ref object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Simple Module</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Simple Module</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseSimpleModule(SimpleModule object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Submodule</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Submodule</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseSubmodule(Submodule object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Submodules</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Submodules</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseSubmodules(Submodules object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Types</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Types</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseTypes(Types object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>Whitespace</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>Whitespace</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public Object caseWhitespace(Whitespace object) {
		return null;
	}

	/**
	 * Returns the result of interpretting the object as an instance of '<em>EObject</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch, but this is the last case anyway.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpretting the object as an instance of '<em>EObject</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject)
	 * @generated
	 */
	public Object defaultCase(EObject object) {
		return null;
	}

} //Ned2Switch

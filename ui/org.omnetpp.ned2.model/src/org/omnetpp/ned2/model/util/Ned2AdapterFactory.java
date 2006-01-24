/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned2.model.util;

import org.eclipse.emf.common.notify.Adapter;
import org.eclipse.emf.common.notify.Notifier;

import org.eclipse.emf.common.notify.impl.AdapterFactoryImpl;

import org.eclipse.emf.ecore.EObject;

import org.omnetpp.ned2.model.*;

import org.omnetpp.ned2.model.meta.Ned2Package;

/**
 * <!-- begin-user-doc -->
 * The <b>Adapter Factory</b> for the model.
 * It provides an adapter <code>createXXX</code> method for each class of the model.
 * <!-- end-user-doc -->
 * @see org.omnetpp.ned2.model.meta.Ned2Package
 * @generated
 */
public class Ned2AdapterFactory extends AdapterFactoryImpl {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String copyright = "";

	/**
	 * The cached model package.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected static Ned2Package modelPackage;

	/**
	 * Creates an instance of the adapter factory.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Ned2AdapterFactory() {
		if (modelPackage == null) {
			modelPackage = Ned2Package.eINSTANCE;
		}
	}

	/**
	 * Returns whether this factory is applicable for the type of the object.
	 * <!-- begin-user-doc -->
	 * This implementation returns <code>true</code> if the object is either the model's package or is an instance object of the model.
	 * <!-- end-user-doc -->
	 * @return whether this factory is applicable for the type of the object.
	 * @generated
	 */
	public boolean isFactoryForType(Object object) {
		if (object == modelPackage) {
			return true;
		}
		if (object instanceof EObject) {
			return ((EObject) object).eClass().getEPackage() == modelPackage;
		}
		return false;
	}

	/**
	 * The switch the delegates to the <code>createXXX</code> methods.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected Ned2Switch modelSwitch = new Ned2Switch() {
		public Object caseChannel(Channel object) {
			return createChannelAdapter();
		}

		public Object caseChannelInterface(ChannelInterface object) {
			return createChannelInterfaceAdapter();
		}

		public Object caseCompoundModule(CompoundModule object) {
			return createCompoundModuleAdapter();
		}

		public Object caseCondition(Condition object) {
			return createConditionAdapter();
		}

		public Object caseConnection(Connection object) {
			return createConnectionAdapter();
		}

		public Object caseConnectionGroup(ConnectionGroup object) {
			return createConnectionGroupAdapter();
		}

		public Object caseConnections(Connections object) {
			return createConnectionsAdapter();
		}

		public Object caseConst(Const object) {
			return createConstAdapter();
		}

		public Object caseExpression(Expression object) {
			return createExpressionAdapter();
		}

		public Object caseExtends(Extends object) {
			return createExtendsAdapter();
		}

		public Object caseFiles(Files object) {
			return createFilesAdapter();
		}

		public Object caseFunction(Function object) {
			return createFunctionAdapter();
		}

		public Object caseGate(Gate object) {
			return createGateAdapter();
		}

		public Object caseGateGroup(GateGroup object) {
			return createGateGroupAdapter();
		}

		public Object caseGates(Gates object) {
			return createGatesAdapter();
		}

		public Object caseImport(Import object) {
			return createImportAdapter();
		}

		public Object caseInterfaceName(InterfaceName object) {
			return createInterfaceNameAdapter();
		}

		public Object caseKeyValue(KeyValue object) {
			return createKeyValueAdapter();
		}

		public Object caseLoop(Loop object) {
			return createLoopAdapter();
		}

		public Object caseModuleInterface(ModuleInterface object) {
			return createModuleInterfaceAdapter();
		}

		public Object caseNedFile(NedFile object) {
			return createNedFileAdapter();
		}

		public Object caseOperator(Operator object) {
			return createOperatorAdapter();
		}

		public Object caseParam(Param object) {
			return createParamAdapter();
		}

		public Object caseParameters(Parameters object) {
			return createParametersAdapter();
		}

		public Object caseParamGroup(ParamGroup object) {
			return createParamGroupAdapter();
		}

		public Object caseProperty(Property object) {
			return createPropertyAdapter();
		}

		public Object casePropertydef(Propertydef object) {
			return createPropertydefAdapter();
		}

		public Object caseRef(Ref object) {
			return createRefAdapter();
		}

		public Object caseSimpleModule(SimpleModule object) {
			return createSimpleModuleAdapter();
		}

		public Object caseSubmodule(Submodule object) {
			return createSubmoduleAdapter();
		}

		public Object caseSubmodules(Submodules object) {
			return createSubmodulesAdapter();
		}

		public Object caseTypes(Types object) {
			return createTypesAdapter();
		}

		public Object caseWhitespace(Whitespace object) {
			return createWhitespaceAdapter();
		}

		public Object defaultCase(EObject object) {
			return createEObjectAdapter();
		}
	};

	/**
	 * Creates an adapter for the <code>target</code>.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param target the object to adapt.
	 * @return the adapter for the <code>target</code>.
	 * @generated
	 */
	public Adapter createAdapter(Notifier target) {
		return (Adapter) modelSwitch.doSwitch((EObject) target);
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.Channel <em>Channel</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.Channel
	 * @generated
	 */
	public Adapter createChannelAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.ChannelInterface <em>Channel Interface</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.ChannelInterface
	 * @generated
	 */
	public Adapter createChannelInterfaceAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.CompoundModule <em>Compound Module</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.CompoundModule
	 * @generated
	 */
	public Adapter createCompoundModuleAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.Condition <em>Condition</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.Condition
	 * @generated
	 */
	public Adapter createConditionAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.Connection <em>Connection</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.Connection
	 * @generated
	 */
	public Adapter createConnectionAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.ConnectionGroup <em>Connection Group</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.ConnectionGroup
	 * @generated
	 */
	public Adapter createConnectionGroupAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.Connections <em>Connections</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.Connections
	 * @generated
	 */
	public Adapter createConnectionsAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.Const <em>Const</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.Const
	 * @generated
	 */
	public Adapter createConstAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.Expression <em>Expression</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.Expression
	 * @generated
	 */
	public Adapter createExpressionAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.Extends <em>Extends</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.Extends
	 * @generated
	 */
	public Adapter createExtendsAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.Files <em>Files</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.Files
	 * @generated
	 */
	public Adapter createFilesAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.Function <em>Function</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.Function
	 * @generated
	 */
	public Adapter createFunctionAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.Gate <em>Gate</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.Gate
	 * @generated
	 */
	public Adapter createGateAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.GateGroup <em>Gate Group</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.GateGroup
	 * @generated
	 */
	public Adapter createGateGroupAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.Gates <em>Gates</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.Gates
	 * @generated
	 */
	public Adapter createGatesAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.Import <em>Import</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.Import
	 * @generated
	 */
	public Adapter createImportAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.InterfaceName <em>Interface Name</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.InterfaceName
	 * @generated
	 */
	public Adapter createInterfaceNameAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.KeyValue <em>Key Value</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.KeyValue
	 * @generated
	 */
	public Adapter createKeyValueAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.Loop <em>Loop</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.Loop
	 * @generated
	 */
	public Adapter createLoopAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.ModuleInterface <em>Module Interface</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.ModuleInterface
	 * @generated
	 */
	public Adapter createModuleInterfaceAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.NedFile <em>Ned File</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.NedFile
	 * @generated
	 */
	public Adapter createNedFileAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.Operator <em>Operator</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.Operator
	 * @generated
	 */
	public Adapter createOperatorAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.Param <em>Param</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.Param
	 * @generated
	 */
	public Adapter createParamAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.Parameters <em>Parameters</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.Parameters
	 * @generated
	 */
	public Adapter createParametersAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.ParamGroup <em>Param Group</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.ParamGroup
	 * @generated
	 */
	public Adapter createParamGroupAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.Property <em>Property</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.Property
	 * @generated
	 */
	public Adapter createPropertyAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.Propertydef <em>Propertydef</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.Propertydef
	 * @generated
	 */
	public Adapter createPropertydefAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.Ref <em>Ref</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.Ref
	 * @generated
	 */
	public Adapter createRefAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.SimpleModule <em>Simple Module</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.SimpleModule
	 * @generated
	 */
	public Adapter createSimpleModuleAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.Submodule <em>Submodule</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.Submodule
	 * @generated
	 */
	public Adapter createSubmoduleAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.Submodules <em>Submodules</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.Submodules
	 * @generated
	 */
	public Adapter createSubmodulesAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.Types <em>Types</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.Types
	 * @generated
	 */
	public Adapter createTypesAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.ned2.model.Whitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.ned2.model.Whitespace
	 * @generated
	 */
	public Adapter createWhitespaceAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for the default case.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @generated
	 */
	public Adapter createEObjectAdapter() {
		return null;
	}

} //Ned2AdapterFactory

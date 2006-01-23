/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.model.ned2.emf.util;

import org.eclipse.emf.common.notify.Adapter;
import org.eclipse.emf.common.notify.Notifier;

import org.eclipse.emf.common.notify.impl.AdapterFactoryImpl;

import org.eclipse.emf.ecore.EObject;

import org.omnetpp.model.ned2.emf.*;

/**
 * <!-- begin-user-doc -->
 * The <b>Adapter Factory</b> for the model.
 * It provides an adapter <code>createXXX</code> method for each class of the model.
 * <!-- end-user-doc -->
 * @see org.omnetpp.model.ned2.emf.Ned2Package
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
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.Channel <em>Channel</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.Channel
	 * @generated
	 */
	public Adapter createChannelAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.ChannelInterface <em>Channel Interface</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.ChannelInterface
	 * @generated
	 */
	public Adapter createChannelInterfaceAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.CompoundModule <em>Compound Module</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.CompoundModule
	 * @generated
	 */
	public Adapter createCompoundModuleAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.Condition <em>Condition</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.Condition
	 * @generated
	 */
	public Adapter createConditionAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.Connection <em>Connection</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.Connection
	 * @generated
	 */
	public Adapter createConnectionAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.ConnectionGroup <em>Connection Group</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.ConnectionGroup
	 * @generated
	 */
	public Adapter createConnectionGroupAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.Connections <em>Connections</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.Connections
	 * @generated
	 */
	public Adapter createConnectionsAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.Const <em>Const</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.Const
	 * @generated
	 */
	public Adapter createConstAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.Expression <em>Expression</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.Expression
	 * @generated
	 */
	public Adapter createExpressionAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.Extends <em>Extends</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.Extends
	 * @generated
	 */
	public Adapter createExtendsAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.Files <em>Files</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.Files
	 * @generated
	 */
	public Adapter createFilesAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.Function <em>Function</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.Function
	 * @generated
	 */
	public Adapter createFunctionAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.Gate <em>Gate</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.Gate
	 * @generated
	 */
	public Adapter createGateAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.GateGroup <em>Gate Group</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.GateGroup
	 * @generated
	 */
	public Adapter createGateGroupAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.Gates <em>Gates</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.Gates
	 * @generated
	 */
	public Adapter createGatesAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.Import <em>Import</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.Import
	 * @generated
	 */
	public Adapter createImportAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.InterfaceName <em>Interface Name</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.InterfaceName
	 * @generated
	 */
	public Adapter createInterfaceNameAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.KeyValue <em>Key Value</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.KeyValue
	 * @generated
	 */
	public Adapter createKeyValueAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.Loop <em>Loop</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.Loop
	 * @generated
	 */
	public Adapter createLoopAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.ModuleInterface <em>Module Interface</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.ModuleInterface
	 * @generated
	 */
	public Adapter createModuleInterfaceAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.NedFile <em>Ned File</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.NedFile
	 * @generated
	 */
	public Adapter createNedFileAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.Operator <em>Operator</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.Operator
	 * @generated
	 */
	public Adapter createOperatorAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.Param <em>Param</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.Param
	 * @generated
	 */
	public Adapter createParamAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.Parameters <em>Parameters</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.Parameters
	 * @generated
	 */
	public Adapter createParametersAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.ParamGroup <em>Param Group</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.ParamGroup
	 * @generated
	 */
	public Adapter createParamGroupAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.Property <em>Property</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.Property
	 * @generated
	 */
	public Adapter createPropertyAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.Propertydef <em>Propertydef</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.Propertydef
	 * @generated
	 */
	public Adapter createPropertydefAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.Ref <em>Ref</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.Ref
	 * @generated
	 */
	public Adapter createRefAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.SimpleModule <em>Simple Module</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.SimpleModule
	 * @generated
	 */
	public Adapter createSimpleModuleAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.Submodule <em>Submodule</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.Submodule
	 * @generated
	 */
	public Adapter createSubmoduleAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.Submodules <em>Submodules</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.Submodules
	 * @generated
	 */
	public Adapter createSubmodulesAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.Types <em>Types</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.Types
	 * @generated
	 */
	public Adapter createTypesAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link org.omnetpp.model.ned2.emf.Whitespace <em>Whitespace</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see org.omnetpp.model.ned2.emf.Whitespace
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

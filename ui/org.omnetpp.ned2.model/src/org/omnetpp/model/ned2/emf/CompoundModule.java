/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.model.ned2.emf;

import java.util.Collection;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Compound Module</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.model.ned2.emf.CompoundModule#getWhitespace <em>Whitespace</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.CompoundModule#getExtends <em>Extends</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.CompoundModule#getInterfaceName <em>Interface Name</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.CompoundModule#getParameters <em>Parameters</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.CompoundModule#getGates <em>Gates</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.CompoundModule#getTypes <em>Types</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.CompoundModule#getSubmodules <em>Submodules</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.CompoundModule#getConnections <em>Connections</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.CompoundModule#isIsNetwork <em>Is Network</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.CompoundModule#getName <em>Name</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.model.ned2.emf.Ned2Package#getCompoundModule()
 * @model extendedMetaData="name='compound-module' kind='elementOnly'"
 * @generated
 */
public class CompoundModule extends EObjectImpl {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String copyright = "";

	/**
	 * The cached value of the '{@link #getWhitespace() <em>Whitespace</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getWhitespace()
	 * @generated
	 * @ordered
	 */
	protected EList whitespace = null;

	/**
	 * The cached value of the '{@link #getExtends() <em>Extends</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getExtends()
	 * @generated
	 * @ordered
	 */
	protected Extends extends_ = null;

	/**
	 * The cached value of the '{@link #getInterfaceName() <em>Interface Name</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getInterfaceName()
	 * @generated
	 * @ordered
	 */
	protected EList interfaceName = null;

	/**
	 * The cached value of the '{@link #getParameters() <em>Parameters</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getParameters()
	 * @generated
	 * @ordered
	 */
	protected Parameters parameters = null;

	/**
	 * The cached value of the '{@link #getGates() <em>Gates</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getGates()
	 * @generated
	 * @ordered
	 */
	protected Gates gates = null;

	/**
	 * The cached value of the '{@link #getTypes() <em>Types</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getTypes()
	 * @generated
	 * @ordered
	 */
	protected Types types = null;

	/**
	 * The cached value of the '{@link #getSubmodules() <em>Submodules</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getSubmodules()
	 * @generated
	 * @ordered
	 */
	protected Submodules submodules = null;

	/**
	 * The cached value of the '{@link #getConnections() <em>Connections</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getConnections()
	 * @generated
	 * @ordered
	 */
	protected Connections connections = null;

	/**
	 * The default value of the '{@link #isIsNetwork() <em>Is Network</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isIsNetwork()
	 * @generated
	 * @ordered
	 */
	protected static final boolean IS_NETWORK_EDEFAULT = false;

	/**
	 * The cached value of the '{@link #isIsNetwork() <em>Is Network</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isIsNetwork()
	 * @generated
	 * @ordered
	 */
	protected boolean isNetwork = IS_NETWORK_EDEFAULT;

	/**
	 * This is true if the Is Network attribute has been set.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	protected boolean isNetworkESet = false;

	/**
	 * The default value of the '{@link #getName() <em>Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getName()
	 * @generated
	 * @ordered
	 */
	protected static final String NAME_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getName() <em>Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getName()
	 * @generated
	 * @ordered
	 */
	protected String name = NAME_EDEFAULT;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected CompoundModule() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EClass eStaticClass() {
		return Ned2Package.Literals.COMPOUND_MODULE;
	}

	/**
	 * Returns the value of the '<em><b>Whitespace</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.model.ned2.emf.Whitespace}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Whitespace</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Whitespace</em>' containment reference list.
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getCompoundModule_Whitespace()
	 * @model type="org.omnetpp.model.ned2.emf.Whitespace" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='whitespace'"
	 * @generated
	 */
	public EList getWhitespace() {
		if (whitespace == null) {
			whitespace = new EObjectContainmentEList(Whitespace.class, this,
					Ned2Package.COMPOUND_MODULE__WHITESPACE);
		}
		return whitespace;
	}

	/**
	 * Returns the value of the '<em><b>Extends</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Extends</em>' containment reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Extends</em>' containment reference.
	 * @see #setExtends(Extends)
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getCompoundModule_Extends()
	 * @model containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='extends'"
	 * @generated
	 */
	public Extends getExtends() {
		return extends_;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain basicSetExtends(Extends newExtends,
			NotificationChain msgs) {
		Extends oldExtends = extends_;
		extends_ = newExtends;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this,
					Notification.SET, Ned2Package.COMPOUND_MODULE__EXTENDS,
					oldExtends, newExtends);
			if (msgs == null)
				msgs = notification;
			else
				msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.CompoundModule#getExtends <em>Extends</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Extends</em>' containment reference.
	 * @see #getExtends()
	 * @generated
	 */
	public void setExtends(Extends newExtends) {
		if (newExtends != extends_) {
			NotificationChain msgs = null;
			if (extends_ != null)
				msgs = ((InternalEObject) extends_).eInverseRemove(this,
						EOPPOSITE_FEATURE_BASE
								- Ned2Package.COMPOUND_MODULE__EXTENDS, null,
						msgs);
			if (newExtends != null)
				msgs = ((InternalEObject) newExtends).eInverseAdd(this,
						EOPPOSITE_FEATURE_BASE
								- Ned2Package.COMPOUND_MODULE__EXTENDS, null,
						msgs);
			msgs = basicSetExtends(newExtends, msgs);
			if (msgs != null)
				msgs.dispatch();
		} else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.COMPOUND_MODULE__EXTENDS, newExtends,
					newExtends));
	}

	/**
	 * Returns the value of the '<em><b>Interface Name</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.model.ned2.emf.InterfaceName}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Interface Name</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Interface Name</em>' containment reference list.
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getCompoundModule_InterfaceName()
	 * @model type="org.omnetpp.model.ned2.emf.InterfaceName" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='interface-name'"
	 * @generated
	 */
	public EList getInterfaceName() {
		if (interfaceName == null) {
			interfaceName = new EObjectContainmentEList(InterfaceName.class,
					this, Ned2Package.COMPOUND_MODULE__INTERFACE_NAME);
		}
		return interfaceName;
	}

	/**
	 * Returns the value of the '<em><b>Parameters</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Parameters</em>' containment reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Parameters</em>' containment reference.
	 * @see #setParameters(Parameters)
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getCompoundModule_Parameters()
	 * @model containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='parameters'"
	 * @generated
	 */
	public Parameters getParameters() {
		return parameters;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain basicSetParameters(Parameters newParameters,
			NotificationChain msgs) {
		Parameters oldParameters = parameters;
		parameters = newParameters;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this,
					Notification.SET, Ned2Package.COMPOUND_MODULE__PARAMETERS,
					oldParameters, newParameters);
			if (msgs == null)
				msgs = notification;
			else
				msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.CompoundModule#getParameters <em>Parameters</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Parameters</em>' containment reference.
	 * @see #getParameters()
	 * @generated
	 */
	public void setParameters(Parameters newParameters) {
		if (newParameters != parameters) {
			NotificationChain msgs = null;
			if (parameters != null)
				msgs = ((InternalEObject) parameters).eInverseRemove(this,
						EOPPOSITE_FEATURE_BASE
								- Ned2Package.COMPOUND_MODULE__PARAMETERS,
						null, msgs);
			if (newParameters != null)
				msgs = ((InternalEObject) newParameters).eInverseAdd(this,
						EOPPOSITE_FEATURE_BASE
								- Ned2Package.COMPOUND_MODULE__PARAMETERS,
						null, msgs);
			msgs = basicSetParameters(newParameters, msgs);
			if (msgs != null)
				msgs.dispatch();
		} else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.COMPOUND_MODULE__PARAMETERS, newParameters,
					newParameters));
	}

	/**
	 * Returns the value of the '<em><b>Gates</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Gates</em>' containment reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Gates</em>' containment reference.
	 * @see #setGates(Gates)
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getCompoundModule_Gates()
	 * @model containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='gates'"
	 * @generated
	 */
	public Gates getGates() {
		return gates;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain basicSetGates(Gates newGates,
			NotificationChain msgs) {
		Gates oldGates = gates;
		gates = newGates;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this,
					Notification.SET, Ned2Package.COMPOUND_MODULE__GATES,
					oldGates, newGates);
			if (msgs == null)
				msgs = notification;
			else
				msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.CompoundModule#getGates <em>Gates</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Gates</em>' containment reference.
	 * @see #getGates()
	 * @generated
	 */
	public void setGates(Gates newGates) {
		if (newGates != gates) {
			NotificationChain msgs = null;
			if (gates != null)
				msgs = ((InternalEObject) gates).eInverseRemove(this,
						EOPPOSITE_FEATURE_BASE
								- Ned2Package.COMPOUND_MODULE__GATES, null,
						msgs);
			if (newGates != null)
				msgs = ((InternalEObject) newGates).eInverseAdd(this,
						EOPPOSITE_FEATURE_BASE
								- Ned2Package.COMPOUND_MODULE__GATES, null,
						msgs);
			msgs = basicSetGates(newGates, msgs);
			if (msgs != null)
				msgs.dispatch();
		} else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.COMPOUND_MODULE__GATES, newGates, newGates));
	}

	/**
	 * Returns the value of the '<em><b>Types</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Types</em>' containment reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Types</em>' containment reference.
	 * @see #setTypes(Types)
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getCompoundModule_Types()
	 * @model containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='types'"
	 * @generated
	 */
	public Types getTypes() {
		return types;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain basicSetTypes(Types newTypes,
			NotificationChain msgs) {
		Types oldTypes = types;
		types = newTypes;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this,
					Notification.SET, Ned2Package.COMPOUND_MODULE__TYPES,
					oldTypes, newTypes);
			if (msgs == null)
				msgs = notification;
			else
				msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.CompoundModule#getTypes <em>Types</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Types</em>' containment reference.
	 * @see #getTypes()
	 * @generated
	 */
	public void setTypes(Types newTypes) {
		if (newTypes != types) {
			NotificationChain msgs = null;
			if (types != null)
				msgs = ((InternalEObject) types).eInverseRemove(this,
						EOPPOSITE_FEATURE_BASE
								- Ned2Package.COMPOUND_MODULE__TYPES, null,
						msgs);
			if (newTypes != null)
				msgs = ((InternalEObject) newTypes).eInverseAdd(this,
						EOPPOSITE_FEATURE_BASE
								- Ned2Package.COMPOUND_MODULE__TYPES, null,
						msgs);
			msgs = basicSetTypes(newTypes, msgs);
			if (msgs != null)
				msgs.dispatch();
		} else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.COMPOUND_MODULE__TYPES, newTypes, newTypes));
	}

	/**
	 * Returns the value of the '<em><b>Submodules</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Submodules</em>' containment reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Submodules</em>' containment reference.
	 * @see #setSubmodules(Submodules)
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getCompoundModule_Submodules()
	 * @model containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='submodules'"
	 * @generated
	 */
	public Submodules getSubmodules() {
		return submodules;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain basicSetSubmodules(Submodules newSubmodules,
			NotificationChain msgs) {
		Submodules oldSubmodules = submodules;
		submodules = newSubmodules;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this,
					Notification.SET, Ned2Package.COMPOUND_MODULE__SUBMODULES,
					oldSubmodules, newSubmodules);
			if (msgs == null)
				msgs = notification;
			else
				msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.CompoundModule#getSubmodules <em>Submodules</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Submodules</em>' containment reference.
	 * @see #getSubmodules()
	 * @generated
	 */
	public void setSubmodules(Submodules newSubmodules) {
		if (newSubmodules != submodules) {
			NotificationChain msgs = null;
			if (submodules != null)
				msgs = ((InternalEObject) submodules).eInverseRemove(this,
						EOPPOSITE_FEATURE_BASE
								- Ned2Package.COMPOUND_MODULE__SUBMODULES,
						null, msgs);
			if (newSubmodules != null)
				msgs = ((InternalEObject) newSubmodules).eInverseAdd(this,
						EOPPOSITE_FEATURE_BASE
								- Ned2Package.COMPOUND_MODULE__SUBMODULES,
						null, msgs);
			msgs = basicSetSubmodules(newSubmodules, msgs);
			if (msgs != null)
				msgs.dispatch();
		} else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.COMPOUND_MODULE__SUBMODULES, newSubmodules,
					newSubmodules));
	}

	/**
	 * Returns the value of the '<em><b>Connections</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Connections</em>' containment reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Connections</em>' containment reference.
	 * @see #setConnections(Connections)
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getCompoundModule_Connections()
	 * @model containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='connections'"
	 * @generated
	 */
	public Connections getConnections() {
		return connections;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain basicSetConnections(Connections newConnections,
			NotificationChain msgs) {
		Connections oldConnections = connections;
		connections = newConnections;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this,
					Notification.SET, Ned2Package.COMPOUND_MODULE__CONNECTIONS,
					oldConnections, newConnections);
			if (msgs == null)
				msgs = notification;
			else
				msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.CompoundModule#getConnections <em>Connections</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Connections</em>' containment reference.
	 * @see #getConnections()
	 * @generated
	 */
	public void setConnections(Connections newConnections) {
		if (newConnections != connections) {
			NotificationChain msgs = null;
			if (connections != null)
				msgs = ((InternalEObject) connections).eInverseRemove(this,
						EOPPOSITE_FEATURE_BASE
								- Ned2Package.COMPOUND_MODULE__CONNECTIONS,
						null, msgs);
			if (newConnections != null)
				msgs = ((InternalEObject) newConnections).eInverseAdd(this,
						EOPPOSITE_FEATURE_BASE
								- Ned2Package.COMPOUND_MODULE__CONNECTIONS,
						null, msgs);
			msgs = basicSetConnections(newConnections, msgs);
			if (msgs != null)
				msgs.dispatch();
		} else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.COMPOUND_MODULE__CONNECTIONS, newConnections,
					newConnections));
	}

	/**
	 * Returns the value of the '<em><b>Is Network</b></em>' attribute.
	 * The default value is <code>"false"</code>.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Is Network</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Is Network</em>' attribute.
	 * @see #isSetIsNetwork()
	 * @see #unsetIsNetwork()
	 * @see #setIsNetwork(boolean)
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getCompoundModule_IsNetwork()
	 * @model default="false" unique="false" unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
	 *        extendedMetaData="kind='attribute' name='is-network'"
	 * @generated
	 */
	public boolean isIsNetwork() {
		return isNetwork;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.CompoundModule#isIsNetwork <em>Is Network</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Is Network</em>' attribute.
	 * @see #isSetIsNetwork()
	 * @see #unsetIsNetwork()
	 * @see #isIsNetwork()
	 * @generated
	 */
	public void setIsNetwork(boolean newIsNetwork) {
		boolean oldIsNetwork = isNetwork;
		isNetwork = newIsNetwork;
		boolean oldIsNetworkESet = isNetworkESet;
		isNetworkESet = true;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.COMPOUND_MODULE__IS_NETWORK, oldIsNetwork,
					isNetwork, !oldIsNetworkESet));
	}

	/**
	 * Unsets the value of the '{@link org.omnetpp.model.ned2.emf.CompoundModule#isIsNetwork <em>Is Network</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isSetIsNetwork()
	 * @see #isIsNetwork()
	 * @see #setIsNetwork(boolean)
	 * @generated
	 */
	public void unsetIsNetwork() {
		boolean oldIsNetwork = isNetwork;
		boolean oldIsNetworkESet = isNetworkESet;
		isNetwork = IS_NETWORK_EDEFAULT;
		isNetworkESet = false;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.UNSET,
					Ned2Package.COMPOUND_MODULE__IS_NETWORK, oldIsNetwork,
					IS_NETWORK_EDEFAULT, oldIsNetworkESet));
	}

	/**
	 * Returns whether the value of the '{@link org.omnetpp.model.ned2.emf.CompoundModule#isIsNetwork <em>Is Network</em>}' attribute is set.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return whether the value of the '<em>Is Network</em>' attribute is set.
	 * @see #unsetIsNetwork()
	 * @see #isIsNetwork()
	 * @see #setIsNetwork(boolean)
	 * @generated
	 */
	public boolean isSetIsNetwork() {
		return isNetworkESet;
	}

	/**
	 * Returns the value of the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Name</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Name</em>' attribute.
	 * @see #setName(String)
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getCompoundModule_Name()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN" required="true"
	 *        extendedMetaData="kind='attribute' name='name'"
	 * @generated
	 */
	public String getName() {
		return name;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.CompoundModule#getName <em>Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Name</em>' attribute.
	 * @see #getName()
	 * @generated
	 */
	public void setName(String newName) {
		String oldName = name;
		name = newName;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.COMPOUND_MODULE__NAME, oldName, name));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain eInverseRemove(InternalEObject otherEnd,
			int featureID, NotificationChain msgs) {
		switch (featureID) {
		case Ned2Package.COMPOUND_MODULE__WHITESPACE:
			return ((InternalEList) getWhitespace())
					.basicRemove(otherEnd, msgs);
		case Ned2Package.COMPOUND_MODULE__EXTENDS:
			return basicSetExtends(null, msgs);
		case Ned2Package.COMPOUND_MODULE__INTERFACE_NAME:
			return ((InternalEList) getInterfaceName()).basicRemove(otherEnd,
					msgs);
		case Ned2Package.COMPOUND_MODULE__PARAMETERS:
			return basicSetParameters(null, msgs);
		case Ned2Package.COMPOUND_MODULE__GATES:
			return basicSetGates(null, msgs);
		case Ned2Package.COMPOUND_MODULE__TYPES:
			return basicSetTypes(null, msgs);
		case Ned2Package.COMPOUND_MODULE__SUBMODULES:
			return basicSetSubmodules(null, msgs);
		case Ned2Package.COMPOUND_MODULE__CONNECTIONS:
			return basicSetConnections(null, msgs);
		}
		return super.eInverseRemove(otherEnd, featureID, msgs);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Object eGet(int featureID, boolean resolve, boolean coreType) {
		switch (featureID) {
		case Ned2Package.COMPOUND_MODULE__WHITESPACE:
			return getWhitespace();
		case Ned2Package.COMPOUND_MODULE__EXTENDS:
			return getExtends();
		case Ned2Package.COMPOUND_MODULE__INTERFACE_NAME:
			return getInterfaceName();
		case Ned2Package.COMPOUND_MODULE__PARAMETERS:
			return getParameters();
		case Ned2Package.COMPOUND_MODULE__GATES:
			return getGates();
		case Ned2Package.COMPOUND_MODULE__TYPES:
			return getTypes();
		case Ned2Package.COMPOUND_MODULE__SUBMODULES:
			return getSubmodules();
		case Ned2Package.COMPOUND_MODULE__CONNECTIONS:
			return getConnections();
		case Ned2Package.COMPOUND_MODULE__IS_NETWORK:
			return isIsNetwork() ? Boolean.TRUE : Boolean.FALSE;
		case Ned2Package.COMPOUND_MODULE__NAME:
			return getName();
		}
		return super.eGet(featureID, resolve, coreType);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void eSet(int featureID, Object newValue) {
		switch (featureID) {
		case Ned2Package.COMPOUND_MODULE__WHITESPACE:
			getWhitespace().clear();
			getWhitespace().addAll((Collection) newValue);
			return;
		case Ned2Package.COMPOUND_MODULE__EXTENDS:
			setExtends((Extends) newValue);
			return;
		case Ned2Package.COMPOUND_MODULE__INTERFACE_NAME:
			getInterfaceName().clear();
			getInterfaceName().addAll((Collection) newValue);
			return;
		case Ned2Package.COMPOUND_MODULE__PARAMETERS:
			setParameters((Parameters) newValue);
			return;
		case Ned2Package.COMPOUND_MODULE__GATES:
			setGates((Gates) newValue);
			return;
		case Ned2Package.COMPOUND_MODULE__TYPES:
			setTypes((Types) newValue);
			return;
		case Ned2Package.COMPOUND_MODULE__SUBMODULES:
			setSubmodules((Submodules) newValue);
			return;
		case Ned2Package.COMPOUND_MODULE__CONNECTIONS:
			setConnections((Connections) newValue);
			return;
		case Ned2Package.COMPOUND_MODULE__IS_NETWORK:
			setIsNetwork(((Boolean) newValue).booleanValue());
			return;
		case Ned2Package.COMPOUND_MODULE__NAME:
			setName((String) newValue);
			return;
		}
		super.eSet(featureID, newValue);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void eUnset(int featureID) {
		switch (featureID) {
		case Ned2Package.COMPOUND_MODULE__WHITESPACE:
			getWhitespace().clear();
			return;
		case Ned2Package.COMPOUND_MODULE__EXTENDS:
			setExtends((Extends) null);
			return;
		case Ned2Package.COMPOUND_MODULE__INTERFACE_NAME:
			getInterfaceName().clear();
			return;
		case Ned2Package.COMPOUND_MODULE__PARAMETERS:
			setParameters((Parameters) null);
			return;
		case Ned2Package.COMPOUND_MODULE__GATES:
			setGates((Gates) null);
			return;
		case Ned2Package.COMPOUND_MODULE__TYPES:
			setTypes((Types) null);
			return;
		case Ned2Package.COMPOUND_MODULE__SUBMODULES:
			setSubmodules((Submodules) null);
			return;
		case Ned2Package.COMPOUND_MODULE__CONNECTIONS:
			setConnections((Connections) null);
			return;
		case Ned2Package.COMPOUND_MODULE__IS_NETWORK:
			unsetIsNetwork();
			return;
		case Ned2Package.COMPOUND_MODULE__NAME:
			setName(NAME_EDEFAULT);
			return;
		}
		super.eUnset(featureID);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public boolean eIsSet(int featureID) {
		switch (featureID) {
		case Ned2Package.COMPOUND_MODULE__WHITESPACE:
			return whitespace != null && !whitespace.isEmpty();
		case Ned2Package.COMPOUND_MODULE__EXTENDS:
			return extends_ != null;
		case Ned2Package.COMPOUND_MODULE__INTERFACE_NAME:
			return interfaceName != null && !interfaceName.isEmpty();
		case Ned2Package.COMPOUND_MODULE__PARAMETERS:
			return parameters != null;
		case Ned2Package.COMPOUND_MODULE__GATES:
			return gates != null;
		case Ned2Package.COMPOUND_MODULE__TYPES:
			return types != null;
		case Ned2Package.COMPOUND_MODULE__SUBMODULES:
			return submodules != null;
		case Ned2Package.COMPOUND_MODULE__CONNECTIONS:
			return connections != null;
		case Ned2Package.COMPOUND_MODULE__IS_NETWORK:
			return isSetIsNetwork();
		case Ned2Package.COMPOUND_MODULE__NAME:
			return NAME_EDEFAULT == null ? name != null : !NAME_EDEFAULT
					.equals(name);
		}
		return super.eIsSet(featureID);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String toString() {
		if (eIsProxy())
			return super.toString();

		StringBuffer result = new StringBuffer(super.toString());
		result.append(" (isNetwork: ");
		if (isNetworkESet)
			result.append(isNetwork);
		else
			result.append("<unset>");
		result.append(", name: ");
		result.append(name);
		result.append(')');
		return result.toString();
	}

} // CompoundModule

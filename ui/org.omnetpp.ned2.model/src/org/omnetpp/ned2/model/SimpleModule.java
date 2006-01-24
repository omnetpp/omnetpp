/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned2.model;

import java.util.Collection;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.common.util.BasicEList;
import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

import org.omnetpp.ned2.model.meta.Ned2Package;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Simple Module</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.ned2.model.SimpleModule#getWhitespace <em>Whitespace</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.SimpleModule#getExtends <em>Extends</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.SimpleModule#getInterfaceName <em>Interface Name</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.SimpleModule#getParameters <em>Parameters</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.SimpleModule#getGates <em>Gates</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.SimpleModule#getName <em>Name</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSimpleModule()
 * @model extendedMetaData="name='simple-module' kind='elementOnly'"
 * @generated
 */
public class SimpleModule extends EObjectImpl {
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
	protected SimpleModule() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EClass eStaticClass() {
		return Ned2Package.Literals.SIMPLE_MODULE;
	}

	/**
	 * Returns the value of the '<em><b>Whitespace</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.ned2.model.Whitespace}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Whitespace</em>' containment reference list.
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSimpleModule_Whitespace()
	 * @model type="org.omnetpp.ned2.model.Whitespace" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='whitespace'"
	 * @generated
	 */
	public EList getWhitespace() {
		if (whitespace == null) {
			whitespace = new EObjectContainmentEList(Whitespace.class, this,
					Ned2Package.SIMPLE_MODULE__WHITESPACE);
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
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSimpleModule_Extends()
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
					Notification.SET, Ned2Package.SIMPLE_MODULE__EXTENDS,
					oldExtends, newExtends);
			if (msgs == null)
				msgs = notification;
			else
				msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.SimpleModule#getExtends <em>Extends</em>}' containment reference.
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
								- Ned2Package.SIMPLE_MODULE__EXTENDS, null,
						msgs);
			if (newExtends != null)
				msgs = ((InternalEObject) newExtends).eInverseAdd(this,
						EOPPOSITE_FEATURE_BASE
								- Ned2Package.SIMPLE_MODULE__EXTENDS, null,
						msgs);
			msgs = basicSetExtends(newExtends, msgs);
			if (msgs != null)
				msgs.dispatch();
		} else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.SIMPLE_MODULE__EXTENDS, newExtends, newExtends));
	}

	/**
	 * Returns the value of the '<em><b>Interface Name</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.ned2.model.InterfaceName}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Interface Name</em>' containment reference list.
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSimpleModule_InterfaceName()
	 * @model type="org.omnetpp.ned2.model.InterfaceName" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='interface-name'"
	 * @generated
	 */
	public EList getInterfaceName() {
		if (interfaceName == null) {
			interfaceName = new EObjectContainmentEList(InterfaceName.class,
					this, Ned2Package.SIMPLE_MODULE__INTERFACE_NAME);
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
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSimpleModule_Parameters()
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
					Notification.SET, Ned2Package.SIMPLE_MODULE__PARAMETERS,
					oldParameters, newParameters);
			if (msgs == null)
				msgs = notification;
			else
				msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.SimpleModule#getParameters <em>Parameters</em>}' containment reference.
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
								- Ned2Package.SIMPLE_MODULE__PARAMETERS, null,
						msgs);
			if (newParameters != null)
				msgs = ((InternalEObject) newParameters).eInverseAdd(this,
						EOPPOSITE_FEATURE_BASE
								- Ned2Package.SIMPLE_MODULE__PARAMETERS, null,
						msgs);
			msgs = basicSetParameters(newParameters, msgs);
			if (msgs != null)
				msgs.dispatch();
		} else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.SIMPLE_MODULE__PARAMETERS, newParameters,
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
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSimpleModule_Gates()
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
					Notification.SET, Ned2Package.SIMPLE_MODULE__GATES,
					oldGates, newGates);
			if (msgs == null)
				msgs = notification;
			else
				msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.SimpleModule#getGates <em>Gates</em>}' containment reference.
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
								- Ned2Package.SIMPLE_MODULE__GATES, null, msgs);
			if (newGates != null)
				msgs = ((InternalEObject) newGates).eInverseAdd(this,
						EOPPOSITE_FEATURE_BASE
								- Ned2Package.SIMPLE_MODULE__GATES, null, msgs);
			msgs = basicSetGates(newGates, msgs);
			if (msgs != null)
				msgs.dispatch();
		} else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.SIMPLE_MODULE__GATES, newGates, newGates));
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
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSimpleModule_Name()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN" required="true"
	 *        extendedMetaData="kind='attribute' name='name'"
	 * @generated
	 */
	public String getName() {
		return name;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.SimpleModule#getName <em>Name</em>}' attribute.
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
					Ned2Package.SIMPLE_MODULE__NAME, oldName, name));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain eInverseRemove(InternalEObject otherEnd,
			int featureID, NotificationChain msgs) {
		switch (featureID) {
		case Ned2Package.SIMPLE_MODULE__WHITESPACE:
			return ((InternalEList) getWhitespace())
					.basicRemove(otherEnd, msgs);
		case Ned2Package.SIMPLE_MODULE__EXTENDS:
			return basicSetExtends(null, msgs);
		case Ned2Package.SIMPLE_MODULE__INTERFACE_NAME:
			return ((InternalEList) getInterfaceName()).basicRemove(otherEnd,
					msgs);
		case Ned2Package.SIMPLE_MODULE__PARAMETERS:
			return basicSetParameters(null, msgs);
		case Ned2Package.SIMPLE_MODULE__GATES:
			return basicSetGates(null, msgs);
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
		case Ned2Package.SIMPLE_MODULE__WHITESPACE:
			return getWhitespace();
		case Ned2Package.SIMPLE_MODULE__EXTENDS:
			return getExtends();
		case Ned2Package.SIMPLE_MODULE__INTERFACE_NAME:
			return getInterfaceName();
		case Ned2Package.SIMPLE_MODULE__PARAMETERS:
			return getParameters();
		case Ned2Package.SIMPLE_MODULE__GATES:
			return getGates();
		case Ned2Package.SIMPLE_MODULE__NAME:
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
		case Ned2Package.SIMPLE_MODULE__WHITESPACE:
			getWhitespace().clear();
			getWhitespace().addAll((Collection) newValue);
			return;
		case Ned2Package.SIMPLE_MODULE__EXTENDS:
			setExtends((Extends) newValue);
			return;
		case Ned2Package.SIMPLE_MODULE__INTERFACE_NAME:
			getInterfaceName().clear();
			getInterfaceName().addAll((Collection) newValue);
			return;
		case Ned2Package.SIMPLE_MODULE__PARAMETERS:
			setParameters((Parameters) newValue);
			return;
		case Ned2Package.SIMPLE_MODULE__GATES:
			setGates((Gates) newValue);
			return;
		case Ned2Package.SIMPLE_MODULE__NAME:
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
		case Ned2Package.SIMPLE_MODULE__WHITESPACE:
			getWhitespace().clear();
			return;
		case Ned2Package.SIMPLE_MODULE__EXTENDS:
			setExtends((Extends) null);
			return;
		case Ned2Package.SIMPLE_MODULE__INTERFACE_NAME:
			getInterfaceName().clear();
			return;
		case Ned2Package.SIMPLE_MODULE__PARAMETERS:
			setParameters((Parameters) null);
			return;
		case Ned2Package.SIMPLE_MODULE__GATES:
			setGates((Gates) null);
			return;
		case Ned2Package.SIMPLE_MODULE__NAME:
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
		case Ned2Package.SIMPLE_MODULE__WHITESPACE:
			return whitespace != null && !whitespace.isEmpty();
		case Ned2Package.SIMPLE_MODULE__EXTENDS:
			return extends_ != null;
		case Ned2Package.SIMPLE_MODULE__INTERFACE_NAME:
			return interfaceName != null && !interfaceName.isEmpty();
		case Ned2Package.SIMPLE_MODULE__PARAMETERS:
			return parameters != null;
		case Ned2Package.SIMPLE_MODULE__GATES:
			return gates != null;
		case Ned2Package.SIMPLE_MODULE__NAME:
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
		result.append(" (name: ");
		result.append(name);
		result.append(')');
		return result.toString();
	}

} // SimpleModule

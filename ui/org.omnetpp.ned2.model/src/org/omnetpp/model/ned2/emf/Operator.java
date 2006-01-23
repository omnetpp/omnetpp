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

import org.eclipse.emf.ecore.util.BasicFeatureMap;
import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.FeatureMap;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Operator</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.model.ned2.emf.Operator#getWhitespace <em>Whitespace</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Operator#getGroup <em>Group</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Operator#getOperator <em>Operator</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Operator#getFunction <em>Function</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Operator#getRef <em>Ref</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Operator#getConst <em>Const</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Operator#getName <em>Name</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.model.ned2.emf.Ned2Package#getOperator()
 * @model extendedMetaData="name='operator' kind='elementOnly'"
 * @generated
 */
public class Operator extends EObjectImpl {
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
	 * The cached value of the '{@link #getGroup() <em>Group</em>}' attribute list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getGroup()
	 * @generated
	 * @ordered
	 */
	protected FeatureMap group = null;

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
	protected Operator() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EClass eStaticClass() {
		return Ned2Package.Literals.OPERATOR;
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
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getOperator_Whitespace()
	 * @model type="org.omnetpp.model.ned2.emf.Whitespace" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='whitespace'"
	 * @generated
	 */
	public EList getWhitespace() {
		if (whitespace == null) {
			whitespace = new EObjectContainmentEList(Whitespace.class, this,
					Ned2Package.OPERATOR__WHITESPACE);
		}
		return whitespace;
	}

	/**
	 * Returns the value of the '<em><b>Group</b></em>' attribute list.
	 * The list contents are of type {@link org.eclipse.emf.ecore.util.FeatureMap.Entry}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Group</em>' attribute list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Group</em>' attribute list.
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getOperator_Group()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.EFeatureMapEntry" many="true"
	 *        extendedMetaData="kind='group' name='group:1'"
	 * @generated
	 */
	public FeatureMap getGroup() {
		if (group == null) {
			group = new BasicFeatureMap(this, Ned2Package.OPERATOR__GROUP);
		}
		return group;
	}

	/**
	 * Returns the value of the '<em><b>Operator</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.model.ned2.emf.Operator}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Operator</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Operator</em>' containment reference list.
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getOperator_Operator()
	 * @model type="org.omnetpp.model.ned2.emf.Operator" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
	 *        extendedMetaData="kind='element' name='operator' group='#group:1'"
	 * @generated
	 */
	public EList getOperator() {
		return ((FeatureMap) getGroup())
				.list(Ned2Package.Literals.OPERATOR__OPERATOR);
	}

	/**
	 * Returns the value of the '<em><b>Function</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.model.ned2.emf.Function}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Function</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Function</em>' containment reference list.
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getOperator_Function()
	 * @model type="org.omnetpp.model.ned2.emf.Function" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
	 *        extendedMetaData="kind='element' name='function' group='#group:1'"
	 * @generated
	 */
	public EList getFunction() {
		return ((FeatureMap) getGroup())
				.list(Ned2Package.Literals.OPERATOR__FUNCTION);
	}

	/**
	 * Returns the value of the '<em><b>Ref</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.model.ned2.emf.Ref}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Ref</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Ref</em>' containment reference list.
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getOperator_Ref()
	 * @model type="org.omnetpp.model.ned2.emf.Ref" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
	 *        extendedMetaData="kind='element' name='ref' group='#group:1'"
	 * @generated
	 */
	public EList getRef() {
		return ((FeatureMap) getGroup())
				.list(Ned2Package.Literals.OPERATOR__REF);
	}

	/**
	 * Returns the value of the '<em><b>Const</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.model.ned2.emf.Const}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Const</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Const</em>' containment reference list.
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getOperator_Const()
	 * @model type="org.omnetpp.model.ned2.emf.Const" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
	 *        extendedMetaData="kind='element' name='const' group='#group:1'"
	 * @generated
	 */
	public EList getConst() {
		return ((FeatureMap) getGroup())
				.list(Ned2Package.Literals.OPERATOR__CONST);
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
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getOperator_Name()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
	 *        extendedMetaData="kind='attribute' name='name'"
	 * @generated
	 */
	public String getName() {
		return name;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.Operator#getName <em>Name</em>}' attribute.
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
					Ned2Package.OPERATOR__NAME, oldName, name));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain eInverseRemove(InternalEObject otherEnd,
			int featureID, NotificationChain msgs) {
		switch (featureID) {
		case Ned2Package.OPERATOR__WHITESPACE:
			return ((InternalEList) getWhitespace())
					.basicRemove(otherEnd, msgs);
		case Ned2Package.OPERATOR__GROUP:
			return ((InternalEList) getGroup()).basicRemove(otherEnd, msgs);
		case Ned2Package.OPERATOR__OPERATOR:
			return ((InternalEList) getOperator()).basicRemove(otherEnd, msgs);
		case Ned2Package.OPERATOR__FUNCTION:
			return ((InternalEList) getFunction()).basicRemove(otherEnd, msgs);
		case Ned2Package.OPERATOR__REF:
			return ((InternalEList) getRef()).basicRemove(otherEnd, msgs);
		case Ned2Package.OPERATOR__CONST:
			return ((InternalEList) getConst()).basicRemove(otherEnd, msgs);
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
		case Ned2Package.OPERATOR__WHITESPACE:
			return getWhitespace();
		case Ned2Package.OPERATOR__GROUP:
			if (coreType)
				return getGroup();
			return ((FeatureMap.Internal) getGroup()).getWrapper();
		case Ned2Package.OPERATOR__OPERATOR:
			return getOperator();
		case Ned2Package.OPERATOR__FUNCTION:
			return getFunction();
		case Ned2Package.OPERATOR__REF:
			return getRef();
		case Ned2Package.OPERATOR__CONST:
			return getConst();
		case Ned2Package.OPERATOR__NAME:
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
		case Ned2Package.OPERATOR__WHITESPACE:
			getWhitespace().clear();
			getWhitespace().addAll((Collection) newValue);
			return;
		case Ned2Package.OPERATOR__GROUP:
			((FeatureMap.Internal) getGroup()).set(newValue);
			return;
		case Ned2Package.OPERATOR__OPERATOR:
			getOperator().clear();
			getOperator().addAll((Collection) newValue);
			return;
		case Ned2Package.OPERATOR__FUNCTION:
			getFunction().clear();
			getFunction().addAll((Collection) newValue);
			return;
		case Ned2Package.OPERATOR__REF:
			getRef().clear();
			getRef().addAll((Collection) newValue);
			return;
		case Ned2Package.OPERATOR__CONST:
			getConst().clear();
			getConst().addAll((Collection) newValue);
			return;
		case Ned2Package.OPERATOR__NAME:
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
		case Ned2Package.OPERATOR__WHITESPACE:
			getWhitespace().clear();
			return;
		case Ned2Package.OPERATOR__GROUP:
			getGroup().clear();
			return;
		case Ned2Package.OPERATOR__OPERATOR:
			getOperator().clear();
			return;
		case Ned2Package.OPERATOR__FUNCTION:
			getFunction().clear();
			return;
		case Ned2Package.OPERATOR__REF:
			getRef().clear();
			return;
		case Ned2Package.OPERATOR__CONST:
			getConst().clear();
			return;
		case Ned2Package.OPERATOR__NAME:
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
		case Ned2Package.OPERATOR__WHITESPACE:
			return whitespace != null && !whitespace.isEmpty();
		case Ned2Package.OPERATOR__GROUP:
			return group != null && !group.isEmpty();
		case Ned2Package.OPERATOR__OPERATOR:
			return !getOperator().isEmpty();
		case Ned2Package.OPERATOR__FUNCTION:
			return !getFunction().isEmpty();
		case Ned2Package.OPERATOR__REF:
			return !getRef().isEmpty();
		case Ned2Package.OPERATOR__CONST:
			return !getConst().isEmpty();
		case Ned2Package.OPERATOR__NAME:
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
		result.append(" (group: ");
		result.append(group);
		result.append(", name: ");
		result.append(name);
		result.append(')');
		return result.toString();
	}

} // Operator

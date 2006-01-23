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
 * A representation of the model object '<em><b>Gate Group</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.model.ned2.emf.GateGroup#getWhitespace <em>Whitespace</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.GateGroup#getCondition <em>Condition</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.GateGroup#getGate <em>Gate</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.model.ned2.emf.Ned2Package#getGateGroup()
 * @model extendedMetaData="name='gate-group' kind='elementOnly'"
 * @generated
 */
public class GateGroup extends EObjectImpl {
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
	 * The cached value of the '{@link #getCondition() <em>Condition</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getCondition()
	 * @generated
	 * @ordered
	 */
	protected Condition condition = null;

	/**
	 * The cached value of the '{@link #getGate() <em>Gate</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getGate()
	 * @generated
	 * @ordered
	 */
	protected EList gate = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected GateGroup() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EClass eStaticClass() {
		return Ned2Package.Literals.GATE_GROUP;
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
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getGateGroup_Whitespace()
	 * @model type="org.omnetpp.model.ned2.emf.Whitespace" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='whitespace'"
	 * @generated
	 */
	public EList getWhitespace() {
		if (whitespace == null) {
			whitespace = new EObjectContainmentEList(Whitespace.class, this,
					Ned2Package.GATE_GROUP__WHITESPACE);
		}
		return whitespace;
	}

	/**
	 * Returns the value of the '<em><b>Condition</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Condition</em>' containment reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Condition</em>' containment reference.
	 * @see #setCondition(Condition)
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getGateGroup_Condition()
	 * @model containment="true" resolveProxies="false" required="true"
	 *        extendedMetaData="kind='element' name='condition'"
	 * @generated
	 */
	public Condition getCondition() {
		return condition;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain basicSetCondition(Condition newCondition,
			NotificationChain msgs) {
		Condition oldCondition = condition;
		condition = newCondition;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this,
					Notification.SET, Ned2Package.GATE_GROUP__CONDITION,
					oldCondition, newCondition);
			if (msgs == null)
				msgs = notification;
			else
				msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.GateGroup#getCondition <em>Condition</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Condition</em>' containment reference.
	 * @see #getCondition()
	 * @generated
	 */
	public void setCondition(Condition newCondition) {
		if (newCondition != condition) {
			NotificationChain msgs = null;
			if (condition != null)
				msgs = ((InternalEObject) condition)
						.eInverseRemove(this, EOPPOSITE_FEATURE_BASE
								- Ned2Package.GATE_GROUP__CONDITION, null, msgs);
			if (newCondition != null)
				msgs = ((InternalEObject) newCondition)
						.eInverseAdd(this, EOPPOSITE_FEATURE_BASE
								- Ned2Package.GATE_GROUP__CONDITION, null, msgs);
			msgs = basicSetCondition(newCondition, msgs);
			if (msgs != null)
				msgs.dispatch();
		} else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.GATE_GROUP__CONDITION, newCondition,
					newCondition));
	}

	/**
	 * Returns the value of the '<em><b>Gate</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.model.ned2.emf.Gate}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Gate</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Gate</em>' containment reference list.
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getGateGroup_Gate()
	 * @model type="org.omnetpp.model.ned2.emf.Gate" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='gate'"
	 * @generated
	 */
	public EList getGate() {
		if (gate == null) {
			gate = new EObjectContainmentEList(Gate.class, this,
					Ned2Package.GATE_GROUP__GATE);
		}
		return gate;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain eInverseRemove(InternalEObject otherEnd,
			int featureID, NotificationChain msgs) {
		switch (featureID) {
		case Ned2Package.GATE_GROUP__WHITESPACE:
			return ((InternalEList) getWhitespace())
					.basicRemove(otherEnd, msgs);
		case Ned2Package.GATE_GROUP__CONDITION:
			return basicSetCondition(null, msgs);
		case Ned2Package.GATE_GROUP__GATE:
			return ((InternalEList) getGate()).basicRemove(otherEnd, msgs);
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
		case Ned2Package.GATE_GROUP__WHITESPACE:
			return getWhitespace();
		case Ned2Package.GATE_GROUP__CONDITION:
			return getCondition();
		case Ned2Package.GATE_GROUP__GATE:
			return getGate();
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
		case Ned2Package.GATE_GROUP__WHITESPACE:
			getWhitespace().clear();
			getWhitespace().addAll((Collection) newValue);
			return;
		case Ned2Package.GATE_GROUP__CONDITION:
			setCondition((Condition) newValue);
			return;
		case Ned2Package.GATE_GROUP__GATE:
			getGate().clear();
			getGate().addAll((Collection) newValue);
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
		case Ned2Package.GATE_GROUP__WHITESPACE:
			getWhitespace().clear();
			return;
		case Ned2Package.GATE_GROUP__CONDITION:
			setCondition((Condition) null);
			return;
		case Ned2Package.GATE_GROUP__GATE:
			getGate().clear();
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
		case Ned2Package.GATE_GROUP__WHITESPACE:
			return whitespace != null && !whitespace.isEmpty();
		case Ned2Package.GATE_GROUP__CONDITION:
			return condition != null;
		case Ned2Package.GATE_GROUP__GATE:
			return gate != null && !gate.isEmpty();
		}
		return super.eIsSet(featureID);
	}

} // GateGroup

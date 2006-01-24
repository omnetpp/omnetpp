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
 * A representation of the model object '<em><b>Loop</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.ned2.model.Loop#getWhitespace <em>Whitespace</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Loop#getExpression <em>Expression</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Loop#getFromValue <em>From Value</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Loop#getParamName <em>Param Name</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Loop#getToValue <em>To Value</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.ned2.model.meta.Ned2Package#getLoop()
 * @model extendedMetaData="name='loop' kind='elementOnly'"
 * @generated
 */
public class Loop extends EObjectImpl {
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
	 * The cached value of the '{@link #getExpression() <em>Expression</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getExpression()
	 * @generated
	 * @ordered
	 */
	protected EList expression = null;

	/**
	 * The default value of the '{@link #getFromValue() <em>From Value</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getFromValue()
	 * @generated
	 * @ordered
	 */
	protected static final String FROM_VALUE_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getFromValue() <em>From Value</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getFromValue()
	 * @generated
	 * @ordered
	 */
	protected String fromValue = FROM_VALUE_EDEFAULT;

	/**
	 * The default value of the '{@link #getParamName() <em>Param Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getParamName()
	 * @generated
	 * @ordered
	 */
	protected static final String PARAM_NAME_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getParamName() <em>Param Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getParamName()
	 * @generated
	 * @ordered
	 */
	protected String paramName = PARAM_NAME_EDEFAULT;

	/**
	 * The default value of the '{@link #getToValue() <em>To Value</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getToValue()
	 * @generated
	 * @ordered
	 */
	protected static final String TO_VALUE_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getToValue() <em>To Value</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getToValue()
	 * @generated
	 * @ordered
	 */
	protected String toValue = TO_VALUE_EDEFAULT;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected Loop() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EClass eStaticClass() {
		return Ned2Package.Literals.LOOP;
	}

	/**
	 * Returns the value of the '<em><b>Whitespace</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.ned2.model.Whitespace}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Whitespace</em>' containment reference list.
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getLoop_Whitespace()
	 * @model type="org.omnetpp.ned2.model.Whitespace" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='whitespace'"
	 * @generated
	 */
	public EList getWhitespace() {
		if (whitespace == null) {
			whitespace = new EObjectContainmentEList(Whitespace.class, this,
					Ned2Package.LOOP__WHITESPACE);
		}
		return whitespace;
	}

	/**
	 * Returns the value of the '<em><b>Expression</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.ned2.model.Expression}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Expression</em>' containment reference list.
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getLoop_Expression()
	 * @model type="org.omnetpp.ned2.model.Expression" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='expression'"
	 * @generated
	 */
	public EList getExpression() {
		if (expression == null) {
			expression = new EObjectContainmentEList(Expression.class, this,
					Ned2Package.LOOP__EXPRESSION);
		}
		return expression;
	}

	/**
	 * Returns the value of the '<em><b>From Value</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>From Value</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>From Value</em>' attribute.
	 * @see #setFromValue(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getLoop_FromValue()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
	 *        extendedMetaData="kind='attribute' name='from-value'"
	 * @generated
	 */
	public String getFromValue() {
		return fromValue;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Loop#getFromValue <em>From Value</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>From Value</em>' attribute.
	 * @see #getFromValue()
	 * @generated
	 */
	public void setFromValue(String newFromValue) {
		String oldFromValue = fromValue;
		fromValue = newFromValue;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.LOOP__FROM_VALUE, oldFromValue, fromValue));
	}

	/**
	 * Returns the value of the '<em><b>Param Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Param Name</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Param Name</em>' attribute.
	 * @see #setParamName(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getLoop_ParamName()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN" required="true"
	 *        extendedMetaData="kind='attribute' name='param-name'"
	 * @generated
	 */
	public String getParamName() {
		return paramName;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Loop#getParamName <em>Param Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Param Name</em>' attribute.
	 * @see #getParamName()
	 * @generated
	 */
	public void setParamName(String newParamName) {
		String oldParamName = paramName;
		paramName = newParamName;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.LOOP__PARAM_NAME, oldParamName, paramName));
	}

	/**
	 * Returns the value of the '<em><b>To Value</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>To Value</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>To Value</em>' attribute.
	 * @see #setToValue(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getLoop_ToValue()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
	 *        extendedMetaData="kind='attribute' name='to-value'"
	 * @generated
	 */
	public String getToValue() {
		return toValue;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Loop#getToValue <em>To Value</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>To Value</em>' attribute.
	 * @see #getToValue()
	 * @generated
	 */
	public void setToValue(String newToValue) {
		String oldToValue = toValue;
		toValue = newToValue;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.LOOP__TO_VALUE, oldToValue, toValue));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain eInverseRemove(InternalEObject otherEnd,
			int featureID, NotificationChain msgs) {
		switch (featureID) {
		case Ned2Package.LOOP__WHITESPACE:
			return ((InternalEList) getWhitespace())
					.basicRemove(otherEnd, msgs);
		case Ned2Package.LOOP__EXPRESSION:
			return ((InternalEList) getExpression())
					.basicRemove(otherEnd, msgs);
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
		case Ned2Package.LOOP__WHITESPACE:
			return getWhitespace();
		case Ned2Package.LOOP__EXPRESSION:
			return getExpression();
		case Ned2Package.LOOP__FROM_VALUE:
			return getFromValue();
		case Ned2Package.LOOP__PARAM_NAME:
			return getParamName();
		case Ned2Package.LOOP__TO_VALUE:
			return getToValue();
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
		case Ned2Package.LOOP__WHITESPACE:
			getWhitespace().clear();
			getWhitespace().addAll((Collection) newValue);
			return;
		case Ned2Package.LOOP__EXPRESSION:
			getExpression().clear();
			getExpression().addAll((Collection) newValue);
			return;
		case Ned2Package.LOOP__FROM_VALUE:
			setFromValue((String) newValue);
			return;
		case Ned2Package.LOOP__PARAM_NAME:
			setParamName((String) newValue);
			return;
		case Ned2Package.LOOP__TO_VALUE:
			setToValue((String) newValue);
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
		case Ned2Package.LOOP__WHITESPACE:
			getWhitespace().clear();
			return;
		case Ned2Package.LOOP__EXPRESSION:
			getExpression().clear();
			return;
		case Ned2Package.LOOP__FROM_VALUE:
			setFromValue(FROM_VALUE_EDEFAULT);
			return;
		case Ned2Package.LOOP__PARAM_NAME:
			setParamName(PARAM_NAME_EDEFAULT);
			return;
		case Ned2Package.LOOP__TO_VALUE:
			setToValue(TO_VALUE_EDEFAULT);
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
		case Ned2Package.LOOP__WHITESPACE:
			return whitespace != null && !whitespace.isEmpty();
		case Ned2Package.LOOP__EXPRESSION:
			return expression != null && !expression.isEmpty();
		case Ned2Package.LOOP__FROM_VALUE:
			return FROM_VALUE_EDEFAULT == null ? fromValue != null
					: !FROM_VALUE_EDEFAULT.equals(fromValue);
		case Ned2Package.LOOP__PARAM_NAME:
			return PARAM_NAME_EDEFAULT == null ? paramName != null
					: !PARAM_NAME_EDEFAULT.equals(paramName);
		case Ned2Package.LOOP__TO_VALUE:
			return TO_VALUE_EDEFAULT == null ? toValue != null
					: !TO_VALUE_EDEFAULT.equals(toValue);
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
		result.append(" (fromValue: ");
		result.append(fromValue);
		result.append(", paramName: ");
		result.append(paramName);
		result.append(", toValue: ");
		result.append(toValue);
		result.append(')');
		return result.toString();
	}

} // Loop

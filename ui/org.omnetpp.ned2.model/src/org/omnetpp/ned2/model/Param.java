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
 * A representation of the model object '<em><b>Param</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.ned2.model.Param#getWhitespace <em>Whitespace</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Param#getExpression <em>Expression</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Param#getProperty <em>Property</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Param#isIsFunction <em>Is Function</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Param#getName <em>Name</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Param#getQualifier <em>Qualifier</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Param#getType <em>Type</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Param#getValue <em>Value</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.ned2.model.meta.Ned2Package#getParam()
 * @model extendedMetaData="name='param' kind='elementOnly'"
 * @generated
 */
public class Param extends EObjectImpl {
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
	 * The cached value of the '{@link #getExpression() <em>Expression</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getExpression()
	 * @generated
	 * @ordered
	 */
	protected Expression expression = null;

	/**
	 * The cached value of the '{@link #getProperty() <em>Property</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getProperty()
	 * @generated
	 * @ordered
	 */
	protected EList property = null;

	/**
	 * The default value of the '{@link #isIsFunction() <em>Is Function</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isIsFunction()
	 * @generated
	 * @ordered
	 */
	protected static final boolean IS_FUNCTION_EDEFAULT = false;

	/**
	 * The cached value of the '{@link #isIsFunction() <em>Is Function</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isIsFunction()
	 * @generated
	 * @ordered
	 */
	protected boolean isFunction = IS_FUNCTION_EDEFAULT;

	/**
	 * This is true if the Is Function attribute has been set.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	protected boolean isFunctionESet = false;

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
	 * The default value of the '{@link #getQualifier() <em>Qualifier</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getQualifier()
	 * @generated
	 * @ordered
	 */
	protected static final String QUALIFIER_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getQualifier() <em>Qualifier</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getQualifier()
	 * @generated
	 * @ordered
	 */
	protected String qualifier = QUALIFIER_EDEFAULT;

	/**
	 * The default value of the '{@link #getType() <em>Type</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getType()
	 * @generated
	 * @ordered
	 */
	protected static final String TYPE_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getType() <em>Type</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getType()
	 * @generated
	 * @ordered
	 */
	protected String type = TYPE_EDEFAULT;

	/**
	 * The default value of the '{@link #getValue() <em>Value</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getValue()
	 * @generated
	 * @ordered
	 */
	protected static final String VALUE_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getValue() <em>Value</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getValue()
	 * @generated
	 * @ordered
	 */
	protected String value = VALUE_EDEFAULT;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected Param() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EClass eStaticClass() {
		return Ned2Package.Literals.PARAM;
	}

	/**
	 * Returns the value of the '<em><b>Whitespace</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.ned2.model.Whitespace}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Whitespace</em>' containment reference list.
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getParam_Whitespace()
	 * @model type="org.omnetpp.ned2.model.Whitespace" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='whitespace'"
	 * @generated
	 */
	public EList getWhitespace() {
		if (whitespace == null) {
			whitespace = new EObjectContainmentEList(Whitespace.class, this,
					Ned2Package.PARAM__WHITESPACE);
		}
		return whitespace;
	}

	/**
	 * Returns the value of the '<em><b>Expression</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Expression</em>' containment reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Expression</em>' containment reference.
	 * @see #setExpression(Expression)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getParam_Expression()
	 * @model containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='expression'"
	 * @generated
	 */
	public Expression getExpression() {
		return expression;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain basicSetExpression(Expression newExpression,
			NotificationChain msgs) {
		Expression oldExpression = expression;
		expression = newExpression;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this,
					Notification.SET, Ned2Package.PARAM__EXPRESSION,
					oldExpression, newExpression);
			if (msgs == null)
				msgs = notification;
			else
				msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Param#getExpression <em>Expression</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Expression</em>' containment reference.
	 * @see #getExpression()
	 * @generated
	 */
	public void setExpression(Expression newExpression) {
		if (newExpression != expression) {
			NotificationChain msgs = null;
			if (expression != null)
				msgs = ((InternalEObject) expression).eInverseRemove(this,
						EOPPOSITE_FEATURE_BASE - Ned2Package.PARAM__EXPRESSION,
						null, msgs);
			if (newExpression != null)
				msgs = ((InternalEObject) newExpression).eInverseAdd(this,
						EOPPOSITE_FEATURE_BASE - Ned2Package.PARAM__EXPRESSION,
						null, msgs);
			msgs = basicSetExpression(newExpression, msgs);
			if (msgs != null)
				msgs.dispatch();
		} else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.PARAM__EXPRESSION, newExpression, newExpression));
	}

	/**
	 * Returns the value of the '<em><b>Property</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.ned2.model.Property}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Property</em>' containment reference list.
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getParam_Property()
	 * @model type="org.omnetpp.ned2.model.Property" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='property'"
	 * @generated
	 */
	public EList getProperty() {
		if (property == null) {
			property = new EObjectContainmentEList(Property.class, this,
					Ned2Package.PARAM__PROPERTY);
		}
		return property;
	}

	/**
	 * Returns the value of the '<em><b>Is Function</b></em>' attribute.
	 * The default value is <code>"false"</code>.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Is Function</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Is Function</em>' attribute.
	 * @see #isSetIsFunction()
	 * @see #unsetIsFunction()
	 * @see #setIsFunction(boolean)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getParam_IsFunction()
	 * @model default="false" unique="false" unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
	 *        extendedMetaData="kind='attribute' name='is-function'"
	 * @generated
	 */
	public boolean isIsFunction() {
		return isFunction;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Param#isIsFunction <em>Is Function</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Is Function</em>' attribute.
	 * @see #isSetIsFunction()
	 * @see #unsetIsFunction()
	 * @see #isIsFunction()
	 * @generated
	 */
	public void setIsFunction(boolean newIsFunction) {
		boolean oldIsFunction = isFunction;
		isFunction = newIsFunction;
		boolean oldIsFunctionESet = isFunctionESet;
		isFunctionESet = true;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.PARAM__IS_FUNCTION, oldIsFunction, isFunction,
					!oldIsFunctionESet));
	}

	/**
	 * Unsets the value of the '{@link org.omnetpp.ned2.model.Param#isIsFunction <em>Is Function</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isSetIsFunction()
	 * @see #isIsFunction()
	 * @see #setIsFunction(boolean)
	 * @generated
	 */
	public void unsetIsFunction() {
		boolean oldIsFunction = isFunction;
		boolean oldIsFunctionESet = isFunctionESet;
		isFunction = IS_FUNCTION_EDEFAULT;
		isFunctionESet = false;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.UNSET,
					Ned2Package.PARAM__IS_FUNCTION, oldIsFunction,
					IS_FUNCTION_EDEFAULT, oldIsFunctionESet));
	}

	/**
	 * Returns whether the value of the '{@link org.omnetpp.ned2.model.Param#isIsFunction <em>Is Function</em>}' attribute is set.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return whether the value of the '<em>Is Function</em>' attribute is set.
	 * @see #unsetIsFunction()
	 * @see #isIsFunction()
	 * @see #setIsFunction(boolean)
	 * @generated
	 */
	public boolean isSetIsFunction() {
		return isFunctionESet;
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
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getParam_Name()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN" required="true"
	 *        extendedMetaData="kind='attribute' name='name'"
	 * @generated
	 */
	public String getName() {
		return name;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Param#getName <em>Name</em>}' attribute.
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
					Ned2Package.PARAM__NAME, oldName, name));
	}

	/**
	 * Returns the value of the '<em><b>Qualifier</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Qualifier</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Qualifier</em>' attribute.
	 * @see #setQualifier(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getParam_Qualifier()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
	 *        extendedMetaData="kind='attribute' name='qualifier'"
	 * @generated
	 */
	public String getQualifier() {
		return qualifier;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Param#getQualifier <em>Qualifier</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Qualifier</em>' attribute.
	 * @see #getQualifier()
	 * @generated
	 */
	public void setQualifier(String newQualifier) {
		String oldQualifier = qualifier;
		qualifier = newQualifier;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.PARAM__QUALIFIER, oldQualifier, qualifier));
	}

	/**
	 * Returns the value of the '<em><b>Type</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Type</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Type</em>' attribute.
	 * @see #setType(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getParam_Type()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
	 *        extendedMetaData="kind='attribute' name='type'"
	 * @generated
	 */
	public String getType() {
		return type;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Param#getType <em>Type</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Type</em>' attribute.
	 * @see #getType()
	 * @generated
	 */
	public void setType(String newType) {
		String oldType = type;
		type = newType;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.PARAM__TYPE, oldType, type));
	}

	/**
	 * Returns the value of the '<em><b>Value</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Value</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Value</em>' attribute.
	 * @see #setValue(String)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getParam_Value()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
	 *        extendedMetaData="kind='attribute' name='value'"
	 * @generated
	 */
	public String getValue() {
		return value;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Param#getValue <em>Value</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Value</em>' attribute.
	 * @see #getValue()
	 * @generated
	 */
	public void setValue(String newValue) {
		String oldValue = value;
		value = newValue;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.PARAM__VALUE, oldValue, value));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain eInverseRemove(InternalEObject otherEnd,
			int featureID, NotificationChain msgs) {
		switch (featureID) {
		case Ned2Package.PARAM__WHITESPACE:
			return ((InternalEList) getWhitespace())
					.basicRemove(otherEnd, msgs);
		case Ned2Package.PARAM__EXPRESSION:
			return basicSetExpression(null, msgs);
		case Ned2Package.PARAM__PROPERTY:
			return ((InternalEList) getProperty()).basicRemove(otherEnd, msgs);
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
		case Ned2Package.PARAM__WHITESPACE:
			return getWhitespace();
		case Ned2Package.PARAM__EXPRESSION:
			return getExpression();
		case Ned2Package.PARAM__PROPERTY:
			return getProperty();
		case Ned2Package.PARAM__IS_FUNCTION:
			return isIsFunction() ? Boolean.TRUE : Boolean.FALSE;
		case Ned2Package.PARAM__NAME:
			return getName();
		case Ned2Package.PARAM__QUALIFIER:
			return getQualifier();
		case Ned2Package.PARAM__TYPE:
			return getType();
		case Ned2Package.PARAM__VALUE:
			return getValue();
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
		case Ned2Package.PARAM__WHITESPACE:
			getWhitespace().clear();
			getWhitespace().addAll((Collection) newValue);
			return;
		case Ned2Package.PARAM__EXPRESSION:
			setExpression((Expression) newValue);
			return;
		case Ned2Package.PARAM__PROPERTY:
			getProperty().clear();
			getProperty().addAll((Collection) newValue);
			return;
		case Ned2Package.PARAM__IS_FUNCTION:
			setIsFunction(((Boolean) newValue).booleanValue());
			return;
		case Ned2Package.PARAM__NAME:
			setName((String) newValue);
			return;
		case Ned2Package.PARAM__QUALIFIER:
			setQualifier((String) newValue);
			return;
		case Ned2Package.PARAM__TYPE:
			setType((String) newValue);
			return;
		case Ned2Package.PARAM__VALUE:
			setValue((String) newValue);
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
		case Ned2Package.PARAM__WHITESPACE:
			getWhitespace().clear();
			return;
		case Ned2Package.PARAM__EXPRESSION:
			setExpression((Expression) null);
			return;
		case Ned2Package.PARAM__PROPERTY:
			getProperty().clear();
			return;
		case Ned2Package.PARAM__IS_FUNCTION:
			unsetIsFunction();
			return;
		case Ned2Package.PARAM__NAME:
			setName(NAME_EDEFAULT);
			return;
		case Ned2Package.PARAM__QUALIFIER:
			setQualifier(QUALIFIER_EDEFAULT);
			return;
		case Ned2Package.PARAM__TYPE:
			setType(TYPE_EDEFAULT);
			return;
		case Ned2Package.PARAM__VALUE:
			setValue(VALUE_EDEFAULT);
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
		case Ned2Package.PARAM__WHITESPACE:
			return whitespace != null && !whitespace.isEmpty();
		case Ned2Package.PARAM__EXPRESSION:
			return expression != null;
		case Ned2Package.PARAM__PROPERTY:
			return property != null && !property.isEmpty();
		case Ned2Package.PARAM__IS_FUNCTION:
			return isSetIsFunction();
		case Ned2Package.PARAM__NAME:
			return NAME_EDEFAULT == null ? name != null : !NAME_EDEFAULT
					.equals(name);
		case Ned2Package.PARAM__QUALIFIER:
			return QUALIFIER_EDEFAULT == null ? qualifier != null
					: !QUALIFIER_EDEFAULT.equals(qualifier);
		case Ned2Package.PARAM__TYPE:
			return TYPE_EDEFAULT == null ? type != null : !TYPE_EDEFAULT
					.equals(type);
		case Ned2Package.PARAM__VALUE:
			return VALUE_EDEFAULT == null ? value != null : !VALUE_EDEFAULT
					.equals(value);
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
		result.append(" (isFunction: ");
		if (isFunctionESet)
			result.append(isFunction);
		else
			result.append("<unset>");
		result.append(", name: ");
		result.append(name);
		result.append(", qualifier: ");
		result.append(qualifier);
		result.append(", type: ");
		result.append(type);
		result.append(", value: ");
		result.append(value);
		result.append(')');
		return result.toString();
	}

} // Param

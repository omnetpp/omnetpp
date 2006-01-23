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
 * A representation of the model object '<em><b>Gate</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.model.ned2.emf.Gate#getWhitespace <em>Whitespace</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Gate#getExpression <em>Expression</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Gate#getProperty <em>Property</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Gate#isIsVector <em>Is Vector</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Gate#getName <em>Name</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Gate#getType <em>Type</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Gate#getVectorSize <em>Vector Size</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.model.ned2.emf.Ned2Package#getGate()
 * @model extendedMetaData="name='gate' kind='elementOnly'"
 * @generated
 */
public class Gate extends EObjectImpl {
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
	 * The default value of the '{@link #isIsVector() <em>Is Vector</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isIsVector()
	 * @generated
	 * @ordered
	 */
	protected static final boolean IS_VECTOR_EDEFAULT = false;

	/**
	 * The cached value of the '{@link #isIsVector() <em>Is Vector</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isIsVector()
	 * @generated
	 * @ordered
	 */
	protected boolean isVector = IS_VECTOR_EDEFAULT;

	/**
	 * This is true if the Is Vector attribute has been set.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	protected boolean isVectorESet = false;

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
	 * The default value of the '{@link #getVectorSize() <em>Vector Size</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getVectorSize()
	 * @generated
	 * @ordered
	 */
	protected static final String VECTOR_SIZE_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getVectorSize() <em>Vector Size</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getVectorSize()
	 * @generated
	 * @ordered
	 */
	protected String vectorSize = VECTOR_SIZE_EDEFAULT;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected Gate() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EClass eStaticClass() {
		return Ned2Package.Literals.GATE;
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
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getGate_Whitespace()
	 * @model type="org.omnetpp.model.ned2.emf.Whitespace" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='whitespace'"
	 * @generated
	 */
	public EList getWhitespace() {
		if (whitespace == null) {
			whitespace = new EObjectContainmentEList(Whitespace.class, this,
					Ned2Package.GATE__WHITESPACE);
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
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getGate_Expression()
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
					Notification.SET, Ned2Package.GATE__EXPRESSION,
					oldExpression, newExpression);
			if (msgs == null)
				msgs = notification;
			else
				msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.Gate#getExpression <em>Expression</em>}' containment reference.
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
						EOPPOSITE_FEATURE_BASE - Ned2Package.GATE__EXPRESSION,
						null, msgs);
			if (newExpression != null)
				msgs = ((InternalEObject) newExpression).eInverseAdd(this,
						EOPPOSITE_FEATURE_BASE - Ned2Package.GATE__EXPRESSION,
						null, msgs);
			msgs = basicSetExpression(newExpression, msgs);
			if (msgs != null)
				msgs.dispatch();
		} else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.GATE__EXPRESSION, newExpression, newExpression));
	}

	/**
	 * Returns the value of the '<em><b>Property</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.model.ned2.emf.Property}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Property</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Property</em>' containment reference list.
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getGate_Property()
	 * @model type="org.omnetpp.model.ned2.emf.Property" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='property'"
	 * @generated
	 */
	public EList getProperty() {
		if (property == null) {
			property = new EObjectContainmentEList(Property.class, this,
					Ned2Package.GATE__PROPERTY);
		}
		return property;
	}

	/**
	 * Returns the value of the '<em><b>Is Vector</b></em>' attribute.
	 * The default value is <code>"false"</code>.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Is Vector</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Is Vector</em>' attribute.
	 * @see #isSetIsVector()
	 * @see #unsetIsVector()
	 * @see #setIsVector(boolean)
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getGate_IsVector()
	 * @model default="false" unique="false" unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
	 *        extendedMetaData="kind='attribute' name='is-vector'"
	 * @generated
	 */
	public boolean isIsVector() {
		return isVector;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.Gate#isIsVector <em>Is Vector</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Is Vector</em>' attribute.
	 * @see #isSetIsVector()
	 * @see #unsetIsVector()
	 * @see #isIsVector()
	 * @generated
	 */
	public void setIsVector(boolean newIsVector) {
		boolean oldIsVector = isVector;
		isVector = newIsVector;
		boolean oldIsVectorESet = isVectorESet;
		isVectorESet = true;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.GATE__IS_VECTOR, oldIsVector, isVector,
					!oldIsVectorESet));
	}

	/**
	 * Unsets the value of the '{@link org.omnetpp.model.ned2.emf.Gate#isIsVector <em>Is Vector</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isSetIsVector()
	 * @see #isIsVector()
	 * @see #setIsVector(boolean)
	 * @generated
	 */
	public void unsetIsVector() {
		boolean oldIsVector = isVector;
		boolean oldIsVectorESet = isVectorESet;
		isVector = IS_VECTOR_EDEFAULT;
		isVectorESet = false;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.UNSET,
					Ned2Package.GATE__IS_VECTOR, oldIsVector,
					IS_VECTOR_EDEFAULT, oldIsVectorESet));
	}

	/**
	 * Returns whether the value of the '{@link org.omnetpp.model.ned2.emf.Gate#isIsVector <em>Is Vector</em>}' attribute is set.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return whether the value of the '<em>Is Vector</em>' attribute is set.
	 * @see #unsetIsVector()
	 * @see #isIsVector()
	 * @see #setIsVector(boolean)
	 * @generated
	 */
	public boolean isSetIsVector() {
		return isVectorESet;
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
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getGate_Name()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN" required="true"
	 *        extendedMetaData="kind='attribute' name='name'"
	 * @generated
	 */
	public String getName() {
		return name;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.Gate#getName <em>Name</em>}' attribute.
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
					Ned2Package.GATE__NAME, oldName, name));
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
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getGate_Type()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
	 *        extendedMetaData="kind='attribute' name='type'"
	 * @generated
	 */
	public String getType() {
		return type;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.Gate#getType <em>Type</em>}' attribute.
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
					Ned2Package.GATE__TYPE, oldType, type));
	}

	/**
	 * Returns the value of the '<em><b>Vector Size</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Vector Size</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Vector Size</em>' attribute.
	 * @see #setVectorSize(String)
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getGate_VectorSize()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
	 *        extendedMetaData="kind='attribute' name='vector-size'"
	 * @generated
	 */
	public String getVectorSize() {
		return vectorSize;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.Gate#getVectorSize <em>Vector Size</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Vector Size</em>' attribute.
	 * @see #getVectorSize()
	 * @generated
	 */
	public void setVectorSize(String newVectorSize) {
		String oldVectorSize = vectorSize;
		vectorSize = newVectorSize;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.GATE__VECTOR_SIZE, oldVectorSize, vectorSize));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain eInverseRemove(InternalEObject otherEnd,
			int featureID, NotificationChain msgs) {
		switch (featureID) {
		case Ned2Package.GATE__WHITESPACE:
			return ((InternalEList) getWhitespace())
					.basicRemove(otherEnd, msgs);
		case Ned2Package.GATE__EXPRESSION:
			return basicSetExpression(null, msgs);
		case Ned2Package.GATE__PROPERTY:
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
		case Ned2Package.GATE__WHITESPACE:
			return getWhitespace();
		case Ned2Package.GATE__EXPRESSION:
			return getExpression();
		case Ned2Package.GATE__PROPERTY:
			return getProperty();
		case Ned2Package.GATE__IS_VECTOR:
			return isIsVector() ? Boolean.TRUE : Boolean.FALSE;
		case Ned2Package.GATE__NAME:
			return getName();
		case Ned2Package.GATE__TYPE:
			return getType();
		case Ned2Package.GATE__VECTOR_SIZE:
			return getVectorSize();
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
		case Ned2Package.GATE__WHITESPACE:
			getWhitespace().clear();
			getWhitespace().addAll((Collection) newValue);
			return;
		case Ned2Package.GATE__EXPRESSION:
			setExpression((Expression) newValue);
			return;
		case Ned2Package.GATE__PROPERTY:
			getProperty().clear();
			getProperty().addAll((Collection) newValue);
			return;
		case Ned2Package.GATE__IS_VECTOR:
			setIsVector(((Boolean) newValue).booleanValue());
			return;
		case Ned2Package.GATE__NAME:
			setName((String) newValue);
			return;
		case Ned2Package.GATE__TYPE:
			setType((String) newValue);
			return;
		case Ned2Package.GATE__VECTOR_SIZE:
			setVectorSize((String) newValue);
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
		case Ned2Package.GATE__WHITESPACE:
			getWhitespace().clear();
			return;
		case Ned2Package.GATE__EXPRESSION:
			setExpression((Expression) null);
			return;
		case Ned2Package.GATE__PROPERTY:
			getProperty().clear();
			return;
		case Ned2Package.GATE__IS_VECTOR:
			unsetIsVector();
			return;
		case Ned2Package.GATE__NAME:
			setName(NAME_EDEFAULT);
			return;
		case Ned2Package.GATE__TYPE:
			setType(TYPE_EDEFAULT);
			return;
		case Ned2Package.GATE__VECTOR_SIZE:
			setVectorSize(VECTOR_SIZE_EDEFAULT);
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
		case Ned2Package.GATE__WHITESPACE:
			return whitespace != null && !whitespace.isEmpty();
		case Ned2Package.GATE__EXPRESSION:
			return expression != null;
		case Ned2Package.GATE__PROPERTY:
			return property != null && !property.isEmpty();
		case Ned2Package.GATE__IS_VECTOR:
			return isSetIsVector();
		case Ned2Package.GATE__NAME:
			return NAME_EDEFAULT == null ? name != null : !NAME_EDEFAULT
					.equals(name);
		case Ned2Package.GATE__TYPE:
			return TYPE_EDEFAULT == null ? type != null : !TYPE_EDEFAULT
					.equals(type);
		case Ned2Package.GATE__VECTOR_SIZE:
			return VECTOR_SIZE_EDEFAULT == null ? vectorSize != null
					: !VECTOR_SIZE_EDEFAULT.equals(vectorSize);
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
		result.append(" (isVector: ");
		if (isVectorESet)
			result.append(isVector);
		else
			result.append("<unset>");
		result.append(", name: ");
		result.append(name);
		result.append(", type: ");
		result.append(type);
		result.append(", vectorSize: ");
		result.append(vectorSize);
		result.append(')');
		return result.toString();
	}

} // Gate

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
 * A representation of the model object '<em><b>Property</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.model.ned2.emf.Property#getWhitespace <em>Whitespace</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Property#getKeyValue <em>Key Value</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Property#getProperty <em>Property</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Property#getName <em>Name</em>}</li>
 *   <li>{@link org.omnetpp.model.ned2.emf.Property#getQualifier <em>Qualifier</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.model.ned2.emf.Ned2Package#getProperty()
 * @model extendedMetaData="name='property' kind='elementOnly'"
 * @generated
 */
public class Property extends EObjectImpl {
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
	 * The cached value of the '{@link #getKeyValue() <em>Key Value</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getKeyValue()
	 * @generated
	 * @ordered
	 */
	protected EList keyValue = null;

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
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected Property() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EClass eStaticClass() {
		return Ned2Package.Literals.PROPERTY;
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
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getProperty_Whitespace()
	 * @model type="org.omnetpp.model.ned2.emf.Whitespace" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='whitespace'"
	 * @generated
	 */
	public EList getWhitespace() {
		if (whitespace == null) {
			whitespace = new EObjectContainmentEList(Whitespace.class, this,
					Ned2Package.PROPERTY__WHITESPACE);
		}
		return whitespace;
	}

	/**
	 * Returns the value of the '<em><b>Key Value</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.model.ned2.emf.KeyValue}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Key Value</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Key Value</em>' containment reference list.
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getProperty_KeyValue()
	 * @model type="org.omnetpp.model.ned2.emf.KeyValue" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='key-value'"
	 * @generated
	 */
	public EList getKeyValue() {
		if (keyValue == null) {
			keyValue = new EObjectContainmentEList(KeyValue.class, this,
					Ned2Package.PROPERTY__KEY_VALUE);
		}
		return keyValue;
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
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getProperty_Property()
	 * @model type="org.omnetpp.model.ned2.emf.Property" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='property'"
	 * @generated
	 */
	public EList getProperty() {
		if (property == null) {
			property = new EObjectContainmentEList(Property.class, this,
					Ned2Package.PROPERTY__PROPERTY);
		}
		return property;
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
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getProperty_Name()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN" required="true"
	 *        extendedMetaData="kind='attribute' name='name'"
	 * @generated
	 */
	public String getName() {
		return name;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.Property#getName <em>Name</em>}' attribute.
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
					Ned2Package.PROPERTY__NAME, oldName, name));
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
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getProperty_Qualifier()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
	 *        extendedMetaData="kind='attribute' name='qualifier'"
	 * @generated
	 */
	public String getQualifier() {
		return qualifier;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.model.ned2.emf.Property#getQualifier <em>Qualifier</em>}' attribute.
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
					Ned2Package.PROPERTY__QUALIFIER, oldQualifier, qualifier));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain eInverseRemove(InternalEObject otherEnd,
			int featureID, NotificationChain msgs) {
		switch (featureID) {
		case Ned2Package.PROPERTY__WHITESPACE:
			return ((InternalEList) getWhitespace())
					.basicRemove(otherEnd, msgs);
		case Ned2Package.PROPERTY__KEY_VALUE:
			return ((InternalEList) getKeyValue()).basicRemove(otherEnd, msgs);
		case Ned2Package.PROPERTY__PROPERTY:
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
		case Ned2Package.PROPERTY__WHITESPACE:
			return getWhitespace();
		case Ned2Package.PROPERTY__KEY_VALUE:
			return getKeyValue();
		case Ned2Package.PROPERTY__PROPERTY:
			return getProperty();
		case Ned2Package.PROPERTY__NAME:
			return getName();
		case Ned2Package.PROPERTY__QUALIFIER:
			return getQualifier();
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
		case Ned2Package.PROPERTY__WHITESPACE:
			getWhitespace().clear();
			getWhitespace().addAll((Collection) newValue);
			return;
		case Ned2Package.PROPERTY__KEY_VALUE:
			getKeyValue().clear();
			getKeyValue().addAll((Collection) newValue);
			return;
		case Ned2Package.PROPERTY__PROPERTY:
			getProperty().clear();
			getProperty().addAll((Collection) newValue);
			return;
		case Ned2Package.PROPERTY__NAME:
			setName((String) newValue);
			return;
		case Ned2Package.PROPERTY__QUALIFIER:
			setQualifier((String) newValue);
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
		case Ned2Package.PROPERTY__WHITESPACE:
			getWhitespace().clear();
			return;
		case Ned2Package.PROPERTY__KEY_VALUE:
			getKeyValue().clear();
			return;
		case Ned2Package.PROPERTY__PROPERTY:
			getProperty().clear();
			return;
		case Ned2Package.PROPERTY__NAME:
			setName(NAME_EDEFAULT);
			return;
		case Ned2Package.PROPERTY__QUALIFIER:
			setQualifier(QUALIFIER_EDEFAULT);
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
		case Ned2Package.PROPERTY__WHITESPACE:
			return whitespace != null && !whitespace.isEmpty();
		case Ned2Package.PROPERTY__KEY_VALUE:
			return keyValue != null && !keyValue.isEmpty();
		case Ned2Package.PROPERTY__PROPERTY:
			return property != null && !property.isEmpty();
		case Ned2Package.PROPERTY__NAME:
			return NAME_EDEFAULT == null ? name != null : !NAME_EDEFAULT
					.equals(name);
		case Ned2Package.PROPERTY__QUALIFIER:
			return QUALIFIER_EDEFAULT == null ? qualifier != null
					: !QUALIFIER_EDEFAULT.equals(qualifier);
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
		result.append(", qualifier: ");
		result.append(qualifier);
		result.append(')');
		return result.toString();
	}

} // Property

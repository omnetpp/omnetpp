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

import org.eclipse.emf.ecore.util.BasicFeatureMap;
import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.FeatureMap;
import org.eclipse.emf.ecore.util.InternalEList;

import org.omnetpp.ned2.model.meta.Ned2Package;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Parameters</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.ned2.model.Parameters#getWhitespace <em>Whitespace</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Parameters#getGroup <em>Group</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Parameters#getProperty <em>Property</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Parameters#getParam <em>Param</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Parameters#getParamGroup <em>Param Group</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Parameters#isIsImplicit <em>Is Implicit</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.ned2.model.meta.Ned2Package#getParameters()
 * @model extendedMetaData="name='parameters' kind='elementOnly'"
 * @generated
 */
public class Parameters extends EObjectImpl {
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
	 * The default value of the '{@link #isIsImplicit() <em>Is Implicit</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isIsImplicit()
	 * @generated
	 * @ordered
	 */
	protected static final boolean IS_IMPLICIT_EDEFAULT = true;

	/**
	 * The cached value of the '{@link #isIsImplicit() <em>Is Implicit</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isIsImplicit()
	 * @generated
	 * @ordered
	 */
	protected boolean isImplicit = IS_IMPLICIT_EDEFAULT;

	/**
	 * This is true if the Is Implicit attribute has been set.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	protected boolean isImplicitESet = false;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected Parameters() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EClass eStaticClass() {
		return Ned2Package.Literals.PARAMETERS;
	}

	/**
	 * Returns the value of the '<em><b>Whitespace</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.ned2.model.Whitespace}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Whitespace</em>' containment reference list.
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getParameters_Whitespace()
	 * @model type="org.omnetpp.ned2.model.Whitespace" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='whitespace'"
	 * @generated
	 */
	public EList getWhitespace() {
		if (whitespace == null) {
			whitespace = new EObjectContainmentEList(Whitespace.class, this,
					Ned2Package.PARAMETERS__WHITESPACE);
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
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getParameters_Group()
	 * @model unique="false" dataType="org.eclipse.emf.ecore.EFeatureMapEntry" many="true"
	 *        extendedMetaData="kind='group' name='group:1'"
	 * @generated
	 */
	public FeatureMap getGroup() {
		if (group == null) {
			group = new BasicFeatureMap(this, Ned2Package.PARAMETERS__GROUP);
		}
		return group;
	}

	/**
	 * Returns the value of the '<em><b>Property</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.ned2.model.Property}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Property</em>' containment reference list.
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getParameters_Property()
	 * @model type="org.omnetpp.ned2.model.Property" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
	 *        extendedMetaData="kind='element' name='property' group='#group:1'"
	 * @generated
	 */
	public EList getProperty() {
		return ((FeatureMap) getGroup())
				.list(Ned2Package.Literals.PARAMETERS__PROPERTY);
	}

	/**
	 * Returns the value of the '<em><b>Param</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.ned2.model.Param}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Param</em>' containment reference list.
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getParameters_Param()
	 * @model type="org.omnetpp.ned2.model.Param" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
	 *        extendedMetaData="kind='element' name='param' group='#group:1'"
	 * @generated
	 */
	public EList getParam() {
		return ((FeatureMap) getGroup())
				.list(Ned2Package.Literals.PARAMETERS__PARAM);
	}

	/**
	 * Returns the value of the '<em><b>Param Group</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.ned2.model.ParamGroup}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Param Group</em>' containment reference list.
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getParameters_ParamGroup()
	 * @model type="org.omnetpp.ned2.model.ParamGroup" containment="true" resolveProxies="false" transient="true" volatile="true" derived="true"
	 *        extendedMetaData="kind='element' name='param-group' group='#group:1'"
	 * @generated
	 */
	public EList getParamGroup() {
		return ((FeatureMap) getGroup())
				.list(Ned2Package.Literals.PARAMETERS__PARAM_GROUP);
	}

	/**
	 * Returns the value of the '<em><b>Is Implicit</b></em>' attribute.
	 * The default value is <code>"true"</code>.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Is Implicit</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Is Implicit</em>' attribute.
	 * @see #isSetIsImplicit()
	 * @see #unsetIsImplicit()
	 * @see #setIsImplicit(boolean)
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getParameters_IsImplicit()
	 * @model default="true" unique="false" unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
	 *        extendedMetaData="kind='attribute' name='is-implicit'"
	 * @generated
	 */
	public boolean isIsImplicit() {
		return isImplicit;
	}

	/**
	 * Sets the value of the '{@link org.omnetpp.ned2.model.Parameters#isIsImplicit <em>Is Implicit</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Is Implicit</em>' attribute.
	 * @see #isSetIsImplicit()
	 * @see #unsetIsImplicit()
	 * @see #isIsImplicit()
	 * @generated
	 */
	public void setIsImplicit(boolean newIsImplicit) {
		boolean oldIsImplicit = isImplicit;
		isImplicit = newIsImplicit;
		boolean oldIsImplicitESet = isImplicitESet;
		isImplicitESet = true;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET,
					Ned2Package.PARAMETERS__IS_IMPLICIT, oldIsImplicit,
					isImplicit, !oldIsImplicitESet));
	}

	/**
	 * Unsets the value of the '{@link org.omnetpp.ned2.model.Parameters#isIsImplicit <em>Is Implicit</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isSetIsImplicit()
	 * @see #isIsImplicit()
	 * @see #setIsImplicit(boolean)
	 * @generated
	 */
	public void unsetIsImplicit() {
		boolean oldIsImplicit = isImplicit;
		boolean oldIsImplicitESet = isImplicitESet;
		isImplicit = IS_IMPLICIT_EDEFAULT;
		isImplicitESet = false;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.UNSET,
					Ned2Package.PARAMETERS__IS_IMPLICIT, oldIsImplicit,
					IS_IMPLICIT_EDEFAULT, oldIsImplicitESet));
	}

	/**
	 * Returns whether the value of the '{@link org.omnetpp.ned2.model.Parameters#isIsImplicit <em>Is Implicit</em>}' attribute is set.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return whether the value of the '<em>Is Implicit</em>' attribute is set.
	 * @see #unsetIsImplicit()
	 * @see #isIsImplicit()
	 * @see #setIsImplicit(boolean)
	 * @generated
	 */
	public boolean isSetIsImplicit() {
		return isImplicitESet;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain eInverseRemove(InternalEObject otherEnd,
			int featureID, NotificationChain msgs) {
		switch (featureID) {
		case Ned2Package.PARAMETERS__WHITESPACE:
			return ((InternalEList) getWhitespace())
					.basicRemove(otherEnd, msgs);
		case Ned2Package.PARAMETERS__GROUP:
			return ((InternalEList) getGroup()).basicRemove(otherEnd, msgs);
		case Ned2Package.PARAMETERS__PROPERTY:
			return ((InternalEList) getProperty()).basicRemove(otherEnd, msgs);
		case Ned2Package.PARAMETERS__PARAM:
			return ((InternalEList) getParam()).basicRemove(otherEnd, msgs);
		case Ned2Package.PARAMETERS__PARAM_GROUP:
			return ((InternalEList) getParamGroup())
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
		case Ned2Package.PARAMETERS__WHITESPACE:
			return getWhitespace();
		case Ned2Package.PARAMETERS__GROUP:
			if (coreType)
				return getGroup();
			return ((FeatureMap.Internal) getGroup()).getWrapper();
		case Ned2Package.PARAMETERS__PROPERTY:
			return getProperty();
		case Ned2Package.PARAMETERS__PARAM:
			return getParam();
		case Ned2Package.PARAMETERS__PARAM_GROUP:
			return getParamGroup();
		case Ned2Package.PARAMETERS__IS_IMPLICIT:
			return isIsImplicit() ? Boolean.TRUE : Boolean.FALSE;
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
		case Ned2Package.PARAMETERS__WHITESPACE:
			getWhitespace().clear();
			getWhitespace().addAll((Collection) newValue);
			return;
		case Ned2Package.PARAMETERS__GROUP:
			((FeatureMap.Internal) getGroup()).set(newValue);
			return;
		case Ned2Package.PARAMETERS__PROPERTY:
			getProperty().clear();
			getProperty().addAll((Collection) newValue);
			return;
		case Ned2Package.PARAMETERS__PARAM:
			getParam().clear();
			getParam().addAll((Collection) newValue);
			return;
		case Ned2Package.PARAMETERS__PARAM_GROUP:
			getParamGroup().clear();
			getParamGroup().addAll((Collection) newValue);
			return;
		case Ned2Package.PARAMETERS__IS_IMPLICIT:
			setIsImplicit(((Boolean) newValue).booleanValue());
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
		case Ned2Package.PARAMETERS__WHITESPACE:
			getWhitespace().clear();
			return;
		case Ned2Package.PARAMETERS__GROUP:
			getGroup().clear();
			return;
		case Ned2Package.PARAMETERS__PROPERTY:
			getProperty().clear();
			return;
		case Ned2Package.PARAMETERS__PARAM:
			getParam().clear();
			return;
		case Ned2Package.PARAMETERS__PARAM_GROUP:
			getParamGroup().clear();
			return;
		case Ned2Package.PARAMETERS__IS_IMPLICIT:
			unsetIsImplicit();
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
		case Ned2Package.PARAMETERS__WHITESPACE:
			return whitespace != null && !whitespace.isEmpty();
		case Ned2Package.PARAMETERS__GROUP:
			return group != null && !group.isEmpty();
		case Ned2Package.PARAMETERS__PROPERTY:
			return !getProperty().isEmpty();
		case Ned2Package.PARAMETERS__PARAM:
			return !getParam().isEmpty();
		case Ned2Package.PARAMETERS__PARAM_GROUP:
			return !getParamGroup().isEmpty();
		case Ned2Package.PARAMETERS__IS_IMPLICIT:
			return isSetIsImplicit();
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
		result.append(", isImplicit: ");
		if (isImplicitESet)
			result.append(isImplicit);
		else
			result.append("<unset>");
		result.append(')');
		return result.toString();
	}

} // Parameters

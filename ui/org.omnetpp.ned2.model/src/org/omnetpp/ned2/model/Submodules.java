/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned2.model;

import java.util.Collection;

import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.common.util.BasicEList;
import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.EObjectImpl;

import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

import org.omnetpp.ned2.model.meta.Ned2Package;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Submodules</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.ned2.model.Submodules#getWhitespace <em>Whitespace</em>}</li>
 *   <li>{@link org.omnetpp.ned2.model.Submodules#getSubmodule <em>Submodule</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSubmodules()
 * @model extendedMetaData="name='submodules' kind='elementOnly'"
 * @generated
 */
public class Submodules extends EObjectImpl {
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
	 * The cached value of the '{@link #getSubmodule() <em>Submodule</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getSubmodule()
	 * @generated
	 * @ordered
	 */
	protected EList submodule = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected Submodules() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EClass eStaticClass() {
		return Ned2Package.Literals.SUBMODULES;
	}

	/**
	 * Returns the value of the '<em><b>Whitespace</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.ned2.model.Whitespace}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Whitespace</em>' containment reference list.
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSubmodules_Whitespace()
	 * @model type="org.omnetpp.ned2.model.Whitespace" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='whitespace'"
	 * @generated
	 */
	public EList getWhitespace() {
		if (whitespace == null) {
			whitespace = new EObjectContainmentEList(Whitespace.class, this,
					Ned2Package.SUBMODULES__WHITESPACE);
		}
		return whitespace;
	}

	/**
	 * Returns the value of the '<em><b>Submodule</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.ned2.model.Submodule}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Submodule</em>' containment reference list.
	 * @see org.omnetpp.ned2.model.meta.Ned2Package#getSubmodules_Submodule()
	 * @model type="org.omnetpp.ned2.model.Submodule" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='submodule'"
	 * @generated
	 */
	public EList getSubmodule() {
		if (submodule == null) {
			submodule = new EObjectContainmentEList(Submodule.class, this,
					Ned2Package.SUBMODULES__SUBMODULE);
		}
		return submodule;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain eInverseRemove(InternalEObject otherEnd,
			int featureID, NotificationChain msgs) {
		switch (featureID) {
		case Ned2Package.SUBMODULES__WHITESPACE:
			return ((InternalEList) getWhitespace())
					.basicRemove(otherEnd, msgs);
		case Ned2Package.SUBMODULES__SUBMODULE:
			return ((InternalEList) getSubmodule()).basicRemove(otherEnd, msgs);
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
		case Ned2Package.SUBMODULES__WHITESPACE:
			return getWhitespace();
		case Ned2Package.SUBMODULES__SUBMODULE:
			return getSubmodule();
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
		case Ned2Package.SUBMODULES__WHITESPACE:
			getWhitespace().clear();
			getWhitespace().addAll((Collection) newValue);
			return;
		case Ned2Package.SUBMODULES__SUBMODULE:
			getSubmodule().clear();
			getSubmodule().addAll((Collection) newValue);
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
		case Ned2Package.SUBMODULES__WHITESPACE:
			getWhitespace().clear();
			return;
		case Ned2Package.SUBMODULES__SUBMODULE:
			getSubmodule().clear();
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
		case Ned2Package.SUBMODULES__WHITESPACE:
			return whitespace != null && !whitespace.isEmpty();
		case Ned2Package.SUBMODULES__SUBMODULE:
			return submodule != null && !submodule.isEmpty();
		}
		return super.eIsSet(featureID);
	}

} // Submodules

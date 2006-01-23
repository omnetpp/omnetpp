/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.model.ned2.emf;

import java.util.Collection;

import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.EObjectImpl;

import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Files</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.model.ned2.emf.Files#getNedFile <em>Ned File</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.model.ned2.emf.Ned2Package#getFiles()
 * @model extendedMetaData="name='files' kind='elementOnly'"
 * @generated
 */
public class Files extends EObjectImpl {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String copyright = "";

	/**
	 * The cached value of the '{@link #getNedFile() <em>Ned File</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getNedFile()
	 * @generated
	 * @ordered
	 */
	protected EList nedFile = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected Files() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EClass eStaticClass() {
		return Ned2Package.Literals.FILES;
	}

	/**
	 * Returns the value of the '<em><b>Ned File</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.model.ned2.emf.NedFile}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Ned File</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Ned File</em>' containment reference list.
	 * @see org.omnetpp.model.ned2.emf.Ned2Package#getFiles_NedFile()
	 * @model type="org.omnetpp.model.ned2.emf.NedFile" containment="true" resolveProxies="false"
	 *        extendedMetaData="kind='element' name='ned-file'"
	 * @generated
	 */
	public EList getNedFile() {
		if (nedFile == null) {
			nedFile = new EObjectContainmentEList(NedFile.class, this,
					Ned2Package.FILES__NED_FILE);
		}
		return nedFile;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain eInverseRemove(InternalEObject otherEnd,
			int featureID, NotificationChain msgs) {
		switch (featureID) {
		case Ned2Package.FILES__NED_FILE:
			return ((InternalEList) getNedFile()).basicRemove(otherEnd, msgs);
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
		case Ned2Package.FILES__NED_FILE:
			return getNedFile();
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
		case Ned2Package.FILES__NED_FILE:
			getNedFile().clear();
			getNedFile().addAll((Collection) newValue);
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
		case Ned2Package.FILES__NED_FILE:
			getNedFile().clear();
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
		case Ned2Package.FILES__NED_FILE:
			return nedFile != null && !nedFile.isEmpty();
		}
		return super.eIsSet(featureID);
	}

} // Files

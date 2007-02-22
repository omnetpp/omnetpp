/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.scave.model.impl;

import org.eclipse.emf.common.notify.Notification;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model.SetOperation;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Set Operation</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.impl.SetOperationImpl#getSourceDataset <em>Source Dataset</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.SetOperationImpl#getFilterPattern <em>Filter Pattern</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public abstract class SetOperationImpl extends EObjectImpl implements SetOperation {
	/**
	 * The cached value of the '{@link #getSourceDataset() <em>Source Dataset</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getSourceDataset()
	 * @generated
	 * @ordered
	 */
	protected Dataset sourceDataset = null;

	/**
	 * The default value of the '{@link #getFilterPattern() <em>Filter Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getFilterPattern()
	 * @generated
	 * @ordered
	 */
	protected static final String FILTER_PATTERN_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getFilterPattern() <em>Filter Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getFilterPattern()
	 * @generated
	 * @ordered
	 */
	protected String filterPattern = FILTER_PATTERN_EDEFAULT;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected SetOperationImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EClass eStaticClass() {
		return ScaveModelPackage.Literals.SET_OPERATION;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Dataset getSourceDataset() {
		if (sourceDataset != null && sourceDataset.eIsProxy()) {
			InternalEObject oldSourceDataset = (InternalEObject)sourceDataset;
			sourceDataset = (Dataset)eResolveProxy(oldSourceDataset);
			if (sourceDataset != oldSourceDataset) {
				if (eNotificationRequired())
					eNotify(new ENotificationImpl(this, Notification.RESOLVE, ScaveModelPackage.SET_OPERATION__SOURCE_DATASET, oldSourceDataset, sourceDataset));
			}
		}
		return sourceDataset;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Dataset basicGetSourceDataset() {
		return sourceDataset;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setSourceDataset(Dataset newSourceDataset) {
		Dataset oldSourceDataset = sourceDataset;
		sourceDataset = newSourceDataset;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.SET_OPERATION__SOURCE_DATASET, oldSourceDataset, sourceDataset));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String getFilterPattern() {
		return filterPattern;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setFilterPattern(String newFilterPattern) {
		String oldFilterPattern = filterPattern;
		filterPattern = newFilterPattern;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.SET_OPERATION__FILTER_PATTERN, oldFilterPattern, filterPattern));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Object eGet(int featureID, boolean resolve, boolean coreType) {
		switch (featureID) {
			case ScaveModelPackage.SET_OPERATION__SOURCE_DATASET:
				if (resolve) return getSourceDataset();
				return basicGetSourceDataset();
			case ScaveModelPackage.SET_OPERATION__FILTER_PATTERN:
				return getFilterPattern();
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
			case ScaveModelPackage.SET_OPERATION__SOURCE_DATASET:
				setSourceDataset((Dataset)newValue);
				return;
			case ScaveModelPackage.SET_OPERATION__FILTER_PATTERN:
				setFilterPattern((String)newValue);
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
			case ScaveModelPackage.SET_OPERATION__SOURCE_DATASET:
				setSourceDataset((Dataset)null);
				return;
			case ScaveModelPackage.SET_OPERATION__FILTER_PATTERN:
				setFilterPattern(FILTER_PATTERN_EDEFAULT);
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
			case ScaveModelPackage.SET_OPERATION__SOURCE_DATASET:
				return sourceDataset != null;
			case ScaveModelPackage.SET_OPERATION__FILTER_PATTERN:
				return FILTER_PATTERN_EDEFAULT == null ? filterPattern != null : !FILTER_PATTERN_EDEFAULT.equals(filterPattern);
		}
		return super.eIsSet(featureID);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String toString() {
		if (eIsProxy()) return super.toString();

		StringBuffer result = new StringBuffer(super.toString());
		result.append(" (filterPattern: ");
		result.append(filterPattern);
		result.append(')');
		return result.toString();
	}

} //SetOperationImpl
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
import org.omnetpp.scave.model.ResultType;
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
 *   <li>{@link org.omnetpp.scave.model.impl.SetOperationImpl#getType <em>Type</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.SetOperationImpl#getCachedIDs <em>Cached IDs</em>}</li>
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
	protected Dataset sourceDataset;

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
	 * The default value of the '{@link #getType() <em>Type</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getType()
	 * @generated
	 * @ordered
	 */
	protected static final ResultType TYPE_EDEFAULT = ResultType.SCALAR_LITERAL;

	/**
	 * The cached value of the '{@link #getType() <em>Type</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getType()
	 * @generated
	 * @ordered
	 */
	protected ResultType type = TYPE_EDEFAULT;

	/**
	 * The default value of the '{@link #getCachedIDs() <em>Cached IDs</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getCachedIDs()
	 * @generated
	 * @ordered
	 */
	protected static final Object CACHED_IDS_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getCachedIDs() <em>Cached IDs</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getCachedIDs()
	 * @generated
	 * @ordered
	 */
	protected Object cachedIDs = CACHED_IDS_EDEFAULT;

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
	@Override
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
	public ResultType getType() {
		return type;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setType(ResultType newType) {
		ResultType oldType = type;
		type = newType == null ? TYPE_EDEFAULT : newType;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.SET_OPERATION__TYPE, oldType, type));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Object getCachedIDs() {
		return cachedIDs;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setCachedIDs(Object newCachedIDs) {
		Object oldCachedIDs = cachedIDs;
		cachedIDs = newCachedIDs;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.SET_OPERATION__CACHED_IDS, oldCachedIDs, cachedIDs));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public Object eGet(int featureID, boolean resolve, boolean coreType) {
		switch (featureID) {
			case ScaveModelPackage.SET_OPERATION__SOURCE_DATASET:
				if (resolve) return getSourceDataset();
				return basicGetSourceDataset();
			case ScaveModelPackage.SET_OPERATION__FILTER_PATTERN:
				return getFilterPattern();
			case ScaveModelPackage.SET_OPERATION__TYPE:
				return getType();
			case ScaveModelPackage.SET_OPERATION__CACHED_IDS:
				return getCachedIDs();
		}
		return super.eGet(featureID, resolve, coreType);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public void eSet(int featureID, Object newValue) {
		switch (featureID) {
			case ScaveModelPackage.SET_OPERATION__SOURCE_DATASET:
				setSourceDataset((Dataset)newValue);
				return;
			case ScaveModelPackage.SET_OPERATION__FILTER_PATTERN:
				setFilterPattern((String)newValue);
				return;
			case ScaveModelPackage.SET_OPERATION__TYPE:
				setType((ResultType)newValue);
				return;
			case ScaveModelPackage.SET_OPERATION__CACHED_IDS:
				setCachedIDs(newValue);
				return;
		}
		super.eSet(featureID, newValue);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public void eUnset(int featureID) {
		switch (featureID) {
			case ScaveModelPackage.SET_OPERATION__SOURCE_DATASET:
				setSourceDataset((Dataset)null);
				return;
			case ScaveModelPackage.SET_OPERATION__FILTER_PATTERN:
				setFilterPattern(FILTER_PATTERN_EDEFAULT);
				return;
			case ScaveModelPackage.SET_OPERATION__TYPE:
				setType(TYPE_EDEFAULT);
				return;
			case ScaveModelPackage.SET_OPERATION__CACHED_IDS:
				setCachedIDs(CACHED_IDS_EDEFAULT);
				return;
		}
		super.eUnset(featureID);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public boolean eIsSet(int featureID) {
		switch (featureID) {
			case ScaveModelPackage.SET_OPERATION__SOURCE_DATASET:
				return sourceDataset != null;
			case ScaveModelPackage.SET_OPERATION__FILTER_PATTERN:
				return FILTER_PATTERN_EDEFAULT == null ? filterPattern != null : !FILTER_PATTERN_EDEFAULT.equals(filterPattern);
			case ScaveModelPackage.SET_OPERATION__TYPE:
				return type != TYPE_EDEFAULT;
			case ScaveModelPackage.SET_OPERATION__CACHED_IDS:
				return CACHED_IDS_EDEFAULT == null ? cachedIDs != null : !CACHED_IDS_EDEFAULT.equals(cachedIDs);
		}
		return super.eIsSet(featureID);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public String toString() {
		if (eIsProxy()) return super.toString();

		StringBuffer result = new StringBuffer(super.toString());
		result.append(" (filterPattern: ");
		result.append(filterPattern);
		result.append(", type: ");
		result.append(type);
		result.append(", cachedIDs: ");
		result.append(cachedIDs);
		result.append(')');
		return result.toString();
	}

} //SetOperationImpl
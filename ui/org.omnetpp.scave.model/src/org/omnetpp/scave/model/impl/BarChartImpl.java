/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.scave.model.impl;

import java.util.Collection;

import org.eclipse.emf.common.notify.Notification;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;

import org.eclipse.emf.ecore.util.EDataTypeUniqueEList;

import org.eclipse.emf.ecore.impl.ENotificationImpl;

import org.omnetpp.scave.model.BarChart;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Bar Chart</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.impl.BarChartImpl#getGroupBy <em>Group By</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class BarChartImpl extends ChartImpl implements BarChart {
	/**
	 * The cached value of the '{@link #getGroupBy() <em>Group By</em>}' attribute list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getGroupBy()
	 * @generated
	 * @ordered
	 */
	protected EList<String> groupBy;

	/**
	 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
	 * @generated
	 */
    protected BarChartImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
	 * @generated
	 */
    @Override
				protected EClass eStaticClass() {
		return ScaveModelPackage.Literals.BAR_CHART;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<String> getGroupBy() {
		if (groupBy == null) {
			groupBy = new EDataTypeUniqueEList<String>(String.class, this, ScaveModelPackage.BAR_CHART__GROUP_BY);
		}
		return groupBy;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public Object eGet(int featureID, boolean resolve, boolean coreType) {
		switch (featureID) {
			case ScaveModelPackage.BAR_CHART__GROUP_BY:
				return getGroupBy();
		}
		return super.eGet(featureID, resolve, coreType);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@SuppressWarnings("unchecked")
		@Override
	public void eSet(int featureID, Object newValue) {
		switch (featureID) {
			case ScaveModelPackage.BAR_CHART__GROUP_BY:
				getGroupBy().clear();
				getGroupBy().addAll((Collection<? extends String>)newValue);
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
			case ScaveModelPackage.BAR_CHART__GROUP_BY:
				getGroupBy().clear();
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
			case ScaveModelPackage.BAR_CHART__GROUP_BY:
				return groupBy != null && !groupBy.isEmpty();
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
		result.append(" (groupBy: ");
		result.append(groupBy);
		result.append(')');
		return result.toString();
	}

} //BarChartImpl
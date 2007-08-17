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
 *   <li>{@link org.omnetpp.scave.model.impl.BarChartImpl#getGroupNameFormat <em>Group Name Format</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.BarChartImpl#getBarNameFormat <em>Bar Name Format</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.BarChartImpl#getBarFields <em>Bar Fields</em>}</li>
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
	 * The default value of the '{@link #getGroupNameFormat() <em>Group Name Format</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getGroupNameFormat()
	 * @generated
	 * @ordered
	 */
	protected static final String GROUP_NAME_FORMAT_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getGroupNameFormat() <em>Group Name Format</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getGroupNameFormat()
	 * @generated
	 * @ordered
	 */
	protected String groupNameFormat = GROUP_NAME_FORMAT_EDEFAULT;

	/**
	 * The default value of the '{@link #getBarNameFormat() <em>Bar Name Format</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getBarNameFormat()
	 * @generated
	 * @ordered
	 */
	protected static final String BAR_NAME_FORMAT_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getBarNameFormat() <em>Bar Name Format</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getBarNameFormat()
	 * @generated
	 * @ordered
	 */
	protected String barNameFormat = BAR_NAME_FORMAT_EDEFAULT;

	/**
	 * The cached value of the '{@link #getBarFields() <em>Bar Fields</em>}' attribute list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getBarFields()
	 * @generated
	 * @ordered
	 */
	protected EList<String> barFields;

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
	public String getGroupNameFormat() {
		return groupNameFormat;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setGroupNameFormat(String newGroupNameFormat) {
		String oldGroupNameFormat = groupNameFormat;
		groupNameFormat = newGroupNameFormat;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.BAR_CHART__GROUP_NAME_FORMAT, oldGroupNameFormat, groupNameFormat));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String getBarNameFormat() {
		return barNameFormat;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setBarNameFormat(String newBarNameFormat) {
		String oldBarNameFormat = barNameFormat;
		barNameFormat = newBarNameFormat;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.BAR_CHART__BAR_NAME_FORMAT, oldBarNameFormat, barNameFormat));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<String> getBarFields() {
		if (barFields == null) {
			barFields = new EDataTypeUniqueEList<String>(String.class, this, ScaveModelPackage.BAR_CHART__BAR_FIELDS);
		}
		return barFields;
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
			case ScaveModelPackage.BAR_CHART__GROUP_NAME_FORMAT:
				return getGroupNameFormat();
			case ScaveModelPackage.BAR_CHART__BAR_NAME_FORMAT:
				return getBarNameFormat();
			case ScaveModelPackage.BAR_CHART__BAR_FIELDS:
				return getBarFields();
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
			case ScaveModelPackage.BAR_CHART__GROUP_NAME_FORMAT:
				setGroupNameFormat((String)newValue);
				return;
			case ScaveModelPackage.BAR_CHART__BAR_NAME_FORMAT:
				setBarNameFormat((String)newValue);
				return;
			case ScaveModelPackage.BAR_CHART__BAR_FIELDS:
				getBarFields().clear();
				getBarFields().addAll((Collection<? extends String>)newValue);
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
			case ScaveModelPackage.BAR_CHART__GROUP_NAME_FORMAT:
				setGroupNameFormat(GROUP_NAME_FORMAT_EDEFAULT);
				return;
			case ScaveModelPackage.BAR_CHART__BAR_NAME_FORMAT:
				setBarNameFormat(BAR_NAME_FORMAT_EDEFAULT);
				return;
			case ScaveModelPackage.BAR_CHART__BAR_FIELDS:
				getBarFields().clear();
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
			case ScaveModelPackage.BAR_CHART__GROUP_NAME_FORMAT:
				return GROUP_NAME_FORMAT_EDEFAULT == null ? groupNameFormat != null : !GROUP_NAME_FORMAT_EDEFAULT.equals(groupNameFormat);
			case ScaveModelPackage.BAR_CHART__BAR_NAME_FORMAT:
				return BAR_NAME_FORMAT_EDEFAULT == null ? barNameFormat != null : !BAR_NAME_FORMAT_EDEFAULT.equals(barNameFormat);
			case ScaveModelPackage.BAR_CHART__BAR_FIELDS:
				return barFields != null && !barFields.isEmpty();
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
		result.append(", groupNameFormat: ");
		result.append(groupNameFormat);
		result.append(", barNameFormat: ");
		result.append(barNameFormat);
		result.append(", barFields: ");
		result.append(barFields);
		result.append(')');
		return result.toString();
	}

} //BarChartImpl
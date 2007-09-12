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

import org.eclipse.emf.ecore.impl.ENotificationImpl;

import org.eclipse.emf.ecore.util.EDataTypeUniqueEList;
import org.omnetpp.scave.model.ScatterChart;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Scatter Chart</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.impl.ScatterChartImpl#getXDataPattern <em>XData Pattern</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.ScatterChartImpl#getIsoDataPattern <em>Iso Data Pattern</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.impl.ScatterChartImpl#isAverageReplications <em>Average Replications</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class ScatterChartImpl extends ChartImpl implements ScatterChart {
	/**
	 * The default value of the '{@link #getXDataPattern() <em>XData Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getXDataPattern()
	 * @generated
	 * @ordered
	 */
	protected static final String XDATA_PATTERN_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getXDataPattern() <em>XData Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getXDataPattern()
	 * @generated
	 * @ordered
	 */
	protected String xDataPattern = XDATA_PATTERN_EDEFAULT;

	/**
	 * The cached value of the '{@link #getIsoDataPattern() <em>Iso Data Pattern</em>}' attribute list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getIsoDataPattern()
	 * @generated
	 * @ordered
	 */
	protected EList<String> isoDataPattern;

	/**
	 * The default value of the '{@link #isAverageReplications() <em>Average Replications</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isAverageReplications()
	 * @generated
	 * @ordered
	 */
	protected static final boolean AVERAGE_REPLICATIONS_EDEFAULT = true;

	/**
	 * The cached value of the '{@link #isAverageReplications() <em>Average Replications</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isAverageReplications()
	 * @generated
	 * @ordered
	 */
	protected boolean averageReplications = AVERAGE_REPLICATIONS_EDEFAULT;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected ScatterChartImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected EClass eStaticClass() {
		return ScaveModelPackage.Literals.SCATTER_CHART;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String getXDataPattern() {
		return xDataPattern;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setXDataPattern(String newXDataPattern) {
		String oldXDataPattern = xDataPattern;
		xDataPattern = newXDataPattern;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.SCATTER_CHART__XDATA_PATTERN, oldXDataPattern, xDataPattern));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<String> getIsoDataPattern() {
		if (isoDataPattern == null) {
			isoDataPattern = new EDataTypeUniqueEList<String>(String.class, this, ScaveModelPackage.SCATTER_CHART__ISO_DATA_PATTERN);
		}
		return isoDataPattern;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public boolean isAverageReplications() {
		return averageReplications;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setAverageReplications(boolean newAverageReplications) {
		boolean oldAverageReplications = averageReplications;
		averageReplications = newAverageReplications;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ScaveModelPackage.SCATTER_CHART__AVERAGE_REPLICATIONS, oldAverageReplications, averageReplications));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public Object eGet(int featureID, boolean resolve, boolean coreType) {
		switch (featureID) {
			case ScaveModelPackage.SCATTER_CHART__XDATA_PATTERN:
				return getXDataPattern();
			case ScaveModelPackage.SCATTER_CHART__ISO_DATA_PATTERN:
				return getIsoDataPattern();
			case ScaveModelPackage.SCATTER_CHART__AVERAGE_REPLICATIONS:
				return isAverageReplications() ? Boolean.TRUE : Boolean.FALSE;
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
			case ScaveModelPackage.SCATTER_CHART__XDATA_PATTERN:
				setXDataPattern((String)newValue);
				return;
			case ScaveModelPackage.SCATTER_CHART__ISO_DATA_PATTERN:
				getIsoDataPattern().clear();
				getIsoDataPattern().addAll((Collection<? extends String>)newValue);
				return;
			case ScaveModelPackage.SCATTER_CHART__AVERAGE_REPLICATIONS:
				setAverageReplications(((Boolean)newValue).booleanValue());
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
			case ScaveModelPackage.SCATTER_CHART__XDATA_PATTERN:
				setXDataPattern(XDATA_PATTERN_EDEFAULT);
				return;
			case ScaveModelPackage.SCATTER_CHART__ISO_DATA_PATTERN:
				getIsoDataPattern().clear();
				return;
			case ScaveModelPackage.SCATTER_CHART__AVERAGE_REPLICATIONS:
				setAverageReplications(AVERAGE_REPLICATIONS_EDEFAULT);
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
			case ScaveModelPackage.SCATTER_CHART__XDATA_PATTERN:
				return XDATA_PATTERN_EDEFAULT == null ? xDataPattern != null : !XDATA_PATTERN_EDEFAULT.equals(xDataPattern);
			case ScaveModelPackage.SCATTER_CHART__ISO_DATA_PATTERN:
				return isoDataPattern != null && !isoDataPattern.isEmpty();
			case ScaveModelPackage.SCATTER_CHART__AVERAGE_REPLICATIONS:
				return averageReplications != AVERAGE_REPLICATIONS_EDEFAULT;
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
		result.append(" (xDataPattern: ");
		result.append(xDataPattern);
		result.append(", isoDataPattern: ");
		result.append(isoDataPattern);
		result.append(", averageReplications: ");
		result.append(averageReplications);
		result.append(')');
		return result.toString();
	}

} //ScatterChartImpl